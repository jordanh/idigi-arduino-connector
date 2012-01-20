import idigi_ws_api
import logging
import time
import unittest
import configuration
import iik_testcase

log = logging.getLogger('discovery')
log.setLevel(logging.INFO)

if len(log.handlers) == 0:
    handler = logging.StreamHandler()
    handler.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)

class DiscoveryTestCase(iik_testcase.TestCase):

    def test_verify_device_type_matches(self):
        log.info("Beginning Test to Verify Device Type.")
        
        log.info("Retrieving Device type for %s." % self.device_config.device_id)
        # Send request for new device_core
        new_device_core = self.api.get_first('DeviceCore', 
                    condition="devConnectwareId='%s'" % self.device_config.device_id)
                            
        # Verify that device type returned matches that in the configuration.
        self.assertEqual(new_device_core.dpDeviceType, self.device_config.device_type,
            "Device type (%s) does not match device type in configuration (%s)." 
            % (new_device_core.dpDeviceType, self.device_config.device_type))
    
    def test_verify_vendor_id_matches(self):
        log.info("Beginning Test to Verify Vendor ID.")
        log.info("Retrieving Vendor ID for %s." % self.device_config.device_id)
        # Send request for new device_core
        new_device_core = self.api.get_first('DeviceCore', 
                    condition="devConnectwareId='%s'" % self.device_config.device_id)
                            
        # Verify that device's vendor id matches that in the configuration.
        self.assertEqual(new_device_core.dvVendorId, self.device_config.vendor_id,
            "Vendor ID (%s) does not match Vendor ID in configuration (%s)."
            % (new_device_core.dvVendorId, self.device_config.vendor_id))


        
if __name__ == '__main__':
    unittest.main()

