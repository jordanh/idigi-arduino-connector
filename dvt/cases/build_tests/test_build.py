import unittest
import shutil
from build_utils import build, generate_rci_code
import sys
import logging

log = logging.getLogger('build_plugin')
log.setLevel(logging.INFO)

if len(log.handlers) == 0:
    handler = logging.StreamHandler()
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)


class BuildTestCase(unittest.TestCase):

    def test_compile(self):
    	(rc,output) = generate_rci_code(self.src, self.device_type, 
    		self.firmware_version, self.hostname, self.username, 
    		self.password, self.config_tool_jar, self.keystore)

    	if rc != 0:
    		self.fail("RCI Config Generation Failed: \n%s" % output)

        print "Building [%s] with CFLAGS [%s]." % (self.src, self.cflags)
        (rc, output) = build(self.src, self.cflags)
        if rc != 0:
            self.fail("%s:\n%s" % ("Did not cleanly compile", output)) 
        else:
            log.info(output)