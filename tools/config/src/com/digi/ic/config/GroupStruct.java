package com.digi.ic.config;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.LinkedList;

import javax.naming.NamingException;

public class GroupStruct {

    private final String name;
    private final int instances;
    private final String description;
    private final String helpDescription;
    private final LinkedList<ElementStruct> elements;
    private final LinkedHashMap<String, String> errorMap;

    public GroupStruct(String name, int count, String description,
            String helpDescription) throws IOException {
        if (count <= 0) {
            throw new IOException("Invalid instance count for the group: "
                    + name);
        }

        if (description == null) {
            throw new IOException("Missing or bad group description");
       }

        this.name = name;
        this.instances = count;
        this.description = description;
        this.helpDescription = helpDescription;
        elements = new LinkedList<ElementStruct>();
        errorMap = new LinkedHashMap<String, String>();
    }

    @Override
    public String toString() {
        String descText = description;
        if (helpDescription != null)
            descText += ":" + helpDescription;

        String descriptor = String.format(
                "<descriptor element=\"%s\" desc=\"%s\">", name, descText);

        return descriptor;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public String getHelpDescription() {
        return helpDescription;
    }

    public int getInstances() {
        return instances;
    }

    public LinkedList<ElementStruct> getElements() {
        return elements;
    }

    public LinkedHashMap<String, String> getErrors() {
        return errorMap;
    }

    public void addElement(ElementStruct theElement) throws NamingException {

        for (ElementStruct element : elements) {
            if (element.getName().equals(theElement.getName())) {
                throw new NamingException("Duplicate <element>: "
                        + theElement.getName());
            }
        }
        elements.add(theElement);
    }

    public void addError(String name, String description) throws IOException {
        if (errorMap.containsKey(name)) {
            throw new IOException("Duplicate <error>: " + name);
        }
        if (description == null) {
            throw new IOException("Missing or bad error description");
      }
        errorMap.put(name, description);
    }

    public void validate() throws Exception{
        
        if (elements.isEmpty()) {
            throw new Exception("No element specified");
        }
        
    }

}
