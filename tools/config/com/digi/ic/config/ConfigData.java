package com.digi.ic.config;

import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;

public class ConfigData {

    public enum ConfigType {
        SETTING (0),
        STATE (1),
        INVALID_TYPE (-1);
        
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
                return INVALID_TYPE;
            }
        }
    }
    private ArrayList<LinkedList<GroupStruct>> groupList; 
//    private LinkedList<GroupStruct> stateGroups;
    private LinkedList<NameStruct> globalErrorGroups;

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
        
        globalErrorGroups = new LinkedList<NameStruct>();
        
    }
    
    public LinkedList<GroupStruct> getSettingGroups() throws IOException
    {
        return getConfigGroup("setting");
    }

    public LinkedList<GroupStruct> getStateGroups() throws IOException
    {
        return getConfigGroup("state");
    }

    public LinkedList<NameStruct> getErrorGroups()
    {
        return globalErrorGroups;
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
             throw new IOException("Invalid type: " + type);
        }
        
        return config;
    }


}
