import logging
import time
import unittest
import datetime
import re
import os
from base64 import b64encode
from xml.dom.minidom import getDOMImplementation

import configuration
import idigi_ws_api
from utils import clean_slate

impl = getDOMImplementation()

log = logging.getLogger('send_data')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

filedata = 'FileData/~/'
expected_content = "iDigi data service sample [0]\n"

config_file = 'config.ini'
config = configuration.DeviceConfiguration(config_file)
    
class SendDataTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
       
    def test_send_data(self):
    
        """ This routine gets the sent file from the cloud and   
        verifies that the correct data is pushed """
        
        log.info("Begin Send data test.")
       
        # Create paths to files.
        file_name = 'test/test.txt'
        file_location = filedata + config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]

        # Check for existing file and its contents.
        try:
            file_content = config.api.get_raw(file_location)
        except Exception:
            file_content = ''
            log.info("File %s doesn't exist" %file_name)
        
        # Verify file's contents        
        self.assertEqual(expected_content, file_content, 
            "File's contents do not match what is expected")
        #if not expected_content == file_content: 
        #    log.info("File's contents do not match what is expected")

    def tearDown(self):
            log.info("Performing cleanup.")
            file_name = 'test/test.txt'
            file_location = filedata + config.device_id + '/' + file_name
            config.api.delete_location(file_location)

if __name__ == '__main__':
    config = configuration.DeviceConfiguration(config_file)
        
    unittest.main() 
