package com.digi.ic.config;

import java.io.IOException;
import java.util.LinkedList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Parser {

    private final static int MAX_DESCRIPTION_LENGTH = 40;

    public static void processFile(String fileName, ConfigData configData) throws IOException, NullPointerException
    {

        try
        {
            tokenScanner = new TokenScanner(fileName);

            token = null;
            isReadToken = true;

            /* first use a Scanner to get each word */
            while (!isReadToken || tokenScanner.hasToken())
            {
                if (isReadToken)
                {
                    token =  tokenScanner.getToken();
                }

                if (token.equalsIgnoreCase("globalerror"))
                {
                    ConfigData.addUserGroupError(getName(), getDescription());
                    isReadToken = true;
                }
                else if (token.equalsIgnoreCase("group"))
                {
                    /* syntax for parsing group:
                     *     group setting or state <name> [instances] <description>
                     */
                    groupConfig = configData.getConfigGroup(tokenScanner.getToken());

                    /* parse name */
                    String nameStr = getName();

                    groupLineNumber = tokenScanner.getLineNumber();

                    /* parse instances */
                    int groupInstances = 1;

                    if (tokenScanner.hasTokenInt())
                    {
                        groupInstances = tokenScanner.getTokenInt();
                    }

                    GroupStruct theGroup = new GroupStruct(nameStr, groupInstances, getDescription());

                    isReadToken = true;
                    /*
                     * Parse elements and errors for the group.
                     */
                    while (tokenScanner.hasToken())
                    {
                        if (isReadToken)
                        {
                            token =  tokenScanner.getToken();
                            isReadToken = false; /* token is already obtained from processElement */
                        }
                        if (token.equalsIgnoreCase("element"))
                        {
                            ElementStruct element = processElement();

                            if (!element.validate())
                            {
                                throw new Exception("Error in <element>: " + element.getName());
                            }

                            theGroup.addElement(element);
                        }
                        else if (token.equalsIgnoreCase("error"))
                        {
                            theGroup.addError(getName(), getDescription());
                            isReadToken = true;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (!theGroup.validate())
                    {
                        throw new Exception("Error in <group>: " + theGroup.getName());
                    }

                    for (GroupStruct g : groupConfig)
                    {
                        if (g.getName().equals(theGroup.getName()))
                        {
                            throw new Exception("Duplicate <group>: " +  theGroup.getName());
                        }
                    }
                    groupConfig.add(theGroup);

                    if (!tokenScanner.hasToken())
                    {
                        /* end of file */
                        break;
                    }

                }
                else
                {
                    throw new Exception("Unrecogized keyword: " + token);
                }
            }
        } catch (NullPointerException e) {
            ConfigGenerator.log("Parser NullPointerException");
            ConfigGenerator.log(e.toString());
            throw new NullPointerException();

        } catch (IOException e) {
            ConfigGenerator.log("Parser IOException");
            errorFoundLog(e.getMessage());
            throw new IOException("Error found in file: " + fileName);

        } catch (Exception e) {
            errorFoundLog(e.getMessage());
            throw new IOException("Error found in file:" + fileName);
        }

        finally {
            tokenScanner.close();
        }

    }

    private static void errorFoundLog(String str)
    {
        ConfigGenerator.log(str);

        if (str.indexOf("<group>") != -1)
            ConfigGenerator.log("Error found in line " + groupLineNumber);
        else if (str.indexOf("<element>") != -1)
            ConfigGenerator.log("Error found in line " + elementLineNumber);
        else
            ConfigGenerator.log("Error found in line " + tokenScanner.getLineNumber());

    }

    private final static Pattern ALPHACHARACTERS = Pattern.compile("\\w+");
    public static boolean checkAlphaCharacters(String s)
    {
        if( s == null){ return false; }
        else
        {
            Matcher m = ALPHACHARACTERS.matcher(s);
            return m.matches();
        }
    }


    public static boolean getFloatingSupport()
    {
        return floatingSupport;
    }

    public static boolean getEnumSupport()
    {
        return enumSupport;
    }

    private static String getName() throws Exception
    {
        String name = tokenScanner.getToken(); //tokenScanner.next();

        if (name == null)
        {
            throw new Exception("Missing name!");
        }

        if (!checkAlphaCharacters(name))
        {
            throw new Exception("Invalid character in the name: " + name);
        }
        return name;
    }

    private static String getDescription() throws Exception
    {

        String description = null;
        if (tokenScanner.hasToken("\\\".*"))
        {
            description = tokenScanner.getTokenInLine("\\\".*?\\\"");
            if (description == null)
            {
                throw new Exception("Invalid description");
            }

            description = description.substring(1,description.lastIndexOf("\""));

            if (description.contains(":"))
            {
                String[] descrHelp = description.split(":", 2);
                if (descrHelp[0].length() > MAX_DESCRIPTION_LENGTH)
                {
                    throw new Exception("desciption > maximum length " + MAX_DESCRIPTION_LENGTH);
                }
                description = descrHelp[0] + "::" + descrHelp[1];
            }

        }
        return description;
    }

    private static String getType() throws Exception
    {
        String type = tokenScanner.getToken(); //tokenScanner.next();

        if (type == null)
        {
            throw new Exception("Missing type");

        }
        ElementStruct.ElementType elementType = ElementStruct.ElementType.toElementType(type);

        if (elementType == ElementStruct.ElementType.ENUM)
        {
            enumSupport = true;
        }
        else if (elementType == ElementStruct.ElementType.FLOAT)
        {
            floatingSupport = true;
        }

        return type;
    }

    private static String getAccess() throws Exception
    {
        String access = tokenScanner.getToken(); // tokenScanner.next();

        if (access == null)
        {
            throw new Exception("Missing access!");

        }
        ElementStruct.AccessType.toAccessType(access);

        return access;
    }

    private static String getMinMax() throws Exception
    {
        String mvalue = tokenScanner.getToken(); // tokenScanner.next();

        if (mvalue == null)
        {
            throw new Exception("Missing min or max value");

        }
        return mvalue;
    }


   private static final ElementStruct processElement() throws Exception
   {
       /*
        * syntax for parsing element:
        *       element <name> <description> type <type> [min <min>] [max <max>] [access <access>] [unit <unit>]
        */
       ElementStruct element = new ElementStruct(getName(), getDescription());
       elementLineNumber = tokenScanner.getLineNumber();
       try {

           while (tokenScanner.hasToken())
           {
               token =  tokenScanner.getToken();

               if (token.equalsIgnoreCase("type"))
               {
                   element.setType(getType());
               }
               else if (token.equalsIgnoreCase("access"))
               {
                   element.setAccess(getAccess());
               }
               else if (token.equalsIgnoreCase("min"))
               {
                   element.setMin(getMinMax());
               }
               else if (token.equalsIgnoreCase("max"))
               {
                    element.setMax(getMinMax());

               }
               else if (token.equalsIgnoreCase("unit"))
               {
                   element.setUnit(getDescription());
               }
               else if (token.equalsIgnoreCase("value"))
               {
                   /*
                    * Parse Value for element with enum type
                    * syntax for parsing value:
                    *       value <name> [description]
                    */
                   String name = getName();
                   String description = null;

                   if (tokenScanner.hasToken("\\\".*"))
                   {
                       description = getDescription();
                   }
                   element.addValue(name, description);
               }
               else
               {
                   break;
               }
           }

       } catch (IOException e) {
           throw new IOException(e.toString());
       }

       return element;

   }


// PRIVATE variables
   private static TokenScanner tokenScanner;
   private static boolean isReadToken;
   private static String token;
   private static int groupLineNumber;
   private static int elementLineNumber;
   private static LinkedList<GroupStruct> groupConfig;

   private static boolean floatingSupport;
   private static boolean enumSupport;

}
