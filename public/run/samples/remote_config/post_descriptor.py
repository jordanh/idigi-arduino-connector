# ****************************************************************************
# Copyright (c) 2011 Digi International Inc., All Rights Reserved
# 
# This software contains proprietary and confidential information of Digi
# International Inc.  By accepting transfer of this copy, Recipient agrees
# to retain this software in confidence, to prevent disclosure to others,
# and to make no use of this software other than that for which it was
# delivered.  This is an unpublished copyrighted work of Digi International
# Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
# prohibited.
# 
# Notwithstanding anything to the contrary therein you may reuse and modify 
# the script for your internal business purposes.  This script is provided 
# AS IS without warranty of any kind.  You accept sole responsibility and 
# liability for its use as well as any modifications you make.
# Restricted Rights Legend
#
# Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# ***************************************************************************
# send_descriptor.py
# 
# -------------------------------------------------
# Usage: send_descriptor.py <username> <password> <device_id> <device_type> <firmware_version>
# -------------------------------------------------

import httplib
import base64

import sys

username = 'username'
password = 'password'

vendor_id = 0x00000000
device_type = 'Device Type Name'
fw_version = 16777216

server_url = 'test.idigi.com'
   
descriptor_table = [ {'name':'descriptor',              'file':'descriptor.xml'},
                     {'name':'descriptor/query_setting','file':'query_setting_descriptor.xml'},
                     {'name':'descriptor/set_setting',  'file':'set_setting_descriptor.xml'},
#                     {'name':'descriptor/set_state',    'file':'set_state_descriptor.xml'},
#                     {'name':'descriptor/query_state',  'file':'query_state_descriptor.xml'},
                   ]
    
def Usage():
    print 'Usage: post_descritpor.py <username> <password> <vendor_id> <device_type> <firmware_version>'
    message = '       It deletes any existing descriptors and inserts'
    
    for descriptor in descriptor_table:
        if len(message) > 80:
            message += '\n      '
        message += ' %s (%s),' % (descriptor['name'], descriptor['file'])
    print '%s to iDigi Cloud\n' % message
    print 'Example: python post_descriptor.py username password 0x00000000 \'My device type\' 16777216\n'
   
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
#    print message
    
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
    else:
        print '%s (%s) inserted' % (descriptor_name, descriptor_file)


def main(argv):
    #process arguments
    count = len(argv);
    if count != 5:
        Usage()
        return 0
    else:
        global username, password
        global vendor_id, device_type, fw_version
        username = argv[0]
        password = argv[1]
        vendor_id = int(argv[2], 16)
        device_type = argv[3]
        fw_version = int(argv[4], 0)
        
    if PostDeleteDescriptor() == -1:
        return -1
        
    for descriptor in descriptor_table:
        PostNewDescriptor(descriptor['name'], descriptor['file']);
        

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))


