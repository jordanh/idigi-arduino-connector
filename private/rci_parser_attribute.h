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

static rcistr_t * attribute_name(rci_attribute_list_t * const list, unsigned int const index)
{
    ASSERT(index < attribute_count(list));

    return &list->pair[index].name;
}

static rcistr_t * attribute_value(rci_attribute_list_t * const list, unsigned int const index)
{
    ASSERT(index < attribute_count(list));

    return &list->pair[index].value;
}

static rcistr_t const * find_attribute_value(rci_attribute_list_t const * const list, cstr_t const * const name)
{
    rcistr_t const * result = NULL;
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
    cstr_to_rcistr(name, &pair->name);
    cstr_to_rcistr(value, &pair->value);
}

static void prep_rci_reply_data(rci_t * const rci)
{
    cstr_to_rcistr(RCI_REPLY, &rci->output.tag);
    add_attribute(&rci->output.attribute, RCI_VERSION, RCI_VERSION_SUPPORTED);
    rci->output.type = rci_output_type_start_tag;
}

static void add_numeric_attribute(rci_attribute_list_t * const list, cstr_t const * const name, unsigned int const value)
{
#define  MAX_NUMERIC_VALUE  "999"
    static cstr_t storage[] = "n" MAX_NUMERIC_VALUE;
    size_t const storage_available = sizeof MAX_NUMERIC_VALUE;
    int const digits_max = storage_available - 1;
    int const digits_used = snprintf(storage + 1, storage_available, "%u", value);

    ASSERT((digits_used >= 1) && (digits_used <= digits_max));
    storage[0] = digits_used;

    add_attribute(list, name, storage);
#undef  MAX_NUMERIC_VALUE
}

