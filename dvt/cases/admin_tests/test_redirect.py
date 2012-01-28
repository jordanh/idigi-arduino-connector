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

class RedirectTestCase(iik_testcase.TestCase):
        
    def test_redirect(self):
    
        """ Sends redirect request to given device and verifies that it
        disconnects and reconnects
        """
        
        self.log.info("***** Beginning Redirect Test *****")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)
        try:
            monitor.start()
        
            self.log.info("Sending Connection Control Redirect to %s." % self.device_config.device_id)
            # Create the request to redirect the device to the same server.
            redirect_request = REDIRECT_REQUEST % \
                (self.device_config.device_id, DESTINATION % self.api.hostname)
            
            # Send SCI redirect request (using admin credentials).
            response = self.device_config.api.sci(redirect_request)
            
            # Print response to request.
            self.log.info("response:\n%s" % response)
            
            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")
            
            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(60)
            self.log.info("Device connected.") 
        finally:
            monitor.stop()
    
    def test_redirect_multi_urls_first_nonidigi(self):
    
        """ Sends redirect request to given device starting with a nonidigi 
        host and verifies that it disconnects and reconnects to an idigi 
        server.
        """
        
        self.log.info("***** Beginning Redirect Test with multiple URLs (first non-iDigi) *****")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)
        try:
            monitor.start()

            self.log.info("Sending Connection Control Redirect to %s." % self.device_config.device_id)
            # Create the request to redirect the device to a nonidigi server and
            # then an iDigi server
            destination = DESTINATION % nonidigi_host + DESTINATION % self.api.hostname
            redirect_request = REDIRECT_REQUEST % \
                (self.device_config.device_id, destination)
            
            # Send SCI redirect request. (using admin credentials).
            response = self.device_config.api.sci(redirect_request)

            self.log.info("response:\n%s" % response)

            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")
            
            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(60)
            self.log.info("Device connected.") 
        finally:
            monitor.stop()
        
    def test_redirect_singleurl_nondigi(self):
        
        """ Sends redirect reqeust to given device starting with a noniDigi
        host and verifies that it disconnets and reconnects to an iDigi 
        server.
        """
        
        self.log.info("***** Beginning Redirect Test with single non-iDigi URL *****")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)
        try:
            monitor.start()

            self.log.info("Sending Connection Control Redirect to %s." % self.device_config.device_id)
            # Create the request to redirect the device to a nonidigi server.
            redirect_request = REDIRECT_REQUEST % \
                (self.device_config.device_id, DESTINATION % nonidigi_host)
                
            # Send sci redirect request. (Using admin credentials).
            response = self.device_config.api.sci(redirect_request)
            self.log.info("response:\n%s" % response)
            
            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")
            
            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(60)
            self.log.info("Device connected.") 
        finally:
            monitor.stop()
    
    def test_redirect_zero_destinations(self):
    
        """ Sends redirect request with no destinations to given device.
        Verifies that an error response is returned.
        """
        
        self.log.info("***** Beginnning Redirect Test with zero destination URLs *****")
        
        self.log.info("Sending Connection Control Redirect to %s." % self.device_config.device_id)
        
        # Create redirect request with no destinations
        redirect_request = REDIRECT_REQUEST % (self.device_config.device_id, "")
        
        # Send SCI redirect request. (Using admin credentials).
        response = self.device_config.api.sci_expect_fail(redirect_request)
        self.log.info("response:\n%s" % response)
        
        # Verifying the request returns an error.
        self.log.info("Determining if we have received error response.")
        error = response.find("error")
        self.assertNotEqual(-1, error, "Received unexpected response from invalid redirect.")

 
if __name__ == '__main__':
    unittest.main()
