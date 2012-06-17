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

static size_t attribute_count(rci_attribute_list_t * const list)
{
    return list->count;
}

static idigi_bool_t have_attributes(rci_attribute_list_t * const list)
{
    return idigi_bool(attribute_count(list) != 0);
}

static void clear_attributes(rci_attribute_list_t * const list)
{
    list->count = 0;
}

static rci_string_t * attribute_name(rci_attribute_list_t * const list, unsigned int const index)
{
    ASSERT(index < attribute_count(list));

    return &list->pair[index].name;
}

static rci_string_t * attribute_value(rci_attribute_list_t * const list, unsigned int const index)
{
    ASSERT(index < attribute_count(list));

    return &list->pair[index].value;
}

static rci_string_t const * find_attribute_value(rci_attribute_list_t const * const list, cstr_t const * const name)
{
    rci_string_t const * result = NULL;
    size_t i;

    for (i = 0; i < list->count; i++)
    {
        rci_attribute_t const * const pair = &list->pair[i];

        if (cstr_equals_rcistr(name, &pair->name))
        {
            result = &pair->value;
            break;
        }
    }

    return result;
}

static void add_attribute(rci_attribute_list_t * const list, cstr_t const * const name, cstr_t const * const value)
{
    size_t const index = list->count;
    rci_attribute_t * const pair = &list->pair[index];

    ASSERT(index < asizeof(list->pair));

    list->count++;
    cstr_to_rci_string(name, &pair->name);
    cstr_to_rci_string(value, &pair->value);
}

static void prep_rci_reply_data(rci_t * const rci)
{
    cstr_to_rci_string(RCI_REPLY, &rci->output.tag);
    add_attribute(&rci->output.attribute, RCI_VERSION, RCI_VERSION_SUPPORTED);
    rci->output.type = rci_output_type_start_tag;
}

static void add_numeric_attribute(rci_attribute_list_t * const list, cstr_t const * const name, unsigned int const value)
{
#define  MAX_NUMERIC_VALUE  "999"
    static cstr_t storage[] = "n" MAX_NUMERIC_VALUE;
    int const digits_max = sizeof MAX_NUMERIC_VALUE - 1;
    size_t const storage_available = digits_max;
    int const digits_used = snprintf(storage + 1, storage_available, "%u", value);

    ASSERT((digits_used >= 1) && (digits_used <= digits_max));
    storage[0] = digits_used;

    add_attribute(list, name, storage);
#undef  MAX_INDEX
}

