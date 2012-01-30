import unittest
import shutil
from build_utils import build
import sys

class BuildTestCase(unittest.TestCase):

    def test_compile(self):
        print "Building [%s] with CFLAGS [%s]." % (self.src, self.cflags)
        (rc, output) = build(self.src, self.cflags)
        if rc != 0:
            self.fail("%s:\n%s" % ("Did not cleanly compile", output)) 