import logging
import time
import unittest
import xml.dom.minidom

import idigi_ws_api
import configuration
from utils import update_firmware

config = configuration.DeviceConfiguration('config.ini')

log = logging.getLogger('test_firmware_errors')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

    
def firmware_update_with_error(instance, config, target, error_message):
    # send request to update firmware
    log.info("Sending request to update firmware.")
    response = update_firmware(config.api, config.device_id, 
        config.firmware_target_file[target], "%d" % target)
        
    # print response
    log.info("response: \n%s" % response)
        
    log.info("Determining if correct error response was returned.")
    
    # Parse response to verify an error was returned.
    dom = xml.dom.minidom.parseString(response)
    error_exists = dom.getElementsByTagName("error")
    instance.assertTrue(error_exists, "Response is not an error message.")
    
    # Verify that correct error message was returned
    correct_error = response.find(error_message)
    instance.assertNotEqual(-1, correct_error,
            "The expected error message (%s) was not returned."
            % error_message)
    time.sleep(4)
    
class FirmwareErrorsTestCase(unittest.TestCase):

    """ Performs a series of tests against several different firmware targets
    and verifies that the correct error response is returned.
    """

    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
    
    def test_download_denied(self):
        log.info("Beginning Test - Firmware Download Denied.")
        target = 5
        error_message = 'the firmware upgrade deferred by client'
        
        firmware_update_with_error(self, config, target, error_message)
           
    def test_download_invalid_size(self):
        log.info("Beginning Test - Firmware Download Invalid Size.")
        target = 6
        error_message = 'client has insufficient space to hold the download'
        
        firmware_update_with_error(self, config, target, error_message)
    
    def test_download_invalid_version(self):
        log.info("Beginning Test - Firmware Download Invalid Version.")
        target = 7
        error_message = 'incompatible version number detected'
        
        firmware_update_with_error(self, config, target, error_message)
        
    def test_download_unauthenticated(self):
        log.info("Beginning Test - Firmware Download Unauthenticated.")
        target = 8
        error_message = 'client has not authenticated the server'
        
        firmware_update_with_error(self, config, target, error_message)
    
    def test_download_not_allowed(self):
        log.info("Beginning Test - Firmware Download Not Allowed.")
        target = 9
        error_message = 'updates are  rejected by the client'
        
        firmware_update_with_error(self, config, target, error_message)
        
    def test_download_configured_to_reject(self):
        log.info("Beginning Test - Firmware Download Configured to Reject.")
        target = 10
        error_message = 'client does not accept upgrades'
        
        firmware_update_with_error(self, config, target, error_message)
        
    def test_download_encountered_error(self):
        log.info("Beginning Test - Firmware Download Encountered Error.")
        target = 11
        error_message = 'client error allocating space for new image or internal processing error'
        
        firmware_update_with_error(self, config, target, error_message)
        
    def test_download_user_abort(self):
        log.info("Beginning Test - Firmware Download User Abort.")
        target = 12
        error_message = 'Aborted By Target'
        
        firmware_update_with_error(self, config, target, error_message) 
    
    def test_download_device_error(self):
        log.info("Beginning Test - Firmware Download Device Error.")
        target = 13
        error_message = 'Aborted By Target'
        
        firmware_update_with_error(self, config, target, error_message)
    
    def test_download_invalid_offset(self):
        log.info("Beginning Test - Firmware Download Invalid Offset.")
        target = 14
        error_message = 'Aborted By Target'
        
        firmware_update_with_error(self, config, target, error_message)
    
    def test_download_invalid_data(self):
        log.info("Beginning Test - Firmware Download Invalid Data.")
        target = 15
        error_message = 'Aborted By Target'
        
        firmware_update_with_error(self, config, target, error_message)
   
    def test_download_hardware_error(self):
        log.info("Beginning Test - Firmware Download Hardware Error.")
        target = 16
        error_message = 'Aborted By Target'
        
        firmware_update_with_error(self, config, target, error_message)

if __name__ == '__main__':

    unittest.main()
