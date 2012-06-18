//package Provision;
import java.io.*;
import java.net.*;
import java.util.*;

import javax.xml.bind.DatatypeConverter;

public class Register {
    private String username;
    private String password;
    private String MacAddress;
    private String vendorId;            // vendor ID is read from iDigi
    private Boolean callDeleteFlag;
    private String errorMessage="Device registration complete";

    // Return codes
    private final int DEVICE_ADDED = 201;
    private final int MULTI_STATUS = 207;
    private final int BAD_REQUEST = 400;
    private final int INTERNAL_SERVER_ERROR = 500;

    public Register(String MacAddress) {

        this.MacAddress = MacAddress;
        System.out.println("iDigi Registering Device : " + MacAddress);
        getUserName();
        getPassword();
        RegisterDevice();
        System.out.println(getErrorMessage());
        callDeleteFlag = true;
    }

    public static void main(String args[]) {
        int num=args.length;

        new Register(args[0]);
    }

    private String sendCloudData(String target, String method, String message) {
        String response = "";
        boolean error = false;
        String cloud = "http://" + "developer.idigi.com" + target;
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
                //System.out.println(message);
                request.write(message);
                request.close();
                int responseCode = connection.getResponseCode();
                if (responseCode != 200 && responseCode != DEVICE_ADDED) {
                        switch (responseCode){
                        case MULTI_STATUS:
                                errorMessage = "Error 207 (Multi-status) A list was passed in and not all were created.";
                                break;
                        case BAD_REQUEST:
                                errorMessage = "This Device may already be registered "
                                              + "or you have reached the device limit of your account.";
                                break;
                        case INTERNAL_SERVER_ERROR:
                                errorMessage = "Error 500 (Internal server error) "
                                        + "Request could not be handled due to internal server error.";
                                break;
                        default:
                                errorMessage = "Error " + responseCode + " - " + connection.getResponseMessage();
                        }
                        error = true;
                        System.out.println(errorMessage);
                }
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
            if (target.equals("/ws/DeviceVendor")) {
                System.out.println("Failed to get the Vendor ID: Verify your account credentials");
            }
            System.exit(1);
        }

        return response;
    }

    private void getVendorId() {
        String response = sendCloudData("/ws/DeviceVendor", "GET", null);

        int startIndex = response.indexOf("<dvVendorId>");
        if (startIndex == -1) {
            System.out.println(username + " has no vendor ID, so please create the vendor ID");
            System.exit(1);
        }

        if (startIndex != response.lastIndexOf("<dvVendorId>")) {
            System.out.println(username+ " has more than one vendor ID, so please specify the correct one");
            System.exit(1);
        }

        startIndex += "<dvVendorId>".length();
        vendorId = response.substring(startIndex, response.indexOf("</dvVendorId>"));
        System.out.println("iDigi Cloud vendor ID: " + vendorId);
    }

    private void RegisterDevice() {
        String DEVICECORE_TAG_START = "<DeviceCore>";
        String DEVICECORE_TAG_END = "</DeviceCore>";
        String ID_TAG_START = "<devMac>";
        String ID_TAG_END = "</devMac>";
        String REGISTER_PAGE = "/ws/DeviceCore";

        String data  = DEVICECORE_TAG_START;
        data        += ID_TAG_START + MacAddress + ID_TAG_END;
        data        += DEVICECORE_TAG_END;

        getVendorId();

        sendCloudData(REGISTER_PAGE, "POST", data);

        try {
            createConfigurationFile();

        } catch (IOException e) {
            System.out.println("Failed to create configuration file.");
            System.exit(1);
        }
    }

    private void getPassword() {
        BufferedReader userInput = new BufferedReader(new InputStreamReader(
                System.in));

        System.out.print("Enter password: ");

        try {
            password = userInput.readLine();
            if (password.isEmpty()) {
                System.out.println("You must enter a password.\nPlease try again.");
                System.exit(1);
            }
        } catch (IOException ioe) {
            System.out.println("Failed to read password");
            System.exit(1);
        }
    }

    private void getUserName() {
        BufferedReader userInput = new BufferedReader(new InputStreamReader(
                System.in));

        System.out.print("Enter username: ");

        try {
            username = userInput.readLine();
            if (username.isEmpty()) {
                System.out.println("You must enter a username.\nPlease try again!");
                System.exit(1);
            }
        } catch (IOException ioe) {
            System.out.println("Failed to read username.");
            System.exit(1);
        }
    }

    private void createConfigurationFile() throws IOException {
        String CONFIG_FILENAME = "idigi.conf";
        BufferedWriter headerWriter;
        int i = Integer.parseInt(vendorId);

        headerWriter = new BufferedWriter(new FileWriter(CONFIG_FILENAME));

        MacAddress = MacAddress.replace(":", "");

        String config_string = "# iDigi configuration file\n\n"
                            + String.format("# Vendor ID obtained from www.idigi.com\n")
                            + String.format("vendor_id=%08x\n\n",i)
                            + String.format("# Unique device ID\n")
                            + String.format("mac_addr=%s \n\n", MacAddress)
                            + String.format("# Location of the iDigi server\n")
                            + String.format("server_url=developer.idigi.com\n\n");

        headerWriter.write(config_string);
        headerWriter.close();
    }

    public String getErrorMessage() {
        return errorMessage;
    }

    private void resetErrorMessage() {
        errorMessage = "";
    }

}
