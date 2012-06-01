package com.digi.ic.config;

import java.io.IOException;
import java.util.LinkedList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Parser {

    private final static int MAX_DESCRIPTION_LENGTH = 40;

    // PRIVATE variables
    private static TokenScanner tokenScanner;
    private static boolean isReadToken;
    private static String token;
    private static int groupLineNumber;
    private static int elementLineNumber;
    private static LinkedList<GroupStruct> groupConfig;

    
    public static void processFile(String fileName, ConfigData configData) throws IOException, NullPointerException {

        try {
            tokenScanner = new TokenScanner(fileName);

            token = null;
            isReadToken = true;

            /* first use a Scanner to get each word */
            while (!isReadToken || tokenScanner.hasToken()) {
                if (isReadToken) {
                    token = tokenScanner.getToken();
                }

                if (token.equalsIgnoreCase("globalerror")) {
                    configData.addUserGroupError(getName(), getLongDescription());
                    isReadToken = true;
                    
                } else if (token.equalsIgnoreCase("group")) {
                    /*
                     * syntax for parsing group: group setting or state <name>
                     * [instances] <description> [help description]
                     */
                    groupConfig = configData.getConfigGroup(tokenScanner.getToken());

                    /* parse name */
                    String nameStr = getName();

                    groupLineNumber = tokenScanner.getLineNumber();

                    /* parse instances */
                    int groupInstances = 1;

                    if (tokenScanner.hasTokenInt()) {
                        groupInstances = tokenScanner.getTokenInt();
                    }

                    GroupStruct theGroup = new GroupStruct(nameStr, groupInstances, getDescription(), getLongDescription());

                    isReadToken = true;
                    /*
                     * Parse elements and errors for the group.
                     */
                    while (tokenScanner.hasToken()) {
                        if (isReadToken) {
                            token = tokenScanner.getToken();
                            isReadToken = false; /*
                                                  * token is already obtained
                                                  * from processElement
                                                  */
                        }
                        
                        if (token.equalsIgnoreCase("element")) {
                            ElementStruct element = processElement();

                            try{
                                element.validate();
                            }
                            catch(Exception e){
                                throw new Exception("Error in <element>: " + element.getName() + "\n\t" + e.getMessage());
                            }

                            theGroup.addElement(element);
                            
                        } else if (token.equalsIgnoreCase("error")) {
                            theGroup.addError(getName(), getLongDescription());
                            isReadToken = true;
                            
                        } else {
                            break;
                        }
                    }

                    try{
                        theGroup.validate();
                    }
                    catch(Exception e){
                        throw new Exception("Error in <group>: " + theGroup.getName() + "\n\t" + e.getMessage());
                    }

                    for (GroupStruct g : groupConfig) {
                        
                        if (g.getName().equals(theGroup.getName())) {
                            throw new Exception("Duplicate <group>: " + theGroup.getName());
                        }
                    }
                    groupConfig.add(theGroup);

                    if (!tokenScanner.hasToken()) {
                        /* end of file */
                        break;
                    }

                } else {
                    throw new Exception("Unrecogized keyword: " + token);
                }
            }
        } catch (NullPointerException e) {
            ConfigGenerator.log("Parser NullPointerException");
            ConfigGenerator.log(e.toString());
            throw new NullPointerException();

        } catch (Exception e) {
            throw new IOException(errorFoundLog(fileName, e.getMessage()));
        }
        finally {
            tokenScanner.close();
        }

    }

    private static String errorFoundLog(String fileName, String str) {
        String message = "Error found in " + fileName + ", line ";

        if (str.indexOf("<group>") != -1)
            message += groupLineNumber;
        else if (str.indexOf("<element>") != -1)
            message += elementLineNumber;
        else
            message += tokenScanner.getLineNumber();

        return message + ": " + str;
    }

    private final static Pattern ALPHACHARACTERS = Pattern.compile("\\w+");

    public static boolean checkAlphaCharacters(String s) {
        if (s == null) {
            return false;
        } else {
            Matcher m = ALPHACHARACTERS.matcher(s);
            return m.matches();
        }
    }

    private static String getName() throws Exception {
        String name = tokenScanner.getToken(); // tokenScanner.next();

        if (name == null) {
            throw new Exception("Missing name!");
        }

        if (!checkAlphaCharacters(name)) {
            throw new Exception("Invalid character in the name: " + name);
        }
        return name;
    }

    private static String getDescription() throws Exception {

        String description = null;
        if (tokenScanner.hasToken("\\\".*")) {
            description = tokenScanner.getTokenInLine("\\\".*?\\\"");
            if (description == null) {
                throw new Exception("Invalid description");
            }

            description = description.substring(1, description
                    .lastIndexOf("\""));

            if (description.length() > MAX_DESCRIPTION_LENGTH) {
                throw new Exception("desciption > maximum length "
                        + MAX_DESCRIPTION_LENGTH);
            }
            description = description.replace(":", "::");

            if (description.length() == 0)
                description = null;

        }
        return description;
    }

    private static String getLongDescription() throws Exception {

        String description = null;
        if (tokenScanner.hasToken("\\\".*")) {
            description = tokenScanner.getTokenInLine("\\\".*?\\\"");
            if (description == null) {
                throw new Exception("Invalid error description");
            }

            description = description.substring(1, description
                    .lastIndexOf("\""));
            description = description.replace(":", "::");

            if (description.length() == 0)
                description = null;
        }
        return description;
    }

    private static String getType() throws Exception {
        String type = tokenScanner.getToken();

        if (type == null) {
            throw new Exception("Missing type");
        }
        return type;
    }

    private static String getAccess() throws Exception {
        String access = tokenScanner.getToken();

        if (access == null) {
            throw new Exception("Missing access!");

        }
        ElementStruct.AccessType.toAccessType(access);

        return access;
    }

    private static String getMinMax() throws Exception {
        String mvalue = tokenScanner.getToken();

        if (mvalue == null) {
            throw new Exception("Missing min or max value");
        }
        

        return mvalue;
    }

    private static final ElementStruct processElement() throws Exception {
        /*
         * syntax for parsing element: element <name> <description> [help
         * description] type <type> [min <min>] [max <max>] [access <access>]
         * [unit <unit>]
         */
        ElementStruct element = new ElementStruct(getName(), getDescription(), getLongDescription());
        
        elementLineNumber = tokenScanner.getLineNumber();
        try {

            while (tokenScanner.hasToken()) {
                token = tokenScanner.getToken();

                if (token.equalsIgnoreCase("type")) {
                    element.setType(getType());
                    
                } else if (token.equalsIgnoreCase("access")) {
                    element.setAccess(getAccess());
                    
                } else if (token.equalsIgnoreCase("min")) {
                    element.setMin(getMinMax());
                    
                } else if (token.equalsIgnoreCase("max")) {
                    element.setMax(getMinMax());

                } else if (token.equalsIgnoreCase("unit")) {
                    element.setUnit(getDescription());
                    
                } else if (token.equalsIgnoreCase("value")) {
                    /*
                     * Parse Value for element with enum type syntax for parsing
                     * value: value <name> [description] [help description]
                     */
                    element.addValue(getName(), getDescription(), getLongDescription());
                    
                } else {
                    break;
                }
            }

        } catch (IOException e) {
            throw new IOException(e.toString());
        }

        return element;

    }

}
