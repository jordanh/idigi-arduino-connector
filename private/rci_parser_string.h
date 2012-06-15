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

#define CSTR_LEN(p)     ((size_t) *(p))
#define CSTR_DATA(p)    ((p) + 1)
typedef char cstr_t;

static void cstr_to_rci_string(cstr_t const * const cstr, rci_string_t * const rcistr)
{
    rcistr->data = CSTR_DATA(cstr);
    rcistr->length = CSTR_LEN(cstr);
}

static void str_to_rcistr(char const * const str, rci_string_t * const rcistr)
{
    rcistr->data = str;
    rcistr->length = strlen(str);
}

static idigi_bool_t buffer_equals_buffer(char const * const str1, size_t const len1, char const * const str2, size_t const len2)
{
    ASSERT(len1 != 0);
    ASSERT(len2 != 0);
    
    return ((len1 == len2) && (memcmp(str1, str2, len1) == 0)) ? idigi_true : idigi_false;
}

static idigi_bool_t cstr_equals_buffer(cstr_t const * const cstr, char const * const str2, unsigned int const len2)
{
    return buffer_equals_buffer(CSTR_DATA(cstr), CSTR_LEN(cstr), str2, len2);
}

static idigi_bool_t cstr_equals_rcistr(cstr_t const * const cstr, rci_string_t const * const rcistr)
{
    return cstr_equals_buffer(cstr, rcistr->data, rcistr->length);
}

static idigi_bool_t cstr_equals_str(cstr_t const * const cstr, char const * const str)
{
    return cstr_equals_buffer(cstr, str, strlen(str));
}

static idigi_bool_t rcistr_to_uint(rci_string_t const * const rcistr, unsigned int * const value)
{
    ASSERT(!isdigit(rcistr->data[rcistr->length]));

    return (sscanf(rcistr->data, "%u", value) == 1);
}

static size_t rcistr_length(rci_string_t const * const string)
{
    return string->length;
}

static char const * rcistr_data(rci_string_t const * const string)
{
    return string->data;
}

static int rcistr_char(rci_string_t const * const string, size_t const index)
{
    ASSERT(index < rcistr_length(string));
    
    return string->data[index];
}

static void clear_rcistr(rci_string_t * const string)
{
    string->data = NULL;
    string->length = 0;
}

static void set_rcistr_length(rci_t const * const rci, rci_string_t * const string)
{
    string->length = (rci->input.destination - string->data);
}

static void adjust_char_pointer(rci_t const * const rci, char const * const old_base, char * * const pointer)
{
    size_t const offset = (*pointer - old_base);
    char * new_base = (char *) rci->input.storage;
     
    *pointer = (new_base + offset);
}

static void adjust_rci_string(rci_t const * const rci, char const * const base, rci_string_t * const string)
{
    char * pointer = (char *) string->data;
    
    adjust_char_pointer(rci, base, &pointer);
    string->data = pointer;
}



