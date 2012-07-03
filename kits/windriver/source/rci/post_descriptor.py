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
# send_descriptor.py
# 
# -------------------------------------------------
# Usage: post_descriptor.py <username> <password> <device_id> <device_type> <firmware_version>
# -------------------------------------------------

import httplib
import base64

import sys, getopt

username = 'whagstrom'
password = '!test!'

vendor_id = 0x02000347
device_type = 'Linux Application'
fw_version = 0x01000000

server_url = 'test.idigi.com'
   
descriptor_table = [ 
#                     {'name':'descriptor',               'file':'descriptor.xml'},
                     {'name':'descriptor/do_command',    'file':'do_command_descriptor.xml'},
#                     {'name':'descriptor/query_setting','file':'query_setting_descriptor.xml'},
#                     {'name':'descriptor/set_setting',  'file':'set_setting_descriptor.xml'},
#                     {'name':'descriptor/set_state',    'file':'set_state_descriptor.xml'},
#                     {'name':'descriptor/query_state',  'file':'query_state_descriptor.xml'},
                   ]
    
def Usage():
    print 'Usage: post_descritpor.py [-d, --delete] <username> <password> <vendor_id> <device_type> <firmware_version>'
    message = '       It deletes any existing descriptors and inserts'
    
    for descriptor in descriptor_table:
        if len(message) > 80:
            message += '\n      '
        message += ' %s (%s),' % (descriptor['name'], descriptor['file'])
    print '%s to iDigi Cloud\n' % message
    print 'Example: python post_descriptor.py username password 0x00000000 \'My device type\' 16777216\n'
    print '\nOptions:'
    print 'd, delete: Delete all descriptors\n'
   
def PostDeleteDescriptor():
    # build delete descriptors url sent to server
    deleteUrl = """/ws/DeviceMetaData?condition=dvVendorId=0x%08X and dmDeviceType='%s' and dmVersion=%d"""%(vendor_id, device_type, fw_version)
    deleteUrl = deleteUrl.replace(" ", "%20")
    
    print deleteUrl
    
    headers = {
            'Authorization': 'Basic ' \
            + base64.encodestring('%s:%s' % (username,password))[:-1]
        }
    
    connection = httplib.HTTPConnection(server_url)
#    connection.set_debuglevel(1) 

    connection.request('DELETE', '%s' %deleteUrl, '', headers)

    response = connection.getresponse()
    response_str = response.read()

    connection.close()

    print response_str
    
    if not response.status == 200:
        print 'Error in deleting descriptor. Got Status code = %d : %s' %(response.status, response.reason)
        return -1

   
def PostNewDescriptor(descriptor_name, descriptor_file):
    # create HTTP basic authentication string, this consists of
    # "username:password" base64 encoded
    auth = base64.encodestring("%s:%s"%(username,password))[:-1]
    
    # open file and encoded the data
    try: 
        fileHandle = open(descriptor_file, 'r')
    except IOError:
        print '\nError: cannot open', request_file
        Usage();
        return -1

    filedata = fileHandle.read()
    filedata = filedata.replace("<", "&lt;")
    filedata = filedata.replace(">", "&gt;")

    # build firmware download message sent to server
    message = """<DeviceMetaData><dvVendorId>0x%08X</dvVendorId>"""% vendor_id
    message += """<dmDeviceType>%s</dmDeviceType>"""% device_type
    message += """<dmVersion>%s</dmVersion>"""% fw_version
    message += """<dmName>%s</dmName>"""% descriptor_name
    message += """<dmData>%s</dmData></DeviceMetaData>"""% filedata
    
#    message = message.replace(" ", "%20")
    print message
    
    # build descriptors url sent to server
    url = "/ws/DeviceMetaData"

    headers = {
            'Content-Type' : 'text/xml',
            'Authorization': 'Basic ' \
            + base64.encodestring('%s:%s' % (username,password))[:-1]
        }
    
    connection = httplib.HTTPConnection(server_url)
#    connection.set_debuglevel(1) 

    connection.request('POST', '%s' %url, message, headers)

    response = connection.getresponse()
    response_str = response.read()
    
    connection.close()

    if not response.status == 201:
        print 'Unable to insert %s (%s)' % (descriptor_name, descriptor_file)
        print 'Got Status code = %d : %s' %(response.status, response.reason)
        print response_str
    else:
        print '%s (%s) inserted' % (descriptor_name, descriptor_file)


def main(argv):
    deleteOption = False
    #process arguments
    count = len(argv);
    print 'option count: %d' %count
    try:
        opts, args = getopt.getopt(argv, "hd", ["delete"]) 
    except getopt.getoptError:
        Usage()
        return 2
        
    for opt, arg in opts:
        if opt == '-h':
            Usage()
            return 0
        elif opt in ("-d", "--delete"):
            print "delete option\n"
            deleteOption = True
            arguments = argv[1:]
            count -= 1

    if count != 0 and count != 5:
        Usage()
        return 2
        
    if count == 5:
        global username, password
        global vendor_id, device_type, fw_version
        
        username = arguments[0]
        password = arguments[1]
        vendor_id = int(arguments[2], 16)
        device_type = arguments[3]
        fw_version = int(arguments[4], 16)
        
    print 'username = %s password = %s vendow_id = 0x%x device_type = %s fw version = 0x%x' %(username, password, vendor_id, device_type, fw_version)
    
    if deleteOption:
        if PostDeleteDescriptor() == -1:
            return  2
    else:
        for descriptor in descriptor_table:
            PostNewDescriptor(descriptor['name'], descriptor['file']);
        
if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))


