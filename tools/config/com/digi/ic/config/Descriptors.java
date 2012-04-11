package com.digi.ic.config;

import java.io.*;
import java.net.*;
import java.util.*;

public class Descriptors {


    private final String RCI_VERSION = "1.1";
    
    private final String RCI_DESCRIPTORS = "<descriptor element=\"rci_request\" desc=\"Remote Command Interface request\">\n" +
                            "<attr name=\"version\" desc=\"RCI version of request.  Response will be returned in this versions response format\" default=\"" + RCI_VERSION +"\">\n" +
                            "<value value=\"" + RCI_VERSION + "\" desc=\"Version " + RCI_VERSION + "\"/></attr>\n";
    
    private final String SETTING_DESCRIPTOR_DESC = "device configuration";
    private final String STATE_DESCRIPTOR_DESC = "device state";

    public Descriptors(String[] args) throws IOException
    {
        int argIndex = 0;
        
        for (argIndex=0; argIndex < args.length; argIndex++)
        {
            if (!args[argIndex].startsWith(ConfigGenerator.DASH))
            {
                break;
            }
        }
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

        vendorId = args[argIndex];
        Scanner vendorIdScan = new Scanner(vendorId.regionMatches(true, 0, "0x", 0, 2) ? vendorId.substring(2) : vendorId); /* skip 0x if provided */
        if (vendorIdScan.hasNextInt(16))
        {
            argIndex++;
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
            ConfigGenerator.usage();
        }

        callDeleteFlag = true;
    }

    public void processDescriptors(ConfigData configData) throws IOException
    {
        
        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;
            
            String configType = type.toString().toLowerCase();
            
            try {
                
                groups = configData.getConfigGroup(configType);
                
            } catch (IOException e) {
                /* end of the ConfigData ConfigType */
                break;
            }
            
            if (!groups.isEmpty())
            {
                sendDescriptors(configType, groups, configData.getGroupGlobalErrors());
                
            }
        }
        
        sendRciDescriptors(configData);
    }

    private String getErrorDescriptors(int id, LinkedList<NameStruct> errors)
    {
        String descriptor = "";
        for (NameStruct error: ConfigData.allErrorList)
        {
            descriptor += "<error_descriptor id=\"" + id + "\" desc=\"" + error.getDescription() + "\" />\n";
            id++;
        }
        for (NameStruct error: errors)
        {
            descriptor += "<error_descriptor id=\"" + id + "\" desc=\"" + error.getDescription() + "\" />\n";
            id++;
        }
        return descriptor;
    }
    
    private void sendDescriptors(String config_type, LinkedList<GroupStruct> groups, LinkedList<NameStruct> global_error) throws IOException 
    {        
        int error_id = 1;
        String desc = SETTING_DESCRIPTOR_DESC;
        
        if (config_type.equalsIgnoreCase(ConfigData.ConfigType.STATE.toString()))
            desc = STATE_DESCRIPTOR_DESC;
        
        String query_descriptors = "<descriptor element=\"query_" + config_type + "\" desc=\"Retrieve " + desc;
        
        query_descriptors += "\" format=\"all_" + config_type + "s_groups\">\n";
        

        query_descriptors += "<format_define name=\"all_" + config_type + "s_groups\">\n";
        
        query_descriptors += getErrorDescriptors(error_id, ConfigData.commandErrorList);
        
        
        String set_descriptors = "<descriptor element=\"set_" + config_type + "\" desc=\"Set " + desc;
        
        set_descriptors += "\" format=\"all_" + config_type + "s_groups\">";
         
        set_descriptors += getErrorDescriptors(error_id, ConfigData.commandErrorList);

        set_descriptors += "</descriptor>";

        for (GroupStruct theGroup: groups)
        {
            query_descriptors += "<descriptor element=\"" + theGroup.getName() + "\" desc=" + theGroup.getDescription() + ">\n";
            if (theGroup.getInstances() > 0)
            {
                query_descriptors += "<attr name=\"index\" desc=" + theGroup.getDescription() + " type=\"int32\" min=\"1\" max=\"" + theGroup.getInstances() + "\" />\n";
            }
            
            for (ElementStruct element: theGroup.getElements())
            {
                query_descriptors += "<element name=\"" + element.getName() + "\" desc=" + element.getDescription() + " type=\"" + element.getType() + "\"";
                if (element.getAccess() != null)
                {
                    query_descriptors += " access=\"" + element.getAccess() + "\"";
                }
                if (element.getMax() != null)
                {
                    query_descriptors += " max=\"" + element.getMax() + "\"";
                }
                if (element.getMin() != null)
                {
                    query_descriptors += " min=\"" + element.getMin() + "\"";
                }
                
                if (element.getUnit() != null)
                {
                    query_descriptors += " unit=" + element.getUnit() + "";
                }
                
                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                {
                    query_descriptors += ">\n";

                    for (NameStruct value: element.getValues())
                    {
                        query_descriptors += "<value value=\"" + value.getName() + "\"";
                        if (value.getDescription() != null)
                        {
                            query_descriptors += " desc=" + value.getDescription();
                        }
                        query_descriptors += " />\n";
                    }
                    
                    query_descriptors += "</element>\n";

                }
                else
                {
                    query_descriptors += " />\n";
                }
            }
            
            error_id = 1;
            query_descriptors += getErrorDescriptors(error_id, ConfigData.groupErrorList);
             
            for (NameStruct error: theGroup.getErrors())
            {
                error_id++;
                query_descriptors += "<error_descriptor id=\"" + error_id + "\" desc=" + error.getDescription() + " />\n";
            }
            query_descriptors += "</descriptor>";
        }
        query_descriptors += "</format_define>\n</descriptor>\n";
        
        debug_log(query_descriptors);
        debug_log(set_descriptors);
        
        uploadDescriptor("descriptor/query_" + config_type, query_descriptors);
        uploadDescriptor("descriptor/set_" + config_type, set_descriptors);
    }

    private void sendRciDescriptors(ConfigData configData) throws IOException
    {
        String descriptors = RCI_DESCRIPTORS;
        
        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;
            
            String configType = type.toString().toLowerCase();
            
            try {
                
                groups = configData.getConfigGroup(configType);
                
            } catch (IOException e) {
                /* end of the ConfigData ConfigType */
                break;
            }
            if (!groups.isEmpty())
            {
                descriptors += "<descriptor element=\"query_" + configType + "\" dscr_avail=\"true\"/>\n";
                descriptors += "<descriptor element=\"set_" + configType + "\" dscr_avail=\"true\"/>\n";
            }
        }
        
        int error_id = 1;
        descriptors += getErrorDescriptors(error_id, ConfigData.globalErrorList);
        descriptors += "</descriptor>";
        
        debug_log(descriptors);
        
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

    
    private void debug_log(String str) throws IOException
    {
        
        String filename = "descritor" + xmlFileIndex + ".xml";
        xmlFileIndex++;
        
        BufferedWriter xmlFile = new BufferedWriter(new FileWriter(filename));

        xmlFile.write("<descriptors>");
        xmlFile.write(str);
        xmlFile.write("</descriptors>");
        
        xmlFile.flush();
        xmlFile.close();

        ConfigGenerator.debug_log(str);
    }
    
    private int xmlFileIndex;
    private String username;
    private String password;
    private String deviceType;
    private String vendorId;
    private String fwVersion;
    private Boolean callDeleteFlag;
}
