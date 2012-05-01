package com.digi.ic.config;

import java.util.LinkedHashMap;

public class RciStrings {

    public RciStrings(String[] strPairs) // throws Exception
    {

        for (int i = 0; i < strPairs.length; i += 2) {
            /*
             * if (errors.containsKey(strPairs[i])) { throw new
             * Exception("Duplicate name: " + strPairs[i]); }
             */
            rciStrings.put(strPairs[i], strPairs[i + 1]);
        }
    }

    public RciStrings() {

    }

    public void addStrings(String key, String value) {
        rciStrings.put(key, value);
    }

    public int size() {
        return rciStrings.size();
    }

    public LinkedHashMap<String, String> getStrings() {
        return rciStrings;
    }

    private final LinkedHashMap<String, String> rciStrings = new LinkedHashMap<String, String>();

}
