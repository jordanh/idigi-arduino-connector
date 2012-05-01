package com.digi.ic.config;

public class RciGroupErrors extends RciStrings {

    /* list group errors "name" "description" */
    private final static String[] rciGroupErrorStrings = { "bad_element", "Bad element", 
                                                           "bad_value", "Bad value" };

    protected RciGroupErrors() {
        super(rciGroupErrorStrings);
    }
}
