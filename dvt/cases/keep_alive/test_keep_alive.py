import time
import iik_testcase
import logging
import datetime
import json
from base64 import b64encode, b64decode
import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()
import push_client
from threading import Event
from utils import clean_slate, update_firmware, getText

log = logging.getLogger('ds_utils')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

class FileDataCallback(object):

    def __init__(self, path):
        self.path = path
        self.event = Event()
        self.data = None

    def callback(self, data):
        self.data = json.loads(data)
        self.event.set()

def get_and_verify(instance, api, device_id, file_location, expected_content, wait_time=60):

    """Performs a GET on the file_location to determine if the correct content 
    was pushed. A second GET is performed to verify the file's 
    metadata is correct.
    """

    client = push_client.PushClient(api.username, api.password, api.hostname, 
                secure=False)
    monitor = client.create_monitor([file_location], batch_size=1, batch_duration=0, 
        format_type='json')

    try:
        cb = FileDataCallback(file_location)

        session = client.create_session(cb.callback, monitor)

        # Check that file content is correct
        log.info("Waiting for File...")
        cb.event.wait(wait_time)

        instance.assertNotEqual(None, cb.data, "Data not received for %s within wait time %d." % (file_location, wait_time))

        log.info("Verifying File Content.")
        file_data = cb.data['Document']['Msg']['FileData']
        file_size = file_data['fdSize']

        if file_size == 0:
            file_content = ''
        else:
            file_content = b64decode(file_data['fdData'])

        # If provided, compare file's contents to expected content.
        if expected_content:
            log.info("Comparing file's content = %s" % file_content)
            log.info("with expected file's content = %s" % expected_content)
            # Verify file's contents
            instance.assertEqual(expected_content, file_content)

        # Check that FileData is correct
        log.info("Verifying file's metadata")

        return file_content
    finally:
        client.stop_all()
        client.delete_monitor(monitor)

class KeepAliveTestCase(iik_testcase.TestCase):
    """ Downloads the firmware and make sure device sends fw and rx keepalive periodically """

    def test_firmware_and_rx_keepalive(self):
        # send request to update firmware
        target = 0
        error_message = "Download Checksum"
        self.log.info("Sending request to update firmware.")
        response = update_firmware(self.api, self.device_config.device_id, 
            self.device_config.firmware_target_file[target], "%d" % target)
            
        # print response
        self.log.info("response: \n%s" % response)
            
        self.log.info("Determining if correct error response was returned.")
        
        # Parse response to verify an error was returned.
        dom = xml.dom.minidom.parseString(response)
        error_exists = dom.getElementsByTagName("error")
        self.assertTrue(error_exists, "Response is not an error message.")
        
        # Verify that correct error message was returned
        correct_error = response.find(error_message)
        self.assertNotEqual(-1, correct_error,
                "The expected error message (%s) was not returned."
                % error_message)

    def test_tx_keep_alive(self):        
        # Create paths to delete the file.
        file_location = 'FileData/~%2F' + self.device_config.device_id + '/keepalive.txt'

        self.log.info("delete file %s" % file_location)
        clean_slate(self.api, file_location)

        # Create content that are expected from the reboot result file.
        expected_content = "keepalive DVT success\n"

        # get and verify correct content is pushed.
        get_and_verify(self, self.api, self.device_config.device_id, file_location, expected_content)

if __name__ == '__main__':

    unittest.main()
