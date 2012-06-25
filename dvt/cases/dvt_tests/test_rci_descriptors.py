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

##################################################################
# Elements and Groups to ignore (to not ruin device config)
##################################################################
EXCLUDE_GROUP=['mgmtnetwork','mgmtglobal','host','dns','arp','tcp','python','boot',]
EXCLUDE_ELEMENT=['password','mgmtconnection','connectionEnabled','serverAddress','serverArray',]
##################################################################

##################################################################
#Set to 1 if you want to test negative test cases.
##################################################################
RUN_ERRORS = 1
##################################################################

##################################################################
#Seconds to sleep in between web service calls.
##################################################################
#NOTE: Each test does 3 web service calls. 
#This means about 300 WSCs are done. Which means iDigi may
#reach it's hourly limit if you are not careful.
RCI_SLEEP = 0
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

QUERY_DESCRIPTOR_SETTING = """<query_descriptor>
    <query_setting/>
</query_descriptor>"""

QUERY_DESCRIPTOR_STATE = """<query_descriptor>
    <query_state/>
</query_descriptor>"""

SET_SETTING = Template("""<set_setting>
    <${group}>
        <${element}>${value}</${element}>
    </${group}>
</set_setting>""")

QUERY_SETTING = Template("""<query_setting>
<${group}/>
</query_setting>""")

SET_STATE = Template("""<set_state>
    <${group}>
        <${element}>${value}</${element}>
    </${group}>
</set_state>""")

QUERY_STATE = Template("""<query_state>
    <${group}/>
</query_state>""")
##################################################################

class Element(object):
    
    def __init__(self,element,value, error, desc, isState=False):
        self.element = element
        self.value = value
        self.error = error
        self.description = desc
        self.isState = isState
        self.group=self.element.parentNode.getAttribute('element')

    def __repr__(self):
        return '%s_%s_%s' % (self.group , 
                             self.element.getAttribute('name'), 
                             self.description)

def replace_entities(aString):
    rtrn = aString.replace('&lt;','<')
    rtrn = rtrn.replace('&gt;','>')
    rtrn = rtrn.replace('&amp;','&')
    rtrn = rtrn.replace('&quot;','"')
    rtrn = rtrn.replace('&apos;',"'")
    return rtrn
    
def send_rci(request, url, 
             username,
             password):
    #Used if we receive bad XML from iDigi/Device
    sleep(RCI_SLEEP)
    response = post(url, data=request, auth=(username, password), verify=False)
    assert_equal(200, response.status_code, "Non 200 Status Code: %d.  " \
        "Response: %s" % (response.status_code, response.content))
    try:
        return parseString(response.content)
    except Exception, e:
        error = "Response was not valid XML: %s" % response.content
        assert 0==1, error

def bad_type_fail():
    "If type is not supported, fail!"
    #Used for error handling.
    ERROR_UNKNOWN_TYPE = 0
    assert(ERROR_UNKNOWN_TYPE)

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

    def __init__(self):
        self.log = logging.getLogger('iik_testcase')
        self.log.setLevel(logging.INFO)
        if len(self.log.handlers) == 0:
            handler = logging.StreamHandler()
            handler.setLevel(logging.INFO)
            formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
            handler.setFormatter(formatter)
            self.log.addHandler(handler)

    def set_and_query(self, test):
        group = test.group
        eName = test.element.getAttribute('name')
        readOnly = False
        if test.element.hasAttribute('access'):
            if test.element.getAttribute('access') == 'read_only':
                readOnly = True
        if test.isState:
            queryValue=QUERY_STATE.substitute(group=group)
            setValue=SET_STATE.substitute(group=group,element=eName,value=test.value)
        else:
            queryValue=QUERY_SETTING.substitute(group=group)
            setValue=SET_SETTING.substitute(group=group,element=eName,value=test.value)
        #Get the current value
        rci = RCI_BASE_TEMPLATE.substitute(request=queryValue, 
                    device_id=IIKPlugin.device_config.device_id)
        doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname, 
            IIKPlugin.api.username, IIKPlugin.api.password)
        currValue = ''
        value = xpath.find('//%s/%s'%(group,eName), doc)
        if value:
            currValue = '%s'%value[0].firstChild.nodeValue
        #Set a new value
        rci = RCI_BASE_TEMPLATE.substitute(request=setValue, 
                    device_id=IIKPlugin.device_config.device_id)
        doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname, 
            IIKPlugin.api.username, IIKPlugin.api.password)
        errs = xpath.find('//error',doc)
        #Check to see if response had errors in it.
        if len(errs) > 0:
            assert_is_not_none(test.error,"Unexpected Errors Returned.  " \
                "Response: %s" % doc.toprettyxml(indent="\t"))
            #Check to see if we expect errors.
                #Check each error returned to make sure they are valid
            for err in errs:
                errorId = int(err.getAttribute('id'))
                assert_not_in(errorId,test.error)
        else:
            #If we expected an error but didn't get any, fail
            assert_is_none(test.error)
        #Check to see if value is correct
        rci = RCI_BASE_TEMPLATE.substitute(request=queryValue, 
                    device_id=IIKPlugin.device_config.device_id)
        doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname, 
            IIKPlugin.api.username, IIKPlugin.api.password)
        #Get the new value
        newValue = ''
        value = xpath.find('//%s/%s'%(group,eName), doc)
        if value:
            newValue = '%s'%value[0].firstChild.nodeValue
        if (test.error is None and not readOnly):
            eq_('%s'%test.value,newValue)
        else:
            assert_not_equal(currValue,newValue)

    def ensure_connected(self):
        self.log.info("Ensuring Device %s is connected." \
            % IIKPlugin.device_config.device_id)
        self.device_core = IIKPlugin.api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" \
                        % IIKPlugin.device_config.device_id)
        
        # If not connected, fail the TestCase.
        if not self.device_core.dpConnectionStatus == '1':
            self.assertEqual('1', self.device_core.dpConnectionStatus, 
                "Device %s not connected." % IIKPlugin.device_config.device_id)

    def test_device_rci(self):
        self.ensure_connected()
        request = [{'query':RCI_DESC_TEMPLATE.substitute(request=QUERY_DESCRIPTOR_SETTING,
                    device_id=IIKPlugin.device_config.device_id),'isState':False},
                   {'query':RCI_DESC_TEMPLATE.substitute(request=QUERY_DESCRIPTOR_STATE, 
                    device_id=IIKPlugin.device_config.device_id),'isState':True},
                   ]
        for r in request:
            isState = r['isState']
            rci=r['query']
            doc = send_rci(rci, 'https://%s/ws/sci' % IIKPlugin.api.hostname, 
                IIKPlugin.api.username, IIKPlugin.api.password)
            print doc.toprettyxml(indent='    ')
            for descriptor in xpath.find('//format_define/descriptor', doc):
                #Get all the possible error id's for this descriptor
                if descriptor.getAttribute('element') not in EXCLUDE_GROUP:
                    errors = []
                    for error in xpath.find('error_descriptor',descriptor):
                        errors.append(int(error.getAttribute('id')))
                    #Get all the elements in this descriptor
                    for element in xpath.find('element', descriptor):
                        if element.getAttribute('name') not in EXCLUDE_ELEMENT:
                            element_type = element.getAttribute('type')
                            bad_type = False
                            # Get min/max values if they have them.
                            eMin = None
                            eMax = None
                            #NOTE: Need these because we are changing min/max into ints
                            ##If the min/max is 0 it will evaluate as False.
                            has_min = False
                            has_max = False
                            has_both = False
                            has_none = True
                            if element.getAttribute('min'):
                                has_min = True
                                if element_type not in ['hex32']:
                                    eMin = int(element.getAttribute('min'),0)
                                else:
                                    eMin = int(element.getAttribute('min'),16)
                            if element.getAttribute('max'):
                                has_max = True
                                if element_type not in ['hex32']:
                                    eMax = int(element.getAttribute('max'),0)
                                else:
                                    eMax = int(element.getAttribute('max'),16)
                            if has_min and has_max:
                                has_both =True
                                if eMax > eMin:
                                    randNum = randrange(eMin, eMax)
                            if has_min or has_max:
                                has_none = False
                            #The following runs tests depending on type of element
                            if element_type in ['string','multiline_string','password']:
                                #Create the tests
                                tests = []
                                if has_min:
                                    tests.append(Element(element,
                                                                get_string(eMin),
                                                                None,
                                                                '%s_no_error_at_min' % element_type))
                                    if eMin > 0:
                                        if RUN_ERRORS:
                                            tests.append(Element(element,get_string(eMin-1),errors,'%s_less_than_min'% element_type,isState))
                                if has_max:
                                    tests.append(Element(element,get_string(eMax),None,'%s_no_error_at_max'% element_type,isState))
                                    if RUN_ERRORS:
                                        tests.append(Element(element,get_string(eMax+1),errors,'%s_more_than_max'% element_type,isState))
                                if has_both:
                                    tests.append(Element(element,get_string(randNum),None,'%s_no_error_within_min_max'% element_type,isState))
                                if has_none:
                                    tests.append(Element(element,"This is a test",None,'%s_no_error'% element_type,isState))
                                    if RUN_ERRORS:
                                        tests.append(Element(element,"",errors,'%s_empty_string'% element_type,isState))
                
                            elif element_type in ['uint32', 'int32']:
                                tests = []
                                if has_min:
                                    tests.append(Element(element,eMin,None,'%s_no_error_at_min'% element_type,isState))
                                    if RUN_ERRORS:
                                        tests.append(Element(element,eMin-1,errors,'%s_less_than_min'% element_type,isState))
                                if has_max:
                                    tests.append(Element(element,eMax,None,'%s_no_error_at_max'% element_type,isState))
                                    if RUN_ERRORS:
                                        tests.append(Element(element,eMax+1,errors,'%s_more_than_min'% element_type,isState))
                                if has_both:
                                    tests.append(Element(element,randNum,None,'%s_no_error_within_min_max'% element_type,isState))
                                if has_none:
                                    tests.append(Element(element,1234545678,None,'%s_no_error'% element_type,isState))
                                    if 'unit32' in element_type:
                                        err = errors
                                    else:
                                        err = None
                                    if RUN_ERRORS or err is None:
                                        tests.append(Element(element,-123,  err, '%s_negative_number'% element_type,isState))
                
                            elif element_type in ['hex32','0xhex']:
                                tests = []
                                if has_min:
                                    hMin = hex(eMin)
                                    hMinM = hex(eMin-1)
                                    if 'hex32' in element_type:
                                        hMin = format_hex(hMin)
                                        hMinM = format_hex(hMinM)
                                    tests.append(Element(element,hMin,None,'%s_no_error_at_min'% element_type,isState))
                                    if RUN_ERRORS:
                                        tests.append(Element(element,hMinM,errors,'%s_less_than_min'% element_type,isState))
                                if has_max:
                                    hMax = hex(eMax)
                                    hMaxP = hex(eMax+1)
                                    if element_type in 'hex32':
                                        hMax = format_hex(hMax)
                                        hMaxP = format_hex(hMaxP)
                                    tests.append(Element(element,hMax,None,'%s_no_error_at_max'% element_type,isState))
                                    if RUN_ERRORS:
                                        tests.append(Element(element,hMaxP,errors,'%s_more_than_min'% element_type,isState))
                                if has_both:
                                    hRand = hex(randNum)
                                    if 'hex32' in element_type:
                                        hRand = format_hex(hRand)
                                    tests.append(Element(element,hRand,None,'%s_no_error_within_min_max'% element_type,isState))
                                if has_none:
                                    hNorm = hex(1289)
                                    if 'hex32' in element_type:
                                        hNorm = format_hex(hNorm)
                                    tests.append(Element(element,hNorm,None,'%s_no_error'% element_type,isState))
                
                            elif 'float' in element_type:
                                tests = []
                                if has_min:
                                    tests.append(Element(element,float(eMin),None,'%s_no_error_at_min'% element_type,isState))
                                    if RUN_ERRORS:
                                        tests.append(Element(element,float(eMin-0.1),errors,'%s_less_than_min'% element_type,isState))
                                if has_max:
                                    tests.append(Element(element,float(eMax),None,'%s_no_error_at_max'% element_type,isState))
                                    if RUN_ERRORS:
                                        tests.append(Element(element,float(eMax+0.1),errors,'%s_more_than_min'% element_type,isState))
                                if has_both:
                                    tests.append(Element(element,float(randNum),None,'%s_no_error_within_min_max'% element_type,isState))
                                if has_none:
                                    tests.append(Element(element,1234545678.123,None,'%s_no_error'% element_type,isState))
                                    tests.append(Element(element,-123.123,  None, '%s_no_error_negative_number'% element_type,isState))
                
                            elif 'enum' in element_type:
                                tests = []
                                for value in xpath.find('value', element):
                                    tests.append(Element(element,value.getAttribute('value'),None,'%s_no_error_%s' % (element_type,value.getAttribute('value')),isState))
                                if RUN_ERRORS:
                                    tests.append(Element(element,"asdasdfkjh",errors,'%s_error_bad_value'% element_type,isState))
                
                            elif 'on_off' in element_type:
                                tests = []
                                tests.append(Element(element,"on",None,'%s_no_error_bad_value'% element_type,isState))
                                tests.append(Element(element,"off",None,'%s_no_error_bad_value'% element_type,isState))
                                if RUN_ERRORS:
                                    tests.append(Element(element,"badValue",errors,'%s_error_bad_value'% element_type,isState))
                
                            elif 'boolean' in element_type:
                                tests = []
                                tests.append(Element(element,"true",None,'%s_no_error_bad_value'% element_type,isState))
                                tests.append(Element(element,"false",None,'%s_no_error_bad_value'% element_type,isState))
                                if RUN_ERRORS:
                                    tests.append(Element(element,"badValue",errors,'%s_error_bad_value'% element_type,isState))
                
                            elif element_type in ['ipv4', 'fqdnv4','fqdnv6']:
                                tests = []
                                #ipv4 tests
                                tests.append(Element(element,"256.256.256.256",None,'%s_no_error'% element_type,isState))
                                tests.append(Element(element,"0.0.0.0",None,'%s_no_error'% element_type,isState))
                                tests.append(Element(element,"10.9.116.1",None,'%s_no_error'% element_type,isState))
                                if RUN_ERRORS:
                                    tests.append(Element(element,"1000.1.1.1",errors,'%s_error_bad_value'% element_type,isState))
                                    tests.append(Element(element,"999.999.999.999",errors,'%s_error_bad_value'% element_type,isState))
                                    tests.append(Element(element,"1000",errors,'%s_error_bad_value'% element_type,isState))
                                    tests.append(Element(element,"asdf",errors,'%s_error_bad_value'% element_type,isState))
                                    tests.append(Element(element,"",errors,'%s_error_bad_value'% element_type,isState))
                                if element_type in ['fqdnv4','fqdnv6']:
                                    #fqdnv4/6 tests
                                    if has_min:
                                        tests.append(Element(element,get_dns(eMin),None,'%s_no_error_has_min_dns'% element_type,isState))
                                        if RUN_ERRORS:
                                            tests.append(Element(element,get_dns(eMin-1),errors,'%s_error_less_than_min_dns'% element_type,isState))
                                    if has_max:
                                        tests.append(Element(element,get_dns(eMax),None,'%s_no_error_has_min_dns'% element_type,isState))
                                        if RUN_ERRORS:
                                            tests.append(Element(element,get_dns(eMax+1),errors,'%s_error_less_than_min_dns'% element_type,isState))
                                    if has_both:
                                        tests.append(Element(element,get_dns(randNum),None,'%s_no_error_within_min_max_dns'% element_type,isState))
                                    if has_none:
                                        tests.append(Element(element,'test.idigi.com',None,'%s_no_error_has_min_dns'% element_type,isState))
                                        if RUN_ERRORS:
                                            tests.append(Element(element,'',errors,'%s_error_empty_dns'% element_type,isState))
                                    if element_type in 'fqdnv6':
                                        #fqdnv6 tests
                                        tests.append(Element(element,"1000:1000:1000:1000:1000:1000:1000:1000",None,'%s_no_error_ipv6'% element_type,isState))
                                        tests.append(Element(element,"ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",None,'%s_no_error_ipv6'% element_type,isState))
                                        tests.append(Element(element,"1000:1000:1000",None,'%s_no_error_ipv6'% element_type,isState))
                                        if RUN_ERRORS:
                                            tests.append(Element(element,"asdf",errors,'%s_error_bad_value'% element_type,isState))
                                            tests.append(Element(element,"",errors,'%s_error_bad_value'% element_type,isState))
                
                            elif 'datetime' in element_type:
                                tests = []
                                tests.append(Element(element,"2011-03-31T00:00:00Z",None,'%s_no_error'% element_type,isState))
                                tests.append(Element(element,"2011-03-31T00:00:00-0600",None,'%s_no_error'% element_type,isState))
                                tests.append(Element(element,"2011-03-31T00:00:00",None,'%s_no_error'% element_type,isState))
                                if RUN_ERRORS:
                                    tests.append(Element(element,"9999-12-31T00:00:00Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-13-31T00:00:00Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-00-31T00:00:00Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-03-32T00:00:00Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"0000-03-31T00:00:00Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-03-31T99:00:00Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-03-31T59:00:00Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-03-31T00:99:00Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-03-31T00:00:99Z",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-03-31T00:00:00-9999",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-03-31T00:00:00-1",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"2011-03-31",errors,'%s_error'% element_type,isState))
                                    tests.append(Element(element,"",errors,'%s_error'% element_type,isState))
                                
                            else:
                                bad_type = True
                                #yield bad_type_fail
                            if not bad_type:
                                for t in tests:
                                    yield self.set_and_query, t
                        
