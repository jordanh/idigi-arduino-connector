package com.digi.ic.config;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;

public class ConfigData {

    /* list all errors which are common to all errors  "name" "description" */ 
    private final static String[] allErrors = {"parser_error", "Parser error",
                                               "bad_xml", "Bad XML"};

    /* list global errors which are common to all errors  "name" "description" */ 
    private final static String[] globalErrors = {"bad_command", "Bad command",
                                                  "invalid_version", "Invalid version"};
    
    /* list command errors which are common to all errors  "name" "description" */ 
    private final static String[] commandErrors = {"bad_group", "Bad group",
                                                   "bad_index", "Bad index"};

    /* list group errors which are common to all errors  "name" "description" */ 
    private final static String[] groupErrors = {"bad_element", "Bad element",
                                                 "bad_value", "Bad value"};

    public enum ConfigType {
        SETTING (0),
        STATE (1),
        MAX (2);
        
        private int index;

        private ConfigType(int index)
        {
            this.index = index;
        }
        
        public int getIndex()
        {
            return index;
        }

        public static ConfigType toConfigType(String str)
        {
            try {
                return valueOf(str.toUpperCase());
            } catch (Exception e) {
                return MAX;
            }
        }
    }
    
    public ConfigData()
    {
        groupList = new ArrayList<LinkedList<GroupStruct>>();
        
        ConfigType type;
        LinkedList<GroupStruct> groups;
        
        type = ConfigType.toConfigType("setting");
        groups = new LinkedList<GroupStruct>();
        groupList.add(type.getIndex(), groups);
        
        type = ConfigType.toConfigType("state");
        groups = new LinkedList<GroupStruct>();
        groupList.add(type.getIndex(), groups);
        
        groupGlobalErrors = new LinkedList<NameStruct>();
        
    }
    
    public LinkedList<GroupStruct> getSettingGroups() throws IOException
    {
        return getConfigGroup("setting");
    }

    public LinkedList<GroupStruct> getStateGroups() throws IOException
    {
        return getConfigGroup("state");
    }

    public LinkedList<NameStruct> getGroupGlobalErrors()
    {
        return groupGlobalErrors;
    }

    
    public LinkedList<GroupStruct> getConfigGroup(String type) throws IOException
    {
        ConfigType t = ConfigType.toConfigType(type);
        
        LinkedList<GroupStruct> config = null;
        
        switch (t)
        {
        case SETTING:
        case STATE:
            config = groupList.get(t.getIndex());
            break;
         default:
             throw new IOException("Missing Setting or State keyword");
        }
        
        return config;
    }

    public static void initTopLevelErrors() throws IOException
    {
        allErrorList = new LinkedList<NameStruct>();
        getErrors(allErrorList, allErrors);
        
        globalErrorList = new LinkedList<NameStruct>();
        getErrors(globalErrorList, globalErrors);
        
        commandErrorList = new LinkedList<NameStruct>();
        getErrors(commandErrorList, commandErrors);

        groupErrorList = new LinkedList<NameStruct>();
        getErrors(groupErrorList, groupErrors);
    }

    private static boolean getErrors(LinkedList<NameStruct> linkedList, String[] error_strings) throws IOException
    {
        for (int i=0; i < error_strings.length; i++)
        {
            if (!Parser.checkAlphaNumeric(error_strings[i]))
            {
                throw new IOException("Invalid name in error strings");
            }
            NameStruct error = new NameStruct(error_strings[i], error_strings[i+1]);
            linkedList.add(error);
            i++;
            
        }
        return true;
    }

    /*  top-level global errors */
    public static LinkedList<NameStruct> allErrorList;
    public static LinkedList<NameStruct> globalErrorList;
    public static LinkedList<NameStruct> commandErrorList;
    public static LinkedList<NameStruct> groupErrorList;
    
    /* user setting and state groups */
    private ArrayList<LinkedList<GroupStruct>> groupList;
    /* user global error */
    private LinkedList<NameStruct> groupGlobalErrors;


}
