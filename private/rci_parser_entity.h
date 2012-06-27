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

static struct
{
    char const * name;
    int value;
} entity_map[] = {
    { RCI_ENTITY_QUOTE, '"' },
    { RCI_ENTITY_AMPERSAND, '&' },
    { RCI_ENTITY_APOSTROPHE, '\'' },
    { RCI_ENTITY_LESS_THAN, '<' },
    { RCI_ENTITY_GREATER_THAN, '>' },
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

static int rci_entity_value(rcistr_t const * const entity)
{
    int result = 0;
    size_t i;

    for (i = 0; i < asizeof(entity_map); i++)
    {
        if (cstr_equals_rcistr(entity_map[i].name, entity))
        {
            result = entity_map[i].value;
            break;
        }
    }

    return result;
}

