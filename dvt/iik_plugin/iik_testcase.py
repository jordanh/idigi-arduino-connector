import unittest
import logging

class TestCase(unittest.TestCase):

    def __init__(self, name):
        unittest.TestCase.__init__(self, name)
        self.log = logging.getLogger('iik_testcase')
        self.log.setLevel(logging.INFO)

        if len(self.log.handlers) == 0:
            handler = logging.StreamHandler()
            handler.setLevel(logging.INFO)
            formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
            handler.setFormatter(formatter)
            self.log.addHandler(handler)

    def setUp(self):
        self.log.info("Ensuring Device %s is connected." \
            % self.device_config.device_id)
        self.device_core = self.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" \
                        % self.device_config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % self.device_config.device_id)

    def tearDown(self):
        pass