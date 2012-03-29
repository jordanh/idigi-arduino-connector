
import java.util.LinkedList;

import javax.management.BadStringOperationException;

public class ElementStruct {
    public String name;
    public String description;
    public String type;
    public String access;
    public String min;
    public String max;
    public String unit;
    public LinkedList<NameStruct> values;
    
    public ElementStruct(String parseName) 
    {
        name = parseName;
        description = null;
        type=null;
        access=null;
        min=null;
        max=null;
        unit=null;
        values = new LinkedList<NameStruct>();
    }

    public void addConfig(String theDescription)
    {
        description = theDescription;
    }
    
    public void addConfigType(String theType) throws BadStringOperationException
    {
        if (type == null)
            type = theType;
        else throw new BadStringOperationException("Duplicate <type>: " + theType);
    }
    
    public void addConfigAccess(String theAccess) throws BadStringOperationException
    {
        if (access == null)
            access = theAccess;
         else throw new BadStringOperationException("Duplicate <access>: " + theAccess);
    }
    
    public void addConfigMin(String theMin) throws BadStringOperationException
    {
        if (min == null)
            min = theMin;
         else throw new BadStringOperationException("Duplicate <min>: " + theMin);
    }
    public void addConfigMax(String theMax) throws BadStringOperationException
    {
        if (max == null)
            max = theMax;
         else throw new BadStringOperationException("Duplicate <max>: " + theMax);
    }

    public void addConfigValue(NameStruct theValue) throws BadStringOperationException
    {
        if (is_enum_type())
        {
            if (!values.equals(theValue))
            {
                values.add(theValue);
            }
            else
            {
                throw new BadStringOperationException("Duplicate <value>: " + theValue.name);
            }
        }
        else
        {
            throw new BadStringOperationException("Invalid <value> for type: " + type);
        }
    }

    public void addConfigUnit(String theUnit) throws BadStringOperationException
    {
        if (unit == null)
            unit = theUnit;
         else throw new BadStringOperationException("Duplicate unit: " + theUnit);
    }

    public boolean is_min_max_needed()
    {
        return Constants.isMinMaxOption(type);
    }

    public boolean is_enum_type()
    {
        return Constants.isType(type, Constants.ETYPE_ENUM);
    }

    public boolean is_float_type()
    {
        return Constants.isType(type, Constants.ETYPE_FLOAT);
    }

    public boolean validate()
    {
        boolean valid = true;
        
        if (description == null)
        {
            generate_config.log("Missing <description> for element: " + name);
            valid = false;
        }
        else if (!Constants.isMinMaxOption(type) && (min != null || max != null))
        {
            generate_config.log("Invalid <min/max> for type: " + type + " on element: " + name);
            valid = false;
        }
        else if (is_enum_type() && values.isEmpty())
        {
            generate_config.log("Missing <value> for element: " + name);
            valid = false;
        }
        else if (!is_enum_type() && !values.isEmpty())
        {
            generate_config.log("Invalid <value> for type: " + type);
            valid = false;
        }
        
        return valid;
    }
}
