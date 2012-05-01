package com.digi.ic.config;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.Map;

public class ConfigData {

    /* user setting and state groups */
    private final ArrayList<LinkedList<GroupStruct>> groupList;

    private static RciStrings userGlobalErrors = new RciStrings();

    /* user global error */
    private static Map<Object, Integer> rciErrorMap = new HashMap<Object, Integer>();

    private static RciStrings rciCommonErrors = new RciCommonErrors();
    private static RciStrings rciGlobalErrors = new RciGlobalErrors();
    private static RciStrings rciCommandErrors = new RciCommandErrors();
    private static RciStrings rciGroupErrors = new RciGroupErrors();
    private static RciStrings rciStrings = new RciParserStrings();


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

    public static LinkedHashMap<String, String> getUserGlobalErrors() {
        return userGlobalErrors.getStrings();
    }

    public static void addUserGroupError(String name, String description)
            throws Exception {

        if ((userGlobalErrors.size() > 0) && (userGlobalErrors.getStrings().containsKey(name))) {
            throw new Exception("Duplicate <globalerror>: " + name);
        }
        userGlobalErrors.addStrings(name, description);
    }

    public static LinkedHashMap<String, String> getRciCommonErrors() {
        return rciCommonErrors.getStrings();
    }

    public static LinkedHashMap<String, String> getRciGlobalErrors() {
        return rciGlobalErrors.getStrings();
    }

    public static LinkedHashMap<String, String> getRciCommandErrors() {
        return rciCommandErrors.getStrings();
    }

    public static LinkedHashMap<String, String> getRciGroupErrors() {
        return rciGroupErrors.getStrings();
    }

    public static int getRciCommonErrorsIndex() {
        return rciErrorMap.get(rciCommonErrors);
    }

    public static int getRciGlobalErrorsIndex() {
        return rciErrorMap.get(rciGlobalErrors);
    }

    public static int getRciCommandErrorsIndex() {
        return rciErrorMap.get(rciCommandErrors);
    }

    public static int getRciGroupErrorsIndex() {
        return rciErrorMap.get(rciGroupErrors);
    }

    public static int getUserGlobalErrorsIndex() {
        return rciErrorMap.get(userGlobalErrors);
    }

    public static LinkedHashMap<String, String> getRciStrings() {
        return rciStrings.getStrings();
    }

    public static Map<Object, Integer> getRciErrorMap() {
        return rciErrorMap;
    }

    public static int getAllErrorsSize() {
        int size = rciCommonErrors.size() + rciGlobalErrors.size()
                + rciCommandErrors.size() + rciGroupErrors.size() 
                + userGlobalErrors.size();

        return size;
    }
}
