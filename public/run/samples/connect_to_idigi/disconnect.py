# disconnect
# Send disconnect SCI operation.
# -------------------------------------------------
# The following lines require manual changes
username = "mlchan" # enter your username
password = "mlchan" # enter your password
device_id = "00000000-00000000-78E7D1FF-FF844782" # enter device id of target
# -------------------------------------------------

import httplib
import base64

# create HTTP basic authentication string, this consists of
# "username:password" base64 encoded
auth = base64.encodestring("%s:%s"%(username,password))[:-1]

# build disconnect message sent to server
message = """<sci_request version="1.0">
    <disconnect>
        <targets>
            <device id="%s"/>
        </targets>
    </disconnect>
</sci_request>
"""%(device_id)

# to what URL to send the request with a given HTTP method
webservice = httplib.HTTP("test.idigi.com",80)
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
            
