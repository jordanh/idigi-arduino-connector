import time
import iik_testcase

from utils import DeviceConnectionMonitor

DISCONNECT_REQUEST = \
"""<sci_request version="1.0">
    <disconnect>
        <targets>
            <device id="%s"/>
        </targets>
    </disconnect>
</sci_request>"""

class DisconnectTestCase(iik_testcase.TestCase):
    
    def test_disconnect(self):
    
        """ Sends disconnect request to given device and verifies that
        the device disconnects and reconnects to an iDigi server.
        """
        
        self.log.info("***** Beginning Normal Disconnect Test *****")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()
            self.log.info("Sending Connection Control Disconnect to %s." % self.device_config.device_id)
            
            # Create disconnect request
            disconnect_request = DISCONNECT_REQUEST % (self.device_config.device_id)
            
            # Send SCI disconnect request
            self.api.sci(disconnect_request)
        
            
            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")
            
            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(30)
            self.log.info("Device connected.") 
            time.sleep(10)
        finally:
            monitor.stop()

    def test_disconnect_with_receive_delay(self):
    
        """ Sends disconnect request to given device and verifies that
        the device disconnects and reconnects to an iDigi server.
        """
        
        self.log.info("***** Beginning Disconnect Test (delay receive) *****")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()
            self.log.info("Sending Connection Control Disconnect to %s." % self.device_config.device_id)
            
            # Create disconnect request
            disconnect_request = DISCONNECT_REQUEST % (self.device_config.device_id)
            
            # Send SCI disconnect request
            self.api.sci(disconnect_request)
        
            
            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(60)
            self.log.info("Device disconnected.")
            
            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(60)
            self.log.info("Device connected.") 
        finally:
            monitor.stop()

        
if __name__ == '__main__':
    unittest.main()
