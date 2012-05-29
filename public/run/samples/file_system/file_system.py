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
# file_system.py
# Send SCI file_system to server. 
# -------------------------------------------------
# Usage: file_system.py <username> <password> <device_id>
# -------------------------------------------------
import httplib
import base64
import sys

expected_content = "iDigi file system sample\n"

test_file = "test_file.txt"
get_path  = test_file
put_path  = test_file
ls_path   = test_file

def Usage():
    print 'Usage: file_system.py <username> <password> <device_id>\n'
   
def PostMessage(username, password, device_id):
    # create HTTP basic authentication string, this consists of
    # "username:password" base64 encoded
    auth = base64.encodestring("%s:%s"%(username,password))[:-1]

    put_data = base64.encodestring(expected_content)[:-1]
    
    # file system message to send to server
    message = """<sci_request version="1.0">
        <file_system>
            <targets>
                <device id="%s"/>
            </targets>
            <commands>
                <put_file path="%s" offset = "0" truncate = "true">
                  <data>%s</data>
                </put_file>
                <get_file path="%s" offset = "0" length = "4294967294"/>
                <ls path="%s"/>
            </commands>
        </file_system>
    </sci_request>
    """ % (device_id, put_path, put_data, get_path, ls_path)

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
    
