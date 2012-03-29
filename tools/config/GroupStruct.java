
import java.util.LinkedList;

import javax.management.BadStringOperationException;

public class GroupStruct {

    public String name;
    public int instances;
    public String description;
    public LinkedList<ElementStruct> elements;
    public LinkedList<NameStruct> errors;
    
    public GroupStruct(String theName)
    {
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
    
    public void addConfigElement(ElementStruct theElement) throws BadStringOperationException
    {
//        icConfigTool.log("add " + theElement.name + " element for group " + name);
        if (!elements.equals(theElement.name))
        {
            elements.add(theElement);
        }
        else
        {
            throw new BadStringOperationException("Duplicate <element>: " + theElement.name);
        }
    }

    public void addConfigError(NameStruct theError) throws BadStringOperationException
    {
        if (!errors.equals(theError.name))
        {
            errors.add(theError);
        }
        else
        {
            throw new BadStringOperationException("Duplicate <error>: " + theError.name + "for group: " + name);
        }
        
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
