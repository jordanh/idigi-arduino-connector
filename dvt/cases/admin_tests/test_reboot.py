import time
import iik_testcase

from utils import determine_disconnect_reconnect

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

class DisconnectTestCase(iik_testcase.TestCase):
    
    def test_disconnect(self):
    
        """ Sends disconnect request to given device and verifies that
        the device disconnects and reconnects to an iDigi server.
        """
        
        self.log.info("***** Beginning Disconnect Test *****")
        last_connected = self.device_core.dpLastConnectTime
        
        self.log.info("Sending Connection Control Disconnect to %s." % self.device_config.device_id)
        
        # Create disconnect request
        disconnect_request = DISCONNECT_REQUEST % (self.device_config.device_id)
        
        # Send SCI disconnect request
        self.api.sci(disconnect_request)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, self.device_config, self.api, last_connected, 60)

class RebootTestCase(iik_testcase.TestCase):
    
    def test_reboot(self):
    
        """ Sends reboot request to given device and verifies that
        the device disconnects and reconnects to an iDigi server.
        """
        
        self.log.info("***** Beginning Reboot Test *****")
        last_connected = self.device_core.dpLastConnectTime
        
        self.log.info("Sending Reboot to %s." % self.device_config.device_id)
        # Create reboot request
        reboot_request = REBOOT_REQUEST % (self.device_config.device_id)

        # Send SCI reboot request
        response = self.api.sci(reboot_request)
        self.log.info("response to reboot request = %s" % response)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, self.device_config, self.api, last_connected, 60)
        
        
if __name__ == '__main__':

    unittest.main()
