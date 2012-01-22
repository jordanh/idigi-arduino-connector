import time
import iik_testcase

from base64 import encodestring

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

# from utils import getText, determine_disconnect_reconnect, update_firmware
from utils import determine_disconnect_reconnect

nonidigi_host = 'google.com'

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

class Redirect3UrlsTestCase(iik_testcase.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        self.log.info("Ensuring Device %s is connected." % self.device_config.device_id)
        self.device_core = self.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % self.device_config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % self.device_config.device_id)

    def test_redirect_three_destinations(self):
    
        """ Sends redirect request to given device and verifies that
        the device disconnects and reconnects to an iDigi server.
        
        Test may terminate IIK device session running in debug.
        """
        
        self.log.info("***** Beginnning Redirect Test with three destination URLs *****")
        last_connected = self.device_core.dpLastConnectTime
        
        # Send redirect with three destinations
        self.log.info("Sending Connection Control Redirect to %s." % self.device_config.device_id)
        destinations = DESTINATION % self.api.hostname + DESTINATION % self.api.hostname + DESTINATION % self.api.hostname
        redirect_request = REDIRECT_REQUEST % \
            (self.device_config.device_id, destinations)
        
        response = self.api.sci_expect_fail(redirect_request)
        self.log.info("response:\n%s" % response)
        
        self.log.info("Determining if device was redirected.")
        redirected = response.find("redirected")
        self.assertNotEqual(-1, redirected, "Unexpected response: %s." % response)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, self.device_config, self.api, last_connected, 30)
       
 
if __name__ == '__main__':
    unittest.main()
