/*
 *  Copyright (c) 2012 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees
 *  to retain this software in confidence, to prevent disclosure to others,
 *  and to make no use of this software other than that for which it was
 *  delivered.  This is an unpublished copyrighted work of Digi International
 *  Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
 *  prohibited.
 *
 *  Restricted Rights Legend
 *
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
 *  Technical Data and Computer Software clause at DFARS 252.227-7031 or
 *  subparagraphs (c)(1) and (2) of the Commercial Computer Software -
 *  Restricted Rights at 48 CFR 52.227-19, as applicable.
 *
 *  Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 *
 * =======================================================================
 *
 */

static struct
{
    char const * name;
    int value;
} entity_map[] = {
    { INDEXED_CSTR(ENTITY_QUOTE_STRING_INDEX), '"' },
    { INDEXED_CSTR(ENTITY_AMPERSAND_STRING_INDEX), '&' },
    { INDEXED_CSTR(ENTITY_APOSTROPHE_STRING_INDEX), '\'' },
    { INDEXED_CSTR(ENTITY_LESS_THAN_STRING_INDEX), '<' },
    { INDEXED_CSTR(ENTITY_GREATER_THAN_STRING_INDEX), '>' },
};
    
static char const * rci_entity_name(int const value)
{
    char const * result = NULL;
    size_t i;
    
    for (i = 0; i < asizeof(entity_map); i++)
    {
        if (value == entity_map[i].value)
        {
            result = entity_map[i].name;
            break;
        }
    }
    
    return result;
}
    
static int rci_entity_value(char const * const name, size_t const bytes)
{
    int result = 0;
    size_t i;
    
    for (i = 0; i < asizeof(entity_map); i++)
    {
        if (cstr_equals_buffer(entity_map[i].name, name, bytes))
        {
            result = entity_map[i].value;
            break;
        }
    }
    
    return result;
}

