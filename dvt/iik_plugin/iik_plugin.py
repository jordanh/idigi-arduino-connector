from nose.plugins import Plugin
import os
import idigi_ws_api
import configuration
import logging
import xpath
from requests import post
from xml.dom.minidom import parseString
from nose.tools import *

log = logging.getLogger('iik_plugin')
log.setLevel(logging.INFO)

if len(log.handlers) == 0:
    handler = logging.StreamHandler()
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)


def send_rci(request):
    username = IIKPlugin.api.username
    password = IIKPlugin.api.password
    url = 'https://%s/ws/sci' % IIKPlugin.api.hostname

    req_data = parseString(request).toxml()
    log.info("Sending SCI Request: \n%s" % req_data)

    response = post(url, 
                    data=req_data, 
                    auth=(username, password), 
                    verify=False)

    assert_equal(200, response.status_code, "Non 200 Status Code: %d.  " \
        "Response: %s" % (response.status_code, response.content))
    try:
        res_data = parseString(response.content)
        log.info("Received SCI Response: \n%s" \
            % res_data.toprettyxml(indent=' '))
        return res_data
    except Exception, e:
        error = "Response was not valid XML: %s" % response.content
        assert 0==1, error

def parse_error(doc):
    errors = xpath.find('//error',doc)
    if len(errors) > 0:
        error = errors[0]
        return (error.getAttribute('id'), 
                xpath.find('desc/text()', error)[0].data, 
                xpath.find('hint/text()', error)[0].data)
    return None

class IIKPlugin(Plugin):
    name = 'iik'
    api = None
    device_config = None

    def options(self, parser, env=os.environ):
        Plugin.options(self, parser, env=env)
        parser.add_option('--idigi_username', action='store', type="string", dest="username", default="iikdvt", help="Username device is provisioned to.")
        parser.add_option('--idigi_password', action='store', type="string", dest="password", default="iik1sfun", help="Password of username device is provisioned to.")
        parser.add_option('--idigi_hostname', action='store', type="string", dest="hostname", default="idigi-e2e.sa.digi.com", help="Server device is connected to.")
        parser.add_option('--iik_device_id', action='store', type="string", dest="device_id", default="00000000-00000000-00409DFF-FF432317", help="Device ID of device running iDigi Connector.")
        parser.add_option('--iik_config', action='store', type='string', dest="config_file", default="config.ini", help="Config File to use to run tests.")
    
    def configure(self, options, conf):
        Plugin.configure(self, options, conf)

        if not self.enabled:
            return
            
        IIKPlugin.api = idigi_ws_api.Api(options.username, options.password, 
            options.hostname)
        IIKPlugin.device_config = configuration.DeviceConfiguration(
            options.device_id, 
            config_file=options.config_file)

    def finalize(self, result):
        pass

    def prepareTestCase(self, test):
        test.test.api = IIKPlugin.api
        test.test.device_config = IIKPlugin.device_config