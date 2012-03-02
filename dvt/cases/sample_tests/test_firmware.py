import time
import iik_testcase
from base64 import encodestring

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

from utils import getText, DeviceConnectionMonitor, update_firmware

target = 0

FIRMWARE_QUERY_REQUEST = \
"""<sci_request version="1.0">
  <query_firmware_targets>
    <targets>
      <device id="%s"/>
    </targets>
  </query_firmware_targets>
</sci_request>"""
    
class FirmwareTestCase(iik_testcase.TestCase):    

    def test_check_firmware_version_match(self):
    
        """ Verifies that firmware version returned from device matches
        firmware version in configuration.
        """
        
        self.log.info("Beginning Test to Confirm Firmware Version.")
        # Retrieve Firmware Version
        self.log.info("Retrieving Firmware Version from %s." % self.device_config.device_id)
        new_device_core = self.api.get_first('DeviceCore',
                        condition="devConnectwareId='%s'" % self.device_config.device_id)
        # Ensure firmware version is correct
        self.assertEqual(new_device_core.dpFirmwareLevelDesc, 
                        self.device_config.firmware_version,
                        "Firmware Version (%s) does not match expected (%s)" % 
                        (new_device_core.dpFirmwareLevelDesc, 
                        self.device_config.firmware_version))
    
    def test_confirm_firmware_targets(self):
    
        """ Verifies that the firmware targets returned match the firmware
        targets in the configuration.
        """
    
        self.log.info("Beginning Test to Validate Firmware Targets.")
        self.log.info("Sending firmware target query to %s." % self.device_config.device_id)
        
        # Send firmware target query request
        firmware_targets_xml = self.api.sci(FIRMWARE_QUERY_REQUEST % 
            self.device_config.device_id)
        
        # Parse request response for firmware targets
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        self.log.info("Determining if the number of targets matches targets in configuration.")
        self.assertEqual(len(targets), len(self.device_config.firmware_target), 
            "number of returned firmware targets (%i) does not match number of targets given (%i)" 
            % (len(targets), len(self.device_config.firmware_target)))
                
        for i in range(len(targets)):
            target = int(targets[i].getAttribute("number"))
            parse_firmware_target = [getText(targets[i].getElementsByTagName("name")[0]),
                                    getText(targets[i].getElementsByTagName("pattern")[0]),
                                    getText(targets[i].getElementsByTagName("version")[0])]
                                    
            self.log.info("firmware target %i name: %s" % (target, parse_firmware_target[0]))
            self.log.info("firmware target %i pattern: %s" % (target, parse_firmware_target[1]))
            self.log.info("firmware target %i version: %s" % (target, parse_firmware_target[2]))
            self.log.info("Determining if firmware target %s matches given target" % target)
            self.assertEqual(parse_firmware_target, self.device_config.firmware_target[target], 
                            "Firmware Target %s does not match given target %s" %  
                            (parse_firmware_target, self.device_config.firmware_target[target])) 
    

    def test_update_firmware_bytarget_multitargets(self):
    
        """ Updates firmware by target and verifies that the firmware
        is submitted and that the device disconnects and reconnects
        """
        
        self.log.info("Beginning Test to Update Firmware by Target.")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()
        
            self.log.info("Sending firmware target query to %s." % self.device_config.device_id)
            
            # Determine if device has firmware targets
            firmware_targets_xml = self.api.sci(FIRMWARE_QUERY_REQUEST % 
                self.device_config.device_id)
            dom = xml.dom.minidom.parseString(firmware_targets_xml)
            targets = dom.getElementsByTagName("target")
            
            self.log.info("Determining if the device has defined firmware targets.")
            self.assertNotEqual(0, len(targets), 
                    "No targets exist on device %s" % self.device_config.device_id)
            
            # Send request to update firmware
            self.log.info("Sending request to update firmware.")
            response = update_firmware(self.api, self.device_config.device_id, 
                self.device_config.firmware_target_file[target], "%d" % target)
            
            # Print response
            self.log.info("response: \n%s" % response)
           
            # Determine if Firmware was submitted
            submitted = response.find("submitted")
            self.assertNotEqual(-1, submitted, "Firmware was not submitted.")        

            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")
            
            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(30)
            self.log.info("Device connected.") 
        finally:
            monitor.stop()
        
    def test_update_firmware_nonexisting_target(self):
    
        """ Sends a firmware upgrade to a non-existing target and verifies
        that an error message is returned.
        """
        
        self.log.info("Beginning Test to Update Non-Existing Firmware Target.")
        # Check time the device was last connected
        last_connected = self.device_core.dpLastConnectTime
        
        # Send firmware target query
        self.log.info("Sending firmware target query to %s." % self.device_config.device_id)
                
        firmware_targets_xml = self.api.sci(FIRMWARE_QUERY_REQUEST % self.device_config.device_id)
        
        # Determine number of firmware targets
        self.log.info("Determining number of firmware targets.")
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        # Choose non-existing target for upgrade
        upgrade_target = len(targets)
        
        #Send upgrade request
        self.log.info("Sending request to update firmware on target %d." % upgrade_target)
        response = update_firmware(self.api, self.device_config.device_id, 
            self.device_config.firmware_target_file[target], "%d" % upgrade_target)
        
        self.log.info("response: \n%s" % response)
        
        # Verify that firmware was not updated 
        # should not find "submitted" in response
        self.log.info("Verify that the firmware was not updated.")
        submitted = response.find("submitted")
        self.assertEqual(-1, submitted, 
                "Firmware was successfully submitted to non-existing target.")
        
        # error message should include "Invalid Target"
        invalid_response = response.find("Invalid Target")
        self.assertNotEqual(-1, invalid_response, 
                "Device returned incorrect response to invalid firmware update.")

if __name__ == '__main__':

    unittest.main()
