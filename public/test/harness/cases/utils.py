from __future__ import division
import datetime
import logging
import time
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()
from base64 import encodestring

log = logging.getLogger('utils')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

def convert_to_datetime(fd_time):

    """ Converts the date/time returned from iDigi to a datetime object"""
    
    dt = datetime.datetime(int(fd_time[0:4]), 
                                int(fd_time[5:7]), 
                                int(fd_time[8:10]),
                                int(fd_time[11:13]), 
                                int(fd_time[14:16]), 
                                int(fd_time[17:19]))
    return dt
    
def total_seconds(dt):

    """ Converts a datetime.delta to an int representing total seconds
    
    requires the 'from __future__ import division' line at beginning
    of file (for use with Python 2.6 and earlier)
    """
    
    return (dt.microseconds + (dt.seconds + dt.days * 24 * 3600) * 10**6) / 10**6
    
def getText(elem):
    rc = []
    for node in elem.childNodes:
        if node.nodeType == node.TEXT_NODE:
            rc.append(node.data)
    return str(''.join(rc))
    
def clean_slate(api, file_location):
    
    """ Checks if a file exists. If it does, the file is deleted."""
    
    log.info("Checking to make sure file does not exist")
    try:
        response = api.get_raw(file_location)
        log.info("File already exists... deleting...")
        api.delete_location(file_location)
    except Exception:
        pass
        
def determine_disconnect_reconnect(instance, config, last_connected, wait_time=15):
    log.info("Determining if Device %s disconnected." 
            % config.device_id)
    new_device_core = config.api.get_first('DeviceCore', 
                            condition="devConnectwareId='%s'" % config.device_id)
                            
    # Ensure device is disconnected as result of SCI request.
    instance.assertEqual('0', new_device_core.dpConnectionStatus,
            "Device %s did not disconnect." % config.device_id)
        
    log.info("Waiting up to %i seconds for device to reconnect." % wait_time)
    # We'll assume that the device reconnects within 10 seconds.
        
    log.info("Determining if Device %s reconnected." \
            % config.device_id)
    
    for i in range(int(wait_time/5)):
        time.sleep(5)
        new_device_core = config.api.get_first('DeviceCore', 
                            condition="devConnectwareId='%s'" % config.device_id)
        if new_device_core.dpConnectionStatus == "1":
            break
           
    # Ensure device has reconnected.
    instance.assertEqual('1', new_device_core.dpConnectionStatus,
                    "Device %s did not reconnect." % config.device_id)
    
    log.info("Initial Last Connect Time: %s." % last_connected)
    log.info("New Last Connect Time: %s." 
            % new_device_core.dpLastConnectTime)
    # Ensure that Last Connection Time has changed from initial Device State
    # instance.assertNotEqual(last_connected, new_device_core.dpLastConnectTime)
    
def update_firmware(api, device, input_firmware, target):
                    
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
    f = open(input_firmware, 'rb')
    data_value = request.createTextNode(encodestring(f.read()))
    f.close()
    data.appendChild(data_value)
    
    update_firmware_element.appendChild(data)
            
    if target:
        update_firmware_element.setAttribute("firmware_target", target)

    return api.sci(request.toprettyxml())
 