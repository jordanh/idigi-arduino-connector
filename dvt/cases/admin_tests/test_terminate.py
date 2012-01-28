import time
import iik_testcase

from base64 import encodestring

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

from utils import DeviceConnectionMonitor


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

target = 0

class TerminateTestCase(iik_testcase.TestCase):
        
    def test_terminate_iik_by_firmware_upgrade(self):
    
        """ Sends an asynchronous firmware update to teminate iik"""
    
        self.log.info("***** Updating Firmware to terminate IIK *****")
    
        # Send firmware target query
        self.log.info("Sending firmware target query to %s." % self.device_config.device_id)
               
        # Find firmware targets
        firmware_targets_xml = self.api.sci(FIRMWARE_QUERY_REQUEST % 
            self.device_config.device_id)
        
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        self.log.info("Determining if the device has defined firmware targets.")
        self.assertNotEqual(0, len(targets), 
                "No targets exist on device %s" % self.device_config.device_id)

        # Create asynchronous update request
        # Send asynchronous update request
        self.log.info("Sending request to update firmware.")
        upgrade_firmware_response = self.api.sci(FIRMWARE_ZERO_DATA_REQUEST % (target, self.device_config.device_id))
        
        self.log.info("Response to firmware Update:\n%s" % upgrade_firmware_response)

        time.sleep(4)
        
 
if __name__ == '__main__':
    unittest.main()
