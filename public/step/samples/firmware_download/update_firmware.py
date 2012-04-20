# ***************************************************************************
# Copyright (c) 1996-2012 Digi International Inc.,
# All rights not expressly granted are reserved.
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# ***************************************************************************
# It updates firmware target = 0 with image file name "image.a"
# -------------------------------------------------
# Usage: update_firmware.py <username> <password> <device_id>
# -------------------------------------------------
image_file = "image.a" # image filename

import httplib
import base64
import sys

def Usage():
    print 'Usage: update_firmware.py <username> <password> <device_id>'
    print '       It opens \"image.a\" file and sends firmware update to device on target 0\n'  
   
def PostMessage(username, password, device_id):
    # create HTTP basic authentication string, this consists of
    # "username:password" base64 encoded
    auth = base64.encodestring("%s:%s"%(username,password))[:-1]
    
    # open download file and encoded the data
    try: 
        fileHandle = open(image_file, 'r')
    except IOError:
        print '\nError: cannot open', image_file
        Usage();
        return -1
        

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


def main(argv):
    #process arguments
    count = len(argv);
    if count != 3:
        Usage()
    else:
        PostMessage(argv[0], argv[1], argv[2])


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))


