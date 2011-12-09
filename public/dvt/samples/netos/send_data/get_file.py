# get_file.py
# Get a file from iDigi Cloud storage
# -------------------------------------------------
# The following lines require manual changes
username = "YourUsername" # enter your username
password = "YourPassword" # enter your password
device_id = "Target Device Id" # enter device id of target
# -------------------------------------------------
import httplib
import base64
   
# create HTTP basic authentication string, this consists of
# "username:password" base64 encoded
auth = base64.encodestring("%s:%s"%(username,password))[:-1]

# device request message to send to server
path = """/ws/FileData/~/%s/test/test.txt"""%(device_id)

# to what URL to send the request with a given HTTP method
webservice = httplib.HTTP("developer.idigi.com",80)
webservice.putrequest("GET", path)

# add the authorization string into the HTTP header
webservice.putheader("Authorization", "Basic %s"%auth)
webservice.putheader("Accept", "text/html; charset=\"UTF-8\"")
#webservice.putheader("Content-length", "%d" % len(message))
webservice.endheaders()
#webservice.send(message)

# get the response
statuscode, statusmessage, header = webservice.getreply()
response_body = webservice.getfile().read()

# print the output to standard out
if statuscode == 200:
    print response_body
else:
    print '\nError: %d %s' %(statuscode, statusmessage)

webservice.close()


