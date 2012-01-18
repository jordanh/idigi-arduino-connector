import idigi_ws_api
import logging
import time
import unittest
import configuration

from utils import determine_disconnect_reconnect

config = configuration.DeviceConfiguration('config.ini')

log = logging.getLogger('connectioncontrol')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

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
        
        log.info("***** Beginning Disconnect Test *****")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending Connection Control Disconnect to %s." % config.device_id)
        
        # Create disconnect request
        disconnect_request = DISCONNECT_REQUEST % (config.device_id)
        
        # Send SCI disconnect request
        config.api.sci(disconnect_request)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, config, last_connected, 30)

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
    
        """ Sends reboot request to given device and verifies that
        the device disconnects and reconnects to an iDigi server.
        """
        
        log.info("***** Beginning Reboot Test *****")
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
