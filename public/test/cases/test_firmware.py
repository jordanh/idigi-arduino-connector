import idigi_ws_api
import logging
import time
import unittest

from base64 import encodestring
import configuration

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()


config = configuration.DeviceConfiguration()

log = logging.getLogger('test_redirect')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

target = 0


def getText(elem):
    rc = []
    for node in elem.childNodes:
        if node.nodeType == node.TEXT_NODE:
            rc.append(node.data)
    return str(''.join(rc))
    
def determine_disconnect_reconnect(instance, last_connected, wait_time=12):
    log.info("Determining if Device %s disconnected." 
            % config.device_id)
    new_device_core = config.api.get_first('DeviceCore', 
                            condition="devConnectwareId='%s'" % config.device_id)
                            
    # Ensure device is disconnected as result of SCI request.
    instance.assertEqual('0', new_device_core.dpConnectionStatus,
            "Device %s did not disconnect." % config.device_id)
        
    log.info("Waiting %i seconds for device to reconnect." % wait_time)
    # We'll assume that the device reconnects within 10 seconds.
    time.sleep(wait_time)
        
    log.info("Determining if Device %s reconnected." \
            % config.device_id)
    new_device_core = config.api.get_first('DeviceCore', 
                            condition="devConnectwareId='%s'" % config.device_id)
        
    # Ensure device has reconnected.
    instance.assertEqual('1', new_device_core.dpConnectionStatus,
            "Device %s did not reconnect." % config.device_id)

    log.info("Initial Last Connect Time: %s." % last_connected)
    log.info("New Last Connect Time: %s." 
            % new_device_core.dpLastConnectTime)
    # Ensure that Last Connection Time has changed from initial Device State
    instance.assertNotEqual(last_connected, new_device_core.dpLastConnectTime)
    
def update_firmware(device,
                    input_firmware,
                    target):
    request = impl.createDocument(None, "sci_request", None)
    sci_element = request.documentElement
    
    update_firmware_element = request.createElement("update_firmware")
    
    targets = request.createElement("targets")
    update_firmware_element.appendChild(targets)
    device_element = request.createElement("device")
    device_element.setAttribute("id", device)
    targets.appendChild(device_element)
        
    sci_element.appendChild(update_firmware_element)
    
        
    data = request.createElement("data")
    f = open(input_firmware, 'rb')
    data_value = request.createTextNode(encodestring(f.read()))
    f.close()
    data.appendChild(data_value)
    
    update_firmware_element.appendChild(data)
    
    #if target_filename:
    #    update_firmware_element.setAttribute("filename", target_filename)
        
    if target:
        update_firmware_element.setAttribute("firmware_target", target)
    
    #response = sci.send_request(request, server, "/ws/sci", user, password)    
    #return response.toprettyxml()
    return config.api.sci(request.toprettyxml())
    
class FirmwareTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
    

    def test_confirm_firmware_version_match(self):
        log.info("Beginning Test to Confirm Firmware Version.")
        # Retrieve Firmware Version
        log.info("Retrieving Firmware Version from %s." % config.device_id)
        new_device_core = config.api.get_first('DeviceCore',
                            condition="devConnectwareId='%s'" % config.device_id)
        # Ensure firmware version is correct
        self.assertEqual(new_device_core.dpFirmwareLevelDesc, config.firmware_version,
                            "Firmware Version (%s) is incorrect.")
    

    def test_validate_firmware_targets(self):
        log.info("Beginning Test to Validate Firmware Targets.")
        log.info("Sending firmware target query to %s." % config.device_id)
        firmware_query_request = \
"""<sci_request version="1.0">
  <query_firmware_targets>
    <targets>
      <device id="%s"/>
    </targets>
  </query_firmware_targets>
</sci_request>""" % (config.device_id)
        
        firmware_targets_xml = config.api.sci(firmware_query_request)
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        log.info("Determining if the number of targets matches targets in configuration.")
        self.assertEqual(len(targets), len(config.firmware_target), 
                "number of returned firmware targets (%i) does not match number of targets given (%i)" 
                % (len(targets), len(config.firmware_target)))
                
        for i in range(len(targets)):
            parse_firmware_target = [getText(targets[i].getElementsByTagName("name")[0]),
                                    getText(targets[i].getElementsByTagName("pattern")[0]),
                                    getText(targets[i].getElementsByTagName("version")[0]),
                                    getText(targets[i].getElementsByTagName("code_size")[0])]
            log.info("firmware target %i name: %s" % (i, parse_firmware_target[0]))
            log.info("firmware target %i pattern: %s" % (i, parse_firmware_target[1]))
            log.info("firmware target %i version: %s" % (i, parse_firmware_target[2]))
            log.info("firmware target %i code size: %s" % (i, parse_firmware_target[3]))
            log.info("Determining if firmware target %s matches given target" % i)
            self.assertEqual(parse_firmware_target, config.firmware_target[i], 
                            "Firmware Target does not match given target")
    

    def test_update_firmware_bytarget_multitargets(self):
        log.info("Beginning Test to Update Firmware by Target.")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending firmware target query to %s." % config.device_id)
        firmware_query_request = \
"""<sci_request version="1.0">
  <query_firmware_targets>
    <targets>
      <device id="%s"/>
    </targets>
  </query_firmware_targets>
</sci_request>""" % (config.device_id)
        
        # Determine if device has firmware targets
        firmware_targets_xml = config.api.sci(firmware_query_request)
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        log.info("Determining if the device has defined firmware targets.")
        self.assertNotEqual(0, len(targets), 
                "No targets exist on device %s" % config.device_id)
        
        # Send request to update firmware
        log.info("Sending request to update firmware.")
        response = update_firmware(config.device_id, config.firmware_target_file[target], "%d" % target)
        
        # Print response
        log.info("response: \n%s" % response)
       
        # Determine if Firmware was submitted
        submitted = response.find("submitted")
        self.assertNotEqual(-1, submitted, "Firmware was not submitted.")
        
        # Check if the device disconnects and reconnects
        determine_disconnect_reconnect(self, last_connected)
        
        #ADD TEST TO VERIFY FIRMWARE VERSION HAS UPDATED
        
        

    def test_update_firmware_nonexisting_target(self):
        log.info("Beginning Test to Update Non-Existing Firmware Target.")
        # Check time the device was last connected
        last_connected = self.device_core.dpLastConnectTime
        
        # Send firmware target query
        log.info("Sending firmware target query to %s." % config.device_id)
        firmware_query_request = \
"""<sci_request version="1.0">
  <query_firmware_targets>
    <targets>
      <device id="%s"/>
    </targets>
  </query_firmware_targets>
</sci_request>""" % (config.device_id)
        
        firmware_targets_xml = config.api.sci(firmware_query_request)
        
        # Determine number of firmware targets
        log.info("Determining number of firmware targets.")
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        # Choose non-existing target for upgrade
        upgrade_target = len(targets)
        
        #Send upgrade request
        log.info("Sending request to update firmware on target %d." % upgrade_target)
        response = update_firmware(config.device_id, config.firmware_target_file[target], "%d" % upgrade_target)
        
        log.info("response: \n%s" % response)
        
        # Verify that firmware was not updated 
        # should not find "submitted" in response
        log.info("Verify that the firmware was not updated.")
        submitted = response.find("submitted")
        self.assertEqual(-1, submitted, 
                "Firmware was successfully submitted to non-existing target.")
        
        # error message should include "Invalid Target"
        invalid_response = response.find("Invalid Target")
        self.assertNotEqual(-1, invalid_response, 
                "Device returned incorrect response to invalid firmware update.")

        
    def test_concurrent_firmware_upgrade(self):
    
        log.info("Beginning Test to Update Firmware While Update in Progress.")
        # Check time the device was last connected
        last_connected = self.device_core.dpLastConnectTime
        
        # Send firmware target query
        log.info("Sending firmware target query to %s." % config.device_id)
        firmware_query_request = \
"""<sci_request version="1.0">
  <query_firmware_targets>
    <targets>
      <device id="%s"/>
    </targets>
  </query_firmware_targets>
</sci_request>""" % (config.device_id)
        
        # Find firmware targets
        firmware_targets_xml = config.api.sci(firmware_query_request)
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        log.info("Determining if the device has defined firmware targets.")
        self.assertNotEqual(0, len(targets), 
                "No targets exist on device %s" % config.device_id)
        
        # Encode firmware for transmittal
        f = open(config.firmware_target_file[target], 'rb')
        data_value = encodestring(f.read())
        f.close()
        
        # Create asynchronous update request
        upgrade_firmware_request_asynch= \
"""<sci_request version="1.1">
    <update_firmware firmware_target="%s" synchronous="false">
        <targets>
            <device id="%s"/>
        </targets>
        <data>%s</data>
    </update_firmware>
</sci_request>""" % ("%d" % target, config.device_id, data_value)

        # Create synchronous update request
        upgrade_firmware_request_synch= \
"""<sci_request version="1.1">
    <update_firmware firmware_target="%s">
        <targets>
            <device id="%s"/>
        </targets>
        <data>%s</data>
    </update_firmware>
</sci_request>""" % ("%d" % target, config.device_id, data_value)
        
        # Send asynchronous update request
        log.info("Sending request to update firmware asynchronously.")
        response_asynch = config.api.sci(upgrade_firmware_request_asynch)
        log.info("Response to Asynchronous Update:\n%s" % response_asynch)
        
        # Send synchronous update request
        log.info("Sending request to update firmware synchronously.")
        response_synch = config.api.sci(upgrade_firmware_request_synch)
        log.info("Response to Synchronous Update:\n%s" % response_synch)
        
        # search for jobId in response to determine if asynchronous firmware
        # upgrade was queued
        jobid = response_asynch.find("jobId")
        self.assertNotEqual(-1, jobid, "Firmware upgrade was not queued.")
        
        # check to make sure synchronous upgrade request returns a busy message
        busy = response_synch.find("Firmware Upgrade Already In Progress")
        self.assertNotEqual(-1, busy, 
                        "Synchronous upgrade returned unexpected response.")
        
        # send GET with jobId to determine if the asynchronous upgrade completed
        dom = xml.dom.minidom.parseString(response_asynch)
        jobid_number = getText(dom.getElementsByTagName("jobId")[0])
        
        log.info("Waiting 60 seconds for asynchronous firmware update to complete.")
        time.sleep(60)
        log.info("Sending GET to determine if asynchronous firmware upgrade has completed.")
        response = config.api.sci_status(jobid_number)
        complete = response.find("complete")
        self.assertNotEqual(-1, complete, 
                        "Asynchronous firmware upgrade did not complete.")
        log.info("complete message:\n%s" % response)
        
        
if __name__ == '__main__':
    config = configuration.DeviceConfiguration()
    unittest.main(argv=" ")
