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

static rcistr_t * attribute_name(rci_attribute_t * const attribute)
{
    return &attribute->name;
}

static rcistr_t * attribute_value(rci_attribute_t * const attribute)
{
    return &attribute->value;
}

static idigi_bool_t have_attribute(rci_attribute_t * const attribute)
{
    return idigi_bool(rcistr_valid(attribute_name(attribute)) && rcistr_valid(attribute_value(attribute)));
}

static void clear_attribute(rci_attribute_t * const attribute)
{
    clear_rcistr(&attribute->name);
    clear_rcistr(&attribute->value);
}

static void set_attribute(rci_attribute_t * const attribute, cstr_t const * const name, cstr_t const * const value)
{
    ASSERT(rcistr_empty(&attribute->name));
    ASSERT(rcistr_empty(&attribute->value));

    cstr_to_rcistr(name, &attribute->name);
    cstr_to_rcistr(value, &attribute->value);
}

static void prep_rci_reply_data(rci_t * const rci)
{
    cstr_to_rcistr(RCI_REPLY, &rci->output.tag);
    set_attribute(&rci->output.attribute, RCI_VERSION, RCI_VERSION_SUPPORTED);
    rci->output.type = rci_output_type_start_tag;
}

static void set_numeric_attribute(rci_attribute_t * const attribute, cstr_t const * const name, unsigned int const value)
{
#define  MAX_NUMERIC_VALUE  "999"
    static cstr_t storage[] = "n" MAX_NUMERIC_VALUE;
    int const digits_max = sizeof MAX_NUMERIC_VALUE - sizeof nul;
    size_t const storage_available = digits_max + sizeof nul;
    int const digits_used = snprintf(storage + 1, storage_available, "%u", value);

    ASSERT((digits_used >= 1) && (digits_used <= digits_max));
    storage[0] = digits_used;

    set_attribute(attribute, name, storage);
#undef  MAX_NUMERIC_VALUE
}

