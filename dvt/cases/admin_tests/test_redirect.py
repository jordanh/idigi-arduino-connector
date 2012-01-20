import idigi_ws_api
import logging
import time
import unittest
import configuration

from base64 import encodestring

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

# from utils import getText, determine_disconnect_reconnect, update_firmware
from utils import determine_disconnect_reconnect

config = configuration.DeviceConfiguration('config.ini')
nonidigi_host = 'google.com'


log = logging.getLogger('connectioncontrol')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

DESTINATION = "<destination>%s</destination>"

REDIRECT_REQUEST = \
"""<sci_request version="1.0">
    <redirect>
        <targets>
            <device id="%s"/>
        </targets>
        <destinations>
            %s
        </destinations>
    </redirect>
</sci_request>"""

DISCONNECT_REQUEST = \
"""<sci_request version="1.0">
    <disconnect>
        <targets>
            <device id="%s"/>
        </targets>
    </disconnect>
</sci_request>"""

REBOOT_REQUEST = \
"""<sci_request version="1.0"> 
  <reboot> 
    <targets> 
      <device id="%s"/> 
    </targets> 
  </reboot>
</sci_request>"""

target = 0

FIRMWARE_QUERY_REQUEST = \
"""<sci_request version="1.0">
  <query_firmware_targets>
    <targets>
      <device id="%s"/>
    </targets>
  </query_firmware_targets>
</sci_request>"""
    
FIRMWARE_ZERO_DATA_REQUEST= \
"""<sci_request version="1.1">
    <update_firmware firmware_target="%d">
        <targets>
            <device id="%s"/>
        </targets>
        <data></data>
    </update_firmware>
</sci_request>"""

class RedirectTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
    
        
    def test_terminate_iik_by_firmware_upgrade(self):
    
        """ Sends an asynchronous firmware update to teminate iik"""
    
        log.info("***** Updating Firmware to terminate IIK *****")
        # Check time the device was last connected
        last_connected = self.device_core.dpLastConnectTime
        
        # Send firmware target query
        log.info("Sending firmware target query to %s." % config.device_id)
               
        # Find firmware targets
        firmware_targets_xml = config.api.sci(FIRMWARE_QUERY_REQUEST % 
            config.device_id)
        
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        log.info("Determining if the device has defined firmware targets.")
        self.assertNotEqual(0, len(targets), 
                "No targets exist on device %s" % config.device_id)

        # Create asynchronous update request
        # Send asynchronous update request
        log.info("Sending request to update firmware.")
        upgrade_firmware_response = config.api.sci(FIRMWARE_ZERO_DATA_REQUEST % (target, config.device_id))
        
        log.info("Response to firmware Update:\n%s" % upgrade_firmware_response)

        time.sleep(4)
        
    def test_redirect(self):
    
        """ Sends redirect request to given device and verifies that it
        disconnects and reconnects
        """
        
        log.info("***** Beginning Redirect Test *****")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        # Create the request to redirect the device to the same server.
        redirect_request = REDIRECT_REQUEST % \
            (config.device_id, DESTINATION % config.api.hostname)
        
        # Send SCI redirect request.
        response = config.api.sci(redirect_request)
        
        # Print response to request.
        log.info("response:\n%s" % response)
        
        # Determine if device disconnected and reconnected.
        determine_disconnect_reconnect(self, config, last_connected)
    
    def test_redirect_multi_urls_first_nonidigi(self):
    
        """ Sends redirect request to given device starting with a nonidigi 
        host and verifies that it disconnects and reconnects to an idigi 
        server.
        """
        
        log.info("***** Beginning Redirect Test with multiple URLs (first non-iDigi) *****")
        last_connected = self.device_core.dpLastConnectTime

        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        # Create the request to redirect the device to a nonidigi server and
        # then an iDigi server
        destination = DESTINATION % nonidigi_host + DESTINATION % config.api.hostname
        redirect_request = REDIRECT_REQUEST % \
            (config.device_id, destination)
        
        # Send SCI redirect request.
        response = config.api.sci(redirect_request)
        log.info("response:\n%s" % response)
        
        # Determine if device disconnected and reconnected
        determine_disconnect_reconnect(self, config, last_connected, 60)
        
    def test_redirect_singleurl_nondigi(self):
        
        """ Sends redirect reqeust to given device starting with a noniDigi
        host and verifies that it disconnets and reconnects to an iDigi 
        server.
        """
        
        log.info("***** Beginning Redirect Test with single non-iDigi URL *****")
        last_connected = self.device_core.dpLastConnectTime

        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        # Create the request to redirect the device to a nonidigi server.
        redirect_request = REDIRECT_REQUEST % \
            (config.device_id, DESTINATION % nonidigi_host)
            
        # Send sci redirect request.
        response = config.api.sci(redirect_request)
        log.info("response:\n%s" % response)
        
        # Determine if device disconnected and reconnected
        determine_disconnect_reconnect(self, config, last_connected, 60)
    
    def test_redirect_zero_destinations(self):
    
        """ Sends redirect request with no destinations to given device.
        Verifies that an error response is returned.
        """
        
        log.info("***** Beginnning Redirect Test with zero destination URLs *****")
        
        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        
        # Create redirect request with no destinations
        redirect_request = REDIRECT_REQUEST % (config.device_id, "")
        
        # Send SCI redirect request.
        response = config.api.sci_expect_fail(redirect_request)
        log.info("response:\n%s" % response)
        
        # Verifying the request returns an error.
        log.info("Determining if we have received error response.")
        error = response.find("error")
        self.assertNotEqual(-1, error, "Received unexpected response from invalid redirect.")

 
if __name__ == '__main__':

    unittest.main()
