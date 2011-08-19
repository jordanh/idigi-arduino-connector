import logging
import time
import unittest
from base64 import encodestring

import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

import idigi_ws_api
import configuration
from utils import getText, determine_disconnect_reconnect, update_firmware

config = configuration.DeviceConfiguration('config.ini')

log = logging.getLogger('firmware')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

target = 0

FIRMWARE_QUERY_REQUEST = \
"""<sci_request version="1.0">
  <query_firmware_targets>
    <targets>
      <device id="%s"/>
    </targets>
  </query_firmware_targets>
</sci_request>"""
    
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
    
        """ Verifies that firmware version returned from device matches
        firmware version in configuration.
        """
        
        log.info("Beginning Test to Confirm Firmware Version.")
        # Retrieve Firmware Version
        log.info("Retrieving Firmware Version from %s." % config.device_id)
        new_device_core = config.api.get_first('DeviceCore',
                        condition="devConnectwareId='%s'" % config.device_id)
        # Ensure firmware version is correct
        self.assertEqual(new_device_core.dpFirmwareLevelDesc, 
                        config.firmware_version,
                        "Firmware Version (%s) does not match expected (%s)" % 
                        (new_device_core.dpFirmwareLevelDesc, 
                        config.firmware_version))
    
    def test_validate_firmware_targets(self):
    
        """ Verifies that the firmware targets returned match the firmware
        targets in the configuration.
        """
    
        log.info("Beginning Test to Validate Firmware Targets.")
        log.info("Sending firmware target query to %s." % config.device_id)
        
        # Send firmware target query request
        firmware_targets_xml = config.api.sci(FIRMWARE_QUERY_REQUEST % 
            config.device_id)
        
        # Parse request response for firmware targets
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        log.info("Determining if the number of targets matches targets in configuration.")
        self.assertEqual(len(targets), len(config.firmware_target), 
            "number of returned firmware targets (%i) does not match number of targets given (%i)" 
            % (len(targets), len(config.firmware_target)))
                
        for i in range(len(targets)):
            target = int(targets[i].getAttribute("number"))
            parse_firmware_target = [getText(targets[i].getElementsByTagName("name")[0]),
                                    getText(targets[i].getElementsByTagName("pattern")[0]),
                                    getText(targets[i].getElementsByTagName("version")[0])]
                                    
            log.info("firmware target %i name: %s" % (target, parse_firmware_target[0]))
            log.info("firmware target %i pattern: %s" % (target, parse_firmware_target[1]))
            log.info("firmware target %i version: %s" % (target, parse_firmware_target[2]))
            log.info("Determining if firmware target %s matches given target" % target)
            self.assertEqual(parse_firmware_target, config.firmware_target[target], 
                            "Firmware Target %s does not match given target %s" %  
                            (parse_firmware_target, config.firmware_target[target])) 
    

    def test_update_firmware_bytarget_multitargets(self):
    
        """ Updates firmware by target and verifies that the firmware
        is submitted and that the device disconnects and reconnects
        """
        
        log.info("Beginning Test to Update Firmware by Target.")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending firmware target query to %s." % config.device_id)
        
        # Determine if device has firmware targets
        firmware_targets_xml = config.api.sci(FIRMWARE_QUERY_REQUEST % 
            config.device_id)
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        log.info("Determining if the device has defined firmware targets.")
        self.assertNotEqual(0, len(targets), 
                "No targets exist on device %s" % config.device_id)
        
        # Send request to update firmware
        log.info("Sending request to update firmware.")
        response = update_firmware(config.api, config.device_id, 
            config.firmware_target_file[target], "%d" % target)
        
        # Print response
        log.info("response: \n%s" % response)
       
        # Determine if Firmware was submitted
        submitted = response.find("submitted")
        self.assertNotEqual(-1, submitted, "Firmware was not submitted.")        

        # Check if the device disconnects and reconnects
        determine_disconnect_reconnect(self, config, last_connected)
        
    
    def test_update_firmware_nonexisting_target(self):
    
        """ Sends a firmware upgrade to a non-existing target and verifies
        that an error message is returned.
        """
        
        log.info("Beginning Test to Update Non-Existing Firmware Target.")
        # Check time the device was last connected
        last_connected = self.device_core.dpLastConnectTime
        
        # Send firmware target query
        log.info("Sending firmware target query to %s." % config.device_id)
                
        firmware_targets_xml = config.api.sci(FIRMWARE_QUERY_REQUEST % config.device_id)
        
        # Determine number of firmware targets
        log.info("Determining number of firmware targets.")
        dom = xml.dom.minidom.parseString(firmware_targets_xml)
        targets = dom.getElementsByTagName("target")
        
        # Choose non-existing target for upgrade
        upgrade_target = len(targets)
        
        #Send upgrade request
        log.info("Sending request to update firmware on target %d." % upgrade_target)
        response = update_firmware(config.api, config.device_id, 
            config.firmware_target_file[target], "%d" % upgrade_target)
        
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
    
        """ Sends an asynchronous firmware update followed quickly by a
        synchronous firmware update. Also verifies the correct responses
        for each request and verifies that the device disconnects and 
        reconnects
        """
    
        log.info("Beginning Test to Update Firmware While Update in Progress.")
        # Check time the device was last connected
        last_connected = self.device_core.dpLastConnectTime
        
        # Send firmware target query
        log.info("Sending firmware target query to %s." % config.device_id)
               
        # Find firmware targets
        firmware_targets_xml = config.api.sci(FIRMWARE_QUERY_REQUEST % 
            config.device_id)
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
        
        log.info("Waiting up to 60 seconds for asynchronous firmware update to complete.")
        # Poll every 5 seconds to determine if firmware update has completed
        for i in range(11):
            time.sleep(5)
            response = config.api.sci_status(jobid_number)
            complete = response.find("complete")
            if complete != -1:
                break
        
        # Do final check to determine if firmware update has completed
        log.info("complete message:\n%s" % response)
        self.assertNotEqual(-1, complete, 
                        "Asynchronous firmware upgrade did not complete.")
        
        determine_disconnect_reconnect(self, config, last_connected)
        
if __name__ == '__main__':

    unittest.main()
