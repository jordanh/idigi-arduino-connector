package com.digi.ic.config;

public class RciGlobalErrors extends RciStrings {
    /* list global errors s "name" "description" */
    private final static String[] rciGlobalErrorStrings = { "bad_command", "Bad command",
                                                            "invalid_version", "Invalid version" };

    protected RciGlobalErrors() {
        super(rciGlobalErrorStrings);
    }

}
