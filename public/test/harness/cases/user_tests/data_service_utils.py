import logging
import time
import datetime
from base64 import b64encode
import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

import idigi_ws_api
from utils import convert_to_datetime, total_seconds, getText

log = logging.getLogger('ds_utils')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

def update_firmware(api, device, target, content):

    """ Sends firmware update request to the device with the given content."""
    
    request = impl.createDocument(None, "sci_request", None)
    sci_element = request.documentElement
    
    update_firmware_element = request.createElement("update_firmware")
    
    targets = request.createElement("targets")
    update_firmware_element.appendChild(targets)
    device_element = request.createElement("device")
    device_element.setAttribute("id", device)
    targets.appendChild(device_element)
        
    sci_element.appendChild(update_firmware_element)
    
        
    data = request.createElement("data")
    data_value = request.createTextNode(b64encode(content))
    data.appendChild(data_value)
    
    update_firmware_element.appendChild(data)
        
    if target:
        update_firmware_element.setAttribute("firmware_target", target)

    return api.sci(request.toprettyxml())
    
def update_and_verify(instance, api, device_id, target, content, 
                    datetime_created, file_location, 
                    file_name, expected_content=None, dne=False,
                    original_created_time=None, wait_time=5):
                    
    """Sends firmware update to trigger a data push, then performs
    a GET on the file_location to determine if the correct content 
    was pushed. A second GET is performed to verify the file's 
    metadata is correct.
    """
    
    # Send first firmware update to Archive=True target
    log.info("Sending firmware update to create file.")
    # Collect current datetime for later comparison
    i = len(datetime_created)
    datetime_created.append(datetime.datetime.utcnow())
    
    # Send firmware update to target
    response = update_firmware(api, device_id, "%d" % target, content)
            
    # Check that file content is correct
    log.info("Verifying file's content")
    time.sleep(wait_time)
    
    # send GET to retrieve file's content
    file_content = api.get_raw(file_location)
    
    # If provided, compare file's contents to expected content.
    if not expected_content:
        expected_content = content
    
    # Verify file's contents
    instance.assertEqual(expected_content, file_content, 
        "File's contents (%s) do not match what is expected (%s)"
        % (file_content, expected_content))
    
    # Check that FileData is correct
    log.info("Verifying file's metadata")
    fd_response = api.get_first(file_location, condition="fdName='%s'" % 
        file_name)
    
    # Verify that file's Modified Date/time is within 2 minutes of sampled
    # date/time
    fd_datetime_modified = convert_to_datetime(fd_response.fdLastModifiedDate)
    delta = total_seconds(abs(fd_datetime_modified - datetime_created[i]))
    instance.assertTrue(delta < 120, 
        "File's Last Modified Date/Time is not correct (delta=%d)." % delta)
    
    # If supplied, verify that file's Created Date/time is within 2 minutes
    # of sampled date/time
    if original_created_time:
        fd_datetime_created = convert_to_datetime(fd_response.fdCreatedDate)
        delta = total_seconds(abs(fd_datetime_created - original_created_time))
        instance.assertTrue(delta < 120, 
            "File's Create Date/Time is not correct.")
    
    # If the file did not previously exist (Does Not Exist), verify that
    # the created date/time is the same as the last modified date/time    
    if dne and not original_created_time:
        instance.assertEqual(fd_response.fdCreatedDate, 
            fd_response.fdLastModifiedDate, 
            "File's created date/time does not match file's last modified date/time.")
    
    return file_content

def get_filedatahistory(api, file_history_location):
    
    """ Retrieves the filedata history from the file_history_location
    and then parses it into separate enries
    """
    
    log.info("Verifying data in FileDataHistory")
    fdh_response = api.get_raw(file_history_location, embed='true', 
        orderby='fdLastModifiedDate')
    dom = xml.dom.minidom.parseString(fdh_response)
    return dom.getElementsByTagName("FileDataHistory")   
    
def check_filedatahistory(instance, fdh, datetime_created, 
                            encoded_content, files_pushed_after, dne=False):
    
    """ takes a list of filedatahistory entries and verifies the content
    and metadata.
    """
    
    # find correct filedatahistory entry    
    file = len(fdh) - (files_pushed_after + 1)
    modified_date = getText(fdh[file].getElementsByTagName("fdLastModifiedDate")[0])
    data = getText(fdh[file].getElementsByTagName("fdData")[0])
    
    instance.assertEqual(data, encoded_content, 
        "File's contents do not match what is expected (%s != %s)." % (data, encoded_content))
     
    # Verify that file's Last Modified Date/time is within 2 minutes of
    # sampled date/time
    fd_modified_date = convert_to_datetime(modified_date)
    delta = total_seconds(abs(fd_modified_date - datetime_created))
    instance.assertTrue(delta < 120, 
        "File's Last Modified Date/Time is incorrect (delta = %d)" % delta)
    
    if dne:
        created_date = getText(fdh[file].getElementsByTagName("fdCreatedDate")[0])
        instance.assertEqual(created_date, modified_date, 
            "File's created date/time does not match file's last modified date/time.")
        
    

