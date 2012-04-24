package com.digi.ic.config;

import java.io.*;
import java.net.*;
import java.util.*;
import java.util.regex.Pattern;

import javax.xml.bind.DatatypeConverter;

public class Descriptors {


    private final String RCI_VERSION = "1.1";

    private final String RCI_DESCRIPTORS = "<descriptor element=\"rci_request\" desc=\"Remote Command Interface request\">\n" +
                            "<attr name=\"version\" desc=\"RCI version of request.  Response will be returned in this versions response format\" default=\"" + RCI_VERSION +"\">\n" +
                            "<value value=\"" + RCI_VERSION + "\" desc=\"Version " + RCI_VERSION + "\"/></attr>\n";

    private final String SETTING_DESCRIPTOR_DESCRIPTION = "device configuration";
    private final String STATE_DESCRIPTOR_DESCRIPTION = "device state";

    public Descriptors(String[] args) throws IOException
    {
        int argIndex;

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
                ConfigGenerator.log("IO error!");
                System.exit(1);
            }
        }
        else
        {
            String [] userpass = credential.split(":");

            username = userpass[0];
            password = userpass[1];
        }

        vendorId = args[argIndex];
        if (Pattern.matches("(0[xX])?\\p{XDigit}+", vendorId))
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
            ConfigGenerator.log("Invalid f/w version!");
            ConfigGenerator.usage();
        }

        callDeleteFlag = true;
    }

    public void processDescriptors(ConfigData configData) throws Exception
    {

        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;

            String configType = type.toString().toLowerCase();

            try {

                groups = configData.getConfigGroup(configType);

            } catch (Exception e) {
                /* end of the ConfigData ConfigType */
                break;
            }

            if (!groups.isEmpty())
            {
                sendDescriptors(configType, groups, configData);

            }
        }

        sendRciDescriptors(configData);
    }

    private String getErrorDescriptors(int id, LinkedHashMap<String, String> errorMap)
    {
        String descriptor = "";
        int errorId = id;

        for (String errorName : errorMap.keySet())
        {
            descriptor += "<error_descriptor id=\"" + errorId + "\" desc=\"" + errorMap.get(errorName) + "\" />\n";
            errorId++;
        }

        return descriptor;
    }

    private void sendDescriptors(String config_type, LinkedList<GroupStruct> groups, ConfigData configData) throws Exception
    {
        String desc = SETTING_DESCRIPTOR_DESCRIPTION;

        if (config_type.equalsIgnoreCase(ConfigData.ConfigType.STATE.toString()))
            desc = STATE_DESCRIPTOR_DESCRIPTION;

        /* setup query command descriptor */
        String query_descriptors = "<descriptor element=\"query_" + config_type + "\" desc=\"Retrieve " + desc;

        query_descriptors += "\" format=\"all_" + config_type + "s_groups\">\n";

        query_descriptors += "<format_define name=\"all_" + config_type + "s_groups\">\n";


        /*
         * get all errors for query command descriptor.
         * 1. common errors.
         * 2. command errors.
         * 3. user glboal errors
         *
         * We must offset the error_id for command errors.
         */
        query_descriptors += getErrorDescriptors(ConfigData.getRciCommonErrorsIndex(), ConfigData.getRciCommonErrors()) +
                             getErrorDescriptors(ConfigData.getRciCommandErrorsIndex(), ConfigData.getRciCommandErrors()) +
                             getErrorDescriptors(ConfigData.getUserGlobalErrorsIndex(), ConfigData.getUserGlobalErrors());


        String set_descriptors = "<descriptor element=\"set_" + config_type + "\" desc=\"Set " + desc;

        set_descriptors += "\" format=\"all_" + config_type + "s_groups\">";

        /*
         * get all errors for set command descriptor.
         * 1. common errors.
         * 2. command errors.
         * 3. user glboal errors
         *
         * We must offset the error_id for command errors.
         */
        set_descriptors += getErrorDescriptors(ConfigData.getRciCommonErrorsIndex(), ConfigData.getRciCommonErrors()) +
                           getErrorDescriptors(ConfigData.getRciCommandErrorsIndex(), ConfigData.getRciCommandErrors()) +
                           getErrorDescriptors(ConfigData.getUserGlobalErrorsIndex(), ConfigData.getUserGlobalErrors());

        set_descriptors += "</descriptor>";

        for (GroupStruct theGroup: groups)
        {
            query_descriptors += "<descriptor element=\"" + theGroup.getName() + "\" desc=\"" + theGroup.getDescription() + "\">\n";
            if (theGroup.getInstances() > 1)
            {
                query_descriptors += "<attr name=\"index\" desc=\"" + theGroup.getDescription() + "\" type=\"int32\" min=\"1\" max=\"" + theGroup.getInstances() + "\" />\n";
            }

            for (ElementStruct element: theGroup.getElements())
            {
                query_descriptors += "<element name=\"" + element.getName() + "\" desc=\"" + element.getDescription() + "\" type=\"" + element.getType() + "\"";
                if (element.getAccess() != null)
                {
                    query_descriptors += String.format(" access=\"%s\"", element.getAccess());
                }
                if (element.getMax() != null)
                {
                    query_descriptors += String.format(" max=\"%s\"", element.getMax());
                }
                if (element.getMin() != null)
                {
                    query_descriptors += String.format(" min=\"%s\"", element.getMin());
                }

                if (element.getUnit() != null)
                {
                    query_descriptors += String.format(" unit=\"%s\"", element.getUnit());
                }

                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                {
                    query_descriptors += ">\n";

                    LinkedHashMap<String, String> valueMap = element.getValues();
                    for (String key : valueMap.keySet())
                    {
                        query_descriptors += String.format("<value value=\"%s\"", key);
                        if (valueMap.get(key) != null)
                        {
                            query_descriptors += String.format(" desc=\"%s\"", valueMap.get(key));
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

            /*
             * Write error for individual groups
             * 1. common errors
             * 2. group errors
             * 3. user global error
             * 4. user group error
             *
             * We must offset the error id for group errors.
             */
            query_descriptors += getErrorDescriptors(ConfigData.getRciCommonErrorsIndex(), ConfigData.getRciCommonErrors()) +
                                 getErrorDescriptors(ConfigData.getRciGroupErrorsIndex(), ConfigData.getRciGroupErrors()) +
                                 getErrorDescriptors(ConfigData.getUserGlobalErrorsIndex(), ConfigData.getUserGlobalErrors());

            query_descriptors += getErrorDescriptors(ConfigData.getAllErrorsSize()+1, theGroup.getErrors());

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

            } catch (Exception e) {
                /* end of the ConfigData ConfigType */
                break;
            }
            if (!groups.isEmpty())
            {
                descriptors += "<descriptor element=\"query_" + configType + "\" dscr_avail=\"true\"/>\n";
                descriptors += "<descriptor element=\"set_" + configType + "\" dscr_avail=\"true\"/>\n";
            }
        }

        descriptors += getErrorDescriptors(ConfigData.getRciCommonErrorsIndex(), ConfigData.getRciCommonErrors()) +
                       getErrorDescriptors(ConfigData.getRciGlobalErrorsIndex(), ConfigData.getRciGlobalErrors()) +
                       "</descriptor>";

        debug_log(descriptors);

        uploadDescriptor("descriptor", descriptors);
    }

    private String sendCloudData(String target, String method, String message)
    {
        String response = "";
        String cloud = "http://" + ConfigGenerator.getServerName() + target;
        String credential = username + ":" + password;
        String encodedCredential = DatatypeConverter.printBase64Binary(credential.getBytes());

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
            ConfigGenerator.log("Failed to " + method + " " + target);
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
            ConfigGenerator.log(username + " has no vendor ID, so please create the vendor ID");
            System.exit(1);
        }

        if (startIndex != response.lastIndexOf("<dvVendorId>"))
        {
            ConfigGenerator.log(username + " has more than one vendor ID, so please specify the correct one");
            System.exit(1);
        }

        startIndex += "<dvVendorId>".length();
        vendorId = response.substring(startIndex, response.indexOf("</dvVendorId>"));
        ConfigGenerator.log("iDigi Cloud registered vendor ID: " + vendorId);
    }

    private String tagMessageSegment(String tagName, String value)
    {
        return "<" + tagName + ">" + value + "</" + tagName + ">";
    }

    private String replaceXmlEntities(String buffer)
    {
        return buffer.replace("<", "&lt;").replace(">", "&gt;");
    }

    private void uploadDescriptor(String descName, String buffer)
    {
        ConfigGenerator.debug_log("Uploading description:" + descName);

        if (callDeleteFlag)
        {
            String target = "/ws/DeviceMetaData?condition=dvVendorId=" + vendorId + " and dmDeviceType=\'" + deviceType + "\' and dmVersion=" + fwVersion;

            String response = sendCloudData(target.replace(" ", "%20"), "DELETE", null);
            ConfigGenerator.debug_log("Deleted: " + vendorId + "/" + deviceType);
            ConfigGenerator.debug_log(response);
            callDeleteFlag = false;
        }

        String message = "<DeviceMetaData>";
        message += tagMessageSegment("dvVendorId", vendorId);
        message += tagMessageSegment("dmDeviceType", deviceType);
        message += tagMessageSegment("dmVersion", fwVersion);
        message += tagMessageSegment("dmName", descName);
        message += tagMessageSegment("dmData", replaceXmlEntities(buffer));
        message += "</DeviceMetaData>";

        String response = sendCloudData("/ws/DeviceMetaData", "POST", message);
        ConfigGenerator.debug_log("Created: " + vendorId + "/" + deviceType + "/" + descName);
        ConfigGenerator.debug_log(response);
    }

    private void debug_log(String str) throws IOException
    {
// debugging code
/*
        String filename = "descritor" + xmlFileIndex + ".xml";
        xmlFileIndex++;

        BufferedWriter xmlFile = new BufferedWriter(new FileWriter(filename));

        xmlFile.write("<descriptors>");
        xmlFile.write(str);
        xmlFile.write("</descriptors>");

        xmlFile.flush();
        xmlFile.close();
*/
        ConfigGenerator.debug_log(str);
    }

//    private int xmlFileIndex;
    private String username;
    private String password;
    private final String deviceType;
    private String vendorId;
    private final String fwVersion;
    private Boolean callDeleteFlag;
}
