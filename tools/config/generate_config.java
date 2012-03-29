
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.zip.DataFormatException;

import javax.management.BadStringOperationException;

public class generate_config {

    
    public static void main(String[] args) 
    {
        final int FILE_NAME = 0;
        final int DESCRIPTOR_NAME = 1;
        
        try {
            
            log(args.length);
            
            if (args.length != 1 && args.length != 6 && args.length != 5)
            {
                /* testing on <config_filename> for args.length == 1 */
                throw new DataFormatException("");
            }
            else
            {
            
                ArrayList<String[]> fileConfigList = new ArrayList<String[]>();
                
                int config_index = 0;
                
                if (args.length == 6)
                {
                    /* check 4th argument for -sysinfo */ 
                    if (args[4].matches("-sysinfo:(.*)"))
                    {
                        String[] sysinfoConfig = {args[4].substring("-sysinfo:".length()), Descriptors.system_info_config_string};
                        fileConfigList.add(config_index, sysinfoConfig);
                    }
                    else
                    {
                        throw new DataFormatException("Invalid argument: " + args[4]);
                    }
                    
                    config_index++;
                }
                String[]  dataConfig = {args[args.length -1], Descriptors.data_config_string};
                fileConfigList.add(config_index, dataConfig);
                    
                Descriptors descriptors = new Descriptors(args);
                
                for (String[] fileConfig : fileConfigList)
                {
                    LinkedList<GroupStruct> groupList = new LinkedList<GroupStruct>();
                    LinkedList<NameStruct> globalErrorList = new LinkedList<NameStruct>();
                 
                    log("filename: " + fileConfig[FILE_NAME]);
                    log("descriptor: " + fileConfig[DESCRIPTOR_NAME]);
                    
                    generate_config parser = new generate_config(fileConfig[FILE_NAME]);
                    parser.processFile(groupList, globalErrorList);
                    
                    descriptors.processDescriptors(fileConfig[DESCRIPTOR_NAME], groupList, globalErrorList);
                }
               
                descriptors.processRciDescriptors((fileConfigList.size() > 1));
            }
            
        } catch (DataFormatException e) {
            
            System.err.println(e.getMessage());

            log("Syntax: java -jar icConfigTool <username[:password]> <vendor_id> <device_type> <fw_version> [-sysinfo:<filename>] <config_filename>\n");
            log("Where:");
            log("      username          = username to log in iDigi Cloud. You will be prompted to enter the password");
            log("      username:password = optional for username and password to log in iDigi Cloud");
            log("      vendor_id       = Vendor ID obtained from iDigi Cloud registration");
            log("      device_type     = Device type string with quotes(i.e. \"device type\")");
            log("      fw_version      = firmware version number");
            log("      -sysinfo        = option for specifying iDigi Connector Configuration file which is given in <filename> for System Information");
            log("      config_filename = iDigi Connector Configration file for data configurations");

        } catch (Exception e) {
            if (e.getCause() != null)
                System.err.println(e.getMessage());
            e.printStackTrace();
        }
    }

    /**
    Constructor.
    @param aFileName full name of an existing, readable file.
    * @throws IOException 
   */
   public generate_config(String aFileName) throws FileNotFoundException
   {
       fFile = new File(aFileName);
       if (!fFile.exists()) 
       {
           throw new FileNotFoundException(aFileName + " not found.");
       }
       
       if (!(fFile.isFile() && fFile.canRead())) 
       {
           throw new FileNotFoundException("Unable to access " + aFileName);
       }
       read_token = true;
   }

   public final ElementStruct processElement(Scanner scanner) throws BadStringOperationException
   {
       /*
        * syntax for parsing element:
        *       element <name> <description> type <type> [min <min>] [max <max>] [access <access>] [unit <unit>]
        */
       
       ElementStruct element = new ElementStruct(Parser.getName(scanner));
       
       element.addConfig(Parser.getDescription(scanner));
       
       while (scanner.hasNext())
       {
           token = scanner.next();
           if (token.equalsIgnoreCase("type"))
           {
               element.addConfigType(Parser.getType(scanner));
           }
           else if (token.equalsIgnoreCase("access"))
           {
               element.addConfigAccess(Parser.getAccess(scanner));
           }
           else if (token.equalsIgnoreCase("min"))
           {
               if (!element.is_float_type())
               {
                   element.addConfigMin(Parser.getInteger(scanner));
               }
               else
               {
                   element.addConfigMin(Parser.getFloat(scanner));
               }
               
           }
           else if (token.equalsIgnoreCase("max"))
           {
               if (!element.is_float_type())
               {
                   element.addConfigMax(Parser.getInteger(scanner));
               }
               else
               {
                   element.addConfigMax(Parser.getFloat(scanner));
                   
               }
               
           }
           else if (token.equalsIgnoreCase("unit"))
           {
               element.addConfigUnit(Parser.getDescription(scanner));
           }
           else if (token.equalsIgnoreCase("value"))
           {
               /* 
                * Parse Value for element with enum type
                * syntax for parsing value:
                *       value <name> [description]
                */
               NameStruct value = new NameStruct(Parser.getName(scanner));
               
//                               if (scanner.hasNext("\""))
               if (scanner.hasNext("\\\".*"))
               {
                   value.addConfig(Parser.getDescription(scanner));
               }
               element.addConfigValue(value);
           }
           else
           {
               break;
           }
       }

       return element;
   }
   
   public final void processFile(LinkedList<GroupStruct> groupList, LinkedList<NameStruct> errorList) throws Exception 
   {
       
       Scanner scanner = new Scanner(new FileReader(fFile));

       groups = groupList;
       globalErrors = errorList;

       try 
       {
           token = null;
           
           //first use a Scanner to get each word
           while (scanner.hasNext())
           {
               if (read_token)
               {
                   token = scanner.next();
               }
               
               if (token.equalsIgnoreCase("globalerror"))
               {
                   NameStruct error = new NameStruct(Parser.getName(scanner));
                   
                   error.addConfig(Parser.getDescription(scanner));
                   globalErrors.add(error);
               }
               else if (token.equalsIgnoreCase("group"))
               {
                   /* syntax for parsing group:
                    *     group <name> [instances] <description>
                    */
                   GroupStruct group = new GroupStruct(Parser.getName(scanner));
                   if (scanner.hasNextInt())
                   {
                       group.addConfig(scanner.nextInt());
                   }
                   group.addConfig(Parser.getDescription(scanner));
                   read_token = true;
                   /*
                    * Parse elements and errors for the group.
                    */
                   while (scanner.hasNext())
                   {
                       if (read_token)
                       {
                           token = scanner.next();
                           read_token = false; /* token is already obtained from processElement */
                       }
                       
                       if (token.equalsIgnoreCase("element"))
                       {
                           ElementStruct element = processElement(scanner);
                           if (!element.validate())
                           {
                               throw new IOException("Error found for element: " + element.name);
                           }
                           group.addConfigElement(element);
                       }
                       else if (token.equalsIgnoreCase("error"))
                       {
                           NameStruct error = new NameStruct(Parser.getName(scanner));
                           error.addConfig(Parser.getDescription(scanner));
                           group.addConfigError(error);
                           read_token = true;
                       }
                       else
                       {
                           break;
                       }
                   }
                   
                   if (!group.validate())
                   {
                       throw new IOException("Error found for group: " + group.name);
                   }
                   
                   if (!groups.equals(group.name))
                   {
                       groups.add(group);
                   }
                   else
                   {
                       throw new BadStringOperationException("Duplicate <group>: " + group.name);
                   }
               }
               else
               {
                   throw new IOException("Unrecogized keyword: " + token);
               }
           }
           
       } catch (IOException e) {
           System.err.println(e.getMessage());
           e.printStackTrace();
           throw new IOException(e.getMessage());
           
       } catch (BadStringOperationException e) {
           System.err.println(e.getMessage());
           e.printStackTrace();
           throw new IOException(e.getMessage());
       }
       
       finally {
         //ensure the underlying stream is always closed
         //this only has any effect if the item passed to the Scanner
         //constructor implements Closeable (which it does in this case).
         scanner.close();
       }
       
   }
   
     
   // PRIVATE 
   private final File fFile;
   private boolean read_token;
   private String token;
   private LinkedList<GroupStruct> groups;
   private LinkedList<NameStruct> globalErrors;

   public static void log(Object aObject)
   {
       System.out.println(String.valueOf(aObject));
   }
   
}
