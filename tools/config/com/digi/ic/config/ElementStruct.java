package com.digi.ic.config;

import java.util.LinkedList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.management.BadStringOperationException;

public class ElementStruct {
    
    public enum ElementType {
        STRING (true),
        MULTILINE_STRING (true),
        PASSWORD (true),
        INT32 (true), 
        UINT32 (true),
        HEX32 (true),
        XHEX (true),
        FLOAT (true),
        ENUM (false),
        ON_OFF (false),
        BOOLEAN (false),
        IPV4 (false),
        FQDNV4 (true),
        FQDNV6 (true),
        DATETIME (false),
        INVALID_TYPE (false);

        /* special type since string cannot start 0x (zero)  */
        private final static String STRING_0XHEX = "0xhex";
        
        private final boolean minMaxSupport;
        
        private ElementType(boolean support) 
        {
            this.minMaxSupport = support;
        }

        public boolean minMaxSupport() { return minMaxSupport; }
        
        public static ElementType toElementType(String str)
        {
            try {
                
                /* special case for 0xhex type */
                if (str.equalsIgnoreCase(STRING_0XHEX))
                {
                    return valueOf(str.toUpperCase().substring(1));
                }
                else
                {
                    return valueOf(str.toUpperCase());
                }
            } catch (Exception e) {
                return INVALID_TYPE;
            }
            
        }
    }
    
    public enum AccessType {
        READ_ONLY,
        WRITE_ONLY,
        READ_WRITE,
        INVALID_TYPE;
        
        public static AccessType toAccessType(String str)
        {
            try {
                return valueOf(str.toUpperCase());
                
            } catch (Exception e) {
                return INVALID_TYPE;
            }
        }
    }
    
 
    public ElementStruct(String nameStr, String descStr) throws BadStringOperationException
    {
/*        if (nameStr == null)
        {
            throw new BadStringOperationException("Missing element name");
        }
*/
        
        if (descStr == null)
        {
            throw new BadStringOperationException("Missing element description");
        }

        name = nameStr;
        description = descStr;
        values = new LinkedList<NameStruct>();
        
    }

    public void setType(String theType) throws BadStringOperationException
    {
        if (type == null)
            type = theType;
        else throw new BadStringOperationException("Duplicate <type>: " + theType);
    }
    
    public void setAccess(String theAccess) throws BadStringOperationException
    {
        if (access == null)
            access = theAccess;
         else throw new BadStringOperationException("Duplicate <access>: " + theAccess);
    }
    
    public void setMin(String theMin) throws BadStringOperationException
    {
        if (min == null)
            min = theMin;
         else throw new BadStringOperationException("Duplicate <min>: " + theMin);
    }
    public void setMax(String theMax) throws BadStringOperationException
    {
        if (max == null)
            max = theMax;
         else throw new BadStringOperationException("Duplicate <max>: " + theMax);
    }

    public void addValue(NameStruct theValue) throws BadStringOperationException
    {
        if (ElementType.toElementType(type) == ElementType.ENUM)
        {
            for (NameStruct value : values)
            {
                if (value.getName().equals(theValue.getName()))
                {
                    throw new BadStringOperationException("Duplicate <value>: " + theValue.getName());
                }
            }
            values.add(theValue);
        }
        else
        {
            throw new BadStringOperationException("Invalid <value> for type: " + type);
        }
    }

    public void setUnit(String theUnit) throws BadStringOperationException
    {
        if (unit == null)
            unit = theUnit;
         else throw new BadStringOperationException("Duplicate unit: " + theUnit);
    }
    
    public String getName()
    {
        return name;
    }
    
    public String getDescription()
    {
        return description;
    }
    
    public String getType()
    {
        return type;
    }

    public String getAccess()
    {
        return access;
    }

    public String getMin()
    {
        return min;
    }

    public String getMax()
    {
        return max;
    }

    public String getUnit()
    {
        return unit;
    }
    
    public LinkedList<NameStruct> getValues()
    {
        return values;
    }
    
    public boolean validate()
    {
        boolean valid = true;
        
        ElementType etype = ElementType.toElementType(type);
        
        if (!etype.minMaxSupport() && (min != null || max != null))
        {
            ConfigGenerator.log("Type: " + type + " should not have <min/max> on element: " + name);
            valid = false;
        }
        
        switch (etype)
        {
        case ENUM:
            if (values.isEmpty())
            {
                ConfigGenerator.log("Missing <value> for element: " + name);
                valid = false;
            }
            break;
            
        case FLOAT:
            if ((!isValidFloat(min)) || (!isValidFloat(max)))
            {
                ConfigGenerator.log("Invalid float value for element " + name);
                valid = false;
            }
            break;
            
        case XHEX:
        case HEX32:
            boolean prefix = false;
            if (etype == ElementType.XHEX)
            {
                prefix = true;
            }
            if ((!isValidHex(min, prefix)) || (!isValidHex(max, prefix)))
            {
                ConfigGenerator.log("Invalid hex value for element " + name);
                valid = false;
            }
            break;
            
        default:
            long min_value = 0;
            long max_value = 0;
            if (min != null)
            {
                min_value = Long.parseLong(min);
            }
            if (max != null)
            {
                max_value = Long.parseLong(max);
            }
            
            if (min_value < 0 || max_value < 0 || max_value < min_value)
            {
                ConfigGenerator.log("Invalid min or max value for type: " + etype.toString().toLowerCase());
                valid = false;
            }
            break;
        }
        
        
        if (etype != ElementType.ENUM && !values.isEmpty())
        {
            ConfigGenerator.log("Invalid <value> for type: " + type + " on element: " + name);
            valid = false;
        }
        return valid;
    }

    private boolean isValidFloat(String str) 
    {
        boolean isValid = true;
        if (str != null)
        {
            try {
                Float.parseFloat(str);
            } 
            catch (NumberFormatException e){
                isValid = false;
            }
        }
        return isValid;
    }
    
    private boolean isValidHex(String str, boolean prefix_0x)
    {
        boolean isValid = true;
        
        String HEXCOMPILE = "[A-Fa-f0-9]";
        if (prefix_0x)
        {
            HEXCOMPILE= "^0x" + "[A-Fa-f0-9]";
        }
        
        Pattern HEXPATTERN = Pattern.compile(HEXCOMPILE);

        if (str != null)
        {
            Matcher m = HEXPATTERN.matcher(str);
            isValid = m.matches();
        }
        
        return isValid;

    }
    private String name;
    private String description;
    private String type;
    private String access;
    private String min;
    private String max;
    private String unit;
    private LinkedList<NameStruct> values;

}
