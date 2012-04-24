package com.digi.ic.config;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.LinkedList;

import javax.naming.NamingException;

public class GroupStruct {

    public GroupStruct(String nameStr, int count, String descStr) throws IOException
    {
        if (count < 0)
        {
            throw new IOException("Invalid instance count for the group: " + nameStr);
        }

        if (descStr == null)
        {
            throw new IOException("Missing group description");
        }

        name = nameStr;
        instances= count;
        description=descStr;
        elements = new LinkedList<ElementStruct>();
        errorMap = new LinkedHashMap<String, String>();
    }

    public String getName()
    {
        return name;
    }

    public String getDescription()
    {
        return description;
    }

    public int getInstances()
    {
        return instances;
    }

    public LinkedList<ElementStruct> getElements()
    {
        return elements;
    }

    public LinkedHashMap<String, String> getErrors()
    {
        return errorMap;
    }

    public void addElement(ElementStruct theElement) throws NamingException
    {

        for (ElementStruct element : elements)
        {
            if (element.getName().equals(theElement.getName()))
            {
                throw new NamingException("Duplicate <element>: " + theElement.getName());
            }
        }
        elements.add(theElement);
    }

    public void addError(String name, String description) throws IOException
    {
        if (errorMap.containsKey(name))
        {
            throw new IOException("Duplicate <error>: " + name);
        }
        errorMap.put(name, description);
    }

    public boolean validate()
    {
        boolean isValid = true;

        if (elements.isEmpty())
        {
            ConfigGenerator.log("No element specified");
            isValid = false;
         }

        return isValid;
    }

    private final String name;
    private final int instances;
    private final String description;
    private final LinkedList<ElementStruct> elements;
    private final LinkedHashMap<String, String> errorMap;


}
