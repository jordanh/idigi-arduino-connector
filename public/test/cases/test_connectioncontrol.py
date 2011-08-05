import idigi_ws_api
import logging
import time
import unittest
import configuration

config = configuration.DeviceConfiguration()
nonidigi_host = 'google.com'


log = logging.getLogger('test_connectioncontrol')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)


def determine_disconnect_reconnect(instance, last_connected, wait_time=15):
    log.info("Determining if Device %s disconnected." 
            % config.device_id)
    new_device_core = config.api.get_first('DeviceCore', 
                            condition="devConnectwareId='%s'" % config.device_id)
                            
    # Ensure device is disconnected as result of SCI request.
    instance.assertEqual('0', new_device_core.dpConnectionStatus,
            "Device %s did not disconnect." % config.device_id)
        
    log.info("Waiting up to %i seconds for device to reconnect." % wait_time)
    # We'll assume that the device reconnects within 10 seconds.
        
    log.info("Determining if Device %s reconnected." \
            % config.device_id)
    
    for i in range(wait_time/5):
        time.sleep(5)
        new_device_core = config.api.get_first('DeviceCore', 
                            condition="devConnectwareId='%s'" % config.device_id)
        if new_device_core.dpConnectionStatus == "1":
            break
           
    # Ensure device has reconnected.
    instance.assertEqual('1', new_device_core.dpConnectionStatus,
                    "Device %s did not reconnect." % config.device_id)

    log.info("Initial Last Connect Time: %s." % last_connected)
    log.info("New Last Connect Time: %s." 
            % new_device_core.dpLastConnectTime)
    # Ensure that Last Connection Time has changed from initial Device State
    # instance.assertNotEqual(last_connected, new_device_core.dpLastConnectTime)
    

class RedirectTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
    
    @unittest.skip("")
    def test_redirect(self):
        log.info("Beginning Redirect Test")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        # SCI request to send the redirect, redirect device to same server.
        redirect_request = \
"""<sci_request version="1.0">
    <redirect>
        <targets>
            <device id="%s"/>
        </targets>
        <destinations>
            <destination>%s</destination>
        </destinations>
    </redirect>
</sci_request>""" % (config.device_id, config.api.hostname)

        response = config.api.sci(redirect_request)
        
        log.info("response:\n%s" % response)
        
        # Determine if device disconnected and reconnected
        determine_disconnect_reconnect(self, last_connected, 15)
    
    @unittest.skip("")
    def test_redirect_multi_urls_first_nonidigi(self):
        log.info("Beginning Redirect Test with multiple URLs (first non-iDigi).")
        last_connected = self.device_core.dpLastConnectTime

        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        # SCI request to send the redirect, redirect device to same server.
        redirect_request = \
"""<sci_request version="1.0">
    <redirect>
        <targets>
            <device id="%s"/>
        </targets>
        <destinations>
            <destination>%s</destination>
            <destination>%s</destination>
        </destinations>
    </redirect>
</sci_request>""" % (config.device_id, nonidigi_host, config.api.hostname)

        response = config.api.sci(redirect_request)
        log.info("response:\n%s" % response)
        
        # Determine if device disconnected and reconnected
        determine_disconnect_reconnect(self, last_connected, 45)
        
    @unittest.skip("")
    def test_redirect_singleurl_nondigi(self):
        log.info("Beginning Redirect Test with single non-iDigi URL.")
        last_connected = self.device_core.dpLastConnectTime

        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        # SCI request to send the redirect, redirect device to same server.
        redirect_request = \
"""<sci_request version="1.0">
    <redirect>
        <targets>
            <device id="%s"/>
        </targets>
        <destinations>
            <destination>%s</destination>
        </destinations>
    </redirect>
</sci_request>""" % (config.device_id, nonidigi_host)

        response = config.api.sci(redirect_request)
        log.info("response:\n%s" % response)
        
        # Determine if device disconnected and reconnected
        determine_disconnect_reconnect(self, last_connected, 45)
    
    @unittest.skip("")
    def test_redirect_zero_destinations(self):
        log.info("Beginnning Redirect Test with zero destination URLs.")
        
        # Send redirect without destinations
        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        redirect_request = \
"""<sci_request version="1.0">
    <redirect>
        <targets>
            <device id="%s"/>
        </targets>
        <destinations>
        </destinations>
    </redirect>
</sci_request>""" % (config.device_id)

        
        response = config.api.sci_expect_fail(redirect_request)
        log.info("response:\n%s" % response)
        
        log.info("Determining if we have received error response.")
        error = response.find("error")
        self.assertNotEqual(-1, error, "Received unexpected response from invalid redirect.")
    
    @unittest.skip("skip three destinations -- FAILS in DEBUG mode")
    def test_redirect_three_destinations(self):
        log.info("Beginnning Redirect Test with three destination URLs.")
        last_connected = self.device_core.dpLastConnectTime
        
        # Send redirect with three destinations
        log.info("Sending Connection Control Redirect to %s." % config.device_id)
        redirect_request = \
"""<sci_request version="1.0">
    <redirect>
        <targets>
            <device id="%s"/>
        </targets>
        <destinations>
            <destination>%s</destination>
            <destination>%s</destination>
            <destination>%s</destination>
        </destinations>
    </redirect>
</sci_request>""" % (config.device_id, config.api.hostname, config.api.hostname, config.api.hostname)

        
        response = config.api.sci_expect_fail(redirect_request)
        log.info("response:\n%s" % response)
        
        log.info("Determining if device was redirected.")
        redirected = response.find("redirected")
        self.assertNotEqual(-1, redirected, "Unexpected response: %s." % response)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, last_connected, 60)
        
        
        
class DisconnectTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
                        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
    
    @unittest.skip("")
    def test_disconnect(self):
        log.info("Beginning Disconnect Test")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending Connection Control Disconnect to %s." % config.device_id)
        # SCI request to send the disconnect
        disconnect_request = \
"""<sci_request version="1.0">
    <disconnect>
        <targets>
            <device id="%s"/>
        </targets>
    </disconnect>
</sci_request>""" % (config.device_id)

        config.api.sci(disconnect_request)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, last_connected)

class RebootTestCase(unittest.TestCase):
    
    def setUp(self):
        # Ensure device is connected.
        log.info("Ensuring Device %s is connected." % config.device_id)
        self.device_core = config.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % config.device_id)
                        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % config.device_id)
                
    def test_reboot(self):
        log.info("Beginning Reboot Test")
        last_connected = self.device_core.dpLastConnectTime
        
        log.info("Sending Reboot to %s." % config.device_id)
        
        reboot_request = \
"""<sci_request version="1.0"> 
  <reboot> 
    <targets> 
      <device id="%s"/> 
    </targets> 
  </reboot>
</sci_request>""" % (config.device_id)

        response = config.api.sci(reboot_request)
        log.info("response to reboot request = %s" % response)
        
        # Determine if device disconnects and reconnects
        determine_disconnect_reconnect(self, last_connected, 30)
        
        
if __name__ == '__main__':
    config = configuration.DeviceConfiguration()
    unittest.main()
