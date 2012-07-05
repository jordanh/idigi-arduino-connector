from requests import post
import xpath
from time import sleep
from string import Template
from nose.tools import *
from xml.dom.minidom import parseString,getDOMImplementation
import logging
from string import Template
dom = getDOMImplementation()

from iik_plugin import IIKPlugin, send_rci

log = logging.getLogger('test_rci')
log.setLevel(logging.INFO)

if len(log.handlers) == 0:
    handler = logging.StreamHandler()
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)

def parse_error(doc):
    errors = xpath.find('//error',doc)
    if len(errors) > 0:
        error = errors[0]

        desc = xpath.find('desc/text()', error)
        if len(desc) > 0:
            desc = desc[0].data
        else:
            desc = ''

        hint = xpath.find('hint/text()', error)
        if len(hint) > 0:
            hint = hint[0].data
        else:
            hint = ''

        return (error.getAttribute('id'), desc, hint)
    return None

RCI_BASE_TEMPLATE = Template("""<sci_request version="1.0">
  <send_message cache="${cache}">
    <targets>
      <device id="${device_id}"/>
    </targets>
    <rci_request ${rci_attributes}>
      ${request}
    </rci_request>
  </send_message>
</sci_request>""")

DEFAULT_DEVICE_ID=IIKPlugin.device_config.device_id
DEFAULT_RCI_ATTRIBUTES='version="1.1"'
DEFAULT_RCI_CACHE="false"

class TestCase(object):

    def __init__(self, test, error, name=''):
        self.test=test
        self.error=error
        self.name=name

class RCIRequest(object):

    def __init__(self,rci,error,name='',
                    rci_attributes=DEFAULT_RCI_ATTRIBUTES,
                    cache=DEFAULT_RCI_CACHE):
        self.rci=rci
        self.name=name
        self.error=error
        self.rci_attributes=rci_attributes
        self.cache=cache

    def __repr__(self):
        return "test_%s"%self.name

def send_request(rci_request):
    request = RCI_BASE_TEMPLATE.substitute(cache=rci_request.cache,
                                            device_id=DEFAULT_DEVICE_ID,
                                            request=rci_request.rci,
                                            rci_attributes=rci_request.rci_attributes)

    doc = send_rci(request)

    error = parse_error(doc)
    if error:
        log.info('Error: %s.  Description: %s.  Hint: %s' % error)
        perr=error[1]
    else:
        perr=None
    assert_equal(perr,rci_request.error, 'Expected: "%s" Recieved: "%s"' % (rci_request.error,perr))

def test_bad_rci_versions():
    tests = [
            TestCase('version="2.0"',
                        'Invalid version',
                        'version_2.0'),
            TestCase('version="1.0"',
                        'Invalid version',
                        'version_1.0'),
            TestCase('',
                        None,
                        'version_blank'),
            ]

    for test in tests:
        request = RCIRequest('<query_state/>',
                                test.error,
                                'bad_rci_%s'%test.name,
                                test.test)
        yield send_request, request

def test_query_descriptor():
    tests = [
            TestCase('<query_descriptor/>',
                        'Bad command',
                        'query_descriptor_only'),
            TestCase('<query_descriptor></query_descriptor>',
                        'Bad command',
                        'query_descriptor_blank'),
            TestCase('<query_descriptor><query_setting/></query_descriptor>',
                        'Bad command',
                        'query_setting'),
            TestCase('<query_descriptor><query_state/></query_descriptor>',
                        'Bad command',
                        'query_state'),
            ]

    for test in tests:
        request = RCIRequest(test.test,
                                test.error,
                                'query_descriptor_%s'%test.name)
        yield send_request, request

def test_query_setting_and_state():
    tests = [
            TestCase('<query_setting><unkownGroup/></query_setting>','Bad group','query_setting_unknown_group'),
            TestCase('<query_state><unkownGroup/></query_state>','Bad group','query_state_unknown_group'),
            TestCase('<query_setting><serial/></query_setting>',None,'query_setting_no_index_group'),
            TestCase('<query_state><gps_stats/></query_state>',None,'query_state_no_index_group'),
            TestCase('<query_setting><serial index="10"></serial></query_setting>',"Bad index",'query_setting_invalid_index_group'),
            TestCase('<query_state><gps_stats index="10"></gps_stats></query_state>',"Bad index",'query_state_invalid_index_group'),
            TestCase('<query_setting><serial/><system/></query_setting>',None,'query_setting_multiple_groups'),
            #TestCase('<query_state><debug_info/><gps_stats/></query_state>',None,'query_state_multiple_groups'),
            TestCase('<query_setting><serial/></query_setting>',None,'query_setting_no_element_1'),
            TestCase('<query_state><gps_stats/></query_state>',None,'query_state_no_element_1'),
            TestCase('<query_setting><serial></serial></query_setting>',None,'query_setting_no_element_2'),
            TestCase('<query_state><gps_stats></gps_stats></query_state>',None,'query_state_no_element_2'),
            TestCase('<query_setting><serial><unknownElement/></serial></query_setting>','Bad element','query_setting_unknown_element'),
            TestCase('<query_state><gps_stats><unknownElement/></gps_stats></query_state>','Bad element','query_state_unknown_element'),
            ]


    for test in tests:
        request = RCIRequest(test.test,test.error,test.name)
        yield send_request, request

def test_set_setting_and_state():
    tests = [
            TestCase('<set_setting><unkownGroup><someElement>12345</someElement></unkownGroup></set_setting>','Bad group','set_setting_unknown_group'),
            TestCase('<set_state><unkownGroup><someElement>12345</someElement></unkownGroup></set_state>','Bad group','set_state_unknown_group'),
            TestCase('<set_setting><system><description>Testing No Index</description></system></set_setting>',None,'set_setting_no_index_group'),
            TestCase('<set_state><gps_stats><latitude>45</latitude></gps_stats></set_state>',None,'set_state_no_index_group'),
            TestCase('<set_setting><system index="10"><description>Testing Invalid Index</description></system></set_setting>',"Bad index",'set_setting_invalid_index_group'),
            TestCase('<set_state><gps_stats index="10"><latitude>45</latitude></gps_stats></set_state>',"Bad index",'set_state_invalid_index_group'),
            TestCase('<set_setting><system><description>Testing Multiple Groups</description><contact>Testing Contact</contact></system></set_setting>',None,'set_setting_multiple_groups'),
            #TestCase('<set_state><debug_info/><gps_stats/></set_state>',None,'set_state_multiple_groups'),
            TestCase('<set_setting><system/></set_setting>','Parser error','set_setting_no_element_1'),
            TestCase('<set_state><gps_stats/></set_state>','Parser error','set_state_no_element_1'),
            TestCase('<set_setting><system></system></set_setting>','Parser error','set_setting_no_element_2'),
            TestCase('<set_state><gps_stats></gps_stats></set_state>','Parser error','set_state_no_element_2'),
            TestCase('<set_setting><system><unknownElement>Testing Unkown Element</unknownElement></system></set_setting>','Bad element','set_setting_unknown_element'),
            TestCase('<set_state><gps_stats><unknownElement/></gps_stats></set_state>','Bad element','set_state_unknown_element'),
            TestCase('<set_setting><system><description>asdfghjklaasdfghjklaasdfghjklaasdfghjklaasdfghjklaasdfghjklaasdfghjkla</description></system></set_setting>','Bad value','set_setting_out_of_range'),
            TestCase('<set_state><gps_stats><latitude>9999999999999999999999999999999999999999999999999999999999999999999999999999999</latitude></gps_stats></set_state>','Bad value','set_state_out_of_range'),
            TestCase("""<set_setting><system><description>Testing No Index
                Multiple lines
                to be bad test.</description></system></set_setting>""",'Bad value','set_setting_bad_type'),
            TestCase('<set_state><gps_stats><latitude>0xff</latitude></gps_stats></set_state>','Bad value','set_state_bad_type'),
            ]


    for test in tests:
        request = RCIRequest(test.test,test.error,test.name)
        yield send_request, request
