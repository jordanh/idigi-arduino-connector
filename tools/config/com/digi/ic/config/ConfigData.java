
import java.io.IOException;
import java.util.LinkedList;

public class ConfigData {

    public enum ConfigType {
        STATE (0),
        SETTING (1),
        MAX_TYPE (2);
        
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
                return MAX_TYPE;
            }
        }
    }
    private LinkedList<GroupStruct> settingGroups;
    private LinkedList<GroupStruct> stateGroups;
    private LinkedList<NameStruct> globalErrorGroups;

    public ConfigData()
    {
        settingGroups = new LinkedList<GroupStruct>();
        stateGroups = new LinkedList<GroupStruct>();
        globalErrorGroups = new LinkedList<NameStruct>();
        
    }
    public LinkedList<GroupStruct> getSettingGroups()
    {
        return settingGroups;
    }

    public LinkedList<GroupStruct> getStateGroups()
    {
        return stateGroups;
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
        case STATE:
            config = stateGroups;
            break;
        case SETTING:
            config = settingGroups;
            break;
         default:
             throw new IOException("Invalid type: " + type);
        }
        
        return config;
    }


}
