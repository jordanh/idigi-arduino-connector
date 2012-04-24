package com.digi.ic.config;

import java.io.IOException;

public class ConfigGenerator {

    public final static String VERSION = "1.0.0.0";

    private static final String NO_DESC_OPTION = "nodesc";
    private static final String HELP_OPTION = "help";
    private static final String VERBOSE_OPTION = "verbose";

    /* internal option to set different server name */
    private static final String SERVER_OPTION = "s";
    private static final String SERVER_NAME = "developer.idigi.com";

    public static final String DASH = "-";
    public static void usage()
    {
        ConfigGenerator generator = new ConfigGenerator();
        String className = generator.getClass().getName();

        int firstChar = className.lastIndexOf(".") +1;
        if (firstChar != -1)
        {
            className = className.substring(firstChar);
        }

        log("Usage: java -jar " + className + ".jar [" + DASH +  HELP_OPTION + "] [" +
                                DASH + NO_DESC_OPTION + "] [" +
                                DASH + VERBOSE_OPTION + "] <username[:password]> [vendorID] <deviceType> <firmwareVersion> <configFilename>\n");
        log("Options:");
        log("      " + DASH + HELP_OPTION +     "           = optional option to show this menu");
        log("      " + DASH + NO_DESC_OPTION + "         = optional option to exclude error description in the C file (Code size reduction)");
        log("      " + DASH + VERBOSE_OPTION +  "        = optional option to output message about what the tool is doing.");
        log("      username        = username to log in iDigi Cloud. If no password is given you will be prompted to enter the password");
        log("      password        = optional option for password to log in iDigi Cloud");
        log("      vendorID       = optional Vendor ID obtained from iDigi Cloud registration. If not given, tool tries to retrieve it from the Cloud");
        log("      deviceType     = Device type string with quotes(i.e. \"device type\")");
        log("      firmwareVersion      = firmware version number in decimal");
        log("      configFilename = iDigi Connector Configration file");
        System.exit(1);
    }

    public static String getArgumentLogString()
    {
        return argumentLog;
    }

    public static String getServerName()
    {
        return serverName;
    }

    public static void checkCommandLine(String args[]) {

        int argCount = 0;

        argumentLog = "\"";

        serverName = SERVER_NAME;

        for (String arg: args)
        {
            if (arg.startsWith(DASH))
            {
                String str = arg.substring(DASH.length());

                if (str. equals(NO_DESC_OPTION)) {
                    noErrorDescription = true;
                }
                else if (str.equals(VERBOSE_OPTION)) {
                    verboseOption = true;
                }
                else if (arg.equals(HELP_OPTION)) {
                    usage();
                }
                else
                {
                    /* split the [option]=[option value] */
                    String[] options = str.split("=", 2);

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
                argumentLog += arg + " ";
            }
            else {
                argCount++;
                if (argCount == 1)
                {
                    /* get username:password argument */
                    if (arg.indexOf(':') != -1)
                    {
                        argumentLog += arg.split(":")[0];
                        argumentLog += ":*****";
                    }
                    else
                    {
                        argumentLog += arg;
                    }
                }
                else if (argCount == (args.length -3))
                {
                    /* get device type argument */
                    argumentLog += " \"" + arg + "\"";
                }
                else
                {
                    argumentLog += " " + arg;
                }
            }
        }

        argumentLog += "\"";

        /* accept 4 or 5 arguments excluding options */

        if (argCount != 4 && argCount != 5)
        {
            usage();
        }

    }
    public static boolean excludeErrorDescription()
    {
        return noErrorDescription;
    }

    public static void main(String[] args)
    {
        try {

            /* check/get args options */
            ConfigGenerator.checkCommandLine(args);

            /* descriptor constructor for arguments */
            Descriptors descriptors = new Descriptors(args);

            /* parse file */
            String filename = args[args.length -1];
            debug_log("Reading filename: " + filename + "...");

            ConfigData configData = new ConfigData();

            Parser.processFile(filename, configData);

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

    private static String argumentLog;
    private static boolean noErrorDescription;
    private static boolean verboseOption;
    private static String serverName;

}
