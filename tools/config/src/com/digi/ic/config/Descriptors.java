package com.digi.ic.config;

import java.io.*;
import java.net.*;
import java.util.*;

import javax.xml.bind.DatatypeConverter;

public class Descriptors {

    private final String RCI_VERSION = "1.1";

    private final String RCI_DESCRIPTORS = "<descriptor element=\"rci_request\" desc=\"Remote Command Interface request\">\n"
            + "<attr name=\"version\" desc=\"RCI version of request.  Response will be returned in this versions response format\" default=\""
            + RCI_VERSION
            + "\">\n"
            + "<value value=\""
            + RCI_VERSION
            + "\" desc=\"Version " + RCI_VERSION + "\"/></attr>\n";

    private final String SETTING_DESCRIPTOR_DESCRIPTION = "device configuration";
    private final String STATE_DESCRIPTOR_DESCRIPTION = "device state";

//    private int xmlFileIndex;
    private final String username;
    private final String password;
    private final String deviceType;
    private String vendorId;
    private final long fwVersion;
    private Boolean callDeleteFlag;

    public Descriptors(final String username, final String password,
                       final String vendorId, final String deviceType, 
                       final long version) {
        this.username = username;
        this.password = password;
        this.deviceType = deviceType;
        this.fwVersion = version;

        this.vendorId = vendorId;
        if (vendorId == null) getVendorId();

        callDeleteFlag = true;
    }

    public void processDescriptors(ConfigData configData) throws Exception {

        for (ConfigData.ConfigType type : ConfigData.ConfigType.values()) {
            LinkedList<GroupStruct> groups = null;

            String configType = type.toString().toLowerCase();

            try {

                groups = configData.getConfigGroup(configType);

            } catch (Exception e) {
                /* end of the ConfigData ConfigType */
                break;
            }

            if (!groups.isEmpty()) sendDescriptors(configType, groups, configData);
        }

        sendRciDescriptors(configData);
    }

    private String getErrorDescriptors(int id, LinkedHashMap<String, String> errorMap) {
        String descriptor = "";
        int errorId = id;

        for (String errorName : errorMap.keySet()) {
            descriptor += String.format("<error_descriptor id=\"%d\" ", errorId);
            if (errorMap.get(errorName) != null)
                descriptor += String.format("desc=\"%s\" ", errorMap.get(errorName));
            
            descriptor += "/>\n";
                
            errorId++;
        }

        return descriptor;
    }

    private void sendDescriptors(String config_type, LinkedList<GroupStruct> groups, ConfigData configData) throws Exception {
        String desc = SETTING_DESCRIPTOR_DESCRIPTION;

        if (config_type.equalsIgnoreCase(ConfigData.ConfigType.STATE.toString()))
            desc = STATE_DESCRIPTOR_DESCRIPTION;

        /* setup query command descriptor */
        String query_descriptors = String.format("<descriptor element=\"query_%s\" desc=\"Retrieve %s\" format=\"all_%ss_groups\">\n",
                                                 config_type, desc, config_type)
                                   + String.format("<format_define name=\"all_%ss_groups \">\n", config_type);

        /*
         * get all errors for query command descriptor. 1. common errors. 2.
         * command errors. 3. user global errors
         * 
         * We must offset the error_id for command errors.
         */
        query_descriptors += getErrorDescriptors(configData.getRciCommonErrorsIndex(), configData.getRciCommonErrors())
                             + getErrorDescriptors(configData.getRciCommandErrorsIndex(), configData.getRciCommandErrors())
                             + getErrorDescriptors(configData.getUserGlobalErrorsIndex(), configData.getUserGlobalErrors());

        String set_descriptors = String.format("<descriptor element=\"set_%s\" desc=\"Set %s\" format=\"all_%ss_groups\">\n",
                                                config_type, desc, config_type);
    
        /*
         * get all errors for set command descriptor. 1. common errors. 2.
         * command errors. 3. user global errors
         * 
         * We must offset the error_id for command errors.
         */
        set_descriptors += getErrorDescriptors(configData.getRciCommonErrorsIndex(), configData.getRciCommonErrors())
                           + getErrorDescriptors(configData.getRciCommandErrorsIndex(), configData.getRciCommandErrors())
                           + getErrorDescriptors(configData.getUserGlobalErrorsIndex(), configData.getUserGlobalErrors());

        set_descriptors += "</descriptor>";

        for (GroupStruct group : groups) {

            query_descriptors += group.toString();

            if (group.getInstances() > 1) {
                query_descriptors += String.format("<attr name=\"index\" desc=\"%s\" type=\"int32\" min=\"1\" max=\"%d\" />",
                                                    group.getDescription(), group.getInstances());
            }

            for (ElementStruct element : group.getElements()) {

                query_descriptors += element.toString();
                
                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM) {

                    for (ValueStruct value : element.getValues()) {
                        query_descriptors += value.toString();
                    }

                    query_descriptors += "</element>\n";

                }
            }

            /*
             * Write error for individual groups 1. common errors 2. group
             * errors 3. user global error 4. user group error
             * 
             * We must offset the error id for group errors.
             */
            query_descriptors += getErrorDescriptors(configData.getRciCommonErrorsIndex(), configData.getRciCommonErrors())
                                 + getErrorDescriptors(configData.getRciGroupErrorsIndex(), configData.getRciGroupErrors())
                                 + getErrorDescriptors(configData.getUserGlobalErrorsIndex(), configData.getUserGlobalErrors());

            query_descriptors += getErrorDescriptors(configData.getAllErrorsSize() + 1, group.getErrors());

            query_descriptors += "</descriptor>";
        }
        query_descriptors += "</format_define>\n</descriptor>\n";

        debug_log(query_descriptors);
        debug_log(set_descriptors);

        uploadDescriptor("descriptor/query_" + config_type, query_descriptors);
        uploadDescriptor("descriptor/set_" + config_type, set_descriptors);
    }

    private void sendRciDescriptors(ConfigData configData) throws IOException {
        String descriptors = RCI_DESCRIPTORS;

        for (ConfigData.ConfigType type : ConfigData.ConfigType.values()) {
            LinkedList<GroupStruct> groups = null;

            String configType = type.toString().toLowerCase();

            try {

                groups = configData.getConfigGroup(configType);

            } catch (Exception e) {
                /* end of the ConfigData ConfigType */
                break;
            }
            if (!groups.isEmpty()) {
                descriptors += String.format("<descriptor element=\"query_%s\" dscr_avail=\"true\" />\n", configType)
                               + String.format("<descriptor element=\"set_%s\" dscr_avail=\"true\" />\n", configType);
            }
        }

        descriptors += getErrorDescriptors(configData.getRciCommonErrorsIndex(), configData.getRciCommonErrors())
                       + getErrorDescriptors(configData.getRciGlobalErrorsIndex(), configData.getRciGlobalErrors()) 
                       + "</descriptor>";

        debug_log(descriptors);

        uploadDescriptor("descriptor", descriptors);
    }

    private String sendCloudData(String target, String method, String message) {
        String response = "";
        String cloud = "http://" + ConfigGenerator.getServerName() + target;
        String credential = username + ":" + password;
        String encodedCredential = DatatypeConverter.printBase64Binary(credential.getBytes());

        try {
            URL url = new URL(cloud);
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod(method);
            connection.setRequestProperty("Content-Type", "text/xml");
            connection.setRequestProperty("Authorization", "Basic " + encodedCredential);

            if (message != null) {
                connection.setDoOutput(true);

                OutputStreamWriter request = new OutputStreamWriter(connection.getOutputStream());
                request.write(message);
                request.close();
            }

            connection.connect();
            BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));
            String respLine;

            while ((respLine = reader.readLine()) != null) {
                response += respLine;
            }
            reader.close();
            connection.disconnect();
            
        } catch (Exception x) {
            ConfigGenerator.log("Failed to " + method + " " + target);
            System.err.println(x);
            System.exit(1);
        }

        return response;
    }

    private void getVendorId() {
        String response = sendCloudData("/ws/DeviceVendor", "GET", null);

        int startIndex = response.indexOf("<dvVendorId>");
        if (startIndex == -1) {
            ConfigGenerator.log(username + " has no vendor ID, so please create the vendor ID");
            System.exit(1);
        }

        if (startIndex != response.lastIndexOf("<dvVendorId>")) {
            ConfigGenerator.log(username+ " has more than one vendor ID, so please specify the correct one");
            System.exit(1);
        }

        startIndex += "<dvVendorId>".length();
        vendorId = response.substring(startIndex, response.indexOf("</dvVendorId>"));
        ConfigGenerator.log("iDigi Cloud registered vendor ID: " + vendorId);
    }

    private String tagMessageSegment(String tagName, String value) {
        return "<" + tagName + ">" + value + "</" + tagName + ">";
    }

    private String replaceXmlEntities(String buffer) {
        return buffer.replace("<", "&lt;").replace(">", "&gt;");
    }

    private void uploadDescriptor(String descName, String buffer) {
        ConfigGenerator.debug_log("Uploading description:" + descName);

        if (callDeleteFlag) {
            String target = String.format("/ws/DeviceMetaData?condition=dvVendorId=%s and dmDeviceType=\'%s\' and dmVersion=%d",
                                            vendorId, deviceType, fwVersion);

            String response = sendCloudData(target.replace(" ", "%20"),
                    "DELETE", null);
            ConfigGenerator
                    .debug_log("Deleted: " + vendorId + "/" + deviceType);
            ConfigGenerator.debug_log(response);
            callDeleteFlag = false;
        }

        String message = "<DeviceMetaData>";
        message += tagMessageSegment("dvVendorId", vendorId);
        message += tagMessageSegment("dmDeviceType", deviceType);
        message += tagMessageSegment("dmVersion", String.format("%d", fwVersion));
        message += tagMessageSegment("dmName", descName);
        message += tagMessageSegment("dmData", replaceXmlEntities(buffer));
        message += "</DeviceMetaData>";

        String response = sendCloudData("/ws/DeviceMetaData", "POST", message);
        ConfigGenerator.debug_log("Created: " + vendorId + "/" + deviceType + "/" + descName);
        ConfigGenerator.debug_log(response);
    }

    private void debug_log(String str) throws IOException {
        // debugging code

/*        String filename = "descritor" + xmlFileIndex + ".xml";
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

}
