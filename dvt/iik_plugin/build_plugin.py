from nose.plugins import Plugin
import os

class BuildPlugin(Plugin):
    name = 'build'

    def options(self, parser, env=os.environ):
        Plugin.options(self, parser, env=env)
        parser.add_option('--build_cflags', action='store', type="string", dest="cflags", help="CFLAGS to add to compile", default="")
        parser.add_option('--build_src', action='store', type="string", dest="src", help="Source Directory to Build From.", default=".")
    def configure(self, options, conf):
        Plugin.configure(self, options, conf)

        if not self.enabled:
            return
            
        self.cflags = options.cflags
        self.src = options.src

    def finalize(self, result):
        pass

    def prepareTestCase(self, test):
        test.test.cflags = self.cflags
        test.test.src = self.src
