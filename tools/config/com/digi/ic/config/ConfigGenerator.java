package com.digi.ic.config;

import java.io.IOException;
import java.util.LinkedList;

public class ConfigGenerator {

    // Constants
    public static final String DASH = "-";
    private static final String NO_EDESC_OPTION = DASH + "nodesc";
    private static final String HELP_OPTION = DASH + "help";
    private static final String VERBOSE_OPTION = DASH + "verbose";

    public final static String VERSION = "1.0.0.0";
    
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
    
    private static String[] argumentList;
    private static boolean noErrorDescription;
    private static boolean verboseOption;

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

    public static void usage()
    {
        ConfigGenerator generator = new ConfigGenerator();

        log("Usage: java -jar " + generator.getClass().getName() + ".jar [" + HELP_OPTION + "] [" + NO_EDESC_OPTION + "] [" + VERBOSE_OPTION + "] <username[:password]> [vendor_id] <device_type> <fw_version> <config_filename>\n");
        log("Options:");
        log("      " + HELP_OPTION +     "           = optional to show this menu");
        log("      " + NO_EDESC_OPTION + "         = optional to exclude error description in the C file (Code size reduction)");
        log("      " + VERBOSE_OPTION +  "        = optional to output message about what the tool is doing.");
        log("      username        = username to log in iDigi Cloud. If no password is given you will be prompted to enter the password");
        log("      password        = optional for password to log in iDigi Cloud");
        log("      vendor_id       = optional Vendor ID obtained from iDigi Cloud registration. If not given, tool tries to retrieve it from the Cloud");
        log("      device_type     = Device type string with quotes(i.e. \"device type\")");
        log("      fw_version      = firmware version number in decimal");
        log("      config_filename = iDigi Connector Configration file");
        System.exit(1);
    }
    
    public static String getArgumentListString()
    {
        String argString = "\"";
        for (String theArg: argumentList)
        {
            if (theArg.equalsIgnoreCase(argumentList[0]) &&
                theArg.indexOf(':') != -1)
            {
                argString += theArg.split(":")[0];
                argString += ":*****";
            }
            else if (theArg.equalsIgnoreCase(argumentList[2]))
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
    
    
    public static int commandLine(String args[]) {
        int i = 0;
        int n = 0;
        String arg;
  
        for (i=0; i < args.length; i++)
        {
            arg = args[i];
            if (arg.equals(NO_EDESC_OPTION)) {
                noErrorDescription = true;
            }
            if (arg.equals(VERBOSE_OPTION)) {
                verboseOption = true;
            }
            else if (arg.equals(HELP_OPTION)) {
                usage();
            }
            else {
                n++;
            }
        }
        
        /* accept 4 or 5 arguments excluding options */
        
        if (n != 4 && n != 5)
        {
            usage();
        }
        
        argumentList = args;
        return n;
    }
    public static boolean getLimitErrorDescription()
    {
        return noErrorDescription;
    }
    
    public static void main(String[] args) 
    {
        try {
            
            /* initial top-level error elements */
            ConfigGenerator.initErrors();
            
            /* check/get args options */
            ConfigGenerator.commandLine(args);

            ConfigData configData = new ConfigData();
            
            Descriptors descriptors = new Descriptors(args);
            
            /* parse file */
            String filename = args[args.length -1];
            debug_log("Reading filename: " + filename + "...");

            Parser parser = new Parser(filename);
            
            if (!parser.processFile(configData))
            {
                throw new IOException("Error found in file: " +filename);
            }
            if (configData.getSettingGroups().isEmpty() && configData.getStateGroups().isEmpty())
            {
                throw new IOException("No groups specified in file: " + filename);
            }
            
            debug_log("Number of setting groups: " + configData.getSettingGroups().size());
            debug_log("Number of state groups: " + configData.getStateGroups().size());
            
            /* Generate H and C files */
            debug_log("Generating C and H files...");
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
                
        } catch (IOException e) {
            log(e.getMessage());
            
            if (verboseOption)
            {
                e.printStackTrace();
                if (e.getCause() != null)
                    System.err.println(e.getMessage());
            }
        }
    }

    public static void log(Object aObject)
    {
        System.out.println(String.valueOf(aObject));
    }

    public static void debug_log(Object aObject)
    {
        if (verboseOption)
        {
            System.out.println(String.valueOf(aObject));
        }
    }
}
