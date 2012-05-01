package com.digi.ic.config;

public class RciGlobalErrors extends RciStrings {
    /* list all errors which are common to all errors "name" "description" */
    /* list global errors which are common to all errors "name" "description" */
    private final static String[] rciGlobalErrorStrings = { "bad_command", "Bad command",
                                                            "invalid_version", "Invalid version" };

    protected RciGlobalErrors() {
        super(rciGlobalErrorStrings);
    }

}
