from nose.plugins import Plugin
import os

class BuildPlugin(Plugin):
    name = 'build'

    def options(self, parser, env=os.environ):
        Plugin.options(self, parser, env=env)
        parser.add_option('--build_username', action='store', type="string", dest="username", default="iikdvt", help="Username device is provisioned to.")
        parser.add_option('--build_password', action='store', type="string", dest="password", default="iik1sfun", help="Password of username device is provisioned to.")
        parser.add_option('--build_hostname', action='store', type="string", dest="hostname", default="idigi-e2e.sa.digi.com", help="Server device is connected to.")
        parser.add_option('--build_device_type', action='store', type="string", dest="device_type", default="Linux Application", help="Device Type to Use for Config Tool.")
        parser.add_option('--build_firmware_version', action='store', type='string', dest="firmware_version", default="0", help="Firmware Version to Use for Config Tool.")
        parser.add_option('--build_config_tool_jar', action='store', type="string", dest="config_tool_jar", default="ConfigGenerator.jar", help="Config Tool Jar used to generate RCI Config Code")
        parser.add_option('--build_cflags', action='store', type="string", dest="cflags", help="CFLAGS to add to compile", default="")
        parser.add_option('--build_src', action='store', type="string", dest="src", help="Source Directory to Build From.", default=".")

    def configure(self, options, conf):
        Plugin.configure(self, options, conf)

        if not self.enabled:
            return
            
        self.cflags = options.cflags
        self.src = options.src
        self.username = options.username
        self.password = options.password
        self.hostname = options.hostname
        self.device_type = options.device_type
        self.firmware_version = options.firmware_version
        self.config_tool_jar = options.config_tool_jar

    def finalize(self, result):
        pass

    def prepareTestCase(self, test):
        test.test.cflags = self.cflags
        test.test.src = self.src
        test.test.device_type = self.device_type
        test.test.username = self.username
        test.test.password = self.password
        test.test.hostname = self.hostname
        test.test.firmware_version = self.firmware_version
        test.test.config_tool_jar = self.config_tool_jar
