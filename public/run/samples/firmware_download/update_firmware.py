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
# “AS IS” without warranty of any kind.  You accept sole responsibility and 
# liability for its use as well as any modifications you make.
# Restricted Rights Legend
#
# Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# ***************************************************************************
# update_firmware.py
# Send an image file to do firmware upgrade using update_firmware SCI operation.
# It updates firmware target = 0 with image file name "image.a"
# -------------------------------------------------
# The following lines require manual changes
username = "YourUsername" # enter your username
password = "YourPassword" # enter your password
device_id = "Target Device Id" # enter device id of target
# -------------------------------------------------
image_file = "image.a" # image filename

import httplib
import base64
import sys

# create HTTP basic authentication string, this consists of
# "username:password" base64 encoded
auth = base64.encodestring("%s:%s"%(username,password))[:-1]

# open download file and encoded the data
try: 
    fileHandle = open(image_file, 'r')
except IOError:
    print '\nError: cannot open ', image_file
    print 'Usage: python update_firmware.py image.a'
    
else:
    # build firmware download message sent to server
    message = """<sci_request version="1.0">
        <update_firmware firmware_target="0" filename="%s">
            <targets>
                <device id="%s"/>
            </targets>
            <data>%s</data>
        </update_firmware>
    </sci_request>
    """%(image_file, device_id, base64.encodestring("%s"%fileHandle.read())[:-1])
    
    # to what URL to send the request with a given HTTP method
    webservice = httplib.HTTP("developer.idigi.com",80)
    webservice.putrequest("POST", "/ws/sci")
    
    # add the authorization string into the HTTP header
    webservice.putheader("Authorization", "Basic %s"%auth)
    webservice.putheader("Content-type", "text/xml; charset=\"UTF-8\"")
    webservice.putheader("Content-length", "%d" % len(message))
    webservice.endheaders()
    webservice.send(message)
    
    # get the response
    statuscode, statusmessage, header = webservice.getreply()
    response_body = webservice.getfile().read()
    
    # print the output to standard out
    if statuscode == 200:
        print '\nResponse:'
        print response_body
    else:
        print '\nError: %d %s' %(statuscode, statusmessage)
        print response_body
        
    webservice.close()
            
