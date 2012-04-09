# ****************************************************************************
# Copyright (c) 2012 Digi International Inc., All Rights Reserved
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
    """ % (device_id, test_file, put_data, test_file, test_file)

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
    
