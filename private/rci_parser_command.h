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

static cstr_t const * const rci_command[] = {
    RCI_SET_SETTING,
    RCI_SET_STATE,
    RCI_QUERY_SETTING,
    RCI_QUERY_STATE
};

static rci_command_t find_rci_command(rcistr_t const * const tag)
{
    size_t i;
    rci_command_t result = rci_command_unknown;

    CONFIRM(rci_command_set_state == (rci_command_set_setting + 1));
    CONFIRM(rci_command_query_setting == (rci_command_set_state + 1));
    CONFIRM(rci_command_query_state == (rci_command_query_setting + 1));

    for (i = 0; i < asizeof(rci_command); i++)
    {
        if (cstr_equals_rcistr(rci_command[i], tag))
        {
            result = (rci_command_t) (rci_command_set_setting + i);
            break;
        }
    }

    return result;
}

static void set_rci_command_tag(rci_command_t const command, rcistr_t * const tag)
{
    int const index = (command - rci_command_set_setting);

    ASSERT((command >= rci_command_set_setting) && (command <= rci_command_query_state));

    cstr_to_rcistr(rci_command[index], tag);
}

static idigi_bool_t is_query_command(rci_command_t const command);
static idigi_bool_t is_set_command(rci_command_t const command)
{
    idigi_bool_t const is_set = idigi_bool((command == rci_command_set_setting) || (command == rci_command_set_state));

    ASSERT(is_set || is_query_command(command));

    return is_set;
}

static idigi_bool_t is_query_command(rci_command_t const command)
{
    idigi_bool_t const is_query = idigi_bool((command == rci_command_query_setting) || (command == rci_command_query_state));

    ASSERT(is_query || is_set_command(command));

    return is_query;
}
