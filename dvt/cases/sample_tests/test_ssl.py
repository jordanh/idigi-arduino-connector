import iik_testcase
import shutil
from utils import DeviceConnectionMonitor

DISCONNECT_REQUEST = \
"""<sci_request version="1.0">
    <disconnect>
        <targets>
            <device id="%s"/>
        </targets>
    </disconnect>
</sci_request>"""

good_ca_cert_src = './dvt/cases/test_files/idigi-ca-cert-public.crt'
good_ca_cert_dst = './public/certificates/idigi-ca-cert-public.crt'
bad_ca_cert_src  = './dvt/cases/test_files/intruder-ca-cert.crt'

class SSLTestCase(iik_testcase.TestCase):

    def test_ssl_with_good_ca_cert(self):
        self.log.info("SSL connection test with valid ca certificate")
        
        self.log.info("Retrieving Device type for %s." % self.device_config.device_id)
        # Send request for new device_core
        new_device_core = self.api.get_first('DeviceCore', 
                    condition="devConnectwareId='%s'" % self.device_config.device_id)
                            
        # Verify that device type returned matches that in the configuration.
        self.assertEqual(new_device_core.dpDeviceType, self.device_config.device_type,
            "Device type (%s) does not match device type in configuration (%s)." 
            % (new_device_core.dpDeviceType, self.device_config.device_type))

        # Verify that device's vendor id matches that in the configuration.
        self.assertEqual(new_device_core.dvVendorId, self.device_config.vendor_id,
            "Vendor ID (%s) does not match Vendor ID in configuration (%s)."
            % (new_device_core.dvVendorId, self.device_config.vendor_id))

    def test_ssl_with_intruder_ca_cert(self):
        self.log.info("SSL connection test with intruder ca certificate")

        monitor = DeviceConnectionMonitor(self.api, self.device_config.device_id)

        try:
            monitor.start()

            try:
                shutil.copy(bad_ca_cert_src, good_ca_cert_dst);
            except Exception, e:
                print ">>> Error: %s" %e

            self.log.info("Sending Connection Control Disconnect to %s." % self.device_config.device_id)
            # Create disconnect request
            disconnect_request = DISCONNECT_REQUEST % (self.device_config.device_id)

            # Send SCI disconnect request
            self.api.sci(disconnect_request)


            self.log.info("Waiting for iDigi to disconnect device.")
            monitor.wait_for_disconnect(30)
            self.log.info("Device disconnected.")

            self.log.info("Waiting for Device to reconnect.")

            timeout = 10
            monitor.connect_event.wait(timeout)
            if monitor.connect_data is None:                
                self.log.info("Device is still disconnected... copying valid certificate back")
            else:
                raise Exception('Device %s did connect within %d seconds with bad CA certificate.' % \
                    (self.device_id, timeout))

            try:
                shutil.copy(good_ca_cert_src, good_ca_cert_dst);
            except Exception, e:
                print ">>> Error: %s" %e

            monitor.wait_for_connect(timeout)
            self.log.info("Device connected.")
             
        finally:
            monitor.stop()

if __name__ == '__main__':
    unittest.main()
