import logging
import time
import datetime
import json
from base64 import b64encode, b64decode
import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()
import push_client
from threading import Event
from utils import convert_to_datetime, total_seconds, getText

log = logging.getLogger('ds_utils')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.INFO)
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

class FileDataCallback(object):

    def __init__(self, path):
        self.path = path
        self.event = Event()
        self.data = None

    def callback(self, data):
        self.data = json.loads(data)
        self.event.set()

def update_and_verify(instance, api, device_id, target, content, 
                    datetime_created, file_location, 
                    file_name, expected_content=None, dne=False,
                    original_created_time=None, wait_time=60):
                    
    """Sends firmware update to trigger a data push, then performs
    a GET on the file_location to determine if the correct content 
    was pushed. A second GET is performed to verify the file's 
    metadata is correct.
    """

    client = push_client.PushClient(api.username, api.password, api.hostname, 
                secure=False)
    
    try:
        monitor = client.create_monitor([file_location], batch_size=1, batch_duration=0, 
            format_type='json')

        cb = FileDataCallback(file_location)

        session = client.create_session(cb.callback, monitor)

        # Send first firmware update to Archive=True target
        log.info("Sending firmware update to create file.")
        # Collect current datetime for later comparison
        i = len(datetime_created)
        datetime_created.append(datetime.datetime.utcnow())
        
        # Send firmware update to target
        response = update_firmware(api, device_id, "%d" % target, content)
                
        # Check that file content is correct
        log.info("Waiting for File Content.")
        cb.event.wait(wait_time)

        instance.assertNotEqual(None, cb.data, "Data not received for %s within wait time %d." % (file_location, wait_time))

        log.info("Verifying File Content.")
        file_data = cb.data['Document']['Msg']['FileData']
        file_size = file_data['fdSize']

        if file_size == 0:
            file_content = ''
        else:
            file_content = b64decode(file_data['fdData'])
        
        # If provided, compare file's contents to expected content.
        if not expected_content:
            expected_content = content
        
        # Verify file's contents
        instance.assertEqual(expected_content, file_content)
        
        # Check that FileData is correct
        log.info("Verifying file's metadata")
        
        # Verify that file's Modified Date/time is within 2 minutes of sampled
        # date/time
        fd_datetime_modified = convert_to_datetime(file_data['fdLastModifiedDate'])
        delta = total_seconds(abs(fd_datetime_modified - datetime_created[i]))
        # temporarily remove assertion, iDigi bug needs to be resolved
        instance.assertTrue(delta < 120, 
            "File's Last Modified Date/Time is not correct (delta=%d)." % delta)
        
        # If supplied, verify that file's Created Date/time is within 2 minutes
        # of sampled date/time
        if original_created_time:
            fd_datetime_created = convert_to_datetime(file_data['fdCreatedDate'])
            delta = total_seconds(abs(fd_datetime_created - original_created_time))
            instance.assertTrue(delta < 120, "File's Create Date/Time is not correct.")
        
        # If the file did not previously exist (Does Not Exist), verify that
        # the created date/time is the same as the last modified date/time    
        if dne and not original_created_time:
            instance.assertEqual(file_data['fdCreatedDate'], 
                file_data['fdLastModifiedDate'], 
                "File's created date/time does not match file's last modified date/time.")
        
        return file_content
    finally:
        client.stop_all()

def get_filedatahistory(api, file_history_location):
    
    """ Retrieves the filedata history from the file_history_location
    and then parses it into separate enries
    """
    
    log.info("Verifying data in %s" % file_history_location)
    fdh_response = api.get_raw(file_history_location, embed='true', 
        orderby='fdLastModifiedDate')
    
    # iDigi unfortunately returns the FileData payload if there is only
    # one matching historical entry, if more than one it returns XML.
    # Vantive opened for this, but until then, parse XML if XML, otherwise
    # return raw payload.
    # This will create weakness in our validation if only 1 response is 
    # returned as the modification/creation times cannot be inspected.
    if fdh_response.startswith('<?xml'):
        dom = xml.dom.minidom.parseString(fdh_response)
        return dom.getElementsByTagName("FileDataHistory")
    else:
        return fdh_response
    
def check_filedatahistory(instance, fdh, datetime_created, 
                            content, files_pushed_after, dne=False):
    
    """ takes a list of filedatahistory entries and verifies the content
    and metadata.
    """
    
    if type(fdh) == str:
        # Due to iDigi bug, if History only had one element, we can't get at 
        # the metadata.  This is temporary until bug is fixed.
        data = fdh
    else:    
        # find correct filedatahistory entry    
        file = len(fdh) - (files_pushed_after + 1)
        modified_date = getText(fdh[file].getElementsByTagName("fdLastModifiedDate")[0])
        data = b64decode(getText(fdh[file].getElementsByTagName("fdData")[0]))
    
    instance.assertEqual(data, content, 
        "File's contents do not match what is expected.")
     
    # Verify that file's Last Modified Date/time is within 2 minutes of
    # sampled date/time
    # fd_modified_date = convert_to_datetime(modified_date)
    # delta = total_seconds(abs(fd_modified_date - datetime_created))
    # instance.assertTrue(delta < 120, 
    #        "File's Last Modified Date/Time is incorrect (delta = %d)" % delta)
    
    if dne and type(fdh) != str:
        created_date = getText(fdh[file].getElementsByTagName("fdCreatedDate")[0])
        instance.assertEqual(created_date, modified_date, 
            "File's created date/time does not match file's last modified date/time.")
        
    

