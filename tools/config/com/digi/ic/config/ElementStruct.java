package com.digi.ic.config;

import java.io.IOException;
import java.util.LinkedList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ElementStruct {
    
    private final static int INT32_MIN_VALUE = -2147483648;
    private final static int INT32_MAX_VALUE = 2147483647;
    private final static long UINT32_MAX_VALUE = 4294967295L;

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
    
    public static boolean includeFloatLimit()
    {
        return floatLimit;
    }
 
    public ElementStruct(String nameStr, String descStr) throws IOException
    {
/*        if (nameStr == null)
        {
            throw new IOException("Missing element name");
        }
*/
        
        if (descStr == null)
        {
            throw new IOException("Missing element description");
        }

        name = nameStr;
        description = descStr;
        values = new LinkedList<NameStruct>();
        
    }

    public void setType(String theType) throws IOException
    {
        if (type == null)
            type = theType;
        else throw new IOException("Duplicate <type>: " + theType);
    }
    
    public void setAccess(String theAccess) throws IOException
    {
        if (access == null)
            access = theAccess;
         else throw new IOException("Duplicate <access>: " + theAccess);
    }
    
    public void setMin(String theMin) throws IOException
    {
        if (min == null)
            min = theMin;
         else throw new IOException("Duplicate <min>: " + theMin);
    }
    public void setMax(String theMax) throws IOException
    {
        if (max == null)
            max = theMax;
         else throw new IOException("Duplicate <max>: " + theMax);
    }

    public void addValue(NameStruct theValue) throws IOException
    {
        if (type == null)
        {
            throw new IOException("Missing <type> on element: " + name);
        }
        else if (ElementType.toElementType(type) == ElementType.ENUM)
        {
            for (NameStruct value : values)
            {
                if (value.getName().equals(theValue.getName()))
                {
                    throw new IOException("Duplicate <value>: " + theValue.getName());
                }
            }
            values.add(theValue);
        }
        else
        {
            throw new IOException("Invalid <value> for type: " + type);
        }
    }

    public void setUnit(String theUnit) throws IOException
    {
        if (unit == null)
            unit = theUnit;
         else throw new IOException("Duplicate unit: " + theUnit);
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
    
    public void validate() throws Exception
    {
        ElementType etype = ElementType.toElementType(type);
        
        if (type == null)
        {
            ConfigGenerator.log("Missing <type>!");
            throw new Exception("Error on \"" + name + "\" element!");
        }
        
        switch (etype)
        {
        case ENUM:
            if (values.isEmpty())
            {
                ConfigGenerator.log("Missing <value>!");
                throw new Exception("Error on \"" + name + "\" element!");
            }
            break;
            
        case FLOAT:
            if ((!isValidFloat(min)) || (!isValidFloat(max)))
            {
                ConfigGenerator.log("Invalid float value!");
                throw new Exception("Error on \"" + name + "\" element!");
            }
            
            if ((min != null) && (max != null))
            {
                float minValue = Float.parseFloat(min);
                float maxValue = Float.parseFloat(max);
                
                if (minValue > maxValue)
                {
                    ConfigGenerator.log("Error min value > max value!");
                    throw new Exception("Error on \"" + name + "\" element!");
                }
            }
            
            if (((min == null) || (max == null)) && ((min != null) || (max != null)))
            {
                floatLimit = true; 
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
                ConfigGenerator.log("Invalid hex value!");
                throw new Exception("Error on \"" + name + "\" element!");
            }
            if ((min != null) && (max != null))
            {
                int index = 0;
                if (prefix)
                {
                    index = 2;
                }
                
                int minValue = Integer.parseInt(min.substring(index), 16);
                int maxValue = Integer.parseInt(max.substring(index), 16);
                
                if (minValue > maxValue)
                {
                    ConfigGenerator.log("Error min value > max value!");
                    throw new Exception("Error on \"" + name + "\" element!");
                }
            }
            break;

        default:
            long minValue = (etype == ElementStruct.ElementType.INT32) ? INT32_MIN_VALUE : 0;
            long maxValue = (etype == ElementStruct.ElementType.INT32) ? INT32_MAX_VALUE : UINT32_MAX_VALUE;
            
            if (min != null)
            {
                try {
                    minValue = Long.parseLong(min);
                } catch (NumberFormatException e) {
                    ConfigGenerator.log("Invalid min value: " + min);
                    throw new Exception("Error on \"" + name + "\" element!");
                }
            }
            if (max != null)
            {
                try {
                    maxValue = Long.parseLong(max);
                } catch (NumberFormatException e) {
                    ConfigGenerator.log("Invalid max value: " + max);
                    throw new Exception("Error on \"" + name + "\" element!");
                }
                    
            }
            
            if (etype != ElementStruct.ElementType.INT32)
            {
                if ((minValue < 0) || (maxValue < 0))
                {
                    ConfigGenerator.log("Invalid min or max value for type: " + etype.toString().toLowerCase());
                    throw new Exception("Error on \"" + name + "\" element!");
                }
                if ((minValue > UINT32_MAX_VALUE) || (maxValue > UINT32_MAX_VALUE))
                {
                    ConfigGenerator.log("Overflow min or max value for type: " + etype.toString().toLowerCase());
                    throw new Exception("Error on \"" + name + "\" element!");
                }
            }
            else if ((minValue < INT32_MIN_VALUE) || (minValue > INT32_MAX_VALUE) || 
                     (maxValue < INT32_MIN_VALUE) || (maxValue > INT32_MAX_VALUE))
            {
                ConfigGenerator.log("Overflow min or max value for type: " + etype.toString().toLowerCase());
                throw new Exception("Error on \"" + name + "\" element!");
            }
            
            if (minValue > maxValue)
            {
                ConfigGenerator.log("Error min value > max value!");
                throw new Exception("Error on \"" + name + "\" element!");
            }

            break;
        }
        
        if (!etype.minMaxSupport() && (min != null || max != null))
        {
            ConfigGenerator.log("\"" + type + "\" type should not have <min/max>!");
            throw new Exception("Error on \"" + name + "\" element!");
        }

        if (etype != ElementType.ENUM && !values.isEmpty())
        {
            ConfigGenerator.log("Invalid <value> for type: " + type);
            throw new Exception("Error on \"" + name + "\" element!");
        }
        
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
        
        String HEXCOMPILE = "[A-Fa-f0-9]{1,8}";
        if (prefix_0x)
        {
            HEXCOMPILE = "0x[A-Fa-f0-9]{1,8}";
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

    private static boolean floatLimit;
}
