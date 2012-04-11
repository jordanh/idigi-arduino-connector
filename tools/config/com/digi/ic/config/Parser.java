package com.digi.ic.config;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.management.BadStringOperationException;
import javax.naming.NamingException;

public class Parser {
    
    /**
    Constructor.
    @param aFileName full name of an existing, readable file.
    * @throws FileNotFoundException 
    */
    public Parser(String fileName) throws FileNotFoundException
    {
        configFile = new File(fileName);
        if (!configFile.exists()) 
        {
            throw new FileNotFoundException(fileName + " not found.");
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

    public final boolean processFile(ConfigData configData) throws IOException
    {
        
        boolean processOk = true;
        
        lineScanner = new Scanner(new FileReader(configFile));
        tokenScanner = null;

        errorConfig = configData.getErrorGroups();
        
        try 
        {
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
                    
                    for (NameStruct e : errorConfig)
                    {
                        if (e.name.equals(error.name))
                        {
                            throw new BadStringOperationException("Duplicate <globalerror>: " + error.name);
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
                    
                    GroupStruct theGroup = new GroupStruct(getName());

                    groupLineNumber = lineNumber;
                    
                    if (hasTokenInt())
                    {
                        theGroup.addConfig(getTokenInt());
                    }
                    
                    theGroup.addConfig(getDescription());
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
                                throw new IOException("Error found for element: " + element.name);
                            }
                            theGroup.addConfigElement(element);
                        }
                        else if (token.equalsIgnoreCase("error"))
                        {
                            NameStruct error = new NameStruct(getName(), getDescription());
                            theGroup.addConfigError(error);
                            isReadToken = true;
                        }
                        else
                        {
                            break;
                        }
                    }
                    
                    if (!theGroup.validate())
                    {
                        throw new IOException("Error found for group: " + theGroup.name);
                    }
                    
                    for (GroupStruct g : groupConfig)
                    {
                        if (g.name.equals(theGroup.name))
                        {
                            throw new NamingException("Duplicate <group>: " +  theGroup.name);
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
                log("Error found in line " + groupLineNumber);
            else if (msg.indexOf("element") != -1)
                log("Error found in line " + elementLineNumber);
                
            log(e.toString());

        } catch (BadStringOperationException e) {
            processOk = false;
            log("Error found in line " + lineNumber);
            log(e.toString());
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

    private String getName() throws BadStringOperationException 
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
    
    private String getDescription() throws BadStringOperationException 
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
    
    private String getType() throws BadStringOperationException
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
    
    private String getAccess() throws BadStringOperationException
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

    private String getMinMax() throws BadStringOperationException
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
   

   private final ElementStruct processElement() throws BadStringOperationException
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
                   element.addConfigType(getType());
               }
               else if (token.equalsIgnoreCase("access"))
               {
                   element.addConfigAccess(getAccess());
               }
               else if (token.equalsIgnoreCase("min"))
               {
                   element.addConfigMin(getMinMax());
               }
               else if (token.equalsIgnoreCase("max"))
               {
                    element.addConfigMax(getMinMax());
                   
               }
               else if (token.equalsIgnoreCase("unit"))
               {
                   element.addConfigUnit(getDescription());
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
                       value.addConfig(getDescription());
                   }
                   element.addConfigValue(value);
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
   
   private String getToken()
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
//               log("line " + lineNumber + ": " + line);
               
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
   private int getTokenInt() throws BadStringOperationException
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

   private String getTokenInLine(String pattern)
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
               //log("line " + lineNumber + ": " + line);
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
   private boolean hasToken()
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
   
   private boolean hasToken(String pattern)
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

   private boolean hasTokenInt()
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
   private final File configFile;
   private boolean isReadToken;
   private String token;
   private int lineNumber;
   private int groupLineNumber;
   private int elementLineNumber;
   private LinkedList<GroupStruct> groupConfig;
   private LinkedList<NameStruct> errorConfig;
   
   private Scanner lineScanner;
   private Scanner tokenScanner;

   private static boolean floatingSupport;
   private static boolean enumSupport;


   public static void log(Object aObject)
   {
       System.out.println(String.valueOf(aObject));
   }

}
