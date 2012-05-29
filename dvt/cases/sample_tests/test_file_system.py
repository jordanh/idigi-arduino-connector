import time
import iik_testcase
import datetime
import re
import os
import tempfile
import base64

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

from utils import getText

class SendFileGetTestCase(iik_testcase.TestCase):
       
    def test_file_get(self):
    
        """ Sends file get command. 
        """
        data_length = 4096 
        data = os.urandom(data_length)
        target_data = os.urandom(data_length)
        tmp_file = tempfile.NamedTemporaryFile(delete=False)
        target_file = tempfile.NamedTemporaryFile(delete=False)

        try:
            tmp_file.write(data)
            tmp_file.close()

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
                </sci_request>""" % (self.device_config.device_id, tmp_file.name, target_file.name, base64.encodestring(target_data))

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
                </sci_request>""" % (self.device_config.device_id, '/tmp')

            self.log.info("Beginning Test for file get command.")
            self.log.info("Sending file get command for \"%s\" to server for device id  %s." % (tmp_file.name, self.device_config.device_id))
            self.log.info("Sending file put command for \"%s\" to server for device id  %s." % (target_file.name, self.device_config.device_id))

            # Send device request
            file_get_response = self.api.sci(my_file_getput_request)
            self.log.info("%s" % file_get_response)
            
            # Parse request response 
            dom = xml.dom.minidom.parseString(file_get_response)
            get_data = dom.getElementsByTagName("get_file")

            recv_data =  base64.b64decode(getText(get_data[0].getElementsByTagName("data")[0]))

            infile = open(tmp_file.name, "r")
            in_text = infile.read()
            infile.close()

            out_file = open(target_file.name, "r")
            out_text = out_file.read()
            out_file.close()

            self.log.info("!!! data len: \"%d\, in len: \"%d\", out len: \"%d\"," %  (len(data), len(in_text), len(out_text)))

            self.assertEqual(in_text, recv_data,
                "get file error") 
     
            self.assertEqual(out_text, target_data,
                "put file error") 

            self.log.info("Sending file ls command for \"%s\" to server for device id  %s." % ('/tmp', self.device_config.device_id))
            file_ls_response = self.api.sci(my_file_ls_request)
            self.log.info("%s" % file_ls_response)
        finally:
            os.remove(tmp_file.name)
            os.remove(target_file.name)

if __name__ == '__main__':
    unittest.main() 
