import logging
import time
import unittest
import datetime
import re
import os
from base64 import b64encode

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

import idigi_ws_api
import configuration


import configuration
import idigi_ws_api
# from utils import clean_slate
from utils import getText, determine_disconnect_reconnect, clean_slate

impl = getDOMImplementation()

log = logging.getLogger('device_request')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

expected_content = "iDigi device request sample [0]\n"

config_file = 'config.ini'
config = configuration.DeviceConfiguration(config_file)


class SendDeviceRequestTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
       
    def test_device_request(self):
    
        """ Verifies that the device request returned match the expected 
        response.
        """
        my_target_name = "myTarget"
        my_target_response = "My device response data"

        my_target_device_request = \
            """<sci_request version="1.0">
              <data_service>
                <targets>
                  <device id="%s"/>
                </targets>
                <requests>
                <device_request target_name="%s">My device request data</device_request>
                </requests>
              </data_service>
            </sci_request>""" % (config.device_id, my_target_name)

        log.info("Beginning Test for device request.")
        log.info("Sending device request for \"%s\" target_name to server for device id  %s." % (my_target_name, config.device_id))
        
        # Send device request
        device_request_response = config.api.sci(my_target_device_request)
        
        # Parse request response 
        dom = xml.dom.minidom.parseString(device_request_response)
        device_response = dom.getElementsByTagName("device_request")
        
        # Validate target name
        log.info("Determining if the target_name is \"%s\"." % my_target_name)
        target_name = device_response[0].getAttribute('target_name')
        self.assertEqual(target_name, my_target_name, 
            "returned target (%s) is not (%s)" 
            % (target_name, my_target_name))

        # Validate status 
        log.info("Determining if status is success.")
        status = device_response[0].getAttribute("status")
        self.assertEqual(status, '0', 
            "returned status (%s) is not success status" 
            % status)
        
        # Validate response data
        log.info("Determining if response data is expected \"%s\"." % my_target_response)
        
        data = device_response[0].firstChild.data
        self.assertEqual(data, my_target_response,
            "returned target (%s) is not (%s)" 
            % (data, my_target_response))

    def test_invalid_target_device_request(self):
    
        """ Verifies that the device request returned match the expected 
        response.
        """
        my_target_name = "invalidTarget"

        my_target_device_request = \
            """<sci_request version="1.0">
              <data_service>
                <targets>
                  <device id="%s"/>
                </targets>
                <requests>
                <device_request target_name="%s">My device request data</device_request>
                </requests>
              </data_service>
            </sci_request>""" % (config.device_id, my_target_name)

        log.info("Beginning Test for device request.")
        log.info("Sending device request for \"%s\" target_name to server for device id  %s." % (my_target_name, config.device_id))
        
        # Send device request
        device_request_response = config.api.sci(my_target_device_request)
        
        # Parse request response 
        dom = xml.dom.minidom.parseString(device_request_response)
        device_response = dom.getElementsByTagName("device_request")
        
        # Validate cancel message response
        log.info("Determining if \"%s\" target is cancelled." % my_target_name)
        error = getText(device_response[0].getElementsByTagName('error')[0])

        cancelled_response = "Message transmission cancelled"

        self.assertEqual(error, cancelled_response,
            "returned target (%s) is not (%s)" 
            % (error, cancelled_response))

if __name__ == '__main__':
    config = configuration.DeviceConfiguration(config_file)  
    unittest.main() 
