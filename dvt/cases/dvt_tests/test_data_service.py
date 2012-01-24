import time
import iik_testcase
import datetime
import re
import os
from base64 import b64encode
from xml.dom.minidom import getDOMImplementation

import idigi_ws_api
from utils import clean_slate
from data_service_utils import update_firmware, update_and_verify, \
                                get_filedatahistory, check_filedatahistory

filedata = 'FileData/~/'
filedatapush = 'FileData/~%2F'
filedatahistory = 'FileDataHistory/~/'

APPEND = 'Data Service PUT, Append'
ARCHIVE = 'Data Service PUT, Archive'
BOTH = 'Data Service PUT, Both'
BUSY = 'Data Service PUT, Busy'
CANCEL_AT_START = 'Data Service PUT, Cancel at start'
CANCEL_IN_MIDDLE = 'Data Service PUT, Cancel in middle'
TIMEOUT = 'Data Service PUT, Timeout'
    
class DataServiceArchiveTestCase(iik_testcase.TestCase):
       
    def test_archive_true(self):
    
        """ Sends firmware updates to a data service firmware target and 
        verifies that the correct data is pushed to filedata and 
        filedatahistory. Also verifies that all metadata is correct.
        """
        
        self.log.info("Beginning Data Service Test - Archive = True")
       
        # Check if appropriate target exists
        if ARCHIVE in self.device_config.data_service_target:
            target = self.device_config.data_service_target[ARCHIVE]['target']
            file_name = self.device_config.data_service_target[ARCHIVE]['file_name']
        else:
            self.log.info("No Archive=True target exists, skipping test")
            return
        
        # Create content to be pushed.
        content = [os.urandom(8), os.urandom(8)]
        datetime_created = []

        # Create paths to files.
        file_push_location = filedatapush + self.device_config.device_id + '/' + file_name
        file_location = filedata + self.device_config.device_id + '/' + file_name
        file_history_location = filedatahistory + self.device_config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, self.api, self.device_config.device_id, target, 
            content[0], datetime_created, file_push_location, file_name)
        
        # Verify filedatahistory contents and metadata.
        check_filedatahistory(self, get_filedatahistory(self.api, 
            file_history_location), datetime_created[0], content[0], 
            0)
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, self.api, self.device_config.device_id, target, 
            content[1], datetime_created, file_push_location, file_name)
        
        fdh = get_filedatahistory(self.api, file_history_location)
        
        # Verify filedatahistory contents and metadata for first file pushed.
        check_filedatahistory(self, fdh, datetime_created[0], 
            content[0], 1)
        
        # Verify filedatahistory contents and metadata for second file pushed.
        check_filedatahistory(self, fdh, datetime_created[1], 
            content[1], 0)
        
    def test_archive_true_file_dne(self):
        
        """ Sends firmware updates to a data service firmware target and 
        verifies that the correct data is pushed to filedata and 
        filedatahistory. Also verifies that all metadata is correct.
        
        File does not previously exist before firmware update.
        """
        
        self.log.info("Beginning Data Service Test - Archive = True, File does not previously exist")
        
        # Check if appropriate target exists
        if ARCHIVE in self.device_config.data_service_target:
            target = self.device_config.data_service_target[ARCHIVE]['target']
            file_name = self.device_config.data_service_target[ARCHIVE]['file_name']
        else:
            self.log.info("No Archive=True target exists")
            return
        
        # Create content to be pushed
        content = os.urandom(8)
        datetime_created = []
        
        # Create paths to files.
        file_push_location = filedatapush + self.device_config.device_id + '/' + file_name
        file_location = filedata + self.device_config.device_id + '/' + file_name
        file_history_location = filedatahistory + self.device_config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]

        # If the file exists, delete it.
        clean_slate(self.api, file_location)
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, self.api, self.device_config.device_id, target, 
            content, datetime_created, file_push_location, file_name, dne=True)
        
        # Verify filedatahistory contents and metadata.
        check_filedatahistory(self, get_filedatahistory(self.api, 
            file_history_location), datetime_created[0], content, 
            0, dne=True)      

    def test_archive_zero_data(self):
        
        """ Sends firmware updates containing no data to a data service 
        firmware target and verifies that the correct data is pushed 
        to filedata and filedatahistory. Also verifies that all metadata 
        is correct.
        """
        
        self.log.info("Beginning Data Service Test - Archive = True, No Data")
       
        # Check if appropriate target exists
        if ARCHIVE in self.device_config.data_service_target:
            target = self.device_config.data_service_target[ARCHIVE]['target']
            file_name = self.device_config.data_service_target[ARCHIVE]['file_name']
        else:
            self.log.info("No Archive=True target exists, skipping test")
            return
        
        # Create content to be pushed.
        content = ["", ""]
        datetime_created = []

        # Create paths to files.
        file_push_location = filedatapush + self.device_config.device_id + '/' + file_name
        file_location = filedata + self.device_config.device_id + '/' + file_name
        file_history_location = filedatahistory + self.device_config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, self.api, self.device_config.device_id, target, 
            content[0], datetime_created, file_push_location, file_name)
        
        # Verify filedatahistory contents and metadata.
        check_filedatahistory(self, get_filedatahistory(self.api, 
            file_history_location), datetime_created[0], content[0], 
            0)
        
        # Send firmware update and verify correct content is pushed.
        update_and_verify(self, self.api, self.device_config.device_id, target, 
            content[1], datetime_created, file_push_location, file_name)
        
        fdh = get_filedatahistory(self.api, file_history_location)
        
        # Verify filedatahistory contents and metadata for first file pushed.
        check_filedatahistory(self, fdh, datetime_created[0], 
            content[0], 1)
        
        # Verify filedatahistory contents and metadata for second file pushed.
        check_filedatahistory(self, fdh, datetime_created[1], 
            content[1], 0)

    def tearDown(self):
        self.log.info("Performing cleanup.")
        self.api.delete_location(filedata+self.device_config.data_service_target[ARCHIVE]['file_name'])
    
    
class DataServiceAppendTestCase(iik_testcase.TestCase):
                
    def test_append_true(self):
    
        """ Sends firmware updates to a data service firmware target and 
        verifies that the correct data is pushed to filedata and correctly 
        appended. Also verifies that all metadata is correct.
        """
        
        self.log.info("Beginning Data Service Test - Append = True")
        
        # Check if appropriate target exists.
        if APPEND in self.device_config.data_service_target:
            target = self.device_config.data_service_target[APPEND]['target']
            file_name = self.device_config.data_service_target[APPEND]['file_name']
        else:
            self.log.info("No Append=True target exists")
            return
        
        # Create content to be pushed.
        content = [os.urandom(8), os.urandom(8)]
        datetime_created = []
        
        # Create path to files.
        file_push_location = filedatapush + self.device_config.device_id + '/' + file_name
        file_location = filedata + self.device_config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
        
        # Check for existing file and its contents.
        try:
            original_content = self.api.get_raw(file_location)
        except Exception:
            original_content = ""
            self.log.info("No file previously exists.")
        
        # Send firmware update and verify correct content is pushed and
        # appended to origiinal content.
        first_content = update_and_verify(self, self.api, self.device_config.device_id, 
            target, content[0], datetime_created, file_push_location, file_name, 
            expected_content=original_content+content[0])
        
        # Send firmware update and verify correct content is pushed and
        # appended to the contents found after the first firmware update.
        update_and_verify(self, self.api, self.device_config.device_id, target, 
            content[1], datetime_created, file_push_location, file_name, 
            expected_content=first_content+content[1])
 
    def test_append_true_file_dne(self):
    
        """ Sends firmware updates to a data service firmware target and 
        verifies that the correct data is pushed to filedata and correctly 
        appended. Also verifies that all metadata is correct.
        
        File does not previously exist before firmware update.
        """
        
        self.log.info("Beginning Data Service Test - Append = True, File does not previously exist")
        
        # Check if appropriate firmware target exists.
        if APPEND in self.device_config.data_service_target:
            target = self.device_config.data_service_target[APPEND]['target']
            file_name = self.device_config.data_service_target[APPEND]['file_name']
        else:
            self.log.info("No Append=True target exists")
            return
        
        # Create content to be pushed.
        content = [os.urandom(8), os.urandom(8)]
        datetime_created = []
        
        # Create file paths.
        file_push_location = filedatapush + self.device_config.device_id + '/' + file_name
        file_location = filedata + self.device_config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
            
        # Check that file does not already exist.
        clean_slate(self.api, file_location)
        
        # Send firmware update and verify that correct content is pushed.
        update_and_verify(self, self.api, self.device_config.device_id, target, 
            content[0], datetime_created, file_push_location, file_name, dne=True)
          
    def test_append_zero_data(self):

        """ Sends firmware updates containgin no data to a data service 
        firmware target and verifies that the correct data is pushed to 
        filedata and correctly appended. Also verifies that all metadata 
        is correct.
        """
        
        self.log.info("Beginning Data Service Test - Append = True, No Data")
        
        # Check if appropriate target exists.
        if APPEND in self.device_config.data_service_target:
            target = self.device_config.data_service_target[APPEND]['target']
            file_name = self.device_config.data_service_target[APPEND]['file_name']
        else:
            self.log.info("No Append=True target exists")
            return
        
        # Create content to be pushed.
        content = ["", ""]
        datetime_created = []
        
        # Create path to files.
        file_push_location = filedatapush + self.device_config.device_id + '/' + file_name
        file_location = filedata + self.device_config.device_id + '/' + file_name
        match = re.match(".*\/(.*)$", file_name)
        if match:
            file_name = match.groups()[0]
        
        # Check for existing file and its contents.
        try:
            original_content = self.api.get_raw(file_location)
        except Exception:
            original_content = ""
            self.log.info("No file previously exists.")
        
        # Send firmware update and verify correct content is pushed and
        # appended to origiinal content.
        first_content = update_and_verify(self, self.api, self.device_config.device_id,
            target, content[0], datetime_created, file_push_location, file_name, 
            expected_content=original_content+content[0])
        
        # Send firmware update and verify correct content is pushed and
        # appended to the contents found after the first firmware update.
        update_and_verify(self, self.api, self.device_config.device_id, target, 
            content[1], datetime_created, file_push_location, file_name, 
            expected_content=first_content+content[1])

    def tearDown(self):
        self.log.info("Performing cleanup.")
        self.api.delete_location(filedata+self.device_config.data_service_target[APPEND]['file_name'])

class DataServiceBothTestCase(iik_testcase.TestCase):
                
    def test_both_true(self):
    
        """ Sends a firmware update to a data service firmware target which
        pushes data with Both Append and Archive set to true. Test verifies
        that no file is created.
        """
        
        self.log.info("Beginning Data Service Test - Both = True")
        
        # Check if appropriate firmware target exists.
        if BOTH in self.device_config.data_service_target:
            target = self.device_config.data_service_target[BOTH]['target']
            file_name = self.device_config.data_service_target[BOTH]['file_name']
        else:
            self.log.info("No Both=True target exists")
            return
        
        content = os.urandom(8)
        
        # Create file path.
        file_location = filedata + self.device_config.device_id + '/' + file_name
                
        # Check that file does not already exist.
        clean_slate(self.api, file_location)
        
        # Send first firmware update to Both=True target
        self.log.info("Sending first firmware update to create file.")
        datetime_created = []
        datetime_created.append(datetime.datetime.utcnow())
        response = update_firmware(self.api, self.device_config.device_id, "%d" % 
            target, content)
        
        # Verify correct response to request (may not have one)
        
        # Check that file was not created
        time.sleep(8)
        self.assertRaises(Exception, self.api.get_raw, file_location)

def send_and_receive_target_data(instance, target_str, wait_time, file_expected):

    target = instance.device_config.data_service_target[target_str]['target']
    file_name = instance.device_config.data_service_target[target_str]['file_name']

    f = open(instance.device_config.firmware_target_file[target], 'rb')
    file_content = f.read()
    f.close()

    # Create file path.
    file_location = filedata + instance.device_config.device_id + '/' + file_name
    # Check that file does not already exist.
    clean_slate(instance.api, file_location)

    # Send first firmware update to Both=True target
    instance.log.info("Sending first firmware update to create file.")
    datetime_created = []
    datetime_created.append(datetime.datetime.utcnow())
    response = update_firmware(instance.api, instance.device_config.device_id, "%d" %target, file_content)

    # Verify correct response to request (may not have one)

    time.sleep(wait_time)

    if file_expected:
        expected_content = instance.api.get_raw(file_location)
        instance.assertEqual(expected_content, file_content,
                             "File's contents do not match what is expected")
        instance.api.delete_location(file_location)
    else:
        # Check that file was not created
        instance.assertRaises(Exception, instance.api.get_raw, file_location)

class DataServiceSpecialTestCase(iik_testcase.TestCase):

    def test_ds_busy(self):

        """ Sends a firmware update to a data service firmware target which
            pushes data when client app is temporarily busy. Test verifies
            that the file is created. """

        self.log.info("Beginning Data Service Test - Busy")
        send_and_receive_target_data(self, BUSY, 10, True)


    def test_ds_cancel_at_start(self):

        """ Sends a firmware update to a data service firmware target which
            pushes data and client app cancels the transaction when data 
            transfer starts. Test verifies that the file is not created. """

        self.log.info("Beginning Data Service Test - Cancel at start")
        send_and_receive_target_data(self, CANCEL_AT_START, 10, False)


    def test_ds_cancel_in_middle(self):

        """ Sends a firmware update to a data service firmware target which
            pushes data and client app cancels the transaction in the middle
            of data transfer. Test verifies that the file is not created. """

        self.log.info("Beginning Data Service Test - Cancel in middle")
        send_and_receive_target_data(self, CANCEL_IN_MIDDLE, 10, False)

    def no_ds_timeout(self):

       """ Sends a firmware update to a data service firmware target which
           pushes data and client app times out in the middle of the data
           transfer. Test verifies that the file is not created. """

       self.log.info("Beginning Data Service Test - Timeout")
       send_and_receive_target_data(self, TIMEOUT, 70, False)
    
if __name__ == '__main__':
    unittest.main() 
