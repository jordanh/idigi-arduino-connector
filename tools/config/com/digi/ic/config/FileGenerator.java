package com.digi.ic.config;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.LinkedList;

public class FileGenerator {

    private final static String HEADER_FILENAME = "remote_config.h";
    private final static String SOURCE_FILENAME = "remote_config.c"; 
    
    private final static String SPACES = "    ";
    private final static String UNDERSCORE = "_";
    private final static String ADD = " + ";
    
    private final static String IDIGI = "idigi";
    private final static String DEFINE = "#define ";
    private final static String ERROR = "error";
    
    private final static String INCLUDE_HEADER = "#include \"idigi_remote.h\"\n\n";
    
    private final static String TYPEDEF_ENUM = "typedef enum {\n";
    
    private final static String GLOBAL_RCI_ERROR = "idigi_rci_error";
    private final static String GLOBAL_ERROR = "idigi_global_error";

    private final static String IDIGI_REMOTE_ALL_STRING = "idigi_remote_all_strings";
    private final static String IDIGI_REMOTE_GROUP_TABLE = "idigi_group_table";
    
    private final static String COUNT_STRING = "COUNT";
    private final static String OFFSET_STRING = "OFFSET";
    
    private final static String CHAR_CONST_STRING = "static char const * const ";
    private final static String ENUM_STRING = "enum";

    private final static String INT_MAX_WIDTH_STRING = UNDERSCORE + "WIDTH = INT_MAX\n}";
    private final static String ID_T_STRING = UNDERSCORE + "id_t;\n\n";


    private final static String RCI_PARSER_USES_ERROR_DESCRIPTIONS = "RCI_PARSER_USES_ERROR_DESCRIPTIONS\n";
    private final static String RCI_PARSER_USES_ENUMERATIONS = "RCI_PARSER_USES_ENUMERATIONS\n";
    private final static String RCI_PARSER_USES_FLOATING_POINT = "RCI_PARSER_USES_FLOATING_POINT\n";
    
    
    public static void generateFile(ConfigData configData) throws IOException
    {
        try {

            initialize();
            
            headerWriter.write("#ifndef REMOTE_CONFIG_H\n" + 
                                "#define REMOTE_CONFIG_H\n\n" + 
                                INCLUDE_HEADER); // H file header
            
            if (!ConfigGenerator.getLimitErrorDescription())
            {
                headerWriter.write(DEFINE + RCI_PARSER_USES_ERROR_DESCRIPTIONS);
            }
            if (Parser.getEnumSupport())
            {
                headerWriter.write(DEFINE + RCI_PARSER_USES_ENUMERATIONS);
            }
            if (Parser.getEnumSupport())
            {
                headerWriter.write(DEFINE + RCI_PARSER_USES_FLOATING_POINT);
            }

            sourceWriter.write(INCLUDE_HEADER); //  C file include
            
            writeDefineRciParserStringsHeader();
            
            /* Write all global error enum in H file */
            writeGlobalErrorHeader(configData.getGroupGlobalErrors());

            /* Write all group enum in H file */
            writeGroupHeader(configData);
            
            headerWriter.write("\n#endif /* REMOTE_CONFIG_H */\n"); // end of H file

            /* Start writing C file */
            
            /* Write all string length and index defines in C file */
            writeDefineGlobalErrors(configData.getGroupGlobalErrors());
            
            /* write idigi remote all strings in source file */
            sourceWriter.write("\nchar const " + IDIGI_REMOTE_ALL_STRING + "[] = {\n");
            
            writeRemoteRciParserStrings();
            
            for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
            {
                LinkedList<GroupStruct> theConfig = null;
                
                configType = type.toString().toLowerCase();
                
                try {
                    
                    theConfig = configData.getConfigGroup(configType);
                    
                } catch (IOException e) {
                    /* end of the ConfigData ConfigType */
                    break;
                }
                
                if (!theConfig.isEmpty())
                {
                    writeRemoteAllStrings(theConfig);
                }
            }
            writeErrorsRemoteAllStrings(configData.getGroupGlobalErrors());
            sourceWriter.write(SPACES + "\'\\0\'\n};\n"); // end of IDIGI_REMOTE_ALL_STRING

            /* write structures in source file */
            writeStructures(configData);
            
            ConfigGenerator.log("Files created: " + SOURCE_FILENAME +" and "+ HEADER_FILENAME);
            
        } catch (IOException e) {
            throw new IOException(e.getMessage());
        }
        
        finally {
            headerWriter.flush();
            headerWriter.close();
            
            sourceWriter.flush();
            sourceWriter.close();
            
        }
    }

    private static void initialize() throws IOException 
    {
        
        headerWriter = new BufferedWriter(new FileWriter(HEADER_FILENAME));
        sourceWriter = new BufferedWriter(new FileWriter(SOURCE_FILENAME));
        ConfigGenerator generator = new ConfigGenerator();
        
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();
        
        String note_string = "/*\n * This is an auto-generated file - DO NOT EDIT! \n";
        note_string += " * This is generated by " + generator.getClass().getName() + " tool \n";
        note_string += " * This file was generated on: " + dateFormat.format(date) + " \n";
        note_string += " * The command line arguments were: " + ConfigGenerator.getArgumentListString() + " \n";
        note_string += " * The version of " + generator.getClass().getName() + " tool was: " + ConfigGenerator.VERSION + "\n*/\n\n";
        
        headerWriter.write(note_string);
        sourceWriter.write(note_string);
        
    }

    private final static String[] rciParserStrings = {"ENTITY_QUOTE", "quot",
                                                    "ENTITY_AMPERSAND", "amp", 
                                                    "ENTITY_APOSTROPHE", "apos",
                                                    "ENTITY_LESS_THAN", "lt",
                                                    "ENTITY_GREATER_THAN", "gt",
                                                    "SET_SETTING", "set_setting",
                                                    "QUERY_SETTING", "query_setting",
                                                    "SET_STATE", "set_state",
                                                    "QUERY_STATE", "query_state",
                                                    "VERSION", "version",
                                                    "INDEX", "index"
    };

    private static void writeDefineRciParserStringsHeader() throws IOException 
    {
        
        for (int i=0; i < rciParserStrings.length; i++)
        {
            String defineName = "RCI" + UNDERSCORE + rciParserStrings[i++].toUpperCase();
            /* define name string index */
            headerWriter.write(getDefineIndex(defineName, rciParserStrings[i].toUpperCase()));
        }
    }
    
    private static void writeRemoteRciParserStrings() throws IOException
    {
        for (int i=0; i < rciParserStrings.length; i++)
        {
            String defineName = "RCI" + UNDERSCORE + rciParserStrings[i++].toUpperCase();
            sourceWriter.write(getCharString(defineName, rciParserStrings[i]));
        }

    }
    
    private static void writeDefineStrings(LinkedList<GroupStruct> groups) throws IOException
    {
        String defineName = null;

        
        for (GroupStruct group: groups)
        {
            defineName = getDefineString(group.getName());
            /* define name string index */
            sourceWriter.write(getDefineIndex(defineName, group.getName()));
            
            for (ElementStruct element: group.getElements())
            {
                defineName = getDefineString(group.getName(), element.getName());
                /* define name string index */
                sourceWriter.write(getDefineIndex(defineName, element.getName()));

                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                {
                    for (NameStruct value: element.getValues())
                    {
                        defineName = getDefineString(group.getName(), element.getName(), value.getName());
                        /* define name string index */
                        sourceWriter.write(getDefineIndex(defineName, value.getName()));
                    }
                 }
            }
        
            if (!ConfigGenerator.getLimitErrorDescription() && !group.getErrors().isEmpty())
            {
                
                for (NameStruct error: group.getErrors())
                {
                    defineName = getDefineString(group.getName(), ERROR, error.getName());
                    /* define name string index */
                    sourceWriter.write(getDefineIndex(defineName, error.getName()));
                }
            }
        }
    }

    private static void writeRemoteAllStrings(LinkedList<GroupStruct> groups) throws IOException
    {
        String define_name;
        
        
        for (GroupStruct group: groups)
        {
            define_name = getDefineString(group.getName());
            
            sourceWriter.write(getCharString(define_name, group.getName()));

            for (ElementStruct element: group.getElements())
            {
                define_name = getDefineString(group.getName(), element.getName());
                sourceWriter.write(getCharString(define_name, element.getName()));
                
                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                {
                    for (NameStruct value: element.getValues())
                    {
                        define_name = getDefineString(group.getName(), element.getName(), value.getName());
                        sourceWriter.write(getCharString(define_name, value.getName()));
                    }
                 }
            }
        
            if (!ConfigGenerator.getLimitErrorDescription() && !group.getErrors().isEmpty())
            {
                for (NameStruct error: group.getErrors())
                {
                    define_name = getDefineString(group.getName(), ERROR, error.getName());
                    sourceWriter.write(getCharString(define_name, error.getName()));
                }
            }
        }
    }

    private static void writeDefineGlobalErrors(LinkedList<NameStruct> globalerrors) throws IOException
    {
        if (!ConfigGenerator.getLimitErrorDescription())
        {
            for (NameStruct error: ConfigData.allErrorList)
            {
                String defineName = GLOBAL_RCI_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                /* define name string index */
                sourceWriter.write(getDefineIndex(defineName, error.getName()));
            }
            for (NameStruct error: ConfigData.globalErrorList)
            {
                String defineName = GLOBAL_RCI_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                /* define name string index */
                sourceWriter.write(getDefineIndex(defineName, error.getName()));
            }
            for (NameStruct error: ConfigData.commandErrorList)
            {
                String defineName = GLOBAL_RCI_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                /* define name string index */
                sourceWriter.write(getDefineIndex(defineName, error.getName()));
            }
            for (NameStruct error: ConfigData.groupErrorList)
            {
                String defineName = GLOBAL_RCI_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                /* define name string index */
                sourceWriter.write(getDefineIndex(defineName, error.getName()));
            }
            for (NameStruct error: globalerrors)
            {
                String defineName = GLOBAL_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                /* define name string index */
                sourceWriter.write(getDefineIndex(defineName, error.getName()));
            }
        }
    }

    private static void writeErrorsRemoteAllStrings(LinkedList<NameStruct> globalerrors) throws IOException
    {
        if (!ConfigGenerator.getLimitErrorDescription())
        {
            for (NameStruct error: ConfigData.allErrorList)
            {
                String define_name = GLOBAL_RCI_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                sourceWriter.write(getCharString(define_name, error.getName()));
            }
            for (NameStruct error: ConfigData.globalErrorList)
            {
                String define_name = GLOBAL_RCI_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                sourceWriter.write(getCharString(define_name, error.getName()));
            }
            for (NameStruct error: ConfigData.commandErrorList)
            {
                String define_name = GLOBAL_RCI_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                sourceWriter.write(getCharString(define_name, error.getName()));
            }
            for (NameStruct error: ConfigData.groupErrorList)
            {
                String define_name = GLOBAL_RCI_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                sourceWriter.write(getCharString(define_name, error.getName()));
            }
            for (NameStruct error: globalerrors)
            {
                String define_name = GLOBAL_ERROR.toUpperCase() + UNDERSCORE + error.getName().toUpperCase();
                sourceWriter.write(getCharString(define_name, error.getName()));
            }
        }
    }

    private static void writeEnumStructure(String enum_name, LinkedList<NameStruct> values) throws IOException
    {
        String enum_string = enum_name.toLowerCase() + UNDERSCORE + ENUM_STRING;
        
        /* write element enum strings array */
        sourceWriter.write((CHAR_CONST_STRING + enum_string + "[] = {\n"));
        
        for (int value_index = 0; value_index < values.size(); value_index++)
        {
            NameStruct value = values.get(value_index);
            
            /* write idigi_remote_all_strings reference */
            sourceWriter.write(getRemoteAllString(enum_name, value.getName()));
            if (value_index < (values.size() -1))
            {
                sourceWriter.write(",");
            }
            /* write comment */
            sourceWriter.write(COMMENTED(value.getName()));
        }
        /* end of writing element enum strings array */
        sourceWriter.write("};\n\n");

        /* write element value limit structure for enum type */
        String enum_limit_string = "static idigi_element_value_enum_t const " + enum_name.toLowerCase() + "_limit = {\n";
        enum_limit_string +=  SPACES + "asizeof(" + enum_string + "), \n" +
                              SPACES + enum_string + "\n};\n\n";

        sourceWriter.write(enum_limit_string);
    }
    
    private static void writeElementLimitStructures(String element_name, ElementStruct element) throws IOException
    {
        /* write element value limit structure */
        String limit_string = "static idigi_element_value_unsigned_integer_t const " + element_name.toLowerCase() + "_limit = {\n";
        if (element.getMin() == null)
        {
            limit_string += SPACES + "0";
        }
        else
        {
            limit_string += SPACES + element.getMin();
        }
        limit_string += ",\n";
        
        if (element.getMax() == null)
        {
            if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.FLOAT)
            {
                limit_string += SPACES + "FLOAT_MAX";
            }
            else
            {
                limit_string += SPACES + "INT_MAX";
            }
        }
        else
        {
            limit_string += SPACES + element.getMax();
        }
        
        sourceWriter.write(limit_string);
        sourceWriter.write("\n};\n\n");
    }
    
    private static void writeElementArrays(String group_name, LinkedList<ElementStruct> elements) throws IOException
    {
        /* write group element structure array */
        sourceWriter.write("static idigi_group_element_t const " + 
                           getDefineString(group_name).toLowerCase() + UNDERSCORE + "elements" +
                           "[] = {");
        
        for (int element_index = 0; element_index < elements.size(); element_index++)
        {
            ElementStruct element = elements.get(element_index);
            
            String element_name = getDefineString(group_name, element.getName());
            
            String element_string = "\n" + 
                                    SPACES + "{" + getRemoteAllString(element_name) + ", " + COMMENTED(element.getName()) + "\n" +
                                    SPACES + getElementDefine("access", getAccess(element.getAccess())) + 
                                    SPACES + getElementDefine("type", element.getType());
            
            if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM ||
                element.getMin() != null || element.getMax() != null)
            {
                element_string += SPACES + SPACES + "(idigi_element_value_limit_t *)&" + 
                                  element_name.toLowerCase() + "_limit";
            }
            else
            {
                element_string += SPACES + SPACES + "NULL";
            }
            element_string += "\n" + SPACES + "}";
            
            if (element_index < (elements.size()-1))
            {
                element_string += ",";
            }
            
            sourceWriter.write(element_string);
        }
        sourceWriter.write("\n};\n\n");

    }

    private static void writeGlobalErrorStructures(LinkedList<NameStruct> globalErrors) throws IOException
    {
        if (!ConfigGenerator.getLimitErrorDescription())
        {
            int errorCount = 0;
            
            errorCount += ConfigData.allErrorList.size();
            errorCount += ConfigData.globalErrorList.size();
            errorCount += ConfigData.commandErrorList.size();
            errorCount += ConfigData.groupErrorList.size();
            errorCount += globalErrors.size();
            
            if (errorCount > 0)
            {
                sourceWriter.write("char const * const " + GLOBAL_RCI_ERROR + "s[" + errorCount + "] = {\n");
    
                /* top-level all errors */
                for (NameStruct error: ConfigData.allErrorList)
                {
                    sourceWriter.write(getRemoteAllString(GLOBAL_RCI_ERROR.toUpperCase(), error.getName()));
                    errorCount--;
                    if (errorCount > 0)
                    {
                        sourceWriter.write(",");
                    }
                    sourceWriter.write(COMMENTED(error.getName()));
                }
    
                /* top-level global errors */
                for (NameStruct error: ConfigData.globalErrorList)
                {
                    sourceWriter.write(getRemoteAllString(GLOBAL_RCI_ERROR.toUpperCase(), error.getName()));
                    errorCount--;
                    
                    if (errorCount > 0)
                    {
                        sourceWriter.write(",");
                    }
                    sourceWriter.write(COMMENTED(error.getName()));
                }

                /* top-level command errors */
                for (NameStruct error: ConfigData.commandErrorList)
                {
                    sourceWriter.write(getRemoteAllString(GLOBAL_RCI_ERROR.toUpperCase(), error.getName()));
                    errorCount--;
                    if (errorCount > 0)
                    {
                        sourceWriter.write(",");
                    }
                    sourceWriter.write(COMMENTED(error.getName()));
                }
                
                /* top-level group errors */
                for (NameStruct error: ConfigData.groupErrorList)
                {
                    sourceWriter.write(getRemoteAllString(GLOBAL_RCI_ERROR.toUpperCase(), error.getName()));
                    errorCount--;
                    if (errorCount > 0)
                    {
                        sourceWriter.write(",");
                    }
                    sourceWriter.write(COMMENTED(error.getName()));
                }
                
                /* group global errors */
                for (NameStruct error: globalErrors)
                {
                    sourceWriter.write(getRemoteAllString(GLOBAL_ERROR.toUpperCase(), error.getName()));
                    errorCount--;
                    if (errorCount > 0)
                    {
                        sourceWriter.write(",");
                    }
                    sourceWriter.write(COMMENTED(error.getName()));
                }
                sourceWriter.write("};\n\n");
            }
        }
    }
    
    private static void writeErrorStructures(String error_name, LinkedList<NameStruct> localErrors) throws IOException
    {
        if (!ConfigGenerator.getLimitErrorDescription())
        {
            String define_name;
            
            if (!localErrors.isEmpty())
            {
                define_name = getDefineString(error_name, ERROR);
                sourceWriter.write((CHAR_CONST_STRING + define_name.toLowerCase() + "s[] = {\n"));
    
                
                /* local local errors */
                define_name = getDefineString(error_name, ERROR);
                for (int error_index = 0; error_index < localErrors.size(); error_index++)
                {
                    NameStruct error = localErrors.get(error_index);
                    
                    sourceWriter.write(getRemoteAllString(define_name, error.getName()));
                    if (error_index < (localErrors.size() -1))
                    {
                        sourceWriter.write(",");
                    }
                    sourceWriter.write(COMMENTED(error.getName()));
                }
                
                sourceWriter.write("};\n\n");
            }
        }
    }

    private static void writeGroupStructures(LinkedList<GroupStruct> groups) throws IOException
    {
        String define_name;
        
        for (int group_index = 0; group_index < groups.size(); group_index++)
        {
            GroupStruct group = groups.get(group_index);
            
            for (int element_index = 0; element_index < group.getElements().size(); element_index++)
            {
                ElementStruct element = group.getElements().get(element_index);
                
                define_name = getDefineString(group.getName(), element.getName());

                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                {
                    /* write enum structure */
                    writeEnumStructure(define_name, element.getValues());
                }
                else if (element.getMax() != null || element.getMin()!= null)
                {
                    /* write limit structure */
                    writeElementLimitStructures(define_name, element);
                }
            }
            
            /* write element structure */
            writeElementArrays(group.getName(), group.getElements());
            
            writeErrorStructures(group.getName(), group.getErrors());
        }
        
    }

    private static void writeStructures(ConfigData configData) throws IOException
    {
        String define_name;
        
        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;
            
            configType = type.toString().toLowerCase();
            
            try {
                
                groups = configData.getConfigGroup(configType);
                
            } catch (IOException e) {
                /* end of the ConfigData ConfigType */
                break;
            }

            if (!groups.isEmpty())
            {
                writeGroupStructures(groups);
                
                sourceWriter.write("static idigi_group_t const idigi_" + configType + "_groups[] = {");
                
                for (int group_index = 0; group_index < groups.size(); group_index++)
                {
                    GroupStruct group = groups.get(group_index);
        
                    
                    define_name = getDefineString(group.getName(), "elements");
                    String group_string = "\n" + 
                                            SPACES + "{" + getRemoteAllString(getDefineString(group.getName())) + ", " + COMMENTED(group.getName()) + "\n" +
                                            SPACES + SPACES + "1,\n" + 
                                            SPACES + SPACES + (group.getInstances() + 1) + ",\n" +  
                                            SPACES + SPACES + "{" + SPACES + "asizeof(" + define_name.toLowerCase()  + "),\n" +
                                            SPACES + SPACES + SPACES + define_name.toLowerCase() + "\n" +
                                            SPACES + SPACES + "},\n";
                    
                    if ((!ConfigGenerator.getLimitErrorDescription()) && (!group.getErrors().isEmpty()))
                    {
                        define_name = getDefineString(group.getName(), "errors");
            
                        group_string +=   SPACES + SPACES + "{" + SPACES + "asizeof(" + define_name.toLowerCase() + "),\n" +
                                          SPACES + SPACES + SPACES + define_name.toLowerCase() + "\n" +
                                          SPACES + SPACES + "}\n" +
                                          SPACES + "}";
                    }
                    else
                    {
                        group_string += SPACES + SPACES + "{" + SPACES + "0,\n" +
                                        SPACES + SPACES + SPACES + "NULL\n" +
                                        SPACES + SPACES + "}\n" +
                                        SPACES + "}";
                    }
                    
                    if (group_index < (groups.size() -1))
                    {
                        group_string += ",";
                    }
                                            
                    sourceWriter.write(group_string);
                }
                sourceWriter.write("\n};\n\n");
            }            
        }
        
        writeGlobalErrorStructures(configData.getGroupGlobalErrors());

        
        String idigiGroupString = "idigi_group_table_t const " + IDIGI_REMOTE_GROUP_TABLE + "[" + ConfigData.ConfigType.getCount() + "] = {\n";
        
        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;
            
            configType = type.toString().toLowerCase();
            
            try {
                
                groups = configData.getConfigGroup(configType);
                
            } catch (IOException e) {
                /* end of the ConfigData ConfigType */
                break;
            }
            
            if (type.getIndex() != 0)
            {
                idigiGroupString += ",\n";
            }
            
            idigiGroupString += SPACES + "{";
            if (!groups.isEmpty())
            {
                idigiGroupString += "idigi_" + configType + "_groups,\n" + 
                                    SPACES + " asizeof(idigi_" + configType + "_groups)\n" + 
                                    SPACES + "}"; 

            }
            else
            {
                idigiGroupString += "NULL,\n" + SPACES + "0\n" + 
                                    SPACES + "}"; 
            }
            
        }
        idigiGroupString += "\n};\n\n";
        
        sourceWriter.write(idigiGroupString);
    }

    private static void writeErrorHeader(int errorIndex, String enumDefine, LinkedList<NameStruct> errors) throws IOException
    {
        for (NameStruct error: errors)
        {
            String error_string = SPACES + enumDefine + UNDERSCORE + error.getName();
            
            if (errorIndex == 0)
            {
                error_string += " = " + SPACES + enumDefine + UNDERSCORE + OFFSET_STRING;
            }
            errorIndex++;
            
            error_string += ",\n";
            
            headerWriter.write(error_string);
        }

    }
    private static void writeGlobalErrorHeader(LinkedList<NameStruct> globalerrors) throws IOException
    {
        int errorCount = ConfigData.allErrorList.size();
        
        errorCount += ConfigData.globalErrorList.size();
        errorCount += ConfigData.commandErrorList.size();
        errorCount += ConfigData.groupErrorList.size();
        errorCount += globalerrors.size();
        
        if (errorCount > 0)
        {
            int errorIndex = 0;
            
            /* write typedef enum for error */
            headerWriter.write("\n" + TYPEDEF_ENUM + SPACES + GLOBAL_RCI_ERROR + UNDERSCORE + OFFSET_STRING + " = 1,\n");
            
            writeErrorHeader(errorIndex, GLOBAL_RCI_ERROR, ConfigData.allErrorList);
            errorIndex += ConfigData.allErrorList.size();
            
            writeErrorHeader(errorIndex, GLOBAL_RCI_ERROR, ConfigData.globalErrorList);
            errorIndex += ConfigData.globalErrorList.size();

            writeErrorHeader(errorIndex, GLOBAL_RCI_ERROR, ConfigData.commandErrorList);
            errorCount += ConfigData.commandErrorList.size();
            
            writeErrorHeader(errorIndex, GLOBAL_RCI_ERROR, ConfigData.groupErrorList);
            errorCount += ConfigData.groupErrorList.size();
            
            writeErrorHeader(errorIndex, GLOBAL_ERROR, globalerrors);
            
            headerWriter.write(SPACES + GLOBAL_ERROR + UNDERSCORE + COUNT_STRING + ",\n");
            headerWriter.write(SPACES + GLOBAL_ERROR + INT_MAX_WIDTH_STRING + GLOBAL_ERROR + ID_T_STRING);
            
        }
    }
    
    private static void writeEnumHeader(LinkedList<GroupStruct> groups, LinkedList<NameStruct> globalerrors) throws IOException
    {

        for (GroupStruct group: groups)
        {
            /* build element enum string for element enum */
            String element_enum_string = TYPEDEF_ENUM;
            
            for (ElementStruct element: group.getElements())
            {
                /* add element name */
                element_enum_string += getEnumString(group.getName(), element.getName()) + ",\n";
                
                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                {
                    /* write typedef enum for value */
                    headerWriter.write(TYPEDEF_ENUM);
                    
                    for (NameStruct value: element.getValues())
                    {
                        headerWriter.write(getEnumString(group.getName(), element.getName(), value.getName()) + ",\n");
                    }
                    /* done typedef enum for value */ 
                    headerWriter.write(endEnumString(group.getName(), element.getName())); 
                }
            }
            /* done typedef enum for element */
            
            element_enum_string += endEnumString(group.getName());
            
            headerWriter.write(element_enum_string);
            
            if (!group.getErrors().isEmpty())
            {
                headerWriter.write(TYPEDEF_ENUM);
                
                for (int i=0; i < group.getErrors().size(); i++)
                {
                    NameStruct error = group.getErrors().get(i);
                    
                    String enumString = getEnumString(group.getName(), ERROR, error.getName());
                    if (i == 0)
                    {
                        if (!globalerrors.isEmpty())
                        {
                            enumString += " = " +  GLOBAL_ERROR + UNDERSCORE + COUNT_STRING;
                        }
                        else if (!ConfigData.allErrorList.isEmpty() || !ConfigData.groupErrorList.isEmpty())
                        {
                            enumString += " = " +  GLOBAL_RCI_ERROR + UNDERSCORE + COUNT_STRING;
                        }
                        else
                        {
                            enumString += " = 1";
                            
                        }
                    }
                    enumString += ",\n";
                    
                    headerWriter.write(enumString);
                }
                headerWriter.write(endEnumString(group.getName(), ERROR));
            }
            
        }

    }

    private static void writeGroupHeader(ConfigData configData) throws IOException
    {

        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;
            
            configType = type.toString().toLowerCase();
            
            try {
                
                groups = configData.getConfigGroup(configType);
                
            } catch (IOException e) {
                /* end of the ConfigData ConfigType */
                break;
            }
            
            /* build group enum string for group enum */
            String group_enum_string = TYPEDEF_ENUM;
            
            if (!groups.isEmpty())
            {
                /* Write all enum in H file */
                writeEnumHeader(groups, configData.getGroupGlobalErrors());
            
                /* Write all string length and index defines in C file */
                writeDefineStrings(groups);
            }

            for (GroupStruct group: groups)
            {
                /* add each group enum */ 
                group_enum_string += getEnumString(group.getName()) + ",\n";
            }

            /* write group enum buffer to headerWriter */
            group_enum_string += endEnumString(null);
            headerWriter.write(group_enum_string);
        }
        
    }

    private static String COMMENTED(String comment)
    {
        return " /*" + comment + "*/\n";
    }
    
    private static String getEnumString(String group_name)
    {
        String str = SPACES + IDIGI + UNDERSCORE + configType;

        if (group_name != null)
        {
            str += UNDERSCORE + group_name;
        }
        return str;
    }
    
    private static String getEnumString(String group_name, String name)
    {
        return (getEnumString(group_name) + UNDERSCORE + name);
    }
    
    private static String getEnumString(String group_name, String name, String name1)
    {
        return (getEnumString(group_name, name) + UNDERSCORE + name1);
    }

    private static String endEnumString(String group_name)
    {
        return (getEnumString(group_name) + INT_MAX_WIDTH_STRING + 
                getEnumString(group_name) + ID_T_STRING);
    }

    private static String endEnumString(String group_name, String name)
    {
        return (getEnumString(group_name, name) + INT_MAX_WIDTH_STRING + 
                getEnumString(group_name, name) + ID_T_STRING);
    }
    
    private static String getDefineString(String name)
    {
        return (configType.toUpperCase() + UNDERSCORE + name.toUpperCase());
    }
    private static String getDefineString(String name, String name1)
    {
        return (getDefineString(name) + UNDERSCORE + name1.toUpperCase());
    }

    private static String getDefineString(String name, String name1, String name2)
    {
        return (getDefineString(name, name1) + UNDERSCORE + name2.toUpperCase());
    }

    private static String getDefineIndex(String define_name, String name)
    {
        String str = DEFINE + define_name + SPACES + "(" + IDIGI_REMOTE_ALL_STRING + ADD + prevRemoteStringLength + ")\n";
        prevRemoteStringLength += name.length() + 1;
        return str;
    }

    private static String getCharString(String define_name, String name)
    {
        char[] characters = name.toCharArray();
        
        String quote_char = SPACES + name.length() + ",";
        for (char c: characters)
        {
            quote_char += "\'" + c + "\',";
        }
        quote_char += "\n";
        
        return quote_char;
    }
    
    private static String getRemoteAllString(String define_name)
    {
      return (SPACES + define_name.toUpperCase());

    }

    private static String getRemoteAllString(String define_name, String name)
    {
        return (SPACES + define_name.toUpperCase() + UNDERSCORE + name.toUpperCase());

    }
    private static String getElementDefine(String type_name, String name)
    {
        return (SPACES + IDIGI + UNDERSCORE + "element" + UNDERSCORE + type_name + UNDERSCORE + name + ",\n");
    }
    private static String getAccess(String access)
    {
        if (access == null)
        {
            return "read_write";
        }
        return access;
    }
    private static BufferedWriter sourceWriter;
    private static BufferedWriter headerWriter;
    private static String configType;
    private static int prevRemoteStringLength;

    
}
