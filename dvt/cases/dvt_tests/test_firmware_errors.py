import time
import iik_testcase
import xml.dom.minidom

from utils import update_firmware
    
class FirmwareErrorsTestCase(iik_testcase.TestCase):
    """ Performs a series of tests against several different firmware targets
    and verifies that the correct error response is returned.
    """
    
    def test_download_denied(self):
        self.log.info("Beginning Test - Firmware Download Denied.")
        target = 0
        error_message = 'the firmware upgrade deferred by client'
        
        self.firmware_update_with_error(target, error_message)
           
    def test_download_invalid_size(self):
        self.log.info("Beginning Test - Firmware Download Invalid Size.")
        target = 1
        error_message = 'client has insufficient space to hold the download'
        
        self.firmware_update_with_error(target, error_message)
    
    def test_download_invalid_version(self):
        self.log.info("Beginning Test - Firmware Download Invalid Version.")
        target = 2
        error_message = 'incompatible version number detected'
        
        self.firmware_update_with_error(target, error_message)
        
    def test_download_unauthenticated(self):
        self.log.info("Beginning Test - Firmware Download Unauthenticated.")
        target = 3
        error_message = 'client has not authenticated the server'
        
        self.firmware_update_with_error(target, error_message)
    
    def test_download_not_allowed(self):
        self.log.info("Beginning Test - Firmware Download Not Allowed.")
        target = 4
        error_message = 'updates are  rejected by the client'
        
        self.firmware_update_with_error(target, error_message)
        
    def test_download_configured_to_reject(self):
        self.log.info("Beginning Test - Firmware Download Configured to Reject.")
        target = 5
        error_message = 'client does not accept upgrades'
        
        self.firmware_update_with_error(target, error_message)
        
    def test_download_encountered_error(self):
        self.log.info("Beginning Test - Firmware Download Encountered Error.")
        target = 6
        error_message = 'client error allocating space for new image or internal processing error'
        
        self.firmware_update_with_error(target, error_message)
        
    def test_download_user_abort(self):
        self.log.info("Beginning Test - Firmware Download User Abort.")
        target = 7
        error_message = 'Aborted By Target'
        
        self.firmware_update_with_error(target, error_message) 
    
    def test_download_device_error(self):
        self.log.info("Beginning Test - Firmware Download Device Error.")
        target = 8
        error_message = 'Aborted By Target'
        
        self.firmware_update_with_error(target, error_message)
    
    def test_download_invalid_offset(self):
        self.log.info("Beginning Test - Firmware Download Invalid Offset.")
        target = 9
        error_message = 'Aborted By Target'
        
        self.firmware_update_with_error(target, error_message)
    
    def test_download_invalid_data(self):
        self.log.info("Beginning Test - Firmware Download Invalid Data.")
        target = 10
        error_message = 'Aborted By Target'
        
        self.firmware_update_with_error(target, error_message)
   
    def test_download_hardware_error(self):
        self.log.info("Beginning Test - Firmware Download Hardware Error.")
        target = 11
        error_message = 'Aborted By Target'
        
        self.firmware_update_with_error(target, error_message)

    def firmware_update_with_error(self, target, error_message):
        # send request to update firmware
        self.log.info("Sending request to update firmware.")
        response = update_firmware(self.api, self.device_config.device_id, 
            self.device_config.firmware_target_file[target], "%d" % target)
            
        # print response
        self.log.info("response: \n%s" % response)
            
        self.log.info("Determining if correct error response was returned.")
        
        # Parse response to verify an error was returned.
        dom = xml.dom.minidom.parseString(response)
        error_exists = dom.getElementsByTagName("error")
        self.assertTrue(error_exists, "Response is not an error message.")
        
        # Verify that correct error message was returned
        correct_error = response.find(error_message)
        self.assertNotEqual(-1, correct_error,
                "The expected error message (%s) was not returned."
                % error_message)
        time.sleep(4)

if __name__ == '__main__':

    unittest.main()
