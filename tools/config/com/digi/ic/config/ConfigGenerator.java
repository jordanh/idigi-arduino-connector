package com.digi.ic.config;

import java.io.IOException;

public class ConfigGenerator {

    public final static String VERSION = "1.0.0.0";
    
    public static final String DASH = "-";
    
    private static final String NO_EDESC_OPTION = DASH + "nodesc";
    private static final String HELP_OPTION = DASH + "help";
    private static final String VERBOSE_OPTION = DASH + "verbose";
    
    /* internal option to set different server name */
    private static final String SERVER_OPTION = DASH + "s";
    private static final String SERVER_NAME = "developer.idigi.com";
    
    public static void usage()
    {
        ConfigGenerator generator = new ConfigGenerator();
        String className = generator.getClass().getName();
        
        int firstChar = className.lastIndexOf(".") +1;
        if (firstChar > 0)
        {
            className = className.substring(firstChar);
        }
        
        log("Usage: java -jar " + className + ".jar [" + HELP_OPTION + "] [" + NO_EDESC_OPTION + "] [" + VERBOSE_OPTION + "] <username[:password]> [vendor_id] <device_type> <fw_version> <config_filename>\n");
        log("Options:");
        log("      " + HELP_OPTION +     "           = optional option to show this menu");
        log("      " + NO_EDESC_OPTION + "         = optional option to exclude error description in the C file (Code size reduction)");
        log("      " + VERBOSE_OPTION +  "        = optional option to output message about what the tool is doing.");
        log("      username        = username to log in iDigi Cloud. If no password is given you will be prompted to enter the password");
        log("      password        = optional option for password to log in iDigi Cloud");
        log("      vendor_id       = optional Vendor ID obtained from iDigi Cloud registration. If not given, tool tries to retrieve it from the Cloud");
        log("      device_type     = Device type string with quotes(i.e. \"device type\")");
        log("      fw_version      = firmware version number in decimal");
        log("      config_filename = iDigi Connector Configration file");
        System.exit(1);
    }
    
    public static String getArgumentListString()
    {
        return argumentList;
    }
    
    public static String getServerName()
    {
        return serverName;
    }
    
    public static int checkCommandLine(String args[]) {
        int n = 0;
  
        serverName = SERVER_NAME;
        
        for (String arg: args)
        {
            if (arg.equals(NO_EDESC_OPTION)) {
                noErrorDescription = true;
            }
            else if (arg.equals(VERBOSE_OPTION)) {
                verboseOption = true;
            }
            else if (arg.equals(HELP_OPTION)) {
                usage();
            }
            else if (arg.startsWith(DASH))
            {
                /* split the [option]=[option value] */
                String[] options = arg.split("=", 2);
                
                if ((options.length == 2) && (options[0].equals(SERVER_OPTION)))
                {
                    /* SERVER_OPTION */
                    serverName = options[1];
                }
                else
                {
                    /* no equal separator */
                    usage();
                }
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

        argumentList = "\"";

        for (String arg: args)
        {
            if (!arg.startsWith(DASH))
            {
               /* get username:password argument */
               String str = args[args.length - n];
               if (arg.equalsIgnoreCase(str) &&
                   arg.indexOf(':') != -1)
               {
                   argumentList += arg.split(":")[0];
                   argumentList += ":*****";
               }
               else if (arg.equalsIgnoreCase(args[args.length - 3]))
               {
                   /* get device type argument */
                   argumentList += " \"" + arg + "\"";
                }
                else
                {
                    argumentList += " " + arg;
                }
           }
           else
           {
               argumentList += arg + " ";
           }
        }
        argumentList += "\"";
        
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
            ConfigData.initTopLevelErrors();
            
            /* check/get args options */
            ConfigGenerator.checkCommandLine(args);

            /* descriptor constructor for arguments */
            Descriptors descriptors = new Descriptors(args);
            
            /* parse file */
            String filename = args[args.length -1];
            debug_log("Reading filename: " + filename + "...");

            ConfigData configData = new ConfigData();

            Parser.processFile(filename, configData);
            
//            if ((configData.getSettingGroups() == null) && (configData.getStateGroups() == null))
            if ((configData.getSettingGroups().isEmpty()) && (configData.getStateGroups().isEmpty()))
            {
                throw new IOException("No groups specified in file: " + filename);
            }
            
            debug_log("Number of setting groups: " + configData.getSettingGroups().size());
            debug_log("Number of state groups: " + configData.getStateGroups().size());
            
            /* Generate H and C files */
            debug_log("Generating C and H files...");
            FileGenerator.generateFile(configData);
             
            /* Generate and upload descriptors */
            debug_log("Generating/loading descriptors...");
            descriptors.processDescriptors(configData);
            
            log("Done.");
            
        } catch (Exception e) {
            log (e.toString());
            
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
    
    private static String argumentList;
    private static boolean noErrorDescription;
    private static boolean verboseOption;
    private static String serverName;

}
