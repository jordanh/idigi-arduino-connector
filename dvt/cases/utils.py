from __future__ import division
import datetime
import logging
import time
import push_client
import json
from threading import Event
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()
from base64 import encodestring

log = logging.getLogger('utils')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

class DeviceConnectionMonitor(object):
    """
    Used to monitor a Device for Connection and Disconnection Events.  Utilizes 
    iDigi Push Monitoring to monitor events.
    """

    def __init__(self, api, device_id):
        """
        Create instance.

        Parameters:
        api -- idigi_ws_api.Api to use for creating monitor.
        device_id -- the devConnectwareId of the device.
        """
        self.api = api
        self.device_id = device_id
        self.client = push_client.PushClient(api.username, api.password, 
                                             api.hostname, secure=False)
        self.disconnect_event = Event()
        self.disconnect_data = None
        self.connect_event = Event()
        self.connect_data = None
        self.monitor = None
        self.session = None

    def __session_callback(self, data):
        device_core = json.loads(data)['Document']['Msg']['DeviceCore']
        status = device_core['dpConnectionStatus']
        if status == 1:
            # Device is connected.
            self.connect_data = device_core
            self.connect_event.set()
        elif status == 0:
            # Device is disconnected.
            self.disconnect_data = device_core
            self.disconnect_event.set()

        return True

    
    def start(self):
        """
        Starts by registering a Monitor on the Device and creating a TCP 
        session.
        """
        device_core = self.api.get_first(
            'DeviceCore', condition="devConnectwareId='%s'" % self.device_id)

        self.dev_id = device_core.id.devId
        self.monitor = self.client.create_monitor(
                                        ['DeviceCore/%s' % self.dev_id ], 
                                        batch_size=1, batch_duration=0, 
                                        format_type='json')

        session = self.client.create_session(self.__session_callback, self.monitor)

    def wait_for_connect(self, timeout):
        """
        Waits timeout seconds for device to connect.  If it connects within 
        timeout, returns the DeviceCore payload, otherwise an Exception is 
        raised.
        """
        self.connect_event.wait(timeout)
        if self.connect_data is None:
            raise Exception('Device %s did not connect within %d seconds.' % \
                (self.device_id, timeout))
        
        connect_data = self.connect_data
        self.connect_data = None
        self.connect_event.clear()
        return connect_data

    def wait_for_disconnect(self, timeout):
        """
        Waits timeout seconds for device to disconnect.  If it disconnects within 
        timeout, returns the DeviceCore payload, otherwise an Exception is 
        raised.
        """
        self.disconnect_event.wait(timeout)
        if self.disconnect_data is None:
            raise Exception('Device %s did not disconnect within %d seconds.' % \
                (self.device_id, timeout))
        
        disconnect_data = self.disconnect_data
        self.disconnect_data = None
        self.disconnect_event.clear()
        return disconnect_data
    
    def stop(self):
        """
        Stop the push client and all sessions.
        """
        self.client.stop_all()
        self.client.delete_monitor(self.monitor)

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
 
