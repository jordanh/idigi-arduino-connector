import time
import iik_testcase
import datetime
import re
import os
import fileinput
import base64
from base64 import b64encode, b64decode

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

from utils import getText

expected_content = "iDigi device request sample [0]\n"


class SendFileGetTestCase(iik_testcase.TestCase):
       
    def test_file_get(self):
    
        """ Sends file get command. 
        """
        my_dir = "/home/digi/irl/iik/public/run/samples/file_system/"
        my_fname = "test"
        my_ext     = ".txt"
        my_outext  = ".out"

        my_file_path = my_dir + my_fname + my_ext
        my_out_path  = my_dir + my_fname + my_outext

        infile = open(my_file_path, "r")
        put_data = base64.encodestring("%s"%infile.read())[:-1]
        infile.close()

        my_file_getput_request = \
            """<sci_request version="1.0">
              <file_system>
                <targets>
                  <device id="%s"/>
                </targets>
                <commands>
                <get_file path="%s" offset = "0" length = "4294967294"/>
                <put_file path="%s" offset = "0" truncate = "true">
                  <data>%s</data>
                </put_file>
                </commands>
              </file_system>
            </sci_request>""" % (self.device_config.device_id, my_file_path, my_out_path, put_data)

        my_file_ls_request = \
            """<sci_request version="1.0">
              <file_system>
                <targets>
                  <device id="%s"/>
                </targets>
                <commands>
                <ls path="%s"/>
                </commands>
              </file_system>
            </sci_request>""" % (self.device_config.device_id, my_dir)

        self.log.info("Beginning Test for file get command.")
        self.log.info("Sending file get command for \"%s\" to server for device id  %s." % (my_file_path, self.device_config.device_id))
        self.log.info("Sending file put command for \"%s\" to server for device id  %s." % (my_out_path, self.device_config.device_id))

        # Send device request
        file_get_response = self.api.sci(my_file_getput_request)
        self.log.info("%s" % file_get_response)
        
        # Parse request response 
        dom = xml.dom.minidom.parseString(file_get_response)
        get_data = dom.getElementsByTagName("get_file")
                
        # Print file data
        self.log.info("Received:")

        data =  b64decode(getText(get_data[0].getElementsByTagName("data")[0]))
        
        #self.log.info("File Data: \"%s\"." % data)

        infile = open(my_file_path, "r")
        in_text = infile.read()
        infile.close()

        out_file = open(my_out_path, "r")
        out_text = out_file.read()
        out_file.close()

        self.log.info("!!! data len: \"%d\, in len: \"%d\", out len: \"%d\"," %  (len(data), len(in_text), len(out_text)))

        self.assertEqual(in_text, data,
            "get file error") 
 
        self.assertEqual(in_text, out_text,
            "put file error") 

        self.log.info("Sending file ls command for \"%s\" to server for device id  %s." % (my_out_path, self.device_config.device_id))
        file_ls_response = self.api.sci(my_file_ls_request)
        self.log.info("%s" % file_ls_response)

if __name__ == '__main__':
    unittest.main() 
