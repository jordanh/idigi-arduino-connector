
import java.util.Iterator;
import java.util.List;
import java.util.Arrays;

public class Constants {
    
    
    final static String start_header_string = "#ifndef REMOTE_CONFIG_H\n" + 
                                  "#define REMOTE_CONFIG_H\n\n" +
                                  "#include \"idigi_remote.h\"\n\n";
    
    final static String end_header_string = "\n\n#endif /* REMOTE_CONFIG_H */\n";
    
    final static String typedef_enum_string = "typedef enum {\n";
    
    final static String idigi_group_string = "    idigi_group_";
    final static String idigi_group_last_string = "_width = INT_MAX\n} idigi_group_id_t;\n";

    final static int ETYPE_STRING = 0;
    final static int ETYPE_MULTILINE_STRING = 1;
    final static int ETYPE_PASSWORD = 2;
    final static int ETYPE_INT32 = 3;
    final static int ETYPE_UINT32 = 4;
    final static int ETYPE_HEX32 = 5;
    final static int ETYPE_0XHEX = 6;
    final static int ETYPE_FLOAT = 7;
    final static int ETYPE_ENUM = 8;
    final static int ETYPE_ON_OFF = 9;
    final static int ETYPE_BOOLEAN = 10;
    final static int ETYPE_IPV4 = 11;
    final static int ETYPE_FQDNV4 = 12;
    final static int ETYPE_FQDNV6 = 13;
    final static int ETYPE_DATETIME = 14;
    
    public static final List<String> element_type_list = Arrays.asList("string", "multiline_string", "password", "int32", "uint32", 
                                                                        "hex32", "0xhex", "float", "enum", "on_off",
                                                                        "boolean", "ipv4", "fqdnv4", "fqdnv6", "datetime");
    
    final static boolean[] element_type_is_min_max_option = { true, true, true, true, true, 
                                                             true, true, true, false, false, 
                                                             false, false, true, true, false}; 

    final static List<String> element_access_list = Arrays.asList("read_only", "write_only", "read_write");
 
    

    public static boolean listContainsString(List<String> list, String checkStr) 
    {     
        Iterator<String> iter = list.iterator();
        while(iter.hasNext())     
        {         
            String s = iter.next();         
            if (s.contains(checkStr))
            {             
                return true;
            }     
        }
        return false;
    } 

    public static boolean isElementType(String type)
    {
        return listContainsString(element_type_list, type);
    }
    
    public static boolean isType(String type, int etype)
    {
        if (type.equalsIgnoreCase(element_type_list.get(etype)))
        {
            return true;
        }
        return false;
    }
    
    
    public static boolean isMinMaxOption(String type)
    {
        int i = element_type_list.indexOf(type);
        
        if (i != -1)
        {
            return element_type_is_min_max_option[i];
        }
        return false;
    }

    /*  
     * Finds the index of all entries in the list that matches the regex  
     * @param list The list of strings to check  
     * @param regex The regular expression to use  
     * @return list containing the indexes of all matching entries  
     * */ 
/*    
    List<Integer> getMatchingIndexes(List<String> list, String regex) 
    {   
        ListIterator<String> li = list.listIterator();
        List<Integer> indexes = new ArrayList<Integer>();
        while(li.hasNext()) 
        {
            int i = li.nextIndex();
            String next = li.next();
            if(Pattern.matches(regex, next))
            {       
                indexes.add(i);
            }
        }    
        return indexes;
    }
*/    

}

