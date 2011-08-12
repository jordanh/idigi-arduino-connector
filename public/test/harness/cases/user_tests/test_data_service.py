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
from data_service_utils import update_firmware, update_and_verify, \
                                get_filedatahistory, check_filedatahistory

impl = getDOMImplementation()

log = logging.getLogger('msg_fac')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

filedata = 'FileData/~/'
filedatahistory = 'FileDataHistory/~/'

APPEND = 'Data Service PUT, Append'
ARCHIVE = 'Data Service PUT, Archive'
BOTH = 'Data Service PUT, Both'

config_file = 'config.ini'
config = configuration.DeviceConfiguration(config_file)
    
class DataServiceArchiveTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
       
    def test_archive_true(self):
    
        """ Sends firmware updates to a data service firmware target and 
        verifies that the correct data is pushed to filedata and 
        filedatahistory. Also verifies that all metadata is correct.
        """
        
        log.info("Beginning Data Service Test - Archive = True")
       
        # Check if appropriate target exists
        if ARCHIVE in config.data_service_target:
            target = config.data_service_target[ARCHIVE]['target']
            file_name = config.data_service_target[ARCHIVE]['file_name']
        else:
            log.info("No Archive=True target exists, skipping test")
            return
        
        # Create content to be pushed.
        content = [os.urandom(8), os.urandom(8)]
        encoded_content = [b64encode(content[0]), b64encode(content[1])]
        datetime_created = []

        # Create paths to files.
        file_location = filedata + config.device_id + '/' + file_name
        file_history_location = filedatahistory + config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, config.api, config.device_id, target, 
            content[0], datetime_created, file_location, file_name)
        
        # Verify filedatahistory contents and metadata.
        check_filedatahistory(self, get_filedatahistory(config.api, 
            file_history_location), datetime_created[0], encoded_content[0], 
            0)
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, config.api, config.device_id, target, 
            content[1], datetime_created, file_location, file_name)
        
        fdh = get_filedatahistory(config.api, file_history_location)
        
        # Verify filedatahistory contents and metadata for first file pushed.
        check_filedatahistory(self, fdh, datetime_created[0], 
            encoded_content[0], 1)
        
        # Verify filedatahistory contents and metadata for second file pushed.
        check_filedatahistory(self, fdh, datetime_created[1], 
            encoded_content[1], 0)
        
    def test_archive_true_file_dne(self):
        
        """ Sends firmware updates to a data service firmware target and 
        verifies that the correct data is pushed to filedata and 
        filedatahistory. Also verifies that all metadata is correct.
        
        File does not previously exist before firmware update.
        """
        
        log.info("Beginning Data Service Test - Archive = True, File does not previously exist")
        
        # Check if appropriate target exists
        if ARCHIVE in config.data_service_target:
            target = config.data_service_target[ARCHIVE]['target']
            file_name = config.data_service_target[ARCHIVE]['file_name']
        else:
            log.info("No Archive=True target exists")
            return
        
        # Create content to be pushed
        content = os.urandom(8)
        encoded_content = b64encode(content)
        datetime_created = []
        
        # Create paths to files.
        file_location = filedata + config.device_id + '/' + file_name
        file_history_location = filedatahistory + config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
            
        # If the file exists, delete it.
        clean_slate(config.api, file_location)
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, config.api, config.device_id, target, 
            content, datetime_created, file_location, file_name, dne=True)
        
        # Verify filedatahistory contents and metadata.
        check_filedatahistory(self, get_filedatahistory(config.api, 
            file_history_location), datetime_created[0], encoded_content, 
            0, dne=True)      

    def test_archive_zero_data(self):
        
        """ Sends firmware updates containing no data to a data service 
        firmware target and verifies that the correct data is pushed 
        to filedata and filedatahistory. Also verifies that all metadata 
        is correct.
        """
        
        log.info("Beginning Data Service Test - Archive = True, No Data")
       
        # Check if appropriate target exists
        if ARCHIVE in config.data_service_target:
            target = config.data_service_target[ARCHIVE]['target']
            file_name = config.data_service_target[ARCHIVE]['file_name']
        else:
            log.info("No Archive=True target exists, skipping test")
            return
        
        # Create content to be pushed.
        content = ["", ""]
        encoded_content = content
        datetime_created = []

        # Create paths to files.
        file_location = filedata + config.device_id + '/' + file_name
        file_history_location = filedatahistory + config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, config.api, config.device_id, target, 
            content[0], datetime_created, file_location, file_name)
        
        # Verify filedatahistory contents and metadata.
        check_filedatahistory(self, get_filedatahistory(config.api, 
            file_history_location), datetime_created[0], encoded_content[0], 
            0)
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, config.api, config.device_id, target, 
            content[1], datetime_created, file_location, file_name)
        
        fdh = get_filedatahistory(config.api, file_history_location)
        
        # Verify filedatahistory contents and metadata for first file pushed.
        check_filedatahistory(self, fdh, datetime_created[0], 
            encoded_content[0], 1)
        
        # Verify filedatahistory contents and metadata for second file pushed.
        check_filedatahistory(self, fdh, datetime_created[1], 
            encoded_content[1], 0)

    def tearDown(self):
        log.info("Performing cleanup.")
        config.api.delete_location(filedata+config.data_service_target[ARCHIVE]['file_name'])
    
    
class DataServiceAppendTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
                
    def test_append_true(self):
    
        """ Sends firmware updates to a data service firmware target and 
        verifies that the correct data is pushed to filedata and correctly 
        appended. Also verifies that all metadata is correct.
        """
        
        log.info("Beginning Data Service Test - Append = True")
        
        # Check if appropriate target exists.
        if APPEND in config.data_service_target:
            target = config.data_service_target[APPEND]['target']
            file_name = config.data_service_target[APPEND]['file_name']
        else:
            log.info("No Append=True target exists")
            return
        
        # Create content to be pushed.
        content = [os.urandom(8), os.urandom(8)]
        datetime_created = []
        
        # Create path to files.
        file_location = filedata + config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
        
        # Check for existing file and its contents.
        try:
            original_content = config.api.get_raw(file_location)
        except Exception:
            original_content = ""
            log.info("No file previously exists.")
        
        # Send firmware update and verify correct content is pushed and
        # appended to origiinal content.
        first_content = update_and_verify(self, config.api, config.device_id, 
            target, content[0], datetime_created, file_location, file_name, 
            expected_content=original_content+content[0])
        
        # Send firmware update and verify correct content is pushed and
        # appended to the contents found after the first firmware update.
        update_and_verify(self, config.api, config.device_id, target, 
            content[1], datetime_created, file_location, file_name, 
            expected_content=first_content+content[1])
 
    def test_append_true_file_dne(self):
    
        """ Sends firmware updates to a data service firmware target and 
        verifies that the correct data is pushed to filedata and correctly 
        appended. Also verifies that all metadata is correct.
        
        File does not previously exist before firmware update.
        """
        
        log.info("Beginning Data Service Test - Append = True, File does not previously exist")
        
        # Check if appropriate firmware target exists.
        if APPEND in config.data_service_target:
            target = config.data_service_target[APPEND]['target']
            file_name = config.data_service_target[APPEND]['file_name']
        else:
            log.info("No Append=True target exists")
            return
        
        # Create content to be pushed.
        content = [os.urandom(8), os.urandom(8)]
        datetime_created = []
        
        # Create file paths.
        file_location = filedata + config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
            
        # Check that file does not already exist.
        clean_slate(config.api, file_location)
        
        # Send firmware update and verify that correct content is pushed.
        update_and_verify(self, config.api, config.device_id, target, 
            content[0], datetime_created, file_location, file_name, dne=True)
        
        # Send firmware update and verify that correct content is pushed
        # and appended to first content pushed.
        update_and_verify(self, config.api, config.device_id, target, 
            content[1], datetime_created, file_location, file_name, 
            expected_content=content[0]+content[1], 
            original_created_time=datetime_created[0])
          
    def test_append_zero_data(self):

        """ Sends firmware updates containgin no data to a data service 
        firmware target and verifies that the correct data is pushed to 
        filedata and correctly appended. Also verifies that all metadata 
        is correct.
        """
        
        log.info("Beginning Data Service Test - Append = True, No Data")
        
        # Check if appropriate target exists.
        if APPEND in config.data_service_target:
            target = config.data_service_target[APPEND]['target']
            file_name = config.data_service_target[APPEND]['file_name']
        else:
            log.info("No Append=True target exists")
            return
        
        # Create content to be pushed.
        content = ["", ""]
        datetime_created = []
        
        # Create path to files.
        file_location = filedata + config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
        
        # Check for existing file and its contents.
        try:
            original_content = config.api.get_raw(file_location)
        except Exception:
            original_content = ""
            log.info("No file previously exists.")
        
        # Send firmware update and verify correct content is pushed and
        # appended to origiinal content.
        first_content = update_and_verify(self, config.api, config.device_id,
            target, content[0], datetime_created, file_location, file_name, 
            expected_content=original_content+content[0])
        
        # Send firmware update and verify correct content is pushed and
        # appended to the contents found after the first firmware update.
        update_and_verify(self, config.api, config.device_id, target, 
            content[1], datetime_created, file_location, file_name, 
            expected_content=first_content+content[1])

    def tearDown(self):
        log.info("Performing cleanup.")
        config.api.delete_location(filedata+config.data_service_target[APPEND]['file_name'])

class DataServiceBothTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
                
    def test_both_true(self):
    
        """ Sends a firmware update to a data service firmware target which
        pushes data with Both Append and Archive set to true. Test verifies
        that no file is created.
        """
        
        log.info("Beginning Data Service Test - Both = True")
        
        # Check if appropriate firmware target exists.
        if BOTH in config.data_service_target:
            target = config.data_service_target[BOTH]['target']
            file_name = config.data_service_target[BOTH]['file_name']
        else:
            log.info("No Both=True target exists")
            return
        
        content = os.urandom(8)
        
        # Create file path.
        file_location = filedata + config.device_id + '/' + file_name
                
        # Check that file does not already exist.
        clean_slate(config.api, file_location)
        
        # Send first firmware update to Both=True target
        log.info("Sending first firmware update to create file.")
        datetime_created = []
        datetime_created.append(datetime.datetime.utcnow())
        response = update_firmware(config.api, config.device_id, "%d" % 
            target, content)
        
        # Verify correct response to request (may not have one)
        
        # Check that file was not created
        time.sleep(1)
        self.assertRaises(Exception, config.api.get_raw, file_location)
    
class ContentTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
    
if __name__ == '__main__':

    config = configuration.DeviceConfiguration(config_file)
        
    unittest.main() 
