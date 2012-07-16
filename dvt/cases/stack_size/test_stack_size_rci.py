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

RCI_QUERY_SETTING = \
"""<sci_request version="1.0"> 
  <send_message cache="false"> 
    <targets> 
      <device id="%s"/> 
    </targets> 
    <rci_request version="1.1"> 
      <query_setting><system><description/></system></query_setting>
    </rci_request>
  </send_message>
</sci_request>"""

RCI_SET_SETTING = \
"""<sci_request version="1.0"> 
  <send_message cache="false"> 
    <targets> 
      <device id="%s"/> 
    </targets> 
    <rci_request version="1.1"> 
      <set_setting><system><description>%s</description></system></set_setting>
    </rci_request>
  </send_message>
</sci_request>"""


def send_rci(request):
    
    req_data = parseString(request).toxml()
    log.info("Sending SCI Request: \n%s" % req_data)

    # Send device request
    response = self.api.sci(req_data)
#    response = post(url, 
#                    data=req_data, 
#                    auth=(username, password), 
#                    verify=False)

    assert_equal(200, response.status_code, "Non 200 Status Code: %d.  " \
        "Response: %s" % (response.status_code, response.content))
    try:
        res_data = parseString(response.content)
        log.info("Received SCI Response: \n%s" \
            % res_data.toprettyxml(indent=' '))
        return res_data
    except Exception, e:
        error = "Response was not valid XML: %s" % response.content
        assert 0==1, error

def getNodeText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc

class StackSizeTestCase(iik_testcase.TestCase):

    def test_stacksize_with_rci_query_setting(self):
    
        """ Sends query_setting. 
        """
        self.log.info("**** Stack Size (RCI) Test:  RCI query_setting")
        # Get the current value

        rci_request = (RCI_QUERY_SETTING % self.device_config.device_id);

        # Send RCI request
        rci_response = self.api.sci(rci_request)

        # Parse request response 
        dom = xml.dom.minidom.parseString(rci_response)
        rci_error_response = dom.getElementsByTagName('error')
    
        if len(rci_error_response) != 0:
            self.log.info("Request: %s" % rci_request)
            self.log.info("Response: %s" % rci_response)
            assert_true(error is found, "Got error response")

    def test_stacksize_with_rci_set_setting(self):
    
        """ Sends set_setting. 
        """
        self.log.info("**** Stack Size (RCI) Test:  RCI set_setting")
        # Get the current value

        rci_request = (RCI_SET_SETTING % (self.device_config.device_id, "ic_stacksize DVT testing"));

        # Send RCI request
        rci_response = self.api.sci(rci_request)

        # Parse request response 
        dom = xml.dom.minidom.parseString(rci_response)
        rci_error_response = dom.getElementsByTagName('error')
    
        if len(rci_error_response) != 0:
            self.log.info("Request: %s" % rci_request)
            self.log.info("Response: %s" % rci_response)
            assert_true(error is found, "Got error response")

        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()
            rci_request = (RCI_SET_SETTING % (self.device_config.device_id, "terminate"));

            # Send RCI request
            rci_response = self.api.sci(rci_request)

            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")

            self.log.info("Waiting for Device to reconnect.")
            monitor.wait_for_connect(30)
            self.log.info("Device connected.")

        finally:
            monitor.stop()        
        

if __name__ == '__main__':
    unittest.main()
