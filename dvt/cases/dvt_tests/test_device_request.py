import time
import iik_testcase
import datetime
import re
import os
from base64 import b64encode

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

# from utils import clean_slate
from utils import getText, clean_slate

impl = getDOMImplementation()
class DeviceRequestDvtTestCase(iik_testcase.TestCase):
    
    def test_ds_valid_target(self):    
        """ Verifies that the [valid target] device request returned match the expected response. """
        self.send_device_request("valid target", True, False)

    def test_ds_invalid_target(self):
        """ Verifies that the [invalid target] device request returns expected error. """
        self.send_device_request("invalid target", True, True)

    def test_ds_cancel_request(self):
        """ Verifies that the [cancel request] device request returns expected error. """
        self.send_device_request("cancel request", True, True)

    def test_ds_cancel_in_middle(self):
        """ Verifies that the [cancel in middle] device request returns expected error. """
        self.send_device_request("cancel in middle", True, True)

    def test_ds_cancel_response(self):
        """ Verifies that the [cancel response] device request returns expected error. """
        self.send_device_request("cancel response", True, True)

    def test_ds_busy_request(self):
        """ Verifies that the [busy request] device request returned match the expected response. """
        self.send_device_request("busy request", True, False)

    def test_ds_busy_response(self):
        """ Verifies that the [busy response] device request returned match the expected response. """
        self.send_device_request("busy response", True, False)

    def test_ds_not_handle(self):
        """ Verifies that the [not handle] device request returns expected error. """
        self.send_device_request("not handle", True, False)

    def test_ds_zero_byte_data(self):
        """ Verifies that the [zero byte data] device request returns expected error. """
        self.send_device_request("zero byte data", False, False)

    def test_ds_timeout_response(self):
        """ Verifies that the [timeout response] device request returned match the expected response. """
        self.send_device_request("timeout response", True, True)

    def send_device_request (self, target, send_data, error_expected):

        input_file = 'dvt/cases/test_files/pattern.txt'

        if send_data:
            f = open(input_file, 'rb')
            content = f.read()
            f.close()
        else:
            content = ""

        content_send = content.replace('\r\n', '\n')
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
            </sci_request>""" % (self.device_config.device_id, target, content_send)

        self.log.info("Beginning Test for device request. target[%s]" %target)
        self.log.info("Sending device request for \"%s\" target_name to server for device id  %s." % (target, self.device_config.device_id))

        # Send device request
        device_request_response = self.api.sci(my_target_device_request)

        # Parse request response 
        dom = xml.dom.minidom.parseString(device_request_response)
        device_response = dom.getElementsByTagName("device_request")

        # Validate target name
        if error_expected:
            self.log.info("Determining if \"%s\" target is cancelled." %target)
            error = getText(device_response[0].getElementsByTagName('error')[0])

            if not target == "timeout response":
                cancelled_response = "Message transmission cancelled"
                self.assertEqual(error, cancelled_response,
                                     "returned error (%s) expected error (%s)" 
                                     % (error, cancelled_response))
        else:
            self.log.info("Determining if the target_name is \"%s\"." %target)
            target_name = device_response[0].getAttribute('target_name')
            self.assertEqual(target_name, target, 
                             "returned target (%s) is not (%s)" 
                             % (target_name, target))

            # Validate status
            self.log.info("Determining if status is success.")
            status = device_response[0].getAttribute("status")
            if target == "not handle":
                self.assertEqual(status, '1', "returned error status(%s)" %status)
            else:
                self.assertEqual(status, '0', "returned error status(%s)" %status)

                # Validate response data
                self.log.info("Determining if response data is expected \"%s\"." %target)

                if target == 'zero byte data':
                    if device_response[0].firstChild:
                        self.assertEqual(device_response[0].firstChild, False, 'Data is not expected for zero byte data')
                else:
                    content_recv = device_response[0].firstChild.data
                    self.assertEqual(content_send, content_recv, "Mismatch in the sent and received data [%s]" %target)

if __name__ == '__main__': 
    unittest.main() 
