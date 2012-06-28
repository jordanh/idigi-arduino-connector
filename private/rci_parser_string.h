/*
 * Copyright (c) 2012 Digi International Inc.,
 * All rights not expressly granted are reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 * =======================================================================
 */

typedef char cstr_t;

static cstr_t const * cstr_data(cstr_t const * const cstr)
{
    cstr_t const * const data = cstr + 1;

    return data;
}

static size_t cstr_len(cstr_t const * const cstr)
{
    size_t const length = *cstr;

    return length;
}

static void cstr_to_rcistr(cstr_t const * const cstr, rcistr_t * const rcistr)
{
    rcistr->data = cstr_data(cstr);
    rcistr->length = cstr_len(cstr);
}

static void str_to_rcistr(char const * const str, rcistr_t * const rcistr)
{
    rcistr->data = str;
    rcistr->length = strlen(str);
}

static idigi_bool_t buffer_equals_buffer(char const * const str1, size_t const len1, char const * const str2, size_t const len2)
{
    ASSERT(len1 != 0);
    ASSERT(len2 != 0);

    return idigi_bool((len1 == len2) && (memcmp(str1, str2, len1) == 0));
}

static idigi_bool_t cstr_equals_buffer(cstr_t const * const cstr, char const * const str2, unsigned int const len2)
{
    return buffer_equals_buffer(cstr_data(cstr), cstr_len(cstr), str2, len2);
}

static idigi_bool_t cstr_equals_rcistr(cstr_t const * const cstr, rcistr_t const * const rcistr)
{
    return cstr_equals_buffer(cstr, rcistr->data, rcistr->length);
}

static idigi_bool_t rcistr_equals_rcistr(rcistr_t const * const one, rcistr_t const * const two)
{
    return buffer_equals_buffer(one->data, one->length, two->data, two->length);
}

#if (defined RCI_PARSER_USES_ENUM) || (defined RCI_PARSER_USES_ON_OFF) || (defined RCI_PARSER_USES_BOOLEAN)
static idigi_bool_t cstr_equals_str(cstr_t const * const cstr, char const * const str)
{
    return cstr_equals_buffer(cstr, str, strlen(str));
}
#endif

static idigi_bool_t rcistr_to_uint(rcistr_t const * const rcistr, unsigned int * const value)
{
    ASSERT(!isdigit(rcistr->data[rcistr->length]));

    return idigi_bool(sscanf(rcistr->data, "%u", value) == 1);
}

static size_t rcistr_length(rcistr_t const * const string)
{
    return string->length;
}

static char const * rcistr_data(rcistr_t const * const string)
{
    return string->data;
}

#if defined RCI_PARSER_USES_STRINGS
static int rcistr_char(rcistr_t const * const string, size_t const index)
{
    ASSERT(index < rcistr_length(string));

    return string->data[index];
}
#endif

static void clear_rcistr(rcistr_t * const string)
{
    string->data = NULL;
    string->length = 0;
}

static idigi_bool_t rcistr_empty(rcistr_t const * const string)
{
    return idigi_bool((string->data == NULL) && (string->length == 0));
}

static idigi_bool_t rcistr_valid(rcistr_t const * const string)
{
    return idigi_bool((string->data != NULL) && (string->length != 0));
}

static void begin_rcistr(rci_t const * const rci, rcistr_t * const string)
{
    string->data = rci->input.destination;
}
static void end_rcistr(rci_t const * const rci, rcistr_t * const string)
{
    string->length = (rci->input.destination - string->data);
}

static void adjust_char_pointer(char * const new_base, char const * const old_base, char * * const pointer)
{
    size_t const offset = (*pointer - old_base);

    *pointer = (new_base + offset);
}

static void adjust_rcistr(char * const new_base, char const * const old_base, rcistr_t * const string)
{
    char * pointer = (char *) string->data;

    adjust_char_pointer(new_base, old_base, &pointer);
    string->data = pointer;
}



