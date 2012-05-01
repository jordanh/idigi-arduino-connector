package com.digi.ic.config;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Date;
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
    private final static String FLOAT_HEADER = "\"float.h\"\n";

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

    private final static String CHAR_CONST_STRING = STATIC + "char" + CONST
            + "*" + CONST;
    private final static String ENUM_STRING = "enum";

    private final static String ID_T_STRING = "_id_t;\n\n";

    /* Do not change these (if you do, you also need to update idigi_remote.h */
    private final static String RCI_PARSER_USES_ERROR_DESCRIPTIONS = "RCI_PARSER_USES_ERROR_DESCRIPTIONS\n";
    private final static String RCI_PARSER_USES_ENUMERATIONS = "RCI_PARSER_USES_ENUMERATIONS\n";
    private final static String RCI_PARSER_USES_FLOATING_POINT = "RCI_PARSER_USES_FLOATING_POINT\n";
    private final static String RCI_PARSER_USES_STRINGS = "RCI_PARSER_USES_STRINGS\n";
    private final static String RCI_PARSER_USES_UNSIGNED_INTEGER = "RCI_PARSER_USES_UNSIGNED_INTEGER\n";
    private final static String RCI_PARSER_USES_SIGNED_INTEGER = "RCI_PARSER_USES_INTEGER\n";
//    private final static String RCI_PARSER_USES_ON_OFF = "RCI_PARSER_USES_ON_OFF\n";
//    private final static String RCI_PARSER_USES_BOOLEAN = "RCI_PARSER_USES_UNSIGNED_BOOLEAN\n";

    private String headerFile = HEADER_FILENAME;
    private String sourceFile = SOURCE_FILENAME;
    private final BufferedWriter sourceWriter;
    private final BufferedWriter headerWriter;
    private String configType;
    private int prevRemoteStringLength;

    public FileGenerator(String directoryPath) throws IOException {
        if (directoryPath != null) {
            if (!directoryPath.endsWith("/")) directoryPath += "/";

            headerFile = directoryPath + HEADER_FILENAME;
            sourceFile = directoryPath + SOURCE_FILENAME;
        }

        headerWriter = new BufferedWriter(new FileWriter(headerFile));
        sourceWriter = new BufferedWriter(new FileWriter(sourceFile));

        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();

        String className = ConfigGenerator.class.getName();

        int firstChar = className.lastIndexOf(".") + 1;
        if (firstChar > 0) {
            className = className.substring(firstChar);
        }

        String note_string = "/*\n * This is an auto-generated file - DO NOT EDIT! \n"
                            + String.format(" * This is generated by %s tool \n", className)
                            + String.format(" * This file was generated on: %s \n", dateFormat.format(date))
                            + String.format(" * The command line arguments were: %s\n", ConfigGenerator.getArgumentLogString())
                            + String.format(" * The version of %s tool was: %s */\n\n", className, ConfigGenerator.VERSION);

        headerWriter.write(note_string);
        sourceWriter.write(note_string);
    }

    public void generateFile(ConfigData configData) throws Exception {
        try {

            writeHeaderFile(configData);

            /*
             * Start writing C file 1. include file 2. all #define for all
             * strings from user's groups 3. all #define for all RCI and user's
             * global errors 4. all strings in idigi_remote_all_strings[]
             */
            sourceWriter.write(INCLUDE + "\"" + HEADER_FILENAME + "\"\n\n");

            /* Write all string length and index defines in C file */
            writeDefineStrings(configData);

            /* Write all string length and index defines in C file */
            writeDefineGlobalErrors(configData);

            /* write idigi remote all strings in source file */
            writeRemoteAllStrings(configData);

            /* write structures in source file */
            writeAllStructures(configData);

            ConfigGenerator.log("Files created:\n\t" + sourceFile + "\n\t" + headerFile);

        } catch (IOException e) {
            throw new IOException(e.getMessage());
        }

        finally {
            headerWriter.close();
            sourceWriter.close();
        }

    }

    private void writeHeaderFile(ConfigData configData) throws Exception {
        String defineName = HEADER_FILENAME.replace('.', '_').toLowerCase();
        headerWriter.write(String.format("#ifndef %s\n#define %s\n\n",
                defineName, defineName));

        writeDefineOptionHeader();

        writeDefineRciParserStringsHeader();

        /* Write all global error enum in H file */
        writeGlobalErrorHeader();

        /* Write all group enum in H file */
        writeGroupHeader(configData);

        headerWriter.write(String.format("\n#endif /* %s */\n", defineName));
    }

    private void writeDefineOptionHeader() throws IOException {

        String headerString = "";
        String structString = "";

        if (!ConfigGenerator.excludeErrorDescription()) {
            headerString += DEFINE + RCI_PARSER_USES_ERROR_DESCRIPTIONS;
        }

        String stringDefine = null;
        String uintDefine = null;
        String floatInclude = null;

        for (ElementStruct.ElementType type : ElementStruct.ElementType
                .values()) {
            if (type.isSet()) {
                switch (type) {
                case UINT32:
                case HEX32:
                case XHEX:
                    if (uintDefine == null) {
                        uintDefine = DEFINE + RCI_PARSER_USES_UNSIGNED_INTEGER;
                        headerString += uintDefine;
                    }
                    break;
                    
                case INT32:
                    headerString += DEFINE + RCI_PARSER_USES_SIGNED_INTEGER;
                    structString += "typedef struct {\n"
                                    + "   int32_t min_value;\n"
                                    + "   int32_t max_value;\n"
                                    + "} idigi_element_value_signed_integer_t\n";
                    break;
                    
                case ENUM:
                    headerString += DEFINE + RCI_PARSER_USES_ENUMERATIONS;
                    break;
                    
                case FLOAT:
                    headerString += DEFINE + RCI_PARSER_USES_FLOATING_POINT;
                    floatInclude = INCLUDE + FLOAT_HEADER;
                    
                    break;
                case ON_OFF:
//                    headerString += DEFINE + RCI_PARSER_USES_ON_OFF;
                    break;
                case BOOLEAN:
//                    headerString += DEFINE + RCI_PARSER_USES_BOOLEAN;
                    break;
                default:
                    if (stringDefine == null) {
                        stringDefine = DEFINE + RCI_PARSER_USES_STRINGS;
                        headerString += stringDefine;
                     }
                    break;
                }
            }
        }
        if (floatInclude != null)
            headerString += "\n" + floatInclude;

        headerString += "\n" + INCLUDE + IDIGI_REMOTE_HEADER;

        headerWriter.write(headerString);
    }

    private void writeRemoteAllStrings(ConfigData configData) throws Exception {
        sourceWriter.write(String.format("\nchar const %s[] = {\n",
                IDIGI_REMOTE_ALL_STRING));

        writeRemoteRciParserStrings();

        for (ConfigData.ConfigType type : ConfigData.ConfigType.values()) {
            LinkedList<GroupStruct> theConfig = null;

            configType = type.toString().toLowerCase();

            theConfig = configData.getConfigGroup(configType);

            if (!theConfig.isEmpty()) {
                writeGroupRemoteAllStrings(theConfig);
            }
        }
        writeErrorsRemoteAllStrings(configData);
        sourceWriter.write(" \'\\0\'\n};\n\n"); // end of IDIGI_REMOTE_ALL_STRING
    }

    private void writeDefineRciParserStringsHeader() throws IOException {
        LinkedHashMap<String, String> rciStrings = ConfigData.getRciStrings();

        if (rciStrings.size() > 0) {
            headerWriter.write(String.format("extern char const %s[];\n\n", IDIGI_REMOTE_ALL_STRING));
        }

        for (String key : rciStrings.keySet()) {
            String defineName = RCI_PREFIX + key.toUpperCase();
            /* define name string index */
            headerWriter.write(getDefineStringIndex(defineName, rciStrings.get(key).toUpperCase()));
        }
    }

    private void writeRemoteRciParserStrings() throws IOException {
        LinkedHashMap<String, String> rciStrings = ConfigData.getRciStrings();

        for (String key : rciStrings.keySet()) {
            sourceWriter.write(getCharString(rciStrings.get(key)));
        }

    }

    private void writeDefineStrings(ConfigData configData) throws Exception {
        String defineName = null;

        for (ConfigData.ConfigType type : ConfigData.ConfigType.values()) {
            LinkedList<GroupStruct> groups = null;

            configType = type.toString().toLowerCase();

            groups = configData.getConfigGroup(configType);

            for (GroupStruct group : groups) {
                defineName = getDefineString(group.getName());
                /* define name string index */
                sourceWriter.write(getDefineStringIndex(defineName, group
                        .getName()));

                for (ElementStruct element : group.getElements()) {
                    defineName = getDefineString(group.getName() + "_" + element.getName());
                    /* define name string index */
                    sourceWriter.write(getDefineStringIndex(defineName, element.getName()));

                    if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM) {
                        
                        for (ValueStruct value : element.getValues()) {
                            defineName = getDefineString(group.getName() + "_" + element.getName() + "_" + value.getName());
                            /* define name string index */
                            sourceWriter.write(getDefineStringIndex(defineName, value.getName()));
                        }
                    }
                }

                if ((!ConfigGenerator.excludeErrorDescription()) && (!group.getErrors().isEmpty())) {
                    LinkedHashMap<String, String> errorMap = group.getErrors();
                    for (String key : errorMap.keySet()) {
                        defineName = getDefineString(group.getName() + "_" + ERROR + "_" + key);
                        /* define name string index */
                        sourceWriter.write(getDefineStringIndex(defineName, key));
                    }
                }
            }
        }
    }

    private void writeGroupRemoteAllStrings(LinkedList<GroupStruct> groups) throws Exception {
        for (GroupStruct group : groups) {
            sourceWriter.write(getCharString(group.getName()));

            for (ElementStruct element : group.getElements()) {
                sourceWriter.write(getCharString(element.getName()));

                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM) {
                    for (ValueStruct value : element.getValues()) {
                        sourceWriter.write(getCharString(value.getName()));
                    }
                }
            }

            if ((!ConfigGenerator.excludeErrorDescription()) && (!group.getErrors().isEmpty())) {
                LinkedHashMap<String, String> errorMap = group.getErrors();
                for (String key : errorMap.keySet()) {
                    sourceWriter.write(getCharString(key));
                }
            }
        }
    }

    private void writeDefineErrors(String prefixName, LinkedHashMap<String, String> errorMap) throws IOException {
        for (String key : errorMap.keySet()) {
            String defineName = prefixName.toUpperCase() + "_" + key.toUpperCase();
            /* define name string index */
            sourceWriter.write(getDefineStringIndex(defineName, errorMap.get(key)));
        }
    }

    private void writeDefineGlobalErrors(ConfigData configData) throws IOException {
        if (!ConfigGenerator.excludeErrorDescription()) {
            writeDefineErrors(GLOBAL_RCI_ERROR, ConfigData.getRciCommonErrors());

            writeDefineErrors(GLOBAL_RCI_ERROR, ConfigData.getRciGlobalErrors());

            writeDefineErrors(GLOBAL_RCI_ERROR, ConfigData.getRciCommandErrors());

            writeDefineErrors(GLOBAL_RCI_ERROR, ConfigData.getRciGroupErrors());

            writeDefineErrors(GLOBAL_ERROR, ConfigData.getUserGlobalErrors());
        }
    }

    private void writeLinkedHashMapStrings(LinkedHashMap<String, String> stringMap) throws IOException {
        for (String key : stringMap.keySet()) {
            sourceWriter.write(getCharString(key));
        }

    }

    private void writeErrorsRemoteAllStrings(ConfigData configData) throws IOException {
        if (!ConfigGenerator.excludeErrorDescription()) {
            writeLinkedHashMapStrings(ConfigData.getRciCommonErrors());

            writeLinkedHashMapStrings(ConfigData.getRciGlobalErrors());

            writeLinkedHashMapStrings(ConfigData.getRciCommandErrors());

            writeLinkedHashMapStrings(ConfigData.getRciGroupErrors());

            writeLinkedHashMapStrings(ConfigData.getUserGlobalErrors());
        }
    }

    private void writeEnumStructure(String enum_name, LinkedList<ValueStruct> values) throws IOException {
        String enum_string = enum_name.toLowerCase() + "_" + ENUM_STRING;

        /* write element enum strings array */
        sourceWriter.write(CHAR_CONST_STRING + enum_string + "[] = {\n");


        int size = values.size();
        
        for (int i=0; i < size; i++)
        {
            ValueStruct value = values.get(i);
            
            /* write idigi_remote_all_strings reference */
            sourceWriter.write(getRemoteString(enum_name + "_" + value.getName()));
            
            if (i < (size-1)) sourceWriter.write(",");
            /* write comment */
            sourceWriter.write(COMMENTED(value.getName()));
        }
        /* end of writing element enum strings array */
        sourceWriter.write("};\n\n");

        /* write element value limit structure for enum type */
        String enum_limit_string = String.format("static %s const %s_limit = {\n asizeof(%s),\n %s\n};\n\n",
                                                IDIGI_ELEMENT_VALUE_ENUM, enum_name.toLowerCase(), enum_string,
                                                enum_string);

        sourceWriter.write(enum_limit_string);
    }

    private void writeElementLimitStructures(String element_name, ElementStruct element) throws Exception {
        /* write element value limit structure */
        String limit_string = STATIC;

        ElementStruct.ElementType type = ElementStruct.ElementType.toElementType(element.getType());

        switch (type) {
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

        if (element.getMin() == null) {
            switch (type) {
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
        } else {
            limit_string += " ";
            if (type == ElementStruct.ElementType.HEX32)
                limit_string += "0x";
            limit_string += element.getMin();
        }

        limit_string += ",\n";

        if (element.getMax() == null) {
            switch (type) {
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
        } else {
            limit_string += " ";
            if (type == ElementStruct.ElementType.HEX32) limit_string += "0x";
            limit_string += element.getMax();
        }

        sourceWriter.write(limit_string);
        sourceWriter.write("\n};\n\n");
    }

    private void writeElementArrays(String group_name, LinkedList<ElementStruct> elements) throws Exception {
        /* write group element structure array */
        sourceWriter.write(String.format("static idigi_group_element_t const %s_elements[] = {",
                                        getDefineString(group_name).toLowerCase()));

        for (int element_index = 0; element_index < elements.size(); element_index++) {
            ElementStruct element = elements.get(element_index);

            String element_name = getDefineString(group_name + "_" + element.getName());

            String element_string = "\n"
                                    + " "
                                    + "{"
                                    + getRemoteString(element_name)
                                    + ", "
                                    + COMMENTED(element.getName())
                                    + "  "
                                    + getElementDefine("access", getAccess(element.getAccess()))
                                    + "  " 
                                    + getElementDefine("type", element.getType());

            if ((ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM)
                  || (element.getMin() != null) || (element.getMax() != null)) {
                
                element_string += String.format("  (idigi_element_value_limit_t *)&%s_limit",
                                                element_name.toLowerCase());
            } else {
                element_string += "  NULL";
            }
            element_string += "\n }";

            if (element_index < (elements.size() - 1)) {
                element_string += ",";
            }

            sourceWriter.write(element_string);
        }
        sourceWriter.write("\n};\n\n");

    }

    private void writeGlobalErrorStructures(ConfigData configData) throws IOException {
        
        if (!ConfigGenerator.excludeErrorDescription()) {
            int errorCount = ConfigData.getRciCommonErrors().size()
                            + ConfigData.getRciGlobalErrors().size()
                            + ConfigData.getRciCommandErrors().size()
                            + ConfigData.getRciGroupErrors().size()
                            + ConfigData.getUserGlobalErrors().size();

            if (errorCount > 0) {
                sourceWriter.write(String.format("char const * const %ss[%d] = {\n", GLOBAL_RCI_ERROR, errorCount));
                        
                /* top-level all errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_RCI_ERROR,
                             ConfigData.getRciCommonErrors());

                /* top-level global errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_RCI_ERROR,
                             ConfigData.getRciGlobalErrors());

                /* top-level command errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_RCI_ERROR,
                             ConfigData.getRciCommandErrors());

                /* top-level group errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_RCI_ERROR,
                             ConfigData.getRciGroupErrors());

                /* group global errors */
                errorCount = writeErrorStructures(errorCount, GLOBAL_ERROR,
                             ConfigData.getUserGlobalErrors());

                sourceWriter.write("};\n\n");
            }
        }
    }

    private int writeErrorStructures(int errorCount, String defineName, LinkedHashMap<String, String> errorMap) throws IOException {
        
        for (String key : errorMap.keySet()) {
            sourceWriter.write(getRemoteString(defineName.toUpperCase() + "_" + key));
            errorCount--;
            if (errorCount > 0) {
                sourceWriter.write(",");
            }
            sourceWriter.write(COMMENTED(key));
        }

        return errorCount;
    }

    private void writeErrorStructures(String error_name, LinkedHashMap<String, String> localErrors) throws IOException {
        
        if (!ConfigGenerator.excludeErrorDescription()) {
            String define_name;

            if (!localErrors.isEmpty()) {
                define_name = getDefineString(error_name + "_" + ERROR);
                sourceWriter.write(CHAR_CONST_STRING + define_name.toLowerCase() + "s[] = {\n");

                /* local local errors */
                define_name = getDefineString(error_name + "_" + ERROR);
                int error_count = localErrors.size();
                writeErrorStructures(error_count, define_name, localErrors);

                sourceWriter.write("};\n\n");
            }
        }
    }

    private void writeGroupStructures(LinkedList<GroupStruct> groups) throws Exception {
        
        String define_name;

        for (int group_index = 0; group_index < groups.size(); group_index++) {
            GroupStruct group = groups.get(group_index);

            for (int element_index = 0; element_index < group.getElements()
                    .size(); element_index++) {
                ElementStruct element = group.getElements().get(element_index);

                define_name = getDefineString(group.getName() + "_" + element.getName());

                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM) {
                    /* write enum structure */
                    writeEnumStructure(define_name, element.getValues());
                } else if (element.getMax() != null || element.getMin() != null) {
                    /* write limit structure */
                    writeElementLimitStructures(define_name, element);
                }
            }

            /* write element structure */
            writeElementArrays(group.getName(), group.getElements());

            writeErrorStructures(group.getName(), group.getErrors());
        }

    }

    private void writeAllStructures(ConfigData configData) throws Exception {
        String define_name;

        for (ConfigData.ConfigType type : ConfigData.ConfigType.values()) {
            LinkedList<GroupStruct> groups = null;

            configType = type.toString().toLowerCase();

            groups = configData.getConfigGroup(configType);

            if (!groups.isEmpty()) {
                writeGroupStructures(groups);

                sourceWriter.write(String.format("static idigi_group_t const idigi_%s_groups[] = {", configType));

                for (int group_index = 0; group_index < groups.size(); group_index++) {
                    GroupStruct group = groups.get(group_index);

                    define_name = getDefineString(group.getName() + "_elements");
                    String group_string = String.format("\n {%s, %s", 
                                                        getRemoteString(getDefineString(group.getName())),
                                                        COMMENTED(group.getName()))
                                        + String.format("   %d,\n", group.getInstances())
                                        + String.format("   { asizeof(%s),\n", define_name.toLowerCase())
                                        + String.format("     %s\n   },\n", define_name.toLowerCase());

                    if ((!ConfigGenerator.excludeErrorDescription()) && (!group.getErrors().isEmpty())) {
                        define_name = getDefineString(group.getName() + "_errors");

                        group_string += String.format("   { asizeof(%s),\n", define_name.toLowerCase())
                                        + String.format("     %s\n   }\n }\n", define_name.toLowerCase());

                    } else {
                        group_string += "   { 0,\n     NULL\n   }\n }";
                    }

                    if (group_index < (groups.size() - 1)) {
                        group_string += ",";
                    }

                    sourceWriter.write(group_string);
                }
                sourceWriter.write("\n};\n\n");
            }
        }

        writeGlobalErrorStructures(configData);

        String idigiGroupString = String.format("idigi_group_table_t const %s[%d] = {\n",
                                                IDIGI_REMOTE_GROUP_TABLE, ConfigData.ConfigType
                                               .getConfigTypeCount());

        for (ConfigData.ConfigType type : ConfigData.ConfigType.values()) {
            LinkedList<GroupStruct> groups = null;

            configType = type.toString().toLowerCase();

            groups = configData.getConfigGroup(configType);

            if (type.getIndex() != 0) {
                idigiGroupString += ",\n";
            }

            idigiGroupString += " " + "{";
            if (!groups.isEmpty()) {
                idigiGroupString += String.format("idigi_%s_groups,\n asizeof(idigi_%s_groups)\n }",
                                                   configType, configType);

            } else {
                idigiGroupString += "NULL,\n 0\n }";
            }

        }
        idigiGroupString += "\n};\n\n";

        sourceWriter.write(idigiGroupString);
    }

    private void writeErrorHeader(int errorIndex, String enumDefine, LinkedHashMap<String, String> errorMap) throws IOException {
        
        for (String key : errorMap.keySet()) {
            String error_string = " " + enumDefine + "_" + key;

            if (errorIndex == 1) {
                error_string += " = " + " " + enumDefine + "_" + OFFSET_STRING;
            }
            errorIndex++;

            error_string += ",\n";

            headerWriter.write(error_string);
        }
    }

    private void writeGlobalErrorHeader() throws IOException {

        /* write typedef enum for rci errors */
        headerWriter.write("\n" + TYPEDEF_ENUM + " " + GLOBAL_RCI_ERROR + "_" + OFFSET_STRING + " = 1,\n");

        writeErrorHeader(ConfigData.getRciCommonErrorsIndex(), GLOBAL_RCI_ERROR, ConfigData.getRciCommonErrors());

        writeErrorHeader(ConfigData.getRciGlobalErrorsIndex(), GLOBAL_RCI_ERROR, ConfigData.getRciGlobalErrors());

        writeErrorHeader(ConfigData.getRciCommandErrorsIndex(), GLOBAL_RCI_ERROR, ConfigData.getRciCommandErrors());

        writeErrorHeader(ConfigData.getRciGroupErrorsIndex(), GLOBAL_RCI_ERROR, ConfigData.getRciGroupErrors());

        headerWriter.write(" " + GLOBAL_RCI_ERROR + "_" + COUNT_STRING + "\n} " + GLOBAL_RCI_ERROR + ID_T_STRING);

        /* write typedef enum for user global error */
        String enumName = GLOBAL_ERROR + "_" + OFFSET_STRING;

        headerWriter.write("\n" + TYPEDEF_ENUM + " " + enumName + " = " + GLOBAL_RCI_ERROR + "_" + COUNT_STRING + ",\n");

        writeErrorHeader(1, GLOBAL_ERROR, ConfigData.getUserGlobalErrors());

        String endString = String.format(" %s_%s", GLOBAL_ERROR, COUNT_STRING);

        if (ConfigData.getUserGlobalErrors().isEmpty()) {
            endString += " = " + enumName;
        }
        endString += "\n} " + GLOBAL_ERROR + ID_T_STRING;

        headerWriter.write(endString);

    }

    private void writeEnumHeader(LinkedList<GroupStruct> groups) throws Exception {

        for (GroupStruct group : groups) {
            /* build element enum string for element enum */
            String element_enum_string = TYPEDEF_ENUM;

            for (ElementStruct element : group.getElements()) {
                /* add element name */
                element_enum_string += getEnumString(group.getName() + "_"
                        + element.getName())
                        + ",\n";

                if (ElementStruct.ElementType.toElementType(element.getType()) == ElementStruct.ElementType.ENUM) {
                    /* write typedef enum for value */
                    headerWriter.write(TYPEDEF_ENUM);

                    for (ValueStruct value : element.getValues()) {
                        headerWriter.write(getEnumString(group.getName() + "_" + element.getName() + "_" + value.getName()) + ",\n");
                    }
                    /* done typedef enum for value */
                    headerWriter.write(endEnumString(group.getName() + "_" + element.getName()));
                }
            }
            /* done typedef enum for element */

            element_enum_string += endEnumString(group.getName());

            headerWriter.write(element_enum_string);

            if (!group.getErrors().isEmpty()) {
                headerWriter.write(TYPEDEF_ENUM);

                LinkedHashMap<String, String> errorMap = group.getErrors();
                int index = 0;

                for (String key : errorMap.keySet()) {
                    String enumString = getEnumString(group.getName() + "_" + ERROR + "_" + key);
                    if (index++ == 0) {
                        /* write start index */
                        enumString += " = " + GLOBAL_ERROR + "_" + COUNT_STRING;
                    }

                    enumString += ",\n";

                    headerWriter.write(enumString);
                }
                headerWriter.write(endEnumString(group.getName() + "_" + ERROR));
            }

        }

    }

    private void writeGroupHeader(ConfigData configData) throws Exception {

        for (ConfigData.ConfigType type : ConfigData.ConfigType.values()) {
            LinkedList<GroupStruct> groups = null;

            configType = type.toString().toLowerCase();
            groups = configData.getConfigGroup(configType);

            if (!groups.isEmpty()) {
                /* build group enum string for group enum */
                String group_enum_string = TYPEDEF_ENUM;

                /* Write all enum in H file */
                writeEnumHeader(groups);

                for (GroupStruct group : groups) {
                    /* add each group enum */
                    group_enum_string += getEnumString(group.getName()) + ",\n";
                }

                /* write group enum buffer to headerWriter */
                group_enum_string += endEnumString(null);
                headerWriter.write(group_enum_string);
            }
        }

    }

    private String COMMENTED(String comment) {
        return " /*" + comment + "*/\n";
    }

    private String getEnumString(String enum_name) {
        String str = " " + IDIGI + "_" + configType;

        if (enum_name != null) {
            str += "_" + enum_name;
        }
        return str;
    }

    private String endEnumString(String group_name) {
        return (getEnumString(group_name) + "_" + COUNT_STRING + "\n}"
                + getEnumString(group_name) + ID_T_STRING);
    }

    private String getDefineString(String define_name) {
        return (configType.toUpperCase() + "_" + define_name.toUpperCase());
    }

    private String getDefineStringIndex(String define_name, String string) {
        String str = DEFINE + define_name + " " + "(" + IDIGI_REMOTE_ALL_STRING + "+" + prevRemoteStringLength + ")\n";
        prevRemoteStringLength += string.length() + 1;
        return str;
    }

    private String getCharString(String string) {
        char[] characters = string.toCharArray();

        String quote_char = " " + string.length() + ",";
        for (char c : characters) {
            quote_char += "\'" + c + "\',";
        }
        quote_char += "\n";

        return quote_char;
    }

    private String getRemoteString(String define_name) {
        return (" " + define_name.toUpperCase());

    }

    private String getElementDefine(String type_name, String element_name) {
        return (String.format(" %s_element_%s_%s,\n", IDIGI, type_name, element_name));
    }

    private String getAccess(String access) {
        if (access == null) {
            return "read_write";
        }
        return access;
    }

}
