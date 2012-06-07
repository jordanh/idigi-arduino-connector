package com.digi.ic.config;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.Map;

public class ConfigData {

    /* user setting and state groups */
    private ArrayList<LinkedList<GroupStruct>> groupList;

    private RciStrings userGlobalErrors = new RciStrings();

    /* user global error */
    private Map<Object, Integer> rciErrorMap = new HashMap<Object, Integer>();

    private RciStrings rciCommonErrors = new RciCommonErrors();
    private RciStrings rciGlobalErrors = new RciGlobalErrors();
    private RciStrings rciCommandErrors = new RciCommandErrors();
    private RciStrings rciGroupErrors = new RciGroupErrors();
    private RciStrings rciStrings = new RciParserStrings();


    public enum ConfigType {
        SETTING(0), STATE(1);

        private int index;
        private final static int configTypeCount = 2;

        private ConfigType(int index) {
            this.index = index;
        }

        public int getIndex() {
            return index;
        }

        public static int getConfigTypeCount() {
            return configTypeCount;
        }

        public static ConfigType toConfigType(String str) throws Exception {
            if (str == null) {
                throw new Exception("Missing setting or state keyword!");
            }
            try {
                return valueOf(str.toUpperCase());
            } catch (Exception e) {
                throw new Exception("Invalid setting or state keyword: " + str);
            }
        }
    }

    public ConfigData() throws Exception {
        groupList = new ArrayList<LinkedList<GroupStruct>>();

        ConfigType type;
        LinkedList<GroupStruct> groups;

        type = ConfigType.toConfigType("setting");
        groups = new LinkedList<GroupStruct>();
        groupList.add(type.getIndex(), groups);

        type = ConfigType.toConfigType("state");
        groups = new LinkedList<GroupStruct>();
        groupList.add(type.getIndex(), groups);

        int index = 1;
        rciErrorMap.put(rciCommonErrors, index);

        index += rciCommonErrors.size();
        rciErrorMap.put(rciGlobalErrors, index);

        index += rciGlobalErrors.size();
        rciErrorMap.put(rciCommandErrors, index);

        index += rciCommandErrors.size();
        rciErrorMap.put(rciGroupErrors, index);

        index += rciGroupErrors.size();
        rciErrorMap.put(userGlobalErrors, index);
    }

    public LinkedList<GroupStruct> getSettingGroups() throws Exception {
        return getConfigGroup("setting");
    }

    public LinkedList<GroupStruct> getStateGroups() throws Exception {
        return getConfigGroup("state");
    }

    public LinkedList<GroupStruct> getConfigGroup(String type) throws Exception {
        ConfigType groupType = ConfigType.toConfigType(type);

        LinkedList<GroupStruct> config = groupList.get(groupType.getIndex());

        return config;
    }

    public LinkedHashMap<String, String> getUserGlobalErrors() {
        return userGlobalErrors.getStrings();
    }

    public void addUserGroupError(String name, String description)
            throws Exception {

        if ((userGlobalErrors.size() > 0) && (userGlobalErrors.getStrings().containsKey(name))) {
            throw new Exception("Duplicate <globalerror>: " + name);
        }
        
        if (description == null) {
            throw new IOException("Missing or bad globalerror description");
      }

        userGlobalErrors.addStrings(name, description);
    }

    public LinkedHashMap<String, String> getRciCommonErrors() {
        return rciCommonErrors.getStrings();
    }

    public LinkedHashMap<String, String> getRciGlobalErrors() {
        return rciGlobalErrors.getStrings();
    }

    public LinkedHashMap<String, String> getRciCommandErrors() {
        return rciCommandErrors.getStrings();
    }

    public LinkedHashMap<String, String> getRciGroupErrors() {
        return rciGroupErrors.getStrings();
    }

    public int getRciCommonErrorsIndex() {
        return rciErrorMap.get(rciCommonErrors);
    }

    public int getRciGlobalErrorsIndex() {
        return rciErrorMap.get(rciGlobalErrors);
    }

    public int getRciCommandErrorsIndex() {
        return rciErrorMap.get(rciCommandErrors);
    }

    public int getRciGroupErrorsIndex() {
        return rciErrorMap.get(rciGroupErrors);
    }

    public int getUserGlobalErrorsIndex() {
        return rciErrorMap.get(userGlobalErrors);
    }

    public LinkedHashMap<String, String> getRciStrings() {
        return rciStrings.getStrings();
    }

    public Map<Object, Integer> getRciErrorMap() {
        return rciErrorMap;
    }

    public int getAllErrorsSize() {
        int size = rciCommonErrors.size() + rciGlobalErrors.size()
                + rciCommandErrors.size() + rciGroupErrors.size() 
                + userGlobalErrors.size();

        return size;
    }
}
