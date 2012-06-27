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

#define ELEMENT_ID_VARIABLE(rci)        ((rci)->shared.current.element.id)
#define set_element_id(rci, value)      (ELEMENT_ID_VARIABLE(rci) = (value))
#define get_element_id(rci)             (ELEMENT_ID_VARIABLE(rci))
#define invalidate_element_id(rci)      set_element_id(rci, INVALID_ID)
#define have_element_id(rci)            (get_element_id(rci) != INVALID_ID)

static unsigned int find_element_id_in_group(idigi_group_t const * const group, rcistr_t const * const tag)
{
    unsigned int id = INVALID_ID;
    size_t i;

    for (i = 0; i < group->elements.count; i++)
    {
        if (cstr_equals_rcistr(group->elements.data[i].name, tag))
        {
            id = i;
            break;
        }
    }

    return id;
}

static void assign_element_id(rci_t * const rci, rcistr_t const * const tag)
{
    ASSERT(have_group_id(rci));
    {
        unsigned int const id = find_element_id_in_group(get_current_group(rci), tag);

        set_element_id(rci, id);
    }
}

static idigi_group_element_t const * get_current_element(rci_t const * const rci)
{
    ASSERT(have_group_id(rci));
    ASSERT(have_element_id(rci));

    {
        idigi_group_t const * const group = get_current_group(rci);
        unsigned int const id = get_element_id(rci);

        ASSERT(id < group->elements.count);

        return (group->elements.data + id);
    }
}

