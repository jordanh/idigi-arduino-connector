
import java.util.LinkedList;

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
        
        /* username
        password
            ie, if (args[4].split("(.*):(.*)"))

        vendor_id
        device_type
        fw_version
        */
        /* TODO: parse arguments. ignore last 2 arguments for parser */
        
   }

    public void processDescriptors(String config_type, LinkedList<GroupStruct> groups, LinkedList<NameStruct> global_error) 
    {
        
        
        String query_descriptors = "<descriptor element=\"query_" + config_type + "\" desc=\"Retrieve device configuration\" format=\"all_" + config_type +"s_groups\">" +
                                   descriptor_error_string +
                                   "<format_define name=\"all_settings_groups\">";
        
        String set_descriptors = "<descriptor element=\"set_" + config_type + "\" desc=\"Set device configuration\" format=\"all_" + config_type + "s_groups\">" +
                                 descriptor_error_string +
                                 "</descriptor>";

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
        
        System.out.println(query_descriptors);
        System.out.println(set_descriptors);
        
        /* TODO: upload query_descriptors and set descriptors */
    }

    public void processRciDescriptors(boolean systemInfoSupport)
    {
        String descriptors = rci_descriptors;

        descriptors += "<descriptor element=\"query_" + data_config_string + "\" dscr_avail=\"true\"/>\n";
        descriptors += "<descriptor element=\"set_" + data_config_string + "\" dscr_avail=\"true\"/>\n";
      
        
        if (systemInfoSupport)
        {
            descriptors += "<descriptor element=\"set_" + system_info_config_string + "\" dscr_avail=\"true\"/>\n";
        }
        
        descriptors += rci_descriptors_errors;
        
        System.out.println(descriptors);
        
        /* TODO: upload descriptors */
    }
    
    
    private String username;
    private String password;
    private String device_type;
    private long vendor_id;
    private long fw_version;
    

}
