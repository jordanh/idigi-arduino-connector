package com.digi.ic.config;

public class RciGroupErrors extends RciStrings {

    /* list group errors which are common to all errors "name" "description" */
    private final static String[] rciGroupErrorStrings = { "bad_element",
            "Bad element", "bad_value", "Bad value" };

    protected RciGroupErrors() {
        super(rciGroupErrorStrings);
    }
}
