package com.digi.ic.config;

public class NameStruct {
    
    public String name;
    public String description;
    
    public NameStruct(String parseName) 
    {
        name = parseName;
        description=null;
    }

    public NameStruct(String parseName, String descString) 
    {
        name = parseName;
        description=descString;
    }


    public void addConfig(String theDescription)
    {
        description = theDescription;
    }

}
