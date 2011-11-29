# update_firmware.py
# Send update_firmware SCI operation.
# It always update firmware target = 0 with image file name "image.a"
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
            
