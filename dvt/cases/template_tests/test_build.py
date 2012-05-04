import unittest
import shutil
from build_utils import get_template_dirs, setup_platform, build
import sys
sys.path.append('./dvt/scripts')
import config

class BuildTemplateTestCase(unittest.TestCase):

    def setUp(self):
        (self.sandbox_dir, self.template_script_dir, self.template_platform_dir, 
            self.template_test_dir, self.idigi_config_path) = get_template_dirs()
        setup_platform(self.template_script_dir, self.template_platform_dir, '000000:000000')

    def test_build_default(self):
        (rc, output) = build(self.template_test_dir, self.cflags)
        if rc != 0:
            self.fail("%s:\n%s" % ("Did not cleanly compile", output))

    def test_build_default_and_file_system(self):
        config.replace_string(self.idigi_config_path, 'IDIGI_NO_FILE_SYSTEM', 'IDIGI_FILE_SYSTEM')
        (rc, output) = build(self.template_test_dir, self.cflags)
        if rc != 0:
            self.fail("%s:\n%s" % ("Did not cleanly compile", output))

    def test_build_data_service_only(self):
        config.replace_string(self.idigi_config_path, 'IDIGI_COMPRESSION', 'IDIGI_NO_COMPRESSION')
        config.replace_string(self.idigi_config_path, 'IDIGI_FIRMWARE_SERVICE', 'IDIGI_NO_FIRMWARE_SERVICE')
        (rc, output) = build(self.template_test_dir, self.cflags)
        if rc != 0:
            self.fail("%s:\n%s" % ("Did not cleanly compile", output))

    def test_build_no_features(self):
        config.replace_string(self.idigi_config_path, 'IDIGI_COMPRESSION', 'IDIGI_NO_COMPRESSION')
        config.replace_string(self.idigi_config_path, 'IDIGI_FIRMWARE_SERVICE', 'IDIGI_NO_FIRMWARE_SERVICE')
        config.replace_string(self.idigi_config_path, 'IDIGI_DATA_SERVICE', 'IDIGI_NO_DATA_SERVICE')
        (rc, output) = build(self.template_test_dir, self.cflags)
        if rc != 0:
            self.fail("%s:\n%s" % ("Did not cleanly compile", output))     

    def test_build_firmware_only(self):
        config.replace_string(self.idigi_config_path, 'IDIGI_COMPRESSION', 'IDIGI_NO_COMPRESSION')
        config.replace_string(self.idigi_config_path, 'IDIGI_DATA_SERVICE', 'IDIGI_NO_DATA_SERVICE')
        (rc, output) = build(self.template_test_dir, self.cflags)
        if rc != 0:
            self.fail("%s:\n%s" % ("Did not cleanly compile", output))

    def test_build_file_system_only(self):
        config.replace_string(self.idigi_config_path, 'IDIGI_COMPRESSION', 'IDIGI_NO_COMPRESSION')
        config.replace_string(self.idigi_config_path, 'IDIGI_FIRMWARE_SERVICE', 'IDIGI_NO_FIRMWARE_SERVICE')
        config.replace_string(self.idigi_config_path, 'IDIGI_DATA_SERVICE', 'IDIGI_NO_DATA_SERVICE')
        config.replace_string(self.idigi_config_path, 'IDIGI_NO_FILE_SYSTEM', 'IDIGI_FILE_SYSTEM')
        (rc, output) = build(self.template_test_dir, self.cflags)
        if rc != 0:
            self.fail("%s:\n%s" % ("Did not cleanly compile", output))

    def tearDown(self):
        shutil.rmtree(self.sandbox_dir)