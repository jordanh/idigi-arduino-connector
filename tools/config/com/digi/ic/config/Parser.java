package com.digi.ic.config;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.management.BadStringOperationException;
import javax.naming.NamingException;

public class Parser {
    

    public static boolean processFile(String fileName, ConfigData configData) throws IOException
    {
        
        boolean processOk = true;
        
        
        try 
        {
            initialize(fileName);
            
            lineScanner = new Scanner(new FileReader(configFile));
            
            tokenScanner = null;

            errorConfig = configData.getGroupGlobalErrors();

            token = null;
            
            /* first use a Scanner to get each word */
            while (hasToken())
            {
                if (isReadToken)
                {
                    token =  getToken();
                }
                
                if (token == null)
                {
                    /* skip new line */
                }
                else if (token.equalsIgnoreCase("globalerror"))
                {
                    NameStruct error = new NameStruct(getName(), getDescription());
                    
                    for (NameStruct err : errorConfig)
                    {
                        if (err.getName().equals(error.getName()))
                        {
                            throw new BadStringOperationException("Duplicate <globalerror>: " + error.getName());
                        }
                    }
                    errorConfig.add(error);
                    isReadToken = true;
                }
                else if (token.equalsIgnoreCase("group"))
                {
                    /* syntax for parsing group:
                     *     group setting or state <name> [instances] <description>
                     */
                    groupConfig = configData.getConfigGroup(getToken());
                    
                    /* parse name */
                    String nameStr = getName();
                    
                    groupLineNumber = lineNumber;
                    
                    /* parse instances */
                    int groupInstances = 0;
                    
                    if (hasTokenInt())
                    {
                        groupInstances = getTokenInt();
                    }
                    
                    GroupStruct theGroup = new GroupStruct(nameStr, groupInstances, getDescription());

                    isReadToken = true;
                    /*
                     * Parse elements and errors for the group.
                     */
                    while (hasToken())
                    {
                        if (isReadToken)
                        {
                            token =  getToken();
                            isReadToken = false; /* token is already obtained from processElement */
                        }
                        
                        if (token.equalsIgnoreCase("element"))
                        {
                            ElementStruct element = processElement();
                            if (!element.validate())
                            {
                                throw new IOException("Error found for element: " + element.getName());
                            }
                            theGroup.addElement(element);
                        }
                        else if (token.equalsIgnoreCase("error"))
                        {
                            NameStruct error = new NameStruct(getName(), getDescription());
                            theGroup.addError(error);
                            isReadToken = true;
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    if (!theGroup.validate())
                    {
                        throw new IOException("Error found for group: " + theGroup.getName());
                    }
                    
                    for (GroupStruct g : groupConfig)
                    {
                        if (g.getName().equals(theGroup.getName()))
                        {
                            throw new NamingException("Duplicate <group>: " +  theGroup.getName());
                        }
                    }
                    groupConfig.add(theGroup);
                }
                else
                {
                    throw new IOException("Unrecogized keyword: " + token);
                }
            }
            if (groupConfig.isEmpty())
            {
                throw new IOException("No groups specified");
            }
            
        } catch (IOException e) {
            processOk = false;
            System.err.println(e.getMessage());
            
        } catch (NamingException e) {
            processOk = false;
            String msg = e.getMessage();
            
            if (msg.indexOf("group") != -1)
                ConfigGenerator.log("Error found in line " + groupLineNumber);
            else if (msg.indexOf("element") != -1)
                ConfigGenerator.log("Error found in line " + elementLineNumber);
                
            ConfigGenerator.log(e.toString());

        } catch (BadStringOperationException e) {
            processOk = false;
            ConfigGenerator.log("Error found in line " + lineNumber);
            ConfigGenerator.log(e.toString());
        }
        
        finally {
          //ensure the underlying stream is always closed
          //this only has any effect if the item passed to the Scanner
          //constructor implements Closeable (which it does in this case).
          if (tokenScanner != null)
          {
              tokenScanner.close();
          }
          lineScanner.close();
        }
        
        return processOk;
    }

    private final static Pattern ALPHANUMERIC = Pattern.compile("[A-Za-z_0-9]+");
    public static boolean checkAlphaNumeric(String s)
    {
        if( s == null){ return false; }
        else
        {
            Matcher m = ALPHANUMERIC.matcher(s);
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
    
    private static void initialize(String fileName) throws IOException
    {
        configFile = new File(fileName);
        
        if (!configFile.exists()) 
        {
            throw new IOException(fileName + " not found.");
        }
/*        
        if (!(configFile.isFile() && configFile.canRead())) 
        {
            throw new FileNotFoundException("Unable to access " + aFileName);
        }
*/        
        isReadToken = true;
        lineNumber = 0;
        groupLineNumber = 0;
        elementLineNumber = 0;
         
    }

    private static String getName() throws BadStringOperationException 
    {
        String name = getToken(); //tokenScanner.next();
        
        if (name != null)
        {
            if (!checkAlphaNumeric(name)) 
            {
                throw new BadStringOperationException("Invalid character in the name: " + name);
            }
        
        }
        return name;
    }
    
    private static String getDescription() throws BadStringOperationException 
    {

        String description = null;
        if (hasToken("\\\".*"))
        {
            description = getTokenInLine("\\\".*?\\\"");
            if (description == null)
            {
                throw new BadStringOperationException("Invalid description");
            }
        }
        return description;
    }
    
    private static String getType() throws BadStringOperationException
    {
        String type = getToken(); //tokenScanner.next();
        
        if (type == null)
        {
            throw new BadStringOperationException("Missing type");
            
        }
        ElementStruct.ElementType elementType = ElementStruct.ElementType.toElementType(type);
        
        if (elementType == ElementStruct.ElementType.INVALID_TYPE)
        {
            throw new BadStringOperationException("Invalid type: " + type);
        }
        else if (elementType == ElementStruct.ElementType.ENUM)
        {
            enumSupport = true;
        }
        else if (elementType == ElementStruct.ElementType.FLOAT)
        {
            floatingSupport = true;
        }
        
        return type;
    }
    
    private static String getAccess() throws BadStringOperationException
    {
        String access = getToken(); // tokenScanner.next();
        
        if (access == null)
        {
            throw new BadStringOperationException("Missing access");
            
        }
        if (ElementStruct.AccessType.toAccessType(access) == ElementStruct.AccessType.INVALID_TYPE)
        {
            throw new BadStringOperationException("Invalid access:" + access);
        }
        return access;
    }

    private static String getMinMax() throws BadStringOperationException
    {
        String mvalue = getToken(); // tokenScanner.next();
        
        if (mvalue == null)
        {
            throw new BadStringOperationException("Missing min or max value");
            
        }
        try {
            Integer.parseInt(mvalue);
            
        } catch (NumberFormatException e) {
            try {
                Float.parseFloat(mvalue);
                
            } catch (NumberFormatException f) {
                throw new BadStringOperationException("Invalid min or max value:" + mvalue);
            }
        }
        
        return mvalue;
    }
   

   private static final ElementStruct processElement() throws BadStringOperationException
   {
       /*
        * syntax for parsing element:
        *       element <name> <description> type <type> [min <min>] [max <max>] [access <access>] [unit <unit>]
        */
       ElementStruct element = new ElementStruct(getName(), getDescription());
       elementLineNumber = lineNumber;
       try {
           
           while (hasToken())
           {
               token =  getToken();
               if (token == null)
               {
                   /* end of scanner */
               }
               else if (token.equalsIgnoreCase("type"))
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
                   NameStruct value = new NameStruct(getName());
                   
                   if (hasToken("\\\".*"))
                   {
                       value.setDescription(getDescription());
                   }
                   element.addValue(value);
               }
               else
               {
                   break;
               }
           }
           
       } catch (BadStringOperationException e) {
           throw new BadStringOperationException(e.toString());
       }
       
       return element;

   }
   
   private static String getToken()
   {
       String aWord = null;
       
       if (tokenScanner != null && !tokenScanner.hasNext())
       {
           tokenScanner.close();
           tokenScanner = null;
       }
       if (tokenScanner == null)
       {
           while (lineScanner.hasNextLine())
           {
               String line = lineScanner.nextLine();
               lineNumber++;
//               ConfigGenerator.log("line " + lineNumber + ": " + line);
               
               if (line.length() > 0 && line.split(" ").length > 0)
               {
                   tokenScanner = new Scanner(line);
                   break;
               }
           }
       }
       
       if (tokenScanner != null && tokenScanner.hasNext())
           aWord = tokenScanner.next();

       return aWord;
   }
   private static int getTokenInt() throws BadStringOperationException
   {
       int anInt = 0;
       
       if (tokenScanner != null && !tokenScanner.hasNext())
       {
           tokenScanner.close();
           tokenScanner = null;
       }
       if (tokenScanner == null)
       {
           
           while (lineScanner.hasNextLine())
           {
               String line = lineScanner.nextLine();
               lineNumber++;
               
               if (line.length() > 0 && line.split(" ").length > 0)
               {
                   tokenScanner = new Scanner(line);
                   break;
               }
           }
       }
       
       if (tokenScanner != null && tokenScanner.hasNextInt())
           anInt = tokenScanner.nextInt();
       else
           throw new BadStringOperationException("Not integer (expect an integer value)");

       return anInt;
   } 

   private static String getTokenInLine(String pattern)
   {
       String aLine = null;
       
       if (tokenScanner != null && !tokenScanner.hasNext())
       {
           tokenScanner.close();
           tokenScanner = null;
       }
       if (tokenScanner == null)
       {
           while (lineScanner.hasNextLine())
           {
               String line = lineScanner.nextLine();
               lineNumber++;
               //ConfigGenerator.log("line " + lineNumber + ": " + line);
               if (line.length() > 0 && line.split(" ").length > 0)
               {
                   tokenScanner = new Scanner(line);
                   break;
               }
           }
       }
       
       if (tokenScanner.hasNext())
           aLine = tokenScanner.findInLine(pattern);

       return aLine;
   }   
   private static boolean hasToken()
   {
       boolean token_avail = false;
       
       if (tokenScanner != null)
       {
           token_avail = tokenScanner.hasNext();
       }
       if (!token_avail)
       {
           token_avail = lineScanner.hasNext();
       }

       return token_avail;
   }
   
   private static boolean hasToken(String pattern)
   {
       boolean token_avail = false;
       
       if (tokenScanner != null)
       {
           token_avail = tokenScanner.hasNext(pattern);
       }
       
       if (!token_avail)
       {
           token_avail = lineScanner.hasNext(pattern);
       }

       return token_avail;
   }

   private static boolean hasTokenInt()
   {
       boolean token_avail = false;
       
       if (tokenScanner != null)
       {
           token_avail = tokenScanner.hasNextInt();
       }
       
       if (!token_avail)
       {
           token_avail = lineScanner.hasNextInt();
       }

       return token_avail;
   }

// PRIVATE variables
   private static File configFile;
   private static boolean isReadToken;
   private static String token;
   private static int lineNumber;
   private static int groupLineNumber;
   private static int elementLineNumber;
   private static LinkedList<GroupStruct> groupConfig;
   private static LinkedList<NameStruct> errorConfig;
   
   private static Scanner lineScanner;
   private static Scanner tokenScanner;

   private static boolean floatingSupport;
   private static boolean enumSupport;

}
