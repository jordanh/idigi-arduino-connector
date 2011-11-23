# update_firmware.py
# Send update_firmware SCI operation.
# It always update firmware target = 1.
# -------------------------------------------------
# The following lines require manual changes
username = "YourUsername" # enter your username
password = "YourPassword" # enter your password
device_id = "Target Device Id" # enter device id of target
# -------------------------------------------------
import httplib
import base64
import sys

def main(argv):
    #process arguments
    count = len(argv);
    if count != 1:
        print 'Usage: update_firmware.py <download_image_file>\n'
    else:
        # create HTTP basic authentication string, this consists of
        # "username:password" base64 encoded
        auth = base64.encodestring("%s:%s"%(username,password))[:-1]
        
        # open download file and encoded the data
        try: 
            fileHandle = open(argv[0], 'r')
        except IOError:
            print '\nError: cannot open ', argv[0]
            
        else:
            # build firmware download message sent to server
            message = """<sci_request version="1.0">
                <update_firmware firmware_target="0">
                    <targets>
                        <device id="%s"/>
                    </targets>
                    <data>%s</data>
                </update_firmware>
            </sci_request>
            """%(device_id, base64.encodestring("%s"%fileHandle.read())[:-1])
            
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
                
            webservice.close()
            
    

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
    