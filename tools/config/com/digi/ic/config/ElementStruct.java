package com.digi.ic.config;

import java.util.LinkedList;

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
    
 
    public ElementStruct(String theName, String theDesc) throws BadStringOperationException
    {
        if (theName == null)
        {
            throw new BadStringOperationException("No name specified for an element");
        }
        name = theName;
        description = theDesc;
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
        
        if (description == null)
        {
            ConfigGenerator.log("Missing <description> for element: " + name);
            valid = false;
        }
        else if (!etype.minMaxSupport() && (min != null || max != null))
        {
            ConfigGenerator.log("Type: " + type + " should not have <min/max> on element: " + name);
            valid = false;
        }
        else if (etype == ElementType.ENUM && values.isEmpty())
        {
            ConfigGenerator.log("Missing <value> for element: " + name);
            valid = false;
        }
        else if (etype != ElementType.ENUM && !values.isEmpty())
        {
            ConfigGenerator.log("No <value> for element: " + name);
            valid = false;
        }
        if (min != null)
        {
            try {
                if (etype == ElementType.FLOAT)
                {
                    Float.parseFloat(min);
                }
                else
                {
                    int ivalue = Integer.parseInt(min);
                    if (ivalue < 0 && etype == ElementType.INT32)
                    {
                        throw new NumberFormatException(null);
                    }
                }
            } 
            catch (NumberFormatException e){
                ConfigGenerator.log("Invalid <min> value : " + min + " for type: " + type);
            }
        }
        
        if (max != null)
        {
            try {
                if (etype == ElementType.FLOAT)
                {
                    Float.parseFloat(max);
                }
                else
                {
                    int ivalue = Integer.parseInt(max);
                    if (ivalue < 0 && etype == ElementType.INT32)
                    {
                        throw new NumberFormatException(null);
                    }
                }
            } 
            catch (NumberFormatException e){
                ConfigGenerator.log("Invalid <max> value : " + max + " for type: " + type);
            }
        }
        
        return valid;
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
