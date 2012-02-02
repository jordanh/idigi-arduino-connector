import time
import iik_testcase

from base64 import encodestring

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

import idigi_ws_api
from utils import clean_slate, DeviceConnectionMonitor, getText
from data_service_utils import update_firmware, get_and_verify

filedata = 'FileData/~/'
filedatapush = 'FileData/~%2F'

TERMINATE_TEST_FILE = "terminate_test.txt"

def send_device_request(self, target_name, target_content):

    """ Verifies that the device request returned match the expected 
    response.
    """
    my_target_device_request = \
        """<sci_request version="1.0">
          <data_service>
            <targets>
              <device id="%s"/>
            </targets>
            <requests>
            <device_request target_name="%s">%s</device_request>
            </requests>
          </data_service>
        </sci_request>""" % (self.device_config.device_id, target_name, target_content)

    self.log.info("Sending \"%s\" device request" % target_name)
    
    # Send device request
    device_request_response = self.api.sci(my_target_device_request)
        
    # Parse request response 
    dom = xml.dom.minidom.parseString(device_request_response)
    device_response = dom.getElementsByTagName("device_request")
    
    # Validate target name
    my_target_name = device_response[0].getAttribute('target_name')
#    if my_target_name != target_name:
#        self.log.info("Response: %s" % device_request_response)

    self.assertEqual(my_target_name, target_name, 
                    "returned target (%s) is not (%s)" 
                    % (my_target_name, target_name))

    # Validate status 
    self.log.info("Determining if status is success (0).")
    status = device_response[0].getAttribute("status")
    self.log.info("status = \"%s\" device request" % status)
    if status == '0':
        return 0
    else:
        self.log.info("Error \"%s\" device request" % target_name)
        return -1

def delete_file(self, file_name):

    # Create paths to delete the file.
    file_location = filedata + self.device_config.device_id + '/' + file_name

    self.log.info("delete file %s" % file_location)
    # If the file exists, delete it.
    clean_slate(self.api, file_location)


class TerminateTestCase(iik_testcase.TestCase):
       
    def test_terminate_in_application(self):
    
        """ Sends device request to application to call idigi_initiate_terminate
        """
        
        self.log.info("***** Terminate in application thread *****")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()

            # delete the file that sample is going to send for
            # the reboot result.
            delete_file(self, TERMINATE_TEST_FILE)

            # Send device request to request terminate the sample
            status = send_device_request(self, 'request_terminate_in_application', '')
            self.assertTrue(status == 0, "Unable to send device request")
                      
            # Wait for terminate result file.
            # Create content that are expected from the reboot result file.
            expected_content = "terminate_ok"
            datetime_created = []

            # Create path to file for push.
            file_push_location = filedatapush + self.device_config.device_id + '/' + TERMINATE_TEST_FILE
        
            # get and verify correct content is pushed.
            get_and_verify(self, self.api, self.device_config.device_id, 
                           datetime_created, file_push_location, expected_content)
        
        finally:
            monitor.stop()        
        
    def test_terminate_in_put_request(self):
    
        """ Sends device request to application to call idigi_initiate_terminate
        """
        
        self.log.info("***** Terminate in callback *****")
        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()

            # delete the file that sample is going to send for
            # the reboot result.
            delete_file(self, TERMINATE_TEST_FILE)

            # Send device request to request terminate the sample
            status = send_device_request(self, 'request_terminate_in_callback', '')
            self.assertTrue(status == 0, "Unable to send device request")
                       
            # Wait for terminate result file.
            # Create content that are expected from the reboot result file.
            expected_content = "terminate_ok"
            datetime_created = []

            # Create path to file for push.
            file_push_location = filedatapush + self.device_config.device_id + '/' + TERMINATE_TEST_FILE
        
            # get and verify correct content is pushed.
            get_and_verify(self, self.api, self.device_config.device_id, 
                           datetime_created, file_push_location, expected_content)
        
        finally:
            monitor.stop()        

 
if __name__ == '__main__':
    unittest.main()
