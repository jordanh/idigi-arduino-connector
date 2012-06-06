package com.digi.ic.config;

public class RciParserStrings extends RciStrings {

    /* Strings for RCI parser. Macros are created in remote_config.h */
    private final static String[] rciParserStrings = { "ENTITY_QUOTE", "quot",
                                        "ENTITY_AMPERSAND", "amp", 
                                        "ENTITY_APOSTROPHE", "apos",
                                        "ENTITY_LESS_THAN", "lt",
                                        "ENTITY_GREATER_THAN", "gt",
                                        "SET_SETTING", "set_setting",
                                        "QUERY_SETTING", "query_setting",
                                        "SET_STATE", "set_state",
                                        "QUERY_STATE", "query_state",
                                        "VERSION", "version",
                                        "VERSION_SUPPORTED", "1.1",
                                        "REPLY", "rci_reply",
                                        "REQUEST", "rci_request",
                                        "INDEX", "index",
                                        "ID", "id",
                                        "HINT","hint",
                                        "ERROR", "error",
                                        "COMMENT", "!--",
                                        "DESC", "desc"};

    protected RciParserStrings() {
        super(rciParserStrings);
    }

}
