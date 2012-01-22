import time
import iik_testcase
import datetime
import re
import os
from base64 import b64encode
from xml.dom.minidom import getDOMImplementation

from utils import clean_slate

impl = getDOMImplementation()

filedata = 'FileData/~/'
expected_content = "iDigi data service sample [0]\n"
    
class SendDataTestCase(iik_testcase.TestCase):
    
    def test_send_data(self):
    
        """ This routine gets the sent file from the cloud and   
        verifies that the correct data is pushed """
        
        self.log.info("Begin Send data test.")
       
        # Create paths to files.
        file_name = 'test/test.txt'
        file_location = filedata + self.device_config.device_id + '/' + file_name
        self.log.info("Attempting to Retrieve data from File Location: %s" % file_location)
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]

        # Check for existing file and its contents.
        try:
            file_content = self.api.get_raw(file_location)
        except Exception, e:
            self.failureException = e
            self.fail('Failed to Get Device Contents.  Reason: %s' % e)
        
        # Verify file's contents        
        self.assertEqual(expected_content, file_content, 
            "File's contents do not match Expected: %s, Got: %s" %(expected_content, file_content))

    def tearDown(self):
            self.log.info("Performing cleanup.")
            file_name = 'test/test.txt'
            file_location = filedata + self.device_config.device_id + '/' + file_name
            self.api.delete_location(file_location)

if __name__ == '__main__':
    unittest.main() 
