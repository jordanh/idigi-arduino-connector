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


def getNodeText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc
class StackSizeTestCase(iik_testcase.TestCase):

    def test_stacksize_with_fw(self):
    
        """ Sends a firmware update """
    
        self.log.info("**** Stack Size DVT: Firmware Service")

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

        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()
            # Send update request
            request = (FIRMWARE_DATA_REQUEST % (fw_target_num, self.device_config.device_id, data_value))

            self.log.info("Sending request to update firmware.")
            response = self.api.sci(request)
            self.log.info("Response to:\n%s" % response)

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
