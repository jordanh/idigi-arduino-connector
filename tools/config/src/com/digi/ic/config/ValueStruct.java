package com.digi.ic.config;

public class ValueStruct {

    private final String name;
    private final String description;
    private final String helpDescription;

    public ValueStruct(String name, String description, String helpDescription) {

        this.name = name;
        this.description = description;
        this.helpDescription = helpDescription;
    }

    public String getName() {
        return name;
    }

    @Override
    public String toString() {
        String descText = "";

        if (description != null)
            descText += description;
        if (helpDescription != null)
            descText += ":" + helpDescription;

        String descriptor = String
                .format("<value value=\"%s\"", name);
        if (descText.length() > 0)
            descriptor += String.format(" desc=\"%s\"", descText);
        descriptor += " />";

        return descriptor;
    }

}
