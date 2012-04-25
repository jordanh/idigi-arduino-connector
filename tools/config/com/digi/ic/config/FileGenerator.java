package com.digi.ic.config;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.LinkedList;

public class FileGenerator {


    private final static String HEADER_FILENAME = "remote_config.h";
    private final static String SOURCE_FILENAME = "remote_config.c";

    private final static String RCI_PREFIX = "RCI_";

    private final static String IDIGI = "idigi";
    private final static String DEFINE = "#define ";
    private final static String INCLUDE = "#include ";
    private final static String ERROR = "error";

    private final static String IDIGI_REMOTE_HEADER = "\"idigi_remote.h\"\n\n";
    private final static String INTEGER_LIMIT_HEADER = "\"limits.h\"\n";
    private final static String FLOAT_LIMIT_HEADER = "\"float.h\"\n";

    private final static String TYPEDEF_ENUM = "typedef enum {\n";

    private final static String GLOBAL_RCI_ERROR = "idigi_rci_error";
    private final static String GLOBAL_ERROR = "idigi_global_error";

    private final static String IDIGI_REMOTE_ALL_STRING = "idigi_remote_all_strings";
    private final static String IDIGI_REMOTE_GROUP_TABLE = "idigi_group_table";

    private final static String IDIGI_ELEMENT_VALUE_UNSIGNED = "idigi_element_value_unsigned_integer_t";
    private final static String IDIGI_ELEMENT_VALUE_SIGNED = "idigi_element_value_signed_integer_t";
    private final static String IDIGI_ELEMENT_VALUE_STRING = "idigi_element_value_string_t";
    private final static String IDIGI_ELEMENT_VALUE_ENUM = "idigi_element_value_enum_t";
    private final static String IDIGI_ELEMENT_VALUE_FLOAT = "idigi_element_value_float_t";


    private final static String COUNT_STRING = "COUNT";
    private final static String OFFSET_STRING = "OFFSET";
    private final static String STATIC = "static ";
    private final static String CONST = " const ";
    private final static String LIMIT = "limit";

    private final static String CHAR_CONST_STRING = STATIC + "char" + CONST + "*" + CONST;
    private final static String ENUM_STRING = "enum";

    private final static String ID_T_STRING = "_id_t;\n\n";


    private final static String RCI_PARSER_USES_ERROR_DESCRIPTIONS = "RCI_PARSER_USES_ERROR_DESCRIPTIONS\n";
    private final static String RCI_PARSER_USES_ENUMERATIONS = "RCI_PARSER_USES_ENUMERATIONS\n";
    private final static String RCI_PARSER_USES_FLOATING_POINT = "RCI_PARSER_USES_FLOATING_POINT\n";


    public static void generateFile(ConfigData configData) throws Exception
    {
        try {

            initialize();

            headerWriter.write("#ifndef REMOTE_CONFIG_H\n" +
                                "#define REMOTE_CONFIG_H\n\n" +
                                INCLUDE + INTEGER_LIMIT_HEADER); // H file header

            String defineHeader = "";

            if (ElementStruct.includeFloatLimit())
            {
                defineHeader += INCLUDE + FLOAT_LIMIT_HEADER;
            }

            defineHeader += INCLUDE + IDIGI_REMOTE_HEADER;

            if (!ConfigGenerator.excludeErrorDescription())
            {
                defineHeader += DEFINE + RCI_PARSER_USES_ERROR_DESCRIPTIONS;
            }

            if (Parser.getEnumSupport())
            {
                defineHeader += DEFINE + RCI_PARSER_USES_ENUMERATIONS;
            }

            if (Parser.getFloatingSupport())
            {
                defineHeader += DEFINE + RCI_PARSER_USES_FLOATING_POINT;
            }

            defineHeader += "\n";

            headerWriter.write(defineHeader);

            writeDefineRciParserStringsHeader();

            /* Write all global error enum in H file */
            writeGlobalErrorHeader();

            /* Write all group enum in H file */
            writeGroupHeader(configData);

            headerWriter.write("\n#endif /* REMOTE_CONFIG_H */\n"); // end of H file

            /* Start writing C file */
            sourceWriter.write(INCLUDE + "\"" + HEADER_FILENAME + "\"\n\n");

            /* Write all string length and index defines in C file */
            writeDefineStrings(configData);

            /* Write all string length and index defines in C file */
            writeDefineGlobalErrors(configData);

            /* write idigi remote all strings in source file */
            sourceWriter.write(String.format("\nchar const %s[] = {\n", IDIGI_REMOTE_ALL_STRING));

            writeRemoteRciParserStrings();


            for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
            {
                LinkedList<GroupStruct> theConfig = null;

                configType = type.toString().toLowerCase();

                try {

                    theConfig = configData.getConfigGroup(configType);

                } catch (Exception e) {
                    /* end of the ConfigData ConfigType */
                    break;
                }


                if (!theConfig.isEmpty())
                {
                    writeRemoteAllStrings(theConfig);
                }
            }
            writeErrorsRemoteAllStrings(configData);
            sourceWriter.write(" \'\\0\'\n};\n\n"); // end of IDIGI_REMOTE_ALL_STRING

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

        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();

        String className = ConfigGenerator.class.getClass().getName();

        int firstChar = className.lastIndexOf(".") +1;
        if (firstChar > 0)
        {
            className = className.substring(firstChar);
        }

        String note_string = "/*\n * This is an auto-generated file - DO NOT EDIT! \n";
        note_string += " * This is generated by " + className + " tool \n";
        note_string += " * This file was generated on: " + dateFormat.format(date) + " \n";
        note_string += " * The command line arguments were: " + ConfigGenerator.getArgumentLogString() + " \n";
        note_string += " * The version of " + className + " tool was: " + ConfigGenerator.VERSION + "\n*/\n\n";

        headerWriter.write(note_string);
        sourceWriter.write(note_string);

    }

    private static void writeDefineRciParserStringsHeader() throws IOException
    {
        LinkedHashMap<String, String> rciStrings = ConfigData.getRciStrings();

        if (rciStrings.size() > 0)
        {
            headerWriter.write(String.format("extern char const %s[];\n\n", IDIGI_REMOTE_ALL_STRING));
        }

        for (String key : rciStrings.keySet())
        {
            String defineName = RCI_PREFIX + key.toUpperCase();
            /* define name string index */
            headerWriter.write(getDefineStringIndex(defineName, rciStrings.get(key).toUpperCase()));
        }
    }

    private static void writeRemoteRciParserStrings() throws IOException
    {
        LinkedHashMap<String, String> rciStrings = ConfigData.getRciStrings();

        for (String key : rciStrings.keySet())
        {
            sourceWriter.write(getCharString(rciStrings.get(key)));
        }

    }

    private static void writeDefineStrings(ConfigData configData) throws Exception
    {
        String defineName = null;

        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;

            configType = type.toString().toLowerCase();

            try {

                groups = configData.getConfigGroup(configType);

            } catch (Exception e) {
                /* end of the ConfigData ConfigType */
                break;
            }

            for (GroupStruct group: groups)
            {
                defineName = getDefineString(group.getName());
                /* define name string index */
                sourceWriter.write(getDefineStringIndex(defineName, group.getName()));

                for (ElementStruct element: group.getElements())
                {
                    defineName = getDefineString(group.getName() + "_" + element.getName());
                    /* define name string index */
                    sourceWriter.write(getDefineStringIndex(defineName, element.getName()));

                    if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                    {
                        LinkedHashMap<String, String> valueMap = element.getValues();
                        for (String key : valueMap.keySet())
                        {
                            defineName = getDefineString(group.getName() + "_" + element.getName() + "_" + key);
                            /* define name string index */
                            sourceWriter.write(getDefineStringIndex(defineName, key));
                        }
                     }
                }

                if (!ConfigGenerator.excludeErrorDescription() && !group.getErrors().isEmpty())
                {
                    LinkedHashMap<String, String> errorMap = group.getErrors();
                    for (String key : errorMap.keySet())
                    {
                        defineName = getDefineString(group.getName() + "_" + ERROR + "_" + key);
                        /* define name string index */
                        sourceWriter.write(getDefineStringIndex(defineName, key));
                    }
                }
            }
        }
    }

    private static void writeRemoteAllStrings(LinkedList<GroupStruct> groups) throws Exception
    {
        for (GroupStruct group: groups)
        {
            sourceWriter.write(getCharString(group.getName()));

            for (ElementStruct element: group.getElements())
            {
                sourceWriter.write(getCharString(element.getName()));

                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                {
                    LinkedHashMap<String, String> valueMap = element.getValues();
                    for (String key : valueMap.keySet())
                    {
                        sourceWriter.write(getCharString(key));
                    }
                 }
            }

            if (!ConfigGenerator.excludeErrorDescription() && !group.getErrors().isEmpty())
            {
                LinkedHashMap<String, String> errorMap = group.getErrors();
                for (String key : errorMap.keySet())
                {
                    sourceWriter.write(getCharString(key));
                }
            }
        }
    }

    private static void writeDefineErrors(String prefixName, LinkedHashMap<String, String> errorMap) throws IOException
    {
        for (String key : errorMap.keySet())
        {
            String defineName = prefixName.toUpperCase() + "_" + key.toUpperCase();
            /* define name string index */
            sourceWriter.write(getDefineStringIndex(defineName, errorMap.get(key)));
        }
    }

    private static void writeDefineGlobalErrors(ConfigData configData) throws IOException
    {
        if (!ConfigGenerator.excludeErrorDescription())
        {
            writeDefineErrors(GLOBAL_RCI_ERROR, ConfigData.getRciCommonErrors());

            writeDefineErrors(GLOBAL_RCI_ERROR, ConfigData.getRciGlobalErrors());

            writeDefineErrors(GLOBAL_RCI_ERROR, ConfigData.getRciCommandErrors());

            writeDefineErrors(GLOBAL_RCI_ERROR, ConfigData.getRciGroupErrors());

            writeDefineErrors(GLOBAL_ERROR, ConfigData.getUserGlobalErrors());
        }
    }

    private static void writeLinkedHashMapStrings(LinkedHashMap<String, String> stringMap) throws IOException
    {
        for (String key : stringMap.keySet())
        {
            sourceWriter.write(getCharString(key));
        }

    }
    private static void writeErrorsRemoteAllStrings(ConfigData configData) throws IOException
    {
        if (!ConfigGenerator.excludeErrorDescription())
        {
            writeLinkedHashMapStrings(ConfigData.getRciCommonErrors());

            writeLinkedHashMapStrings(ConfigData.getRciGlobalErrors());

            writeLinkedHashMapStrings(ConfigData.getRciCommandErrors());

            writeLinkedHashMapStrings(ConfigData.getRciGroupErrors());

            writeLinkedHashMapStrings(ConfigData.getUserGlobalErrors());
        }
    }

    private static void writeEnumStructure(String enum_name, LinkedHashMap<String, String> valueMap) throws IOException
    {
        String enum_string = enum_name.toLowerCase() + "_" + ENUM_STRING;

        /* write element enum strings array */
        sourceWriter.write(CHAR_CONST_STRING + enum_string + "[] = {\n");

        Iterator<String> interator = valueMap.keySet().iterator();

        while (interator.hasNext()) {

            String key = interator.next();
            /* write idigi_remote_all_strings reference */
            sourceWriter.write(getRemoteAllString(enum_name + "_" + key));
            if (interator.hasNext())
            {
                sourceWriter.write(",");
            }
            /* write comment */
            sourceWriter.write(COMMENTED(key));
        }
        /* end of writing element enum strings array */
        sourceWriter.write("};\n\n");

        /* write element value limit structure for enum type */
        String enum_limit_string = String.format("static %s const %s_limit = {\n asizeof(%s),\n %s\n};\n\n",
                                                IDIGI_ELEMENT_VALUE_ENUM, enum_name.toLowerCase(),
                                                enum_string, enum_string);

        sourceWriter.write(enum_limit_string);
    }

    private static void writeElementLimitStructures(String element_name, ElementStruct element) throws Exception
    {
        /* write element value limit structure */
        String limit_string = STATIC;

        ElementStruct.ElementType type = ElementStruct.ElementType.toElementType(element.getType());

        switch (type)
        {
        case FLOAT:
            limit_string += IDIGI_ELEMENT_VALUE_FLOAT;
            break;
        case UINT32:
        case HEX32:
        case XHEX:
            limit_string += IDIGI_ELEMENT_VALUE_UNSIGNED;
            break;
        case INT32:
            limit_string += IDIGI_ELEMENT_VALUE_SIGNED;
            break;
        default:
            limit_string += IDIGI_ELEMENT_VALUE_STRING;
            break;
        }

        limit_string += CONST + element_name.toLowerCase() + "_" + LIMIT + " = {\n";

        if (element.getMin() == null)
        {
            switch (type)
            {
            case FLOAT:
                limit_string += " " + "FLT_MIN";
                break;
            case INT32:
                limit_string += " " + "INT32_MIN";
                break;
            default:
                limit_string += " " + "0";
                break;
            }
        }
        else
        {
            limit_string += " ";
            if (type == ElementStruct.ElementType.HEX32)
                limit_string += "0x";
            limit_string +=  element.getMin();
        }

        limit_string += ",\n";

        if (element.getMax() == null)
        {
            switch (type)
            {
            case FLOAT:
                limit_string += " " + "FLT_MAX";
                break;
            case INT32:
                limit_string += " " + "INT32_MAX";
                break;
            case UINT32:
            case HEX32:
            case XHEX:
                limit_string += " " + "UINT32_MAX";
                break;
            default:
                limit_string += " " + "SIZE_MAX";
                break;
            }
        }
        else
        {
            limit_string += " ";
            if (type == ElementStruct.ElementType.HEX32)
                limit_string += "0x";
            limit_string += element.getMax();
        }

        sourceWriter.write(limit_string);
        sourceWriter.write("\n};\n\n");
    }

    private static void writeElementArrays(String group_name, LinkedList<ElementStruct> elements) throws Exception
    {
        /* write group element structure array */
        sourceWriter.write(String.format("static idigi_group_element_t const %s_elements[] = {",
                                        getDefineString(group_name).toLowerCase()));

        for (int element_index = 0; element_index < elements.size(); element_index++)
        {
            ElementStruct element = elements.get(element_index);

            String element_name = getDefineString(group_name + "_" + element.getName());

            String element_string = "\n" +
                                    " " + "{" + getRemoteAllString(element_name) + ", " + COMMENTED(element.getName()) +
                                    " " + getElementDefine("access", getAccess(element.getAccess())) +
                                    " " + getElementDefine("type", element.getType());

            if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM ||
                element.getMin() != null || element.getMax() != null)
            {
                element_string += String.format("  (idigi_element_value_limit_t *)&%s_limit",
                                                element_name.toLowerCase());
            }
            else
            {
                element_string += "  NULL";
            }
            element_string += "\n }";

            if (element_index < (elements.size()-1))
            {
                element_string += ",";
            }

            sourceWriter.write(element_string);
        }
        sourceWriter.write("\n};\n\n");

    }

    private static void writeGlobalErrorStructures(ConfigData configData) throws IOException
    {
        if (!ConfigGenerator.excludeErrorDescription())
        {
            int errorCount = ConfigData.getRciCommonErrors().size() +
                            ConfigData.getRciGlobalErrors().size() +
                            ConfigData.getRciCommandErrors().size() +
                            ConfigData.getRciGroupErrors().size() +
                            ConfigData.getUserGlobalErrors().size();

            if (errorCount > 0)
            {
                sourceWriter.write("char const * const " + GLOBAL_RCI_ERROR + "s[" + errorCount + "] = {\n");

                /* top-level all errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_RCI_ERROR, ConfigData.getRciCommonErrors());

                /* top-level global errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_RCI_ERROR, ConfigData.getRciGlobalErrors());

                /* top-level command errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_RCI_ERROR, ConfigData.getRciCommandErrors());

                /* top-level group errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_RCI_ERROR, ConfigData.getRciGroupErrors());

                /* group global errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_ERROR, ConfigData.getUserGlobalErrors());

                sourceWriter.write("};\n\n");
            }
        }
    }

    private static int  writeErrorStructures(int errorCount, String defineName, LinkedHashMap<String, String> errorMap) throws IOException
    {
        for (String key : errorMap.keySet())
        {
            sourceWriter.write(getRemoteAllString(defineName.toUpperCase() + "_" + key));
            errorCount--;
            if (errorCount > 0)
            {
                sourceWriter.write(",");
            }
            sourceWriter.write(COMMENTED(key));
        }

        return errorCount;
    }

    private static void writeErrorStructures(String error_name, LinkedHashMap<String, String> localErrors) throws IOException
    {
        if (!ConfigGenerator.excludeErrorDescription())
        {
            String define_name;

            if (!localErrors.isEmpty())
            {
                define_name = getDefineString(error_name + "_" +  ERROR);
                sourceWriter.write(CHAR_CONST_STRING + define_name.toLowerCase() + "s[] = {\n");

                /* local local errors */
                define_name = getDefineString(error_name + "_" + ERROR);
                int error_count = localErrors.size();
                writeErrorStructures(error_count, define_name, localErrors);

                sourceWriter.write("};\n\n");
            }
        }
    }

    private static void writeGroupStructures(LinkedList<GroupStruct> groups) throws Exception
    {
        String define_name;

        for (int group_index = 0; group_index < groups.size(); group_index++)
        {
            GroupStruct group = groups.get(group_index);

            for (int element_index = 0; element_index < group.getElements().size(); element_index++)
            {
                ElementStruct element = group.getElements().get(element_index);

                define_name = getDefineString(group.getName() + "_" + element.getName());

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

    private static void writeStructures(ConfigData configData) throws Exception
    {
        String define_name;

        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;

            configType = type.toString().toLowerCase();

            try {

                groups = configData.getConfigGroup(configType);

            } catch (Exception e) {
                /* end of the ConfigData ConfigType */
                break;
            }

            if (!groups.isEmpty())
            {
                writeGroupStructures(groups);

                sourceWriter.write(String.format("static idigi_group_t const idigi_%s_groups[] = {", configType));

                for (int group_index = 0; group_index < groups.size(); group_index++)
                {
                    GroupStruct group = groups.get(group_index);


                    define_name = getDefineString(group.getName() + "_elements");
                    String group_string = String.format("\n { %s, %s\n", getRemoteAllString(getDefineString(group.getName())), COMMENTED(group.getName())) +
                                          String.format("   %d,\n", group.getInstances()) +
                                          String.format("   { asizeof(%s),\n", define_name.toLowerCase()) +
                                          String.format("     %s\n   },\n", define_name.toLowerCase());

                    if ((!ConfigGenerator.excludeErrorDescription()) && (!group.getErrors().isEmpty()))
                    {
                        define_name = getDefineString(group.getName() + "_errors");

                        group_string += String.format("   { asizeof(%s),\n", define_name.toLowerCase()) +
                                        String.format("     %s\n   }\n }", define_name.toLowerCase());

                    }
                    else
                    {
                        group_string += "   { 0,\n     NULL\n   }\n }";
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

        writeGlobalErrorStructures(configData);


        String idigiGroupString = String.format("idigi_group_table_t const %s[%d] = {\n", IDIGI_REMOTE_GROUP_TABLE, ConfigData.ConfigType.getConfigTypeCount());

        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;

            configType = type.toString().toLowerCase();

            try {

                groups = configData.getConfigGroup(configType);

            } catch (Exception e) {
                /* end of the ConfigData ConfigType */
                break;
            }

            if (type.getIndex() != 0)
            {
                idigiGroupString += ",\n";
            }

            idigiGroupString += " " + "{";
            if (!groups.isEmpty())
            {
                idigiGroupString += String.format("idigi_%s_groups,\n asizeof(idigi_%s_groups)\n }", configType, configType);

            }
            else
            {
                idigiGroupString += "NULL,\n" + " " + "0\n" +
                                    " " + "}";
            }

        }
        idigiGroupString += "\n};\n\n";

        sourceWriter.write(idigiGroupString);
    }

    private static void writeErrorHeader(int errorIndex, String enumDefine, LinkedHashMap<String, String> errorMap) throws IOException
    {
        for (String key : errorMap.keySet())
        {
            String error_string = " " + enumDefine + "_" + key;

            if (errorIndex == 1)
            {
                error_string += " = " + " " + enumDefine + "_" + OFFSET_STRING;
            }
            errorIndex++;

            error_string += ",\n";

            headerWriter.write(error_string);
        }
    }

    private static void writeGlobalErrorHeader() throws IOException
    {

        /* write typedef enum for rci errors */
        headerWriter.write("\n" + TYPEDEF_ENUM + " " + GLOBAL_RCI_ERROR + "_" + OFFSET_STRING + " = 1,\n");

        writeErrorHeader(ConfigData.getRciCommonErrorsIndex(), GLOBAL_RCI_ERROR, ConfigData.getRciCommonErrors());

        writeErrorHeader(ConfigData.getRciGlobalErrorsIndex(), GLOBAL_RCI_ERROR, ConfigData.getRciGlobalErrors());

        writeErrorHeader(ConfigData.getRciCommandErrorsIndex(), GLOBAL_RCI_ERROR, ConfigData.getRciCommandErrors());

        writeErrorHeader(ConfigData.getRciGroupErrorsIndex(), GLOBAL_RCI_ERROR, ConfigData.getRciGroupErrors());

        headerWriter.write(" " + GLOBAL_RCI_ERROR + "_" + COUNT_STRING + "\n} "  + GLOBAL_RCI_ERROR + ID_T_STRING);

        /* write typedef enum for user global error */
        String enumName = GLOBAL_ERROR + "_" + OFFSET_STRING;

        headerWriter.write("\n" + TYPEDEF_ENUM + " " + enumName + " = " +  GLOBAL_RCI_ERROR + "_" + COUNT_STRING + ",\n");

        writeErrorHeader(1, GLOBAL_ERROR, ConfigData.getUserGlobalErrors());

        String endString = String.format(" %s_%s", GLOBAL_ERROR, COUNT_STRING);

        if (ConfigData.getUserGlobalErrors().isEmpty())
        {
            endString += " = " + enumName;
        }
        endString += "\n} " + GLOBAL_ERROR + ID_T_STRING;

        headerWriter.write(endString);

    }

    private static void writeEnumHeader(LinkedList<GroupStruct> groups) throws Exception
    {

        for (GroupStruct group: groups)
        {
            /* build element enum string for element enum */
            String element_enum_string = TYPEDEF_ENUM;

            for (ElementStruct element: group.getElements())
            {
                /* add element name */
                element_enum_string += getEnumString(group.getName() + "_" + element.getName()) + ",\n";

                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                {
                    /* write typedef enum for value */
                    headerWriter.write(TYPEDEF_ENUM);

                    LinkedHashMap<String, String> valueMap = element.getValues();
                    for (String key : valueMap.keySet())
                    {
                        headerWriter.write(getEnumString(group.getName() + "_" + element.getName() + "_" + key) + ",\n");
                    }
                    /* done typedef enum for value */
                    headerWriter.write(endEnumString(group.getName() + "_" + element.getName()));
                }
            }
            /* done typedef enum for element */

            element_enum_string += endEnumString(group.getName());

            headerWriter.write(element_enum_string);

            if (!group.getErrors().isEmpty())
            {
                headerWriter.write(TYPEDEF_ENUM);

                LinkedHashMap<String, String> errorMap = group.getErrors();
                int index = 0;

                for (String key : errorMap.keySet())
                {
                    String enumString = getEnumString(group.getName() + "_" + ERROR + "_" + key);
                    if (index++ == 0)
                    {
                        /* write start index */
                        enumString += " = " +  GLOBAL_ERROR + "_" + COUNT_STRING;
                    }

                    enumString += ",\n";

                    headerWriter.write(enumString);
                }
                headerWriter.write(endEnumString(group.getName() + "_" + ERROR));
            }

        }

    }

    private static void writeGroupHeader(ConfigData configData) throws Exception
    {

        for (ConfigData.ConfigType type: ConfigData.ConfigType.values())
        {
            LinkedList<GroupStruct> groups = null;

            configType = type.toString().toLowerCase();

            try {

                groups = configData.getConfigGroup(configType);

            } catch (Exception e) {
                /* end of the ConfigData ConfigType */
                break;
            }


            if (!groups.isEmpty())
            {
                /* build group enum string for group enum */
                String group_enum_string = TYPEDEF_ENUM;

                /* Write all enum in H file */
                writeEnumHeader(groups);

                /* Write all string length and index defines in C file */
//                writeDefineStrings(groups);

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

    }

    private static String COMMENTED(String comment)
    {
        return " /*" + comment + "*/\n";
    }

    private static String getEnumString(String enum_name)
    {
        String str = " " + IDIGI + "_" + configType;

        if (enum_name != null)
        {
            str += "_" + enum_name;
        }
        return str;
    }

    private static String endEnumString(String group_name)
    {
        return (getEnumString(group_name) + "_" + COUNT_STRING + "\n}" +
                getEnumString(group_name) + ID_T_STRING);
    }

    private static String getDefineString(String define_name)
    {
        return (configType.toUpperCase() + "_" + define_name.toUpperCase());
    }

    private static String getDefineStringIndex(String define_name, String string)
    {
        String str = DEFINE + define_name + " " + "(" + IDIGI_REMOTE_ALL_STRING + "+" + prevRemoteStringLength + ")\n";
        prevRemoteStringLength += string.length() + 1;
        return str;
    }

    private static String getCharString(String string)
    {
        char[] characters = string.toCharArray();

        String quote_char = " " + string.length() + ",";
        for (char c: characters)
        {
            quote_char += "\'" + c + "\',";
        }
        quote_char += "\n";

        return quote_char;
    }

    private static String getRemoteAllString(String define_name)
    {
      return (" " + define_name.toUpperCase());

    }

    private static String getElementDefine(String type_name, String element_name)
    {
        return (String.format(" %s_element_%s_%s,\n", IDIGI, type_name, element_name));
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
