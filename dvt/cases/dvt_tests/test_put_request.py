import time
import iik_testcase
import datetime
import re
import os
from base64 import b64encode
from data_service_utils import get_filedatahistory, check_filedatahistory
import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

# from utils import clean_slate
from utils import getText, clean_slate

(ds_default, ds_append, ds_archive, ds_both, ds_zero, ds_busy, ds_cancel_start, ds_cancel_middle, ds_timeout) = range(0,9)

default_timeout = 20
test_file_name = './dvt/cases/test_files/pattern.txt'
#test_file_name = '../test_files/pattern.txt'

impl = getDOMImplementation()

def verify_the_file_on_server (instance, test_num, file_path, file_expected):

   instance.log.info("Determining the file and content of %s." %file_path)
   if file_expected:
      if test_num == ds_zero:
         expected_content = ""
      else:
         f = open(test_file_name, 'rb')
         expected_content = f.read()
         f.close()

      if test_num == ds_archive:
         datetime_created = datetime.datetime.utcnow()
         fdh = get_filedatahistory(instance.api, file_path)
         check_filedatahistory(instance, fdh, datetime_created, expected_content, 0);
      else:
         file_content = instance.api.get_raw(file_path)
         instance.assertEqual(expected_content, file_content,
                       "File's contents do not match what is expected")
         instance.api.delete_location(file_path)
   else:
      # Check that file was not created
      instance.assertRaises(Exception, instance.api.get_raw, file_path)

def send_device_put_request (instance, target, file_name):

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
       </sci_request>""" % (instance.device_config.device_id, target, file_name)

   instance.log.info("Sending request to %s to get %s" %(target, file_name))

   # Send device request
   device_request_response = instance.api.sci(my_target_device_request)

   # Parse request response 
   dom = xml.dom.minidom.parseString(device_request_response)
   device_response = dom.getElementsByTagName("device_request")

   # Validate target name
   instance.log.info("Determining if the target_name is \"%s\"." %target)
   target_name = device_response[0].getAttribute('target_name')
   instance.assertEqual(target_name, target, "returned target (%s) is not (%s)" %(target_name, target))

   # Validate status
   instance.log.info("Determining if status is success.")
   status = device_response[0].getAttribute("status")
   instance.assertEqual(status, '0', "returned error status(%s)" %status)

   # Validate response data
   if target == 'zero byte data':
       if device_response[0].firstChild:
           self.assertEqual(device_response[0].firstChild, False, 'Data is not expected for zero byte data')
       else:
           result = ""
   else:
       result = device_response[0].firstChild.data

   print result

   return result

class PutRequestDvtTestCase(iik_testcase.TestCase):
    
    def test_ds_put_request_default(self):    
        """ Put request default test """
        self.process_put_request(ds_default, default_timeout, True)

    def test_ds_put_request_append(self):
        """ Put request append test """
        self.process_put_request(ds_append, default_timeout, True)

    def test_ds_put_request_archive(self):
        """ Put request archive test """
        self.process_put_request(ds_archive, default_timeout, True)

    def test_ds_put_request_both(self):
        """ Put request both append and archive test """
        self.process_put_request(ds_both, default_timeout, False)

    def test_ds_put_request_zero_byte(self):
        """ Put request zero byte test """
        self.process_put_request(ds_zero, default_timeout, True)

    def test_ds_put_request_busy(self):
        """ Put request busy test """
        self.process_put_request(ds_busy, default_timeout, True)

    def test_ds_put_request_cancel_at_start(self):
        """ Put request cancel at start test """
        self.process_put_request(ds_cancel_start, default_timeout, False)

    def test_ds_put_request_cancel_in_middle(self):
        """ Put request cancel in middle test """
        self.process_put_request(ds_cancel_middle, default_timeout, False)

    def test_ds_put_request_timeout(self):
        """ Put request timeout test """
        self.process_put_request(ds_timeout, 80, False)

    def process_put_request (self, test_num, timeout, file_expected):

        file_name = 'test/dvt_ds' + str(test_num) + '.txt'
        if test_num == ds_archive:
            file_location = 'FileDataHistory/~/'
        else:
            file_location = 'FileData/~/'

        file_location = file_location + self.device_config.device_id + '/' + file_name

        # Check that file does not already exist.
        clean_slate(self.api, file_location)

        result = send_device_put_request(self, 'start put request', file_name)
        self.assertEqual(result, file_name, 'test number [%d], expected [%s] but got[%s]' %(test_num, file_name, result))
        
        time_remaining = timeout
        while time_remaining > 0:
            time.sleep(1)
            result = send_device_put_request(self, 'query put request', file_name)
            if not (result == 'wait'):
                break;
            time_remaining -= 1

        if file_expected:
            self.assertEqual(result, 'done', 'put request test %d returned %s, time left %d[of %d] seconds' %(test_num, result, time_remaining, timeout))
        else:
            self.assertEqual(result, 'error', 'put request test %d returned %s, time left %d[of %d] seconds' %(test_num, result, time_remaining, timeout))

        verify_the_file_on_server(self, test_num, file_location, file_expected)

if __name__ == '__main__': 
    unittest.main()

