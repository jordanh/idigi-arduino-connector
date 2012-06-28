from requests import post
import xpath
from time import sleep
from string import Template
from nose.tools import *
from xml.dom.minidom import parseString,getDOMImplementation
import logging
from string import Template
dom = getDOMImplementation()

from iik_plugin import IIKPlugin, send_rci, parse_error

log = logging.getLogger('test_rci')
log.setLevel(logging.INFO)

if len(log.handlers) == 0:
    handler = logging.StreamHandler()
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)

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

class RCIVersionRequest(object):

    def __init__(self, version):
        self.version = version

    def __repr__(self):
        return "rci_version_request_%s" % self.version

def send_version_request(version_request):
    request = RCI_BASE_TEMPLATE.substitute(cache="false", 
      device_id=IIKPlugin.device_config.device_id, 
      rci_attributes=version_request.version, 
      request="<query_state/>")

    doc = send_rci(request)

    error = parse_error(doc)

    log.info('Error: %s.  Description: %s.  Hint: %s' % error)

def test_bad_rci_versions():
    versions = ['version="1.0"', 'version="2.0"', '']

    for version in versions:
        request = RCIVersionRequest(version)
        yield send_version_request, request