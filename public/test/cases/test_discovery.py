import idigi_ws_api
import logging
import time
import unittest
import argparse
import configuration

config = configuration.DeviceConfiguration()

log = logging.getLogger('test_discovery')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)



class DiscoveryTestCase(unittest.TestCase):

    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
    
    def test_verify_device_type_matches(self):
        log.info("Beginning Test to Verify Device Type.")
        log.info("Retrieving Device type for %s." % config.device_id)
        new_device_core = config.api.get_first('DeviceCore', 
                            condition="devConnectwareId='%s'" % config.device_id)
        self.assertEqual(new_device_core.dpDeviceType, config.device_type,
            "Device type (%s) does not match device type in configuration (%s)." 
            % (new_device_core.dpDeviceType, config.device_type))
    
    def test_verify_vendor_id_matches(self):
        log.info("Beginning Test to Verify Vendor ID.")
        log.info("Retrieving Vendor ID for %s." % config.device_id)
        new_device_core = config.api.get_first('DeviceCore', 
                            condition="devConnectwareId='%s'" % config.device_id)
        self.assertEqual(new_device_core.dvVendorId, config.vendor_id,
            "Vendor ID (%s) does not match Vendor ID in configuration (%s)."
            % (new_device_core.dvVendorId, config.vendor_id))


        
if __name__ == '__main__':
    # Parse configuration file from prompt
    parser = argparse.ArgumentParser(description='Test for the Connection Control Facility.')
    parser.add_argument('--config_file', dest="config_file", default="config.ini", help='device configuration file')
    args = parser.parse_args()
    
    config = configuration.DeviceConfiguration(args.config_file)
    
    unittest.main(argv=" ")