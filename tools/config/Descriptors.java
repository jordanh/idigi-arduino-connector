import java.io.*;
import java.net.*;
import java.util.*;

public class Descriptors {

    final static String data_config_string = "setting";
    final static String system_info_config_string = "state";
    
    private final String descriptor_error_string = "\n<error_descriptor id=\"1\" desc=\"Setting group unknown\"/>\n" +
                                                   "<error_descriptor id=\"2\" desc=\"Element not allowed under field element\"/>\n" +
                                                   "<error_descriptor id=\"3\" desc=\"Invalid setting group, index combination\"/>\n" +
                                                   "<error_descriptor id=\"4\" desc=\"Invalid parameter\"/>\n";
    private final String rci_version = "1.1";
    
    private final String rci_descriptors = "<descriptor element=\"rci_request\" desc=\"Remote Command Interface request\">\n" +
                            "<attr name=\"version\" desc=\"RCI version of request.  Response will be returned in this versions response format\" default=\"" + rci_version +"\">\n" +
                            "<value value=\"" + rci_version + "\" desc=\"Version " + rci_version + "\"/></attr>\n";
    
    private final String rci_descriptors_errors = "<error_descriptor id=\"1\" desc=\"Invalid version\"/>\n" +
                                                  "<error_descriptor id=\"2\" desc=\"Unknown command\"/>\n" +
                                                  "</descriptor>\n";

    public Descriptors(String[] args)
    {
         /*
         log("Syntax: java -jar icConfigTool <username[:password]> <vendor_id> <device_type> <fw_version> [-sysinfo:<filename>] <config_filename>");
         log("Where:");
         log("      username          = username to log in iDigi Cloud. You will be prompted to enter the password");
         log("      username:password = optional for username and password to log in iDigi Cloud");
         log("      vendor_id       = Vendor ID obtained from iDigi Cloud registration");
         log("      device_type     = Device type string with quotes(i.e. \"device type\")");
         log("      fw_version      = firmware version number");
         log("      -sysinfo        = option for specifying iDigi Connector Configuration file which is given in <filename> for System Information");
         log("      config_filename = iDigi Connector Configration file for data configurations");
         */

        int argIndex = 0;
        String credential = args[argIndex++];

        if (credential.indexOf(':') == -1)
        {
            BufferedReader userInput = new BufferedReader(new InputStreamReader(System.in));
    
            username = credential;
            System.out.print("Enter password: ");
            try {
                password = userInput.readLine();
            } catch (IOException ioe) {
                System.out.println("IO error!");
                System.exit(1);
            }
        }
        else
        {
            username = credential.split(":")[0];
            password = credential.split(":")[1];
        }

        Scanner vendorIdScan = new Scanner(args[argIndex].substring(2)); /* skip 0x if provided */
        if (vendorIdScan.hasNextInt(16))
        {
            vendorId = args[argIndex++];
        }
        else
        {
            getVendorId();
        }

        deviceType = args[argIndex++];
        fwVersion = args[argIndex];
        Scanner fwVersionScan = new Scanner(fwVersion);
        if (!fwVersionScan.hasNextInt())
        {
            System.out.println("Invalid f/w version!");
            System.exit(1);
        }

        callDeleteFlag = true;
    }

    private final String data_config_descriptor_desc = "device configuration";
    private final String sysinfo_config_descriptor_desc = "device state";
    
    public void processDescriptors(String config_type, LinkedList<GroupStruct> groups, LinkedList<NameStruct> global_error) 
    {        
        String query_descriptors = "<descriptor element=\"query_" + config_type + "\" desc=\"Retrieve ";
        if (config_type.equalsIgnoreCase(data_config_string))
            query_descriptors += data_config_descriptor_desc;
        else
            query_descriptors += sysinfo_config_descriptor_desc;
        query_descriptors += "\" format=\"all_" + config_type + "s_groups\">" +  descriptor_error_string +
                             "<format_define name=\"all_" + config_type + "s_groups\">";
        
        
        String set_descriptors = "<descriptor element=\"set_" + config_type + "\" desc=\"Set ";
        
        if (config_type.equalsIgnoreCase(data_config_string))
            set_descriptors += data_config_descriptor_desc;
        else
            set_descriptors += sysinfo_config_descriptor_desc;
         set_descriptors += "\" format=\"all_" + config_type + "s_groups\">" +
                           descriptor_error_string + "</descriptor>";

        for (GroupStruct theGroup: groups)
        {
            query_descriptors += "<descriptor element=\"" + theGroup.name + "\" desc=" + theGroup.description + ">\n";
            if (theGroup.instances > 0)
            {
                query_descriptors += "<attr name=\"index\" desc=" + theGroup.description + " type=\"int32\" min=\"1\" max=\"" + theGroup.instances + "\" />\n";
            }
            
            for (ElementStruct element: theGroup.elements)
            {
                query_descriptors += "\t<element name=\"" + element.name + "\" desc=" + element.description + " type=\"" + element.type + "\"";
                if (element.access != null)
                {
                    query_descriptors += " access=\"" + element.access + "\"";
                }
                if (element.is_min_max_needed())
                {
                    if (element.max != null)
                    {
                        query_descriptors += " max=\"" + element.max + "\"";
                    }
                    if (element.min != null)
                    {
                        query_descriptors += " min=\"" + element.min + "\"";
                    }
                }
                
                if (element.unit != null)
                {
                    query_descriptors += " unit=\"" + element.unit + "%s\"";
                }
                
                if (element.is_enum_type())
                {
                    query_descriptors += ">\n";

                    for (NameStruct value: element.values)
                    {
                        query_descriptors += "\t\t<value value=\"" + value.name + "\"";
                        if (value.description != null)
                        {
                            query_descriptors += " desc=" + value.description;
                        }
                        query_descriptors += " />\n";
                    }
                    
                    query_descriptors += "\t</element>\n";

                }
                else
                {
                    query_descriptors += " />\n";
                }
            }
            int eid = 0;
            for (NameStruct error: global_error)
            {
                eid++;
                query_descriptors += "\t<error_descriptor id=\"" + eid + "\" desc=" + error.description + " />\n";
            }
            
            for (NameStruct error: theGroup.errors)
            {
                eid++;
                query_descriptors += "\t<error_descriptor id=\"" + eid + "\" desc=" + error.description + " />\n";
            }
            query_descriptors += "</descriptor>";
        }
        query_descriptors += "</format_define>\n</descriptor>\n";
        
        System.out.println(query_descriptors);
        System.out.println(set_descriptors);
        
        uploadDescriptor("descriptor/query_" + config_type, query_descriptors);
        uploadDescriptor("descriptor/set_" + config_type, set_descriptors);
    }

    public void processRciDescriptors(boolean systemInfoSupport)
    {
        String descriptors = rci_descriptors;

        descriptors += "<descriptor element=\"query_" + data_config_string + "\" dscr_avail=\"true\"/>\n";
        descriptors += "<descriptor element=\"set_" + data_config_string + "\" dscr_avail=\"true\"/>\n";
      
        
        if (systemInfoSupport)
        {
            descriptors += "<descriptor element=\"query_" + system_info_config_string + "\" dscr_avail=\"true\"/>\n";
            descriptors += "<descriptor element=\"set_" + system_info_config_string + "\" dscr_avail=\"true\"/>\n";
        }
        
        descriptors += rci_descriptors_errors;
        
        System.out.println(descriptors);
        
        uploadDescriptor("descriptor", descriptors);
    }    

    
    private static String encode(String rawData)
    {
        final String base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" + "abcdefghijklmnopqrstuvwxyz" + "0123456789" + "+/";
        int bytesToPad = (3 - (rawData.length() % 3)) % 3;
        byte[] byteArray;

        switch (bytesToPad)
        {
            case 1:
                byteArray = (rawData + '\000').getBytes();
                break;

            case 2:
                byteArray = (rawData + '\000' + '\000').getBytes();
                break;

            default:
                byteArray = rawData.getBytes();
                break;
        }

        String encodedData = "";
        for (int i = 0; i < byteArray.length; i += 3)
        {
            int j = ((byteArray[i] & 0xff) << 16) + ((byteArray[i + 1] & 0xff) << 8) + (byteArray[i + 2] & 0xff);

            encodedData = encodedData + base64Table.charAt((j >> 18) & 0x3f) + base64Table.charAt((j >> 12) & 0x3f) + base64Table.charAt((j >> 6) & 0x3f) + base64Table.charAt(j & 0x3f);
        }

        return encodedData.substring(0, encodedData.length() - bytesToPad) + "==".substring(0, bytesToPad);
    }

    private String sendCloudData(String target, String method, String message)
    {
        String response = "";
        String cloud = "http://test.idigi.com" + target;
        String encodedCredential = encode(username + ":" + password);

        try
        {
            URL url = new URL(cloud);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod(method);
            connection.setRequestProperty("Content-Type", "text/xml");
            connection.setRequestProperty ("Authorization", "Basic " + encodedCredential);

            if (message != null)
            {
                connection.setDoOutput(true);
    
                OutputStreamWriter request = new OutputStreamWriter(connection.getOutputStream());
                request.write(message);
                request.close();
            }

            connection.connect();
            BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));
            String respLine;
            while ((respLine = reader.readLine()) != null)
            {
                response += respLine;
            }
            reader.close();
            connection.disconnect();
        }
        catch (Exception x)
        {
            System.out.println("Failed to " + method + " " + target);
            System.err.println(x);
            System.exit(1);
        }

        return response;
    }

    private void getVendorId()
    {
        String response = sendCloudData("/ws/DeviceVendor", "GET", null);

        int startIndex = response.indexOf("<dvVendorId>");
        if (startIndex == -1)
        {
            System.out.println(username + " has no vendor ID, so please create the vendor ID");
            System.exit(1);
        }

        if (startIndex != response.lastIndexOf("<dvVendorId>"))
        {
            System.out.println(username + " has more than one vendor ID, so please specify the correct one");
            System.exit(1);
        }

        startIndex += "<dvVendorId>".length();
        vendorId = response.substring(startIndex, response.indexOf("</dvVendorId>"));
        System.out.println("Found vendorID: " + vendorId);
    }

    private void uploadDescriptor(String descName, String buffer)
    {
        if (callDeleteFlag)
        {
            String target = "/ws/DeviceMetaData?condition=dvVendorId=" + vendorId + " and dmDeviceType=\'" + deviceType + "\' and dmVersion=" + fwVersion;

            String response = sendCloudData(target.replace(" ", "%20"), "DELETE", null);
            System.out.println("Deleted: " + vendorId + "/" + deviceType);
            System.out.println(response);
            callDeleteFlag = false;
        }

        String message = "<DeviceMetaData>";
        message += "<dvVendorId>" + vendorId + "</dvVendorId>";
        message += "<dmDeviceType>" + deviceType + "</dmDeviceType>";
        message += "<dmVersion>" + fwVersion + "</dmVersion>";
        message += "<dmName>" + descName + "</dmName>";
        message += "<dmData>" + buffer.replace("<", "&lt;").replace(">", "&gt;") + "</dmData>";
        message += "</DeviceMetaData>";

        String response = sendCloudData("/ws/DeviceMetaData", "POST", message);
        System.out.println("Created: " + vendorId + "/" + deviceType + "/" + descName);
        System.out.println(response);
    }

    private String username;
    private String password;
    private String deviceType;
    private String vendorId;
    private String fwVersion;
    private Boolean callDeleteFlag;
}
