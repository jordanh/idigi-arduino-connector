
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
    
    public ElementStruct(String parseName) throws BadStringOperationException
    {
        if (parseName == null)
        {
            throw new BadStringOperationException("No name specified for an element");
        }
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
            for (NameStruct value : values)
            {
                if (value.name.equals(theValue.name))
                {
                    throw new BadStringOperationException("Duplicate <value>: " + theValue.name);
                }
            }
            values.add(theValue);
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

    public boolean is_signed_integer_type()
    {
        return Constants.isType(type, Constants.ETYPE_INT32);
    }

    public boolean validate()
    {
        boolean valid = true;
       
        if (description == null)
        {
            log("Missing <description> for element: " + name);
            valid = false;
        }
        else if (!Constants.isMinMaxOption(type) && (min != null || max != null))
        {
            log("Type: " + type + " should not have <min/max> on element: " + name);
            valid = false;
        }
        else if (is_enum_type() && values.isEmpty())
        {
            log("Missing <value> for element: " + name);
            valid = false;
        }
        else if (!is_enum_type() && !values.isEmpty())
        {
            generate_config.log("No <value> for element: " + name);
            valid = false;
        }
        if (min != null)
        {
            try {
                if (is_float_type())
                {
                    Float.parseFloat(min);
                }
                else
                {
                    int ivalue = Integer.parseInt(min);
                    if (ivalue < 0 && is_signed_integer_type())
                    {
                        throw new NumberFormatException(null);
                    }
                }
            } 
            catch (NumberFormatException e){
                log("Invalid <min> value : " + min + " for type: " + type);
            }
        }
        
        if (max != null)
        {
            try {
                if (is_float_type())
                {
                    Float.parseFloat(max);
                }
                else
                {
                    int ivalue = Integer.parseInt(max);
                    if (ivalue < 0 && is_signed_integer_type())
                    {
                        throw new NumberFormatException(null);
                    }
                }
            } 
            catch (NumberFormatException e){
                log("Invalid <max> value : " + max + " for type: " + type);
            }
        }
        
        return valid;
    }
    
    public static void log(Object aObject)
    {
        System.out.println(String.valueOf(aObject));
    }

}
