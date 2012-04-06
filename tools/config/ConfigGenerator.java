

import java.io.IOException;
import java.util.LinkedList;
import java.util.zip.DataFormatException;

public class ConfigGenerator {

    /* list all errors which are common to all errors  "name" "description" */ 
    private final static String[] allErrors = {"parser_error", "Parser error",
                                        "bad_xml", "Bad XML"};

    /* list global errors which are common to all errors  "name" "description" */ 
    private final static String[] globalErrors = {"invalid_version", "Invalid version"};
    
    /* list command errors which are common to all errors  "name" "description" */ 
    private final static String[] commandErrors = {"bad_group", "Bad group",
                                        "bad_index", "Bad index"};

    /* list group errors which are common to all errors  "name" "description" */ 
    private final static String[] groupErrors = {"bad_element", "Bad element",
                                        "bad_value", "Bad value"};

/* Add/update initAllErrors, initGlobalErrors, initCommandErrors, and initGroupError */
    public static LinkedList<NameStruct> allErrorList;
    public static LinkedList<NameStruct> globalErrorList;
    public static LinkedList<NameStruct> commandErrorList;
    public static LinkedList<NameStruct> groupErrorList;
    
    public final static String VERSION = "1.0.0.0";

    private static String[] args = null;

    private static boolean getErrors(LinkedList<NameStruct> linkedList, String[] error_strings) throws IOException
    {
        for (int i=0; i < error_strings.length; i++)
        {
            if (!Parser.checkAlphaNumeric(error_strings[i]))
            {
                throw new IOException("Invalid anme in error strings");
            }
            NameStruct error = new NameStruct(error_strings[i], error_strings[i+1]);
            linkedList.add(error);
            i++;
            
        }
        return true;
    }

    public static void initErrors() throws IOException
    {
        allErrorList = new LinkedList<NameStruct>();
        getErrors(allErrorList, allErrors);
        
        globalErrorList = new LinkedList<NameStruct>();
        getErrors(globalErrorList, globalErrors);
        
        commandErrorList = new LinkedList<NameStruct>();
        getErrors(commandErrorList, commandErrors);

        groupErrorList = new LinkedList<NameStruct>();
        getErrors(groupErrorList, groupErrors);
    }

    public static String getArguments()
    {
        String argString = "\"";
        for (String theArg: args)
        {
            if (theArg.equalsIgnoreCase(args[0]) &&
                theArg.indexOf(':') != -1)
            {
                argString += theArg.split(":")[0];
                argString += ":*****";
            }
            else if (theArg.equalsIgnoreCase(args[2]))
            {
                argString += "\"" + theArg + "\"";
            }
            else
            {
                argString += " " + theArg;
            }
        }
        argString += "\"";
        return argString;
    }
    
    public static void setArguments(String[] theArgs)
    {
        args = theArgs;
    }
    
    
    public static void main(String[] args) 
    {
        try {

            ConfigGenerator.setArguments(args);
            ConfigGenerator.initErrors();
            
            if (args.length != 1 && args.length != 5)
            {
                /* testing on <config_filename> for args.length == 1 */
                throw new DataFormatException("");
            }
            else
            {
                ConfigData configData = new ConfigData();
                
                Descriptors descriptors = new Descriptors(args);
                
                /* parse file */
                String filename = args[args.length -1];
                log("filename: " + filename);

                    
                Parser parser = new Parser(filename);
                
                if (!parser.processFile(configData))
                {
                    throw new IOException("Error found in file: " +filename);
                }
                if (configData.getSettingGroups().isEmpty() && configData.getStateGroups().isEmpty())
                {
                    throw new IOException("No groups specified in file: " + filename);
                }
                
                log("Number of setting groups: " + configData.getSettingGroups().size());
                log("Number of state groups: " + configData.getStateGroups().size());
                
                /* Generate H and C files */
                FileGenerator fileGenerator = new FileGenerator();
                
                try {
                    fileGenerator.generateFile(configData);
                    
                } catch (IOException e) {
                    /* TODO: Need to delete generated files */
                    log(e.getMessage());
                    throw new IOException("Unable to generate H and C files");
                }
                
                finally {
                    fileGenerator.SourceFileDone();
                } 
                
                /* Generate and upload descriptors */
                descriptors.processDescriptors(configData);
                
            }
            
        } catch (DataFormatException e) {
            
            ConfigGenerator generator = new ConfigGenerator();
            
            System.err.println(e.getMessage());
            
            log("Syntax: java -jar " + generator.getClass().getName() + ".jar" + " <username[:password]> <vendor_id> <device_type> <fw_version> <config_filename>\n");
            log("Where:");
            log("      username        = username to log in iDigi Cloud. If no password is given you will be prompted to enter the password");
            log("      password        = optional for password to log in iDigi Cloud");
            log("      vendor_id       = Vendor ID obtained from iDigi Cloud registration");
            log("      device_type     = Device type string with quotes(i.e. \"device type\")");
            log("      fw_version      = firmware version number in decimal");
            log("      config_filename = iDigi Connector Configration file");

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
