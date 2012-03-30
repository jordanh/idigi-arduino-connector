
import java.util.LinkedList;

import javax.management.BadStringOperationException;
import javax.naming.NamingException;

public class GroupStruct {

    public String name;
    public int instances;
    public String description;
    public LinkedList<ElementStruct> elements;
    public LinkedList<NameStruct> errors;
    
    public GroupStruct(String theName) throws BadStringOperationException
    {
        if (theName == null)
        {
            throw new BadStringOperationException("No name specified for a group");
        }
        name = theName;
        instances= 0;
        description=null;
        elements = new LinkedList<ElementStruct>();
        errors = new LinkedList<NameStruct>();
//        icConfigTool.log("Group: " + name);
    }

    public void addConfig(int theInstances)
    {
        instances = theInstances;
    }

    public void addConfig(String theDescription)
    {
        description = theDescription;
    }
    
    public void addConfigElement(ElementStruct theElement) throws NamingException
    {
        
        for (ElementStruct element : elements)
        {
            if (element.name.equals(theElement.name))
            {
                throw new NamingException("Duplicate <element>: " + theElement.name);
            }
        }
        elements.add(theElement);
    }

    public void addConfigError(NameStruct theError) throws BadStringOperationException
    {
        for (NameStruct error : errors)
        {
            if (error.name.equals(theError.name))
            {
                throw new BadStringOperationException("Duplicate <error>: " + theError.name);
            }
        }
        errors.add(theError);
    }

    public boolean validate()
    {
        boolean valid = true;
        
        if (description == null)
        {
            generate_config.log("Missing description");
            valid = false;
        }
        else if (elements.isEmpty())
        {
            generate_config.log("No element specified");
            valid = false;
        }
        return valid;
        
    }

}
