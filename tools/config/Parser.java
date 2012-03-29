
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.management.BadStringOperationException;

public class Parser {
    
    public static class AlphaNumeric
    {
        public static Pattern ALPHANUMERIC = Pattern.compile("[A-Za-z_0-9]+");
        public static boolean checkAlphaNumeric(String s)
        {
            if( s == null){ return false; }
            else
            {
                Matcher m = ALPHANUMERIC.matcher(s);
                return m.matches();
            }
        }
    }

    public static String getName(Scanner scanner) throws BadStringOperationException 
    {
        String name = scanner.next();
        
        if (!AlphaNumeric.checkAlphaNumeric(name)) 
        {
            throw new BadStringOperationException("Invalid character in the name: " + name);
        }
        return name;
    }
    
    public static String getDescription(Scanner scanner) throws BadStringOperationException 
    {

        String description = null;
        if (scanner.hasNext("\\\".*"))
        {
            description = scanner.findInLine("\\\".*?\\\"");
            if (description == null)
            {
                throw new BadStringOperationException("Incomplete description");
            }
        }
        return description;
/*        
        String description = scanner.next();
        
        if (description.indexOf('\"') == 0)
        {
            if (description.length() == 1 || description.indexOf('\"', 1) == -1)
            {
            
                Pattern delim = scanner.delimiter();
                
                
                scanner.useDelimiter("\"");
                while (scanner.hasNext())
                {
                    description += " ";
                    description += scanner.next();
                    
                    if (description.lastIndexOf('\"') != -1)
                    {
                        break;
                    }
                }
                scanner.useDelimiter(delim);
            }
            
            if (scanner.hasNext("\""))
            {
                String end_quote = scanner.next("\"");
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
    
    public static String getType(Scanner scanner) throws BadStringOperationException
    {
        String type = scanner.next();
        
        if (!Constants.listContainsString(Constants.element_type_list, type)) 
        {
            throw new BadStringOperationException("Invalid type: " + type);
        }
        return type;
    }
    
    public static String getInteger(Scanner scanner)
    {
        String name = null;
        if (scanner.hasNextInt())
        {
            name = scanner.next();
        }
        
        return name;
    }
    
    public static String getFloat(Scanner scanner)
    {
        String name = null;
        if (scanner.hasNextDouble())
        {
            name = scanner.next();
        }
         
        return name;
    }
    
    public static String getAccess(Scanner scanner) throws BadStringOperationException
    {
        String access = scanner.next();
        
        if (!Constants.listContainsString(Constants.element_access_list, access)) 
        {
            throw new BadStringOperationException("Invalid access:" + access);
        }
        return access;
    }

}
