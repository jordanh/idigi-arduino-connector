
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.zip.DataFormatException;

public class generate_config {

    
    public static void main(String[] args) 
    {
        final int FILE_NAME = 0;
        final int DESCRIPTOR_NAME = 1;
        
        try {
            
            if (args.length != 1 && args.length != 6 && args.length != 5)
            {
                /* testing on <config_filename> for args.length == 1 */
                throw new DataFormatException("");
            }
            else
            {
            
                ArrayList<String[]> fileConfigList = new ArrayList<String[]>();
                boolean system_info_descriptor = false;
                
                int config_index = 0;
                
                if (args.length == 6)
                {
                    /* check 4th argument for -sysinfo */ 
                    if (args[4].matches("-sysinfo:(.*)"))
                    {
                        String[] sysinfoConfig = {args[4].substring("-sysinfo:".length()), Descriptors.system_info_config_string};
                        fileConfigList.add(config_index, sysinfoConfig);
                    }
                    else
                    {
                        throw new DataFormatException("Invalid argument: " + args[4]);
                    }
                    
                    config_index++;
                }
                String[]  dataConfig = {args[args.length -1], Descriptors.data_config_string};
                fileConfigList.add(config_index, dataConfig);
                    
                Descriptors descriptors = new Descriptors(args);
                
                for (String[] fileConfig : fileConfigList)
                {
                    LinkedList<GroupStruct> groupList = new LinkedList<GroupStruct>();
                    LinkedList<NameStruct> globalErrorList = new LinkedList<NameStruct>();
                 
                    log("filename: " + fileConfig[FILE_NAME]);
                    log("descriptor: " + fileConfig[DESCRIPTOR_NAME]);
                    
                    Parser parser = new Parser(fileConfig[FILE_NAME]);
                    
                    if (!parser.processFile(groupList, globalErrorList))
                    {
                        throw new IOException("Error found in file: " + fileConfig[FILE_NAME]);
                    }
                        
                    if (!groupList.isEmpty())
                    {
                        descriptors.processDescriptors(fileConfig[DESCRIPTOR_NAME], groupList, globalErrorList);
                        if (fileConfig[DESCRIPTOR_NAME].equals(Descriptors.system_info_config_string))
                        {
                            system_info_descriptor = true;
                        }
                    }
                }
                
                descriptors.processRciDescriptors(system_info_descriptor);
            }
            
        } catch (DataFormatException e) {
            
            System.err.println(e.getMessage());

            log("Syntax: java -jar icConfigTool <username[:password]> <vendor_id> <device_type> <fw_version> [-sysinfo:<filename>] <config_filename>\n");
            log("Where:");
            log("      username        = username to log in iDigi Cloud. If no password is given you will be prompted to enter the password");
            log("      password        = optional for password to log in iDigi Cloud");
            log("      vendor_id       = Vendor ID obtained from iDigi Cloud registration");
            log("      device_type     = Device type string with quotes(i.e. \"device type\")");
            log("      fw_version      = firmware version number");
            log("      -sysinfo        = option for specifying iDigi Connector Configuration file which is given in <filename> for System Information");
            log("      config_filename = iDigi Connector Configration file for data configurations");

        } catch (IOException e) {
//            if (e.getCause() != null)
//                System.err.println(e.getMessage());
            e.printStackTrace();
        }
    }

    public static void log(Object aObject)
    {
        System.out.println(String.valueOf(aObject));
    }

}
