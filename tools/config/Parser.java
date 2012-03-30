
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
    * @throws IOException 
    */
    public Parser(String aFileName) throws FileNotFoundException
    {
        fFile = new File(aFileName);
        if (!fFile.exists()) 
        {
            throw new FileNotFoundException(aFileName + " not found.");
        }
/*        
        if (!(fFile.isFile() && fFile.canRead())) 
        {
            throw new FileNotFoundException("Unable to access " + aFileName);
        }
*/        
        read_token = true;
        line_number = 0;
        group_line_number = 0;
        element_line_number = 0;
    }

    public final boolean processFile(LinkedList<GroupStruct> groupList, LinkedList<NameStruct> errorList) throws FileNotFoundException
    {
        
        boolean processOk = true;
        
        line_scanner = new Scanner(new FileReader(fFile));
        token_scanner = null;

        globalGroups = groupList;
        globalErrors = errorList;

        try 
        {
            token = null;
            
            //first use a Scanner to get each word
            while (has_token())
            {
                if (read_token)
                {
                    token =  get_token();
                }
                
                if (token == null)
                {
                    
                }
                else if (token.equalsIgnoreCase("globalerror"))
                {
                    NameStruct error = new NameStruct(getName());
                    
                    error.addConfig(getDescription());
                    for (NameStruct e : globalErrors)
                    {
                        if (e.name.equals(error.name))
                        {
                            throw new BadStringOperationException("Duplicate <globalerror>: " + error.name);
                        }
                    }
                    globalErrors.add(error);
                    read_token = true;
                }
                else if (token.equalsIgnoreCase("group"))
                {
                    /* syntax for parsing group:
                     *     group <name> [instances] <description>
                     */
                    GroupStruct theGroup = new GroupStruct(getName());
                    
                    group_line_number = line_number;
                    
                    if (has_token_int())
                    {
                        theGroup.addConfig(get_token_int());
                    }
                    
                    theGroup.addConfig(getDescription());
                    read_token = true;
                    /*
                     * Parse elements and errors for the group.
                     */
                    while (has_token())
                    {
                        if (read_token)
                        {
                            token =  get_token();
                            read_token = false; /* token is already obtained from processElement */
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
                            NameStruct error = new NameStruct(getName());
                            error.addConfig(getDescription());
                            theGroup.addConfigError(error);
                            read_token = true;
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
                    
                    for (GroupStruct g : globalGroups)
                    {
                        if (g.name.equals(theGroup.name))
                        {
                            throw new NamingException("Duplicate <group>: " +  theGroup.name);
                        }
                    }
                    globalGroups.add(theGroup);
                }
                else
                {
                    throw new IOException("Unrecogized keyword: " + token);
                }
            }
            if (globalGroups.isEmpty())
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
                log("Error found in line " + group_line_number);
            else if (msg.indexOf("element") != -1)
                log("Error found in line " + element_line_number);
                
            log(e.toString());

        } catch (BadStringOperationException e) {
            processOk = false;
            log("Error found in line " + line_number);
            log(e.toString());
        }
        
        finally {
          //ensure the underlying stream is always closed
          //this only has any effect if the item passed to the Scanner
          //constructor implements Closeable (which it does in this case).
          if (token_scanner != null)
          {
              token_scanner.close();
          }
          line_scanner.close();
        }
        
        return processOk;
    }

    private final static Pattern ALPHANUMERIC = Pattern.compile("[A-Za-z_0-9]+");
    private boolean checkAlphaNumeric(String s)
    {
        if( s == null){ return false; }
        else
        {
            Matcher m = ALPHANUMERIC.matcher(s);
            return m.matches();
        }
    }


    private String getName() throws BadStringOperationException 
    {
        String name = get_token(); //token_scanner.next();
        
        if (name != null)
        {
            if (!checkAlphaNumeric(name)) 
            {
                throw new BadStringOperationException("Invalid character in the name: " + name);
            }
        
            debug_log("name= " + name);
        }
        return name;
    }
    
    private String getDescription() throws BadStringOperationException 
    {

        String description = null;
        if (has_token("\\\".*"))
        {
            description = find_token_in_line("\\\".*?\\\"");
            if (description == null)
            {
                throw new BadStringOperationException("Invalid description");
            }
            debug_log("desc= " + description);
        }
        return description;
/*
         String description = get_token(); //token_scanner.next();
        
        if (description.indexOf('\"') == 0)
        {
            if (description.length() == 1 || description.indexOf('\"', 1) == -1)
            {
            
                Pattern delim = token_scanner.delimiter();
                
                
                token_scanner.useDelimiter("\"");
                while (has_token())
                {
                    description += " ";
                    description += token_scanner.next();
                    
                    if (description.lastIndexOf('\"') != -1)
                    {
                        break;
                    }
                }
                token_scanner.useDelimiter(delim);
            }
            
            if (token_scanner.hasNext("\""))
            {
                String end_quote = token_scanner.next("\"");
                if (end_quote.equals("\""))
                {
                    description += end_quote; 
                }
                else
                {
                    throw new BadStringOperationException("Missing closing quote in description");
                }
                
            }
        }
        else
        {
            throw new BadStringOperationException("Missing quote in description");
        }
        
        return description;
*/
    }
    
    private String getType() throws BadStringOperationException
    {
        String type = get_token(); //token_scanner.next();
        
        if (type == null)
        {
            throw new BadStringOperationException("Missing type");
            
        }
        if (!Constants.listContainsString(Constants.element_type_list, type)) 
        {
            throw new BadStringOperationException("Invalid type: " + type);
        }
        debug_log("type =" + type);
        return type;
    }
    
    private String getAccess() throws BadStringOperationException
    {
        String access = get_token(); // token_scanner.next();
        
        if (access == null)
        {
            throw new BadStringOperationException("Missing access");
            
        }
        if (!Constants.listContainsString(Constants.element_access_list, access)) 
        {
            throw new BadStringOperationException("Invalid access:" + access);
        }
        debug_log("access = " + access);
        return access;
    }

    private String getMinMax() throws BadStringOperationException
    {
        String mvalue = get_token(); // token_scanner.next();
        
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
        debug_log("min/max = " + mvalue);
        
        return mvalue;
    }
   

   private final ElementStruct processElement() throws BadStringOperationException
   {
       /*
        * syntax for parsing element:
        *       element <name> <description> type <type> [min <min>] [max <max>] [access <access>] [unit <unit>]
        */
       ElementStruct element = new ElementStruct(getName());
       element_line_number = line_number;
       try {
           element.addConfig(getDescription());
           
           while (has_token())
           {
               token =  get_token();
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
                   debug_log("value: ");
                   
    //                               if (scanner.hasNext("\""))
//                   if (token_scanner.hasNext("\\\".*")
                   if (has_token("\\\".*"))
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
   
   private String get_token()
   {
       String aWord = null;
       
       if (token_scanner != null && !token_scanner.hasNext())
       {
           token_scanner.close();
           token_scanner = null;
       }
       if (token_scanner == null)
       {
           while (line_scanner.hasNextLine())
           {
               String line = line_scanner.nextLine();
               line_number++;
//               log("line " + line_number + ": " + line);
               
               if (line.length() > 0 && line.split(" ").length > 0)
               {
                   token_scanner = new Scanner(line);
                   break;
               }
           }
       }
       
       if (token_scanner != null && token_scanner.hasNext())
           aWord = token_scanner.next();

       return aWord;
   }
   private int get_token_int() throws BadStringOperationException
   {
       int anInt = 0;
       
       if (token_scanner != null && !token_scanner.hasNext())
       {
           token_scanner.close();
           token_scanner = null;
       }
       if (token_scanner == null)
       {
           
           while (line_scanner.hasNextLine())
           {
               String line = line_scanner.nextLine();
               line_number++;
//               log("line " + line_number + ": " + line);
               
               if (line.length() > 0 && line.split(" ").length > 0)
               {
                   token_scanner = new Scanner(line);
                   break;
               }
           }
       }
       
       if (token_scanner != null && token_scanner.hasNextInt())
           anInt = token_scanner.nextInt();
       else
           throw new BadStringOperationException("Not integer (expect an integer value)");

       return anInt;
   } 

   private String find_token_in_line(String pattern)
   {
       String aLine = null;
       
       if (token_scanner != null && !token_scanner.hasNext())
       {
           token_scanner.close();
           token_scanner = null;
       }
       if (token_scanner == null)
       {
           while (line_scanner.hasNextLine())
           {
               String line = line_scanner.nextLine();
               line_number++;
               //log("line " + line_number + ": " + line);
               if (line.length() > 0 && line.split(" ").length > 0)
               {
                   token_scanner = new Scanner(line);
                   break;
               }
           }
       }
       
       if (token_scanner.hasNext())
           aLine = token_scanner.findInLine(pattern);

       return aLine;
   }   
   private boolean has_token()
   {
       boolean token_avail = false;
       
       if (token_scanner != null)
       {
           token_avail = token_scanner.hasNext();
       }
       if (!token_avail)
       {
           token_avail = line_scanner.hasNext();
       }

       return token_avail;
   }
   
   private boolean has_token(String pattern)
   {
       boolean token_avail = false;
       
       if (token_scanner != null)
       {
           token_avail = token_scanner.hasNext(pattern);
       }
       
       if (!token_avail)
       {
           token_avail = line_scanner.hasNext(pattern);
       }

       return token_avail;
   }

   private boolean has_token_int()
   {
       boolean token_avail = false;
       
       if (token_scanner != null)
       {
           token_avail = token_scanner.hasNextInt();
       }
       
       if (!token_avail)
       {
           token_avail = line_scanner.hasNextInt();
       }

       return token_avail;
   }

// PRIVATE variables
   private final File fFile;
   private boolean read_token;
   private String token;
   private int line_number;
   private int group_line_number;
   private int element_line_number;
   private LinkedList<GroupStruct> globalGroups;
   private LinkedList<NameStruct> globalErrors;
   
   private Scanner line_scanner;
   private Scanner token_scanner;

   public static void log(Object aObject)
   {
       System.out.println(String.valueOf(aObject));
   }

   public static void debug_log(Object aObject)
   {
//       System.out.println(String.valueOf(aObject));
   }

}
