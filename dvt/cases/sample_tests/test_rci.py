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

    log.info('Error: %s.  Description: %s.  Hint: %s' % error)

def test_bad_rci_versions():
    tests = [
            TestCase('version="1.0"', 
                        '', 
                        'version_1.0'),
            TestCase('version="2.0"', 
                        '', 
                        'version_2.0'),
            TestCase('',
                        '',
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
                        '',
                        'query_descriptor_only'),
            TestCase('<query_descriptor></query_descriptor>',
                        '',
                        'query_descriptor_blank'),
            TestCase('<query_descriptor><query_setting/></query_descriptor>',
                        'Bad Command',
                        'query_setting'),
            TestCase('<query_descriptor><query_state/></query_descriptor>',
                        '',
                        'query_state'),
            ]

    for test in tests:
        request = RCIRequest(test.test,
                                test.error,
                                'query_descriptor_%s'%test.name,
                                DEFAULT_RCI_ATTRIBUTES)
        yield send_request, request