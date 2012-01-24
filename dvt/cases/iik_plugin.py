from nose.plugins import Plugin
import os
import idigi_ws_api
import configuration

class IIKPlugin(Plugin):
    name = 'iik'

    def options(self, parser, env=os.environ):
        Plugin.options(self, parser, env=env)
        parser.add_option('--idigi_username', action='store', type="string", dest="username")
        parser.add_option('--idigi_password', action='store', type="string", dest="password")
        parser.add_option('--idigi_hostname', action='store', type="string", dest="hostname")
        parser.add_option('--iik_device_id', action='store', type="string", dest="device_id")
        parser.add_option('--iik_config', action='store', type='string', dest="config_file")
    
    def configure(self, options, conf):
        Plugin.configure(self, options, conf)

        if not self.enabled:
            return
            
        self.api = idigi_ws_api.Api(options.username, options.password, 
            options.hostname)
        self.device_config = configuration.DeviceConfiguration(
            options.device_id, 
            config_file=options.config_file)

    def finalize(self, result):
        pass

    def prepareTestCase(self, test):
        test.test.api = self.api
        test.test.device_config = self.device_config
