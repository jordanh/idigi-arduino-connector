package com.digi.ic.config;

public class NameStruct {
    
    public NameStruct(String nameStr) 
    {
        name = nameStr;
    }

    public NameStruct(String nameStr, String descStr) 
    {
        name = nameStr;
        description=descStr;
    }


    public void setDescription(String descStr)
    {
        description = descStr;
    }

    public String getName()
    {
        return name;
    }
    
    public String getDescription()
    {
        return description;
    }
    private String name;
    private String description;
    

}
