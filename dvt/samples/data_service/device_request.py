# device_request.py
# Send device request 
# -------------------------------------------------
# The following lines require manual changes
username = "username" # enter your username
password = "password" # enter your password
device_id = "00000000-00000000-device-id" # enter device id of target
# -------------------------------------------------
import httplib
import base64
import re
import random

def get_random_word(wordLen):
    word = ''
    for i in range(wordLen):
        word += random.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789')
    return word
    
def send_device_request(target_name, expectedmsg, request_length):
    request_data = get_random_word(request_length);
    
    message = """<sci_request version="1.0">
        <data_service>
            <targets>
                <device id="%s"/>
            </targets>
            <requests>
            <device_request target_name="%s">%s</device_request>
            </requests>
        </data_service>
    </sci_request>
    """%(device_id, target_name, request_data)
    
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
        if re.search(expectedmsg, response_body):
            print 'Expected response: ', expectedmsg, 'for target = ', target_name
        else:
            print '\nInvalid response'
            print response_body
            
    else:
        print '\nError: %d %s' %(statuscode, statusmessage)
        print response_body
        
    webservice.close()
        

# create HTTP basic authentication string, this consists of
# "username:password" base64 encoded
auth = base64.encodestring("%s:%s"%(username,password))[:-1]

# device request message to send to server
send_device_request('invalid_target', "Message transmission cancelled", 1600);

send_device_request('iik_target', 'status=\"0\"', (1024 * 10));

send_device_request('iik_not_handle_target', 'status=\"1\"', 1024);

send_device_request('iik_cancel_target', "Message transmission cancelled", (1024 * 10));
send_device_request('iik_cancel_response_target', "Message transmission cancelled", 1600);
send_device_request('iik_cancel_response_target1', "Message transmission cancelled", 1024);

