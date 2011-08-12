import idigi_ws_api
import logging
import time
import unittest
import configuration

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
    
    def test_redirect(self):
    
        """ Sends redirect request to given device and verifies that it
        disconnects and reconnects
        """
        
        log.info("Beginning Redirect Test")
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
        
        log.info("Beginning Redirect Test with multiple URLs (first non-iDigi).")
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
        determine_disconnect_reconnect(self, config, last_connected, 30)
        
    def test_redirect_singleurl_nondigi(self):
        
        """ Sends redirect reqeust to given device starting with a noniDigi
        host and verifies that it disconnets and reconnects to an iDigi 
        server.
        """
        
        log.info("Beginning Redirect Test with single non-iDigi URL.")
        last_connected = self.device_core.dpLastConnectTime

        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        # Create the request to redirect the device to a nonidigi server.
        redirect_request = REDIRECT_REQUEST % \
            (config.device_id, DESTINATION % nonidigi_host)
            
        # Send sci redirect request.
        response = config.api.sci(redirect_request)
        log.info("response:\n%s" % response)
        
        # Determine if device disconnected and reconnected
        determine_disconnect_reconnect(self, config, last_connected, 30)
    
    def test_redirect_zero_destinations(self):
    
        """ Sends redirect request with no destinations to given device.
        Verifies that an error response is returned.
        """
        
        log.info("Beginnning Redirect Test with zero destination URLs.")
        
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

    def test_redirect_three_destinations(self):
    
        """ Sends redirect request to given device and verifies that
        the device disconnects and reconnects to an iDigi server.
        
        Test may terminate IIK device session running in debug.
        """
        
        log.info("Beginnning Redirect Test with three destination URLs.")
        last_connected = self.device_core.dpLastConnectTime
        
        # Send redirect with three destinations
        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        destinations = DESTINATION % config.api.hostname + DESTINATION % config.api.hostname + DESTINATION % config.api.hostname
        redirect_request = REDIRECT_REQUEST % \
            (config.device_id, destinations)
        
        response = config.api.sci_expect_fail(redirect_request)
        log.info("response:\n%s" % response)
        
        log.info("Determining if device was redirected.")
        redirected = response.find("redirected")
        self.assertNotEqual(-1, redirected, "Unexpected response: %s." % response)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, config, last_connected, 60)
       
class DisconnectTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
                        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
    
    def test_disconnect(self):
    
        """ Sends disconnect request to given device and verifies that
        the device disconnects and reconnects to an iDigi server.
        """
        
        log.info("Beginning Disconnect Test")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending Connection Control Disconnect to %s." % config.device_id)
        
        # Create disconnect request
        disconnect_request = DISCONNECT_REQUEST % (config.device_id)
        
        # Send SCI disconnect request
        config.api.sci(disconnect_request)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, config, last_connected)

class RebootTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
                        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
                
    def test_reboot(self):
    
        """ Sends reboot request to given device and verifies taht
        the device disconnects and reconnects to an iDigi server.
        """
        
        log.info("Beginning Reboot Test")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending Reboot to %s." % config.device_id)
        # Create reboot request
        reboot_request = REBOOT_REQUEST % (config.device_id)

        # Send SCI reboot request
        response = config.api.sci(reboot_request)
        log.info("response to reboot request = %s" % response)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, config, last_connected, 30)
        
        
if __name__ == '__main__':

    unittest.main()
