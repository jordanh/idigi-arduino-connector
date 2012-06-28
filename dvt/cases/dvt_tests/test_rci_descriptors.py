from requests import post
import xpath
from random import randrange
from time import sleep
from string import Template
from nose.tools import *
from xml.dom.minidom import parseString,getDOMImplementation
import iik_testcase
import logging
dom = getDOMImplementation()

from iik_plugin import IIKPlugin

log = logging.getLogger('iik_testcase')
log.setLevel(logging.INFO)
if len(log.handlers) == 0:
    handler = logging.StreamHandler()
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)

logging.getLogger('requests').setLevel(logging.WARNING)

##################################################################
# Elements and Groups to ignore (to not ruin device config)
##################################################################
EXCLUDE_GROUP=[]
EXCLUDE_ELEMENT=[]
##################################################################

##################################################################
#Templates for XML to send.
##################################################################
RCI_BASE_TEMPLATE = Template("""<sci_request version="1.0"> 
  <send_message cache="false"> 
    <targets> 
      <device id="${device_id}"/> 
    </targets> 
    <rci_request version="1.1"> 
      ${request}
    </rci_request>
  </send_message>
</sci_request>""")

RCI_DESC_TEMPLATE = Template("""<sci_request version="1.0"> 
  <send_message cache="only"> 
    <targets> 
      <device id="${device_id}"/> 
    </targets> 
    <rci_request version="1.1"> 
      ${request}
    </rci_request>
  </send_message>
</sci_request>""")

QUERY_DESCRIPTOR_SETTING = """<query_descriptor><query_setting/></query_descriptor>"""

QUERY_DESCRIPTOR_STATE = """<query_descriptor><query_state/></query_descriptor>"""

SET_SETTING = Template("""<set_setting><${group}><${element}>${value}</${element}></${group}></set_setting>""")

QUERY_SETTING = Template("""<query_setting><${group}/></query_setting>""")

SET_STATE = Template("""<set_state><${group}><${element}>${value}</${element}></${group}></set_state>""")

QUERY_STATE = Template("""<query_state><${group}/></query_state>""")
##################################################################

class RCIGroupTestScenario(object):
    
    def __init__(self, element, value, error, description):
        self.element        = element
        self.value          = value
        self.error          = error
        self.description    = description

    def __repr__(self):
        return '%s_%s_%s' % (self.element.group, 
                             self.element.name, 
                             self.description)

class RciElement(object):

    def __init__(self, group, element):
        self.group   = group
        self.element = element
        self.type    = element.getAttribute('type')
        self.name    = element.getAttribute('name')
        self.min     = None
        self.max     = None
        self.access  = element.getAttribute('access') \
                        if element.hasAttribute('access') else None

        base = 16 if self.type in ['hex32', '0xhex'] else 10

        if element.hasAttribute('min'):
            self.min = int(element.getAttribute('min'), base)
        if element.hasAttribute('max'):
            self.max = int(element.getAttribute('max'), base)

    def has_max(self):
        return self.max is not None

    def has_min(self):
        return self.min is not None

    def has_min_and_max(self):
        return self.has_min() and self.has_max()

    def has_min_or_max(self):
        return self.has_min() or self.has_max()        

    
def send_rci(request, url, 
             username,
             password):

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

def get_string (value):
    chars = []
    for _ in xrange(value):
        rand = randrange(65,90)
        chars.append(chr(rand))
    return ''.join(chars)

def get_dns(value):
    chars = []
    if value < 5:
        stop = value
        append = False
    else:
        stop = value - 4
        append = True
    for _ in xrange(stop):
        rand = randrange(65,90)
        chars.append(chr(rand))
    if append:
        chars.append('.')
        chars.append('C')
        chars.append('O')
        chars.append('M')
    return ''.join(chars)

def format_hex(value):
    return value.split('x')[-1]

class TestRciDescriptors(object):

    def set_and_query_setting(self, test):
        return self.set_and_query(test, setting=True)

    def set_and_query_state(self, test):
        return self.set_and_query(test, setting=False)

    def set_and_query(self, test, setting=True):

        if setting:
            query_template = QUERY_SETTING
            set_template = SET_SETTING
        else:
            query_template = QUERY_STATE
            set_template = SET_STATE

        query_command = query_template.substitute(group = test.element.group)
        set_command   = set_template.substitute(group   = test.element.group,
                                                element = test.element.name,
                                                value   = test.value)

        log.info("Sending initial query command for %s/%s to capture value." \
            % (test.element.group, test.element.name))

        #Get the current value
        rci = RCI_BASE_TEMPLATE.substitute(request=query_command, 
                    device_id=IIKPlugin.device_config.device_id)
        doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname, 
            IIKPlugin.api.username, IIKPlugin.api.password)

        self.parse_errors(doc)

        current_value = ''
        value = xpath.find('//%s/%s' % (test.element.group,
                                        test.element.name), doc)
        if value:
            current_value = '%s'%value[0].firstChild.nodeValue

        #Set a new value
        rci = RCI_BASE_TEMPLATE.substitute(request=set_command, 
                    device_id=IIKPlugin.device_config.device_id)

        log.info("Sending set command for %s/%s to value '%s'." \
                        % (test.element.group, test.element.name, test.value))
        doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname, 
            IIKPlugin.api.username, IIKPlugin.api.password)

        errors = xpath.find('//error',doc)
        #Check to see if response had errors in it.
        if len(errors) > 0 and test.error is None:
            log.info("Ensuring no errors returned in set response.")
            assert_is_not_none(test.error, "Error(s) Unexpectedly Returned")
        
        elif test.error is not None:
            log.info("Ensuring errors were found in set response.")
            #Check to see if we expect errors.
            #Check each error returned to make sure they are valid
            for error in errors:
                error_id = int(error.getAttribute('id'))
                assert_not_in(test.error, error_id)
        else:
            #If we expected an error but didn't get any, fail
            assert_is_none(test.error, 
                "No errors found when expected")

        #Check to see if value is correct
        log.info("Sending query command for %s/%s." \
                    % (test.element.group, test.element.name))
        rci = RCI_BASE_TEMPLATE.substitute(request=query_command, 
                    device_id=IIKPlugin.device_config.device_id)
        doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname, 
            IIKPlugin.api.username, IIKPlugin.api.password)

        self.parse_errors(doc)

        #Get the new value
        new_value = ''
        value = xpath.find('//%s/%s'%(test.element.group,test.element.name), doc)
        if value:
            new_value = '%s'%value[0].firstChild.nodeValue

        if (test.error is None and test.element.access != 'read_only'):
            log.info("Ensuring value was successfully set.")
            eq_('%s'%test.value,new_value, 
                "Set value (%s) doesn't match value returned in query (%s)." \
                    % (test.value, new_value))
        else:
            log.info("Ensuring value was not successfully set.")
            assert_not_equal(current_value,new_value, "Value was set even " \
                "though we expected the set_setting command to fail.")

        log.info("Test was successful.")

    def ensure_connected(self):
        log.info("Ensuring Device %s is connected." \
            % IIKPlugin.device_config.device_id)
        self.device_core = IIKPlugin.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" \
                        % IIKPlugin.device_config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % IIKPlugin.device_config.device_id)

    def parse_group(self, descriptor, setting=True):
        group_name = descriptor.getAttribute('element')
        errors = [ int(e.getAttribute('id')) \
                        for e in xpath.find('error_descriptor',descriptor) ]

        elements = [ RciElement(group_name, e) \
                        for e in xpath.find('element', descriptor) ]

        tests = []
        for element in elements:
            if element.type in ['string', 'multiline_string', 'password']:
                if element.has_min():
                    tests.append(RCIGroupTestScenario(element, 
                        get_string(element.min), 
                        None, 
                        '%s_no_error_at_min_%s' % (element.type, element.min)))

        return tests

    def parse_errors(self, doc):
        errors = xpath.find('//error',doc)
        if len(errors) > 0:
            error = errors[0]
            if error.getAttribute('id') == '2007':
                raise Exception("Invalid XML received from query_setting of device: \n%s" \
                    % xpath.find('hint/text()', error)[0].data)
            else:
                raise Exception("Error (%s) Returned from device: %s. Hint: %s" \
                    % (error.getAttribute('id'), 
                       xpath.find('desc/text()', error)[0].data,
                       xpath.find('hint/text()', error)[0].data))

    def get_descriptors(self, setting=True):
        request_template = QUERY_DESCRIPTOR_SETTING \
                            if setting else QUERY_DESCRIPTOR_STATE

        rci = RCI_DESC_TEMPLATE.substitute(request=request_template,
                        device_id=IIKPlugin.device_config.device_id)

        doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname, 
                IIKPlugin.api.username, IIKPlugin.api.password)

        return [ d for d in xpath.find('//format_define/descriptor', doc) ]

    def test_device_rci(self):
        self.ensure_connected()

        log.info("Retrieving query_setting descriptors.")
        for descriptor in self.get_descriptors(setting=True):
            tests = self.parse_group(descriptor, True)

            for test in tests:
                yield self.set_and_query_setting, test

        log.info("Retrieving query_state descriptors.")
        for descriptor in self.get_descriptors(setting=False):
            tests = self.parse_group(descriptor, False)

            for test in tests:
                yield self.set_and_query_state, test
