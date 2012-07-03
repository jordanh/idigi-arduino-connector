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
  <send_message cache="${cache}">
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

SET_SETTING = Template("""<set_setting><${group} index="${index}"><${element}>${value}</${element}></${group}></set_setting>""")

QUERY_SETTING = Template("""<query_setting><${group}/></query_setting>""")

SET_STATE = Template("""<set_state><${group} index="${index}"><${element}>${value}</${element}></${group}></set_state>""")

QUERY_STATE = Template("""<query_state><${group}/></query_state>""")

def parse_error(doc):
    errors = xpath.find('//error',doc)
    if len(errors) > 0:
        error = errors[0]

        desc = xpath.find('desc/text()', error)
        if len(desc) > 0:
            desc = desc[0].data

        hint = xpath.find('hint/text()', error)
        if len(hint) > 0:
            hint = hint[0].data

        return (error.getAttribute('id'), desc, hint)
    return None

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

        log.info("Sending initial query command for %s/%s to capture value." \
            % (test.element.group, test.element.name))

        #Get the current value
        rci = RCI_BASE_TEMPLATE.substitute(request=query_command,
                    device_id=IIKPlugin.device_config.device_id,
                    cache = 'false')
        doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname,
            IIKPlugin.api.username, IIKPlugin.api.password)

        error = parse_error(doc)
        assert_true(error is None, "Error found: %s " % (error,))
        for element in xpath.find('//%s'%test.element.group,doc):
            index = int(element.getAttribute('index')) \
                        if element.hasAttribute('index') else 1

            set_command   = set_template.substitute(group   = test.element.group,
                                                element = test.element.name,
                                                value   = test.value,
                                                index = index)

            current_value = ''
            value = xpath.find('//%s' % (test.element.name), element)
            childNumber = 0
            if len(value) > 0:
                vals = value[0].childNodes
                for val in vals:
                    if val is not None:
                        currValue = '%s'%val.nodeValue
                        break
                    childNumber += 1

            #Set a new value
            rci = RCI_BASE_TEMPLATE.substitute(request=set_command,
                        device_id=IIKPlugin.device_config.device_id,
                        cache = 'false')

            log.info("Sending set command for %s/%s to value '%s'." \
                            % (test.element.group, test.element.name, test.value))
            print 'RCI REQUEST: %s\n\n' % rci
            doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname,
                IIKPlugin.api.username, IIKPlugin.api.password)

            error = parse_error(doc)
            if test.error is None: # Ensure we have no errors if we don't expect any.
                log.info("Ensuring no errors returned in set response.")
                assert_equal(None, error, "Error found: %s" % (error,))
            elif test.error is not None:
                log.info("Ensuring errors were found in set response.")
                # Throw an error if we expected errors and there were none.
                assert_not_equal(None, error,
                    "No errors were returned when something was expected.")

                assert_true(int(error[0]) in test.error,
                    "Unexpected error: %s : %s" % (error,test.error))

                log.info("Error returned as expected: %s" % (error,))
            #Check to see if value is correct
            log.info("Sending query command for %s/%s." \
                        % (test.element.group, test.element.name))
            rci = RCI_BASE_TEMPLATE.substitute(request=query_command,
                        device_id=IIKPlugin.device_config.device_id,
                        cache = 'false')
            doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname,
                IIKPlugin.api.username, IIKPlugin.api.password)

            error = parse_error(doc)
            assert_true(error is None, "Error found: %s" % (error,))

            #Get the new value

            newElements = xpath.find('//%s'%test.element.group,doc)
            newElement = None

            for e in newElements:
                if e.hasAttribute('index'):
                    if index == int(e.getAttribute('index')):
                        newElement = e
                        break
                else:
                    if index == 1:
                        newElement = e
                        break
            assert_true(newElement is not None, "Could not find previously set element!")

            new_value = ''
            value = xpath.find('//%s'%(test.element.name), newElement)
            if len(value) > 0:
                childs = value[0].childNodes
                if len(childs) <= childNumber:
                    new_value = ''
                else:
                    new_value = '%s'%childs[childNumber].nodeValue

            if (test.error is None and test.element.access != 'read_only'):
                log.info("Ensuring value was successfully set.")
                eq_('%s'%test.value,'%s'%new_value,
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
                    if element.min > 0:
                        tests.append(RCIGroupTestScenario(element,
                            get_string(element.min-1),
                            errors,
                            '%s_less_than_min'%element.type))
                if element.has_max():
                    tests.append(RCIGroupTestScenario(element,
                        get_string(element.max),
                        None,
                        '%s_no_error_at_max'% element.type))
                    tests.append(RCIGroupTestScenario(element,
                        get_string(element.max+1),
                        errors,
                        '%s_more_than_max'% element.type))
                if element.has_min_and_max():
                    tests.append(RCIGroupTestScenario(element,
                        get_string(randrange(element.min,element.max)),
                        None,
                        '%s_no_error_within_min_max'% element.type))
                if not element.has_min_or_max():
                    tests.append(RCIGroupTestScenario(element,
                        "This is a test",
                        None,
                        '%s_no_error'% (element.type)))
                    tests.append(RCIGroupTestScenario(element,
                        "",
                        None,
                        '%s_empty_string'% element.type))

            elif element.type in ['uint32', 'int32']:
                if element.has_min():
                    tests.append(RCIGroupTestScenario(element,
                        element.min,
                        None,
                        '%s_no_error_at_min'% element.type))
                    tests.append(RCIGroupTestScenario(element,
                        element.min-1,
                        errors,
                        '%s_less_than_min'% element.type))
                if element.has_max():
                    tests.append(RCIGroupTestScenario(element,
                        element.max,
                        None,
                        '%s_no_error_at_max'% element.type))
                    tests.append(RCIGroupTestScenario(element,
                        element.max+1,
                        errors,
                        '%s_more_than_max'% element.type))
                if element.has_min_and_max():
                    tests.append(RCIGroupTestScenario(element,
                        randrange(element.min,element.max),
                        None,
                        '%s_no_error_within_min_max'% element.type))
                if not element.has_min_or_max():
                    tests.append(RCIGroupTestScenario(element,
                        1234545678,
                        None,
                        '%s_no_error'% element.type))
                if 'unit32' in element.type:
                    tests.append(RCIGroupTestScenario(element,
                        -123,
                        errors,
                        '%s_negative_number'% element.type))

            elif element.type in ['hex32','0xhex']:
                if element.has_min():
                    hMin = hex(element.min)
                    hMinM = hex(element.min-1)
                    if 'hex32' in element.type:
                        hMin = format_hex(hMin)
                        hMinM = format_hex(hMinM)
                    tests.append(RCIGroupTestScenario(element,
                        hMin,
                        None,
                        '%s_no_error_at_min'% element.type))
                    tests.append(RCIGroupTestScenario(element,
                        hMinM,
                        errors,
                        '%s_less_than_min'% element.type))
                if element.has_max():
                    hMax = hex(element.max)
                    hMaxP = hex(element.max+1)
                    if element.type in 'hex32':
                        hMax = format_hex(hMax)
                        hMaxP = format_hex(hMaxP)
                    tests.append(RCIGroupTestScenario(element,
                        hMax,
                        None,
                        '%s_no_error_at_max'% element.type))
                    tests.append(RCIGroupTestScenario(element,
                        hMaxP,
                        errors,
                        '%s_more_than_min'% element.type))
                if element.has_min_and_max():
                    hRand = hex(randrange(element.min,element.max))
                    if 'hex32' in element.type:
                        hRand = format_hex(hRand)
                    tests.append(RCIGroupTestScenario(element,
                        hRand,
                        None,
                        '%s_no_error_within_min_max'% element.type))
                if not element.has_min_or_max():
                    hNorm = hex(1289)
                    if 'hex32' in element.type:
                        hNorm = format_hex(hNorm)
                    tests.append(RCIGroupTestScenario(element,
                        hNorm,
                        None,
                        '%s_no_error'% element.type))
            elif 'float' in element.type:
                if element.has_min():
                    tests.append(RCIGroupTestScenario(element,
                        float(element.min),
                        None,
                        '%s_no_error_at_min'% element.type))
                    tests.append(RCIGroupTestScenario(element,
                        float(element.min-0.1),
                        errors,
                        '%s_less_than_min'% element.type))
                if element.has_max():
                    tests.append(RCIGroupTestScenario(element,
                        float(element.max),
                        None,
                        '%s_no_error_at_max'% element.type))
                    tests.append(RCIGroupTestScenario(element,
                        float(element.max+0.1),
                        errors,
                        '%s_more_than_min'% element.type))
                if element.has_min_and_max():
                    tests.append(RCIGroupTestScenario(element,
                        float(randrange(element.min,element.max)),
                        None,
                        '%s_no_error_within_min_max'% element.type))
                if not element.has_min_or_max():
                    tests.append(RCIGroupTestScenario(element,
                        1234545678.123,
                        None,
                        '%s_no_error'% element.type))
                    tests.append(RCIGroupTestScenario(element,
                        -123.123,  
                        None, 
                        '%s_no_error_negative_number'% element.type)) 
            elif 'enum' in element.type:
                for value in xpath.find('value', element.element):
                    tests.append(RCIGroupTestScenario(element,
                        value.getAttribute('value'),
                        None,
                        '%s_no_error_%s' % (element.type,value.getAttribute('value'))))
                    tests.append(RCIGroupTestScenario(element,
                        "asdasdfkjh",
                        errors,
                        '%s_error_bad_value'% element.type))

            elif 'on_off' in element.type:
                tests.append(RCIGroupTestScenario(element,
                    "on",
                    None,
                    '%s_no_error'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "off",
                    None,
                    '%s_no_error'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "badValue",
                    errors,
                    '%s_error_bad_value'% element.type))

            elif 'boolean' in element.type:
                tests.append(RCIGroupTestScenario(element,
                    "true",
                    None,
                    '%s_no_error_bad_value'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "false",
                    None,
                    '%s_no_error_bad_value'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "badValue",
                    errors,
                    '%s_error_bad_value'% element.type))
            elif element.type in ['ipv4', 'fqdnv4','fqdnv6']:
                #ipv4 tests
                tests.append(RCIGroupTestScenario(element,
                    "255.255.255.255",
                    None,
                    '%s_no_error_max'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "0.0.0.0",
                    None,
                    '%s_no_error_zeros'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "10.9.116.100",
                    None,
                    '%s_no_error_valid_ip'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "1000.1.1.1",
                    errors,
                    '%s_error_bad_value'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "999.999.999.999",
                    errors,
                    '%s_error_bad_value'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "1000",
                    errors,
                    '%s_error_bad_value'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "asdf",
                    errors,
                    '%s_error_bad_value'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "",
                    errors,
                    '%s_error_bad_value'% element.type))
                if element.type in ['fqdnv4','fqdnv6']:
                    #fqdnv4/6 tests
                    if element.has_min():
                        tests.append(RCIGroupTestScenario(element,
                            get_dns(element.min),
                            None,
                            '%s_no_error_has_min_dns'% element.type))
                        tests.append(RCIGroupTestScenario(element,
                            get_dns(element.min-1),
                            errors,
                            '%s_error_less_than_min_dns'% element.type))
                    if element.has_max():
                        tests.append(RCIGroupTestScenario(element,
                            get_dns(element.max),
                            None,
                            '%s_no_error_has_min_dns'% element.type))
                        tests.append(RCIGroupTestScenario(element,
                            get_dns(element.max+1),
                            errors,
                            '%s_error_less_than_min_dns'% element.type))
                    if element.has_min_and_max():
                        tests.append(RCIGroupTestScenario(element,
                            get_dns(randrange(element.min,element.max)),
                            None,
                            '%s_no_error_within_min_max_dns'% element.type))
                    if not element.has_min_or_max():
                        tests.append(RCIGroupTestScenario(element,
                            'test.idigi.com',
                            None,
                            '%s_no_error_has_min_dns'% element.type))
                        tests.append(RCIGroupTestScenario(element,
                            '',
                            errors,
                            '%s_error_empty_dns'% element.type))
                    if element.type in 'fqdnv6':
                        #fqdnv6 tests
                        tests.append(RCIGroupTestScenario(element,
                            "1000:1000:1000:1000:1000:1000:1000:1000",
                            None,
                            '%s_no_error_ipv6_1000'% element.type))
                        tests.append(RCIGroupTestScenario(element,
                            "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                            None,
                            '%s_no_error_ipv6_ffff'% element.type))
                        tests.append(RCIGroupTestScenario(element,
                            "1000:1000:1000",
                            None,
                            '%s_no_error_ipv6_implied_zeros'% element.type))
                        tests.append(RCIGroupTestScenario(element,
                            "asdf",
                            errors,
                            '%s_error_bad_value_not_hex'% element.type))
                        tests.append(RCIGroupTestScenario(element,
                            "",
                            errors,
                            '%s_error_bad_value_blank'% element.type))

            elif 'datetime' in element.type:
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T00:00:00Z",
                    None,
                    '%s_no_error_0_with_z'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T00:00:00-0600",
                    None,
                    '%s_no_error_0_with_tz_offset'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T00:00:00",
                    None,
                    '%s_no_error_no_tz'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "9999-12-31T00:00:00Z",
                    errors,
                    '%s_error_bad_year'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-13-31T00:00:00Z",
                    errors,
                    '%s_error_bad_month'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-00-31T00:00:00Z",
                    errors,
                    '%s_error_0_month'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-32T00:00:00Z",
                    errors,
                    '%s_error_bad_day'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "0000-03-31T00:00:00Z",
                    errors,
                    '%s_error_0_year'% element.type))                    
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T99:00:00Z",
                    errors,
                    '%s_error_bad_hour_1'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T59:00:00Z",
                    errors,
                    '%s_error_bad_hour_2'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T00:99:00Z",
                    errors,
                    '%s_error_bad_minute'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T00:00:99Z",
                    errors,
                    '%s_error_bad_second'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T00:00:00-9999",
                    errors,
                    '%s_error_bad_tz_offset'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31T00:00:00-1",
                    errors,
                    '%s_error_bad_tz_offset_length'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "2011-03-31",
                    errors,
                    '%s_error_missing_time'% element.type))
                tests.append(RCIGroupTestScenario(element,
                    "",
                    errors,
                    '%s_error_blank_date'% element.type))

        return tests

    def get_descriptors(self, setting=True):
        request_template = QUERY_DESCRIPTOR_SETTING \
                            if setting else QUERY_DESCRIPTOR_STATE

        rci = RCI_BASE_TEMPLATE.substitute(request=request_template,
                        device_id=IIKPlugin.device_config.device_id,
                        cache='only')

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
