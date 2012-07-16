import xpath
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
from utils import DeviceConnectionMonitor

TERMINATE_TEST_FILE = "terminate.file"
my_test_file = "my_test_file.txt"

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


def getNodeText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc
class StackSizeTestCase(iik_testcase.TestCase):

    def test_stacksize_with_fs(self):
    
        """ Sends file get command. 
        """
        my_dir = tempfile.mkdtemp()
        my_file_path  = my_dir + '/' + my_test_file
        my_ls_path   = my_dir

        file_data = self.get_random_word(2048);
        put_data = base64.encodestring(file_data)[:-1]

        self.log.info("**** Stack Size DVT: File System")
        self.log.info("Sending file put and file get commands for \"%s\" to server for device id  %s." % (my_file_path, self.device_config.device_id))

        request = (FILE_PUT_GET_REQUEST % (self.device_config.device_id, my_file_path, put_data, my_file_path))
        # Send device request
        file_get_response = self.api.sci(request)
        #self.log.info("%s" % file_get_response)
        if file_get_response.find('error id="2107"') != -1:
            self.log.info("Service not available.")
            return
        
        # Parse request response 
        self.log.info(file_get_response)
        dom = xml.dom.minidom.parseString(file_get_response)
        get_data = dom.getElementsByTagName("get_file")

        if len(get_data) == 0:
            self.fail("Response didn't contain \"get_file\" element: %s" % file_get_response)

        data =  b64decode(getText(get_data[0].getElementsByTagName("data")[0]))
        
        # Print file data
        #self.log.info("Received:")
        #self.log.info("File Data: \"%s\"." % data)

        infile = open(my_file_path, "r")
        in_text = infile.read()
        infile.close()

        self.log.info("!!! data len: \"%d\", in len: \"%d\"" %  (len(data), len(in_text)))

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

        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()
            request = (FILE_PUT_GET_REQUEST % (self.device_config.device_id, "terminate.test", put_data, my_file_path))
            # Send device request
            file_get_response = self.api.sci(request)

            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")

            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(30)
            self.log.info("Device connected.")

        finally:
            monitor.stop()        

    def get_random_word(self, wordLen):
        word = ''
        for i in range(wordLen):
            word += random.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789')
        return word
        
if __name__ == '__main__':
    unittest.main()
