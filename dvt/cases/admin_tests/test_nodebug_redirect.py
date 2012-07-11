import time
import iik_testcase

from base64 import encodestring

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

from utils import DeviceConnectionMonitor

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

class Redirect3UrlsTestCase(iik_testcase.TestCase):
    
    def test_redirect_three_destinations(self):
    
        """ Sends redirect request to given device and verifies that
        the device disconnects and reconnects to the iDigi Device Cloud.
        
        Test may terminate iDigi Connector device session running in debug.
        """
        
        self.log.info("***** Beginnning Redirect Test with three destination URLs *****")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()
            
            # Send redirect with three destinations
            self.log.info("Sending Connection Control Redirect to %s." % self.device_config.device_id)
            destinations = DESTINATION % self.api.hostname + DESTINATION % self.api.hostname + DESTINATION % self.api.hostname
            redirect_request = REDIRECT_REQUEST % \
                (self.device_config.device_id, destinations)

            response = self.device_config.api.sci_expect_fail(redirect_request)
            self.log.info("response:\n%s" % response)
            
            self.log.info("Determining if device was redirected.")
            redirected = response.find("redirected")
            self.assertNotEqual(-1, redirected, "Unexpected response: %s." % response)
            
            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")
            
            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(30)
            self.log.info("Device connected.") 
        finally:
            monitor.stop()
       
 
if __name__ == '__main__':
    unittest.main()
