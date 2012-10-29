package com.digi.ic.config;

public class RciCommandErrors extends RciStrings {

    /* list command errors "name" "description" */
    private final static String[] rciCommandErrorStrings = { "bad_group", "Bad group", 
                                                             "bad_index", "Bad index" };

    protected RciCommandErrors() {
        super(rciCommandErrorStrings);
    }

}
