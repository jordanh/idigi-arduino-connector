package com.digi.ic.config;

public class RciCommonErrors extends RciStrings {
    /* list all errors which are common to all errors "name" "description" */
    private final static String[] rciCommonErrorStrings = { "parser_error", "Parser error", 
                                                            "bad_xml", "Bad XML" };

    protected RciCommonErrors() {
        super(rciCommonErrorStrings);
    }

}
