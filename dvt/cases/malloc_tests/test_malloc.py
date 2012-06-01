import time
import iik_testcase
import datetime
import re
import os
import fileinput
import random
import base64
import tempfile

from base64 import b64encode, b64decode, encodestring

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

import idigi_ws_api
from utils import getText

my_test_file = "my_test_file.txt"
fw_target_num = 0


FIRMWARE_QUERY_REQUEST = \
"""<sci_request version="1.0">
  <query_firmware_targets>
    <targets>
      <device id="%s"/>
    </targets>
  </query_firmware_targets>
</sci_request>"""
    
FIRMWARE_DATA_REQUEST= \
"""<sci_request version="1.1">
    <update_firmware firmware_target="%d">
        <targets>
            <device id="%s"/>
        </targets>
        <data>%s</data>
    </update_firmware>
</sci_request>"""

FILE_PUT_GET_REQUEST = \
"""<sci_request version="1.0">
  <file_system>
    <targets>
      <device id="%s"/>
    </targets>
    <commands>
    <put_file path="%s" offset = "0" truncate = "true">
      <data>%s</data>
    </put_file>
    <get_file path="%s" offset = "0" length = "4294967294"/>
    </commands>
  </file_system>
</sci_request>""" 

FILE_LS_REQUEST = \
"""<sci_request version="1.0">
  <file_system>
    <targets>
      <device id="%s"/>
    </targets>
    <commands>
    <ls path="%s" hash="any"/>
    </commands>
  </file_system>
</sci_request>"""

FILE_RM_REQUEST = \
"""<sci_request version="1.0">
  <file_system>
    <targets>
      <device id="%s"/>
    </targets>
    <commands>
    <rm path="%s"/>
    </commands>
  </file_system>
</sci_request>"""

TARGET_DEVICE_REQUEST = \
"""<sci_request version="1.0">
  <data_service>
    <targets>
      <device id="%s"/>
    </targets>
    <requests>
    <device_request target_name="%s">%s</device_request>
    </requests>
  </data_service>
</sci_request>"""

class MallocTestCase(iik_testcase.TestCase):

    def test_malloc_fw(self):
    
        """ Sends a firmware update to teminate iik """
    
        self.log.info("**** Malloc Test: Firmware Service")

        # Send firmware target query
        self.log.info("Sending firmware target query to %s." % self.device_config.device_id)
               
        # Find firmware targets
        firmware_targets_xml = self.api.sci(FIRMWARE_QUERY_REQUEST % 
            self.device_config.device_id)
        #self.log.info("TARGETS %s." % firmware_targets_xml)
  
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")

        if len(targets) == 0:
            self.log.info("Service not available")
            return
    
        # Encode firmware for transmittal
        self.log.info("Target %d %s." % (fw_target_num, self.device_config.firmware_target_file[0]))
 
        f = open(self.device_config.firmware_target_file[fw_target_num], 'rb')
        data = f.read()
        data_value = encodestring(data)
        f.close()

        # Send update request
        request = (FIRMWARE_DATA_REQUEST % (fw_target_num, self.device_config.device_id, data_value))

        self.log.info("Sending request to update firmware.")
        response = self.api.sci(request)
        self.log.info("Response to:\n%s" % response)
 
    def test_malloc_fs(self):
    
        """ Sends file get command. 
        """
        my_dir = tempfile.mkdtemp()
        my_file_path  = my_dir + '/' + my_test_file
        my_ls_path   = my_dir

        file_data = self.get_random_word(1000);
        put_data = base64.encodestring(file_data)[:-1]

        self.log.info("**** Malloc Test: File System")
        self.log.info("Sending file put and file get commands for \"%s\" to server for device id  %s." % (my_file_path, self.device_config.device_id))

        request = (FILE_PUT_GET_REQUEST % (self.device_config.device_id, my_file_path, put_data, my_file_path))
        # Send device request
        file_get_response = self.api.sci(request)
        #self.log.info("%s" % file_get_response)
        if file_get_response.find('error id="2107"') != -1:
            self.log.info("Service not available.")
            return
        
        # Parse request response 
        dom = xml.dom.minidom.parseString(file_get_response)
        get_data = dom.getElementsByTagName("get_file")

        data =  b64decode(getText(get_data[0].getElementsByTagName("data")[0]))
        
        # Print file data
        #self.log.info("Received:")
        #self.log.info("File Data: \"%s\"." % data)

        infile = open(my_file_path, "r")
        in_text = infile.read()
        infile.close()

        self.log.info("!!! data len: \"%d\, in len: \"%d\"" %  (len(data), len(in_text)))

        self.assertEqual(in_text, data,
            "get file error") 
        
        request = (FILE_LS_REQUEST % (self.device_config.device_id, my_ls_path))

        self.log.info("Sending file ls command for \"%s\" to server for device id  %s." % (my_ls_path, self.device_config.device_id))
        file_ls_response = self.api.sci(request)
        self.log.info("%s" % file_ls_response)

        request = (FILE_RM_REQUEST % (self.device_config.device_id, my_file_path))
        self.log.info("Sending file rm command for \"%s\" to server for device id  %s." % (my_file_path, self.device_config.device_id))
        file_rm_response = self.api.sci(request)

        os.removedirs(my_dir)


    def test_malloc_ds(self):
    
        self.log.info("**** Malloc Test: Data Service")
        ok = self.invalid_target("invalid_target", 1600)
        if ok == "no_service":
            return
        self.valid_target("data_target", 1024)
        self.valid_target("malloc_target", 100)
        
        
    def invalid_target(self, my_target_name, data_len):
    
        """ Verifies that the device request returned match the expected 
        response.
        """
 
        # Send device request
        device_request_response = self.send_device_request(my_target_name, data_len)
        self.log.info("RESPONSE %s" %device_request_response)
        if device_request_response.find('error id="2108"') != -1:
            self.log.info("Service not available.")
            return "no_service"

        # Parse request response 
        dom = xml.dom.minidom.parseString(device_request_response)
        device_response = dom.getElementsByTagName("device_request")
        
        # Validate cancel message response
        self.log.info("Determining if \"%s\" target is cancelled." % my_target_name)
        error = getText(device_response[0].getElementsByTagName('error')[0])

        cancelled_response = "Message transmission cancelled"

        self.assertEqual(error, cancelled_response,
            "returned target (%s) is not (%s)" 
            % (error, cancelled_response))
        return "ok"

    def valid_target(self, my_target_name, data_len):
    
        """ Verifies that the device request returned match the expected 
        response.
        """
        #if DS_SERVICE == 0:
        #    return

        device_request_response = self.send_device_request(my_target_name, data_len)
        #self.log.info("RESPONSE: %s " % device_request_response)
        if device_request_response.find('error id="2108"') != -1:
            self.log.info("Service not available.")
            return "no_service"

        # Parse request response 
        dom = xml.dom.minidom.parseString(device_request_response)
        device_response = dom.getElementsByTagName("device_request")
        
        # Validate target name
        self.log.info("Determining if the target_name is \"%s\"." % my_target_name)
        target_name = device_response[0].getAttribute('target_name')
        self.log.info("Target: %s " % target_name)
        self.assertEqual(target_name, my_target_name, 
            "returned target (%s) is not (%s)" 
            % (target_name, my_target_name))

        # Validate status 
        self.log.info("Determining if status is success.")
        status = device_response[0].getAttribute("status")
        self.assertEqual(status, '0', 
            "returned status (%s) is not success status" 
            % status)
        return "ok"

    def send_device_request(self, target_name, request_length):
        
        request_data = self.get_random_word(request_length);

        request = (TARGET_DEVICE_REQUEST % (self.device_config.device_id, target_name, request_data))
 
        self.log.info("Sending device request for \"%s\" target_name to server for device id  %s." % (target_name, self.device_config.device_id))
        device_request_response = self.api.sci(request)

        return device_request_response
    
    def get_random_word(self, wordLen):
        word = ''
        for i in range(wordLen):
            word += random.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789')
        return word

if __name__ == '__main__':
    unittest.main()
