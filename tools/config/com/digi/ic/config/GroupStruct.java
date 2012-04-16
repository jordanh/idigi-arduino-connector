package com.digi.ic.config;

import java.util.LinkedList;

import javax.management.BadStringOperationException;
import javax.naming.NamingException;

public class GroupStruct {

    public GroupStruct(String nameStr, int count, String descStr) throws BadStringOperationException
    {
        if (count < 0)
        {
            throw new BadStringOperationException("Invalid instance count for the group: " + nameStr);
        }
/*         if (nameStr == null)
        {
            throw new BadStringOperationException("Missing group name");
        }
*/
        if (descStr == null)
        {
            throw new BadStringOperationException("Missing group description");
        }
        
        name = nameStr;
        instances= count;
        description=descStr;
        elements = new LinkedList<ElementStruct>();
        errors = new LinkedList<NameStruct>();
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
    
    public LinkedList<NameStruct> getErrors()
    {
        return errors;
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

    public void addError(NameStruct theError) throws BadStringOperationException
    {
        for (NameStruct error : errors)
        {
            if (error.getName().equals(theError.getName()))
            {
                throw new BadStringOperationException("Duplicate <error>: " + theError.getName());
            }
        }
        errors.add(theError);
    }

    public boolean validate()
    {
        boolean valid = true;
        if (elements.isEmpty())
        {
            ConfigGenerator.log("No element specified");
            valid = false;
        }
        return valid;
        
    }

    private String name;
    private int instances;
    private String description;
    private LinkedList<ElementStruct> elements;
    private LinkedList<NameStruct> errors;
    

}
