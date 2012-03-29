
public class NameStruct {
    
    public String name;
    public String description;
    
    public NameStruct(String parseName) 
    {
        name = parseName;
        description=null;
    }

    public void addConfig(String theDescription)
    {
        description = theDescription;
    }

}
