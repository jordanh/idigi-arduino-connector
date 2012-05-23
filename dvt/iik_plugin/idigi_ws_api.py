# ***************************************************************************
# Copyright (c) 2012 Digi International Inc.,
# All rights not expressly granted are reserved.
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# ***************************************************************************
"""
idigi_ws_api is a Convenience API for utilizing the RESTful iDigi Web Service
APIs to access and manipulate Resources on the iDigi Server, of which includes
Devices, Customers, and Device Data (Storage, Dia, Xbee).

Example usage:

Creating an API object:

     import idigi_ws_api
     api = idigi_ws_api.Api('username', 'password')

Provisioning a Device against customer account by Device ID and working with it:

     # Create DeviceCore resource
     device = idigi_ws_api.RestResource.create('DeviceCore',
                  devConnectwareId='00000000-00000000-00409DFF-FF0000001')

     location = api.post(device)

     print location
     >> DeviceCore/6576/0

     # Retrieve Device, maps XML response to a custom python object extending
     # RestResource, maps child elements as object attrs.
     device = api.get_first(location)
    
     print device.devRecordStartDate
     >> 2011-01-23T22:37:00Z

     print device
     >> {'dpFirmwareLevel': '0', 'devConnectwareId':
     >> '00000000-00000000-00409DFF-FF000001', 'devTerminated': 'false',
     >> 'devEffectiveStartDate': '2011-01-23T22:37:00Z',
     >> 'dpConnectionStatus': '1', 'grpId': '1', 'cstId': '1',
     >> 'dpDeviceType': 'ConnectPort X2', 'dpRestrictedStatus': '0',
     >> 'devRecordStartDate': '2011-01-23T22:37:00Z',
     >> 'id': <idigi_ws_api.id object at 0x00000000029123C8>}

     print device.id
     >> {'devVersion': '1', 'devId': '6576'}

     print device.id.devVersion
     >> 1

Retrieving Metering Consumption Values from all Metering Server Clusters against
account.

    # Retrieve all 0x0 attributes on the 0x702 (1794) server (0x0) Cluster.
    attributes = api.get('XbeeAttributeDataCore', condition=\"xcClusterId='1794' \
                             and xcClusterType='0' and xaAttributeId='0'\")

    # Map Xbee Address/Endpoint ID to consumption
    consumption_vals = dict([(\"%s/%s\" % (a.id.xpExtAddr, a.id.xeEndpointId),
                              a.xadAttributeIntegerValue) for a in attributes])

    print consumption_vals
    >> { '00:00:06:12:34:5B:00:02/20': '2369',
    >>   'A0:00:01:12:34:04:00:02/18': '7324',
    >>   '00:00:65:10:00:01:00:01/14': '120023'}

Turn on Attribute Reporting on a Metering CSD Attribute

   # Create id identifying the attribute to enable reporting for
   id = idigi_ws_api.RestResource.create('id', xaAttributeId='0', \
            xcClusterId='1794', xcClusterType='0', xeEndpointId='9', \
            xpExtAddr=''00:40:9D:12:34:58:00:02')

   # Create an XbeeAttributeReportingCore object that enables reporting
   # on a 5 minute to 15 minute interval with a reportable change of 50
   report = idigi_ws_api.RestResource.create('XbeeAttributeReportingCore', \
                xarMinReportingInterval='600', xarMaxReportingInterval='1800', \
                xarTimeout='60', xarEnabled='true', xarReportableChange='50', \
                devConnectwareId='00000000-00000000-00409DFF-FF123458', id=id)

   location = api.put(report)

   print location
   >> XbeeAttributeReportingCore/00:40:9D;12;34;58:00:02/9/0/1794/0

   # Delete Report
   api.delete(location)
   
Posting an SCI Request to a Device:

        # SCI request to send the redirect, redirect device to same server.
        redirect_request = \"""
            <sci_request version="1.0">
                <redirect>
                    <targets>
                        <device id="%s"/>
                    </targets>
                    <destinations>
                        <destination>%s</destination>
                    </destinations>
                </redirect>
            </sci_request>\""" % (device_id, api.hostname)

        api.sci(redirect_request)

"""

import logging
import httplib, urllib
from base64 import encodestring
from xml.dom.minidom import getDOMImplementation, parseString, Element

log = logging.getLogger('idigi_ws_api')
log.setLevel(logging.WARN)

handler = logging.StreamHandler()
handler.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

impl = getDOMImplementation()

def getText(elem):
    rc = []
    for node in elem.childNodes:
        if node.nodeType == node.TEXT_NODE:
            rc.append(node.data)
    return str(''.join(rc))

class Result:
    
    def __init__(self, resources, start, remaining, total):
        self.start = start
        self.remaining = remaining
        self.total = total
        self.resources = resources
        
    def __iter__(self):
        return self.resources.__iter__()

class RestResource:
    def __init__(self, **attrs):
        for attr in attrs:
            setattr(self, attr, attrs[attr])
            
    def __str__(self):
        return str(self.__dict__)

    def fill_element(self, document, parent):
        """
        Adds corresponding minidom Elements of the attrs of this
        RestResource.  (Note: Recursive.)
        """
        for k in self.__dict__:
            values = self.__dict__[k]

            if not isinstance(values, list):
                values = [values]

            for value in values:
                element = document.createElement(k)
                if isinstance(value, RestResource):
                    value.fill_element(document,element)
                else:
                    if value: element.appendChild(document.createTextNode(value))
                parent.appendChild(element)

    def todocument(self):
        """
        Returns a minidom Document of this RestResource.
        """
        doc = impl.createDocument(None, type(self).__name__, None)
        root = doc.documentElement
        self.fill_element(doc,root)
        return doc

    @staticmethod
    def create(typename,**attrs):
        """
        Creates a RestResource with the given name and creates attrs out of
        the given kwargs.
        """
        resource = type(typename, (RestResource,object), {})
        return resource(**attrs)

def parse_elements(resource):
    attrs = {}
    for node in resource.childNodes:
        if isinstance(node,Element):
            k = str(node.nodeName)
            v = parse_node(node)
            if attrs.has_key(k):
                if type(attrs[k]) == list:
                    attrs[k].append(v)
                else:
                    attrs[k] = [attrs[k],v]
            else:
                attrs[k] = v
    return attrs
    

def parse_node(node):
    non_text_nodes = [child_node for child_node in node.childNodes \
                      if isinstance(child_node, Element)]

    if len(non_text_nodes) > 0:
        # Non-text nodes, parse into dict
        attrs = parse_elements(node)
        return RestResource.create(str(node.nodeName), **attrs)
    else:
        return getText(node)

def parse_response(response, resource):
    doc = parseString(response)
    resource_elem = doc.documentElement
    resource_name = str(resource_elem.nodeName)
    resources = []

    # if the resource name is result there were multiple entries,
    # return array.
    if resource_name == 'result':
        start = parse_node(resource_elem.getElementsByTagName("requestedStartRow")[0])
        remaining = parse_node(resource_elem.getElementsByTagName("remainingSize")[0])
        total = parse_node(resource_elem.getElementsByTagName("resultTotalRows")[0])
        resources = [node for node in resource_elem.childNodes]
        resource_objs = []
        for resource_elem in resources:
            resource_name = str(resource_elem.nodeName)
            if isinstance(resource_elem, Element) and \
                   resource_name == resource or \
                   (resource_name == "User" and resource == "RawUser"):
                resource_type = type(resource_name, (RestResource,object), {})
                attrs = parse_elements(resource_elem)
                resource_objs.append(resource_type(**attrs))
        result = Result(resource_objs, start, remaining, total)
        return result
    # Otherwise, this is a single entry, just return it.
    else:
        resource = type(resource_name, (RestResource,object), {})
        attrs = dict((str(node.nodeName), getText(node))
                     for node in resource_elem.childNodes)
        return resource(**attrs)
    
class Api:
    
    def __init__(self, username, password,
                 hostname='developer.idigi.com', ws_root='/ws'):
        self.hostname = hostname
        self.username = username
        self.password = password
        self.ws_root = ws_root
        self.headers = {
            'Content-Type' : 'text/xml',
            'Authorization': 'Basic ' \
            + encodestring('%s:%s' % (username,password))[:-1]
        }
    
    def sci(self, request):
        """
        Posts an SCI request (RCI, update firmware, messaging facility to 
        /ws/sci.
        
        Arguments:
        request: The full SCI string to send.
        """
        connection = httplib.HTTPSConnection(self.hostname)
        url = '%s/sci' % self.ws_root
        
        log.debug("POST to sci %s" % request)
        
        connection.request('POST', url, request, self.headers)
        response = connection.getresponse()
        response_str = response.read()
        connection.close()

        if response.status == 202 or response.status == 201 or response.status == 200:
            log.debug("SCI Response: %s" % response_str)
            return response_str
        else:
            raise Exception(
                'Returned Non 201/200 Status Code: %d : %s. Data = %s'
                % (response.status, response.reason, response_str))        
    
    def sci_expect_fail(self, request):
        """
        Posts an SCI request (RCI, update firmware, messaging facility to 
        /ws/sci.
        
        Arguments:
        request: The full SCI string to send.
        """
        connection = httplib.HTTPSConnection(self.hostname)
        url = '%s/sci' % self.ws_root
        
        #log.info("POST to sci %s" % request)
        
        connection.request('POST', url, request, self.headers)
        response = connection.getresponse()
        response_str = response.read()
        connection.close()

        log.debug("SCI Response: %s" % response_str)
        return response_str
 

    def sci_status(self, jobId):
    
        connection = httplib.HTTPSConnection(self.hostname)
        url = '%s/sci/%s' % (self.ws_root, jobId)
        
        log.info("Performing GET on %s" % url)
        
        connection.request('GET', url, headers = self.headers)
        
        response = connection.getresponse()
        response_str = response.read()
        connection.close()
        
        if response.status == 200:
            log.debug("SCI Response: %s" % response_str)
            return response_str
        else:
            raise Exception(
                'Returned Non 200 Status Code: %d : %s. Data = %s'
                % (response.status, response.reason, response_str))
        
        
    def get(self, resource, **params):
        connection = httplib.HTTPSConnection(self.hostname)
        url = '%s/%s' % (self.ws_root, resource)

        if params:
            url += '?' + urllib.urlencode([(key,params[key]) for key in params])

        connection.request('GET', url,
                           headers = self.headers)
        
        log.info("Sending GET to %s" , url)
        response = connection.getresponse()
        response_str = response.read()
        connection.close()

        if response.status == 200:
            response = parse_response(response_str, resource.split('/')[0])
            log.debug("Response: %s" % response_str)
            return response
        else:
            raise Exception('Return Non 200 Status Code %d : %s.  Data = %s'
                            % (response.status, response.reason, response_str))

    def get_raw(self, resource, **params):
        connection = httplib.HTTPSConnection(self.hostname)
        url = '%s/%s' % (self.ws_root, resource)
        
        if params:
            url += '?' + urllib.urlencode([(key,params[key]) for key in params])
        
        connection.request('GET', url, headers = self.headers)
        
        log.info("Sending GET to %s" %url)
        response = connection.getresponse()
        response_str = response.read()
        connection.close()
        
        if response.status == 200:
            return response_str
            
        else:
            raise Exception ('Return Non 200 Status Code %d : %s. Data = %s'
                             % (response.status, response.reason, response_str))

    def get_first(self, resource, **params):
        result = self.get(resource, **params)
        if result and result.resources:
            return result.resources[0]

        # No matches return nothing
        return None
            
    def __update(self, resource, method):
        connection = httplib.HTTPSConnection(self.hostname)
        request = resource.todocument()
        target = type(resource).__name__
        if target == "User":
            target = "RawUser"
            
        log.debug(request.toprettyxml())
        connection.request(method, '%s/%s' % (self.ws_root, target),
                           request.toxml(), self.headers)
        response = connection.getresponse()
        response_str = response.read()
        connection.close()
        if response.status == 201 or response.status == 200:
            location = response.getheader('Location')
            log.debug("Location returned: %s" % location)
            return location
        else:
            raise Exception(
                'Returned Non 201/200 Status Code: %d : %s. Data = %s'
                % (response.status, response.reason, response_str))

    def post(self, resource):
        log.info("POST of %s" % resource)
        return self.__update(resource, 'POST')

    def put(self, resource):
        log.info("PUT of %s" % resource)
        return self.__update(resource, 'PUT')

    def delete_location(self, resource):
        log.info("DELETE to %s" % resource)
        connection = httplib.HTTPSConnection(self.hostname)
        connection.request('DELETE', '%s/%s' % (self.ws_root, resource), \
                           '', self.headers)

        response = connection.getresponse()
        response_str = response.read()
        connection.close()

        if not response.status == 200:
            raise Exception(
                'Returned Non 200 Status Code: %d : %s. Data= %s'
                % (response.status, response.reason, response_str))

    def delete(self, resource):
        if resource.location:
            return self.delete_location(resource.location)
        else:
            raise Exception('Cannot DELETE as Resource has no location.')
