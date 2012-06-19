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

static void rci_generate_error(rci_t * const rci)
{
    switch (rci->error.state)
    {
    case rci_error_state_none:
        rci->error.state = rci_error_state_error_open;
        if (rci->input.command != rci_command_unseen)
        {
            goto done;
        }

        prep_rci_reply_data(rci);
        break;

    case rci_error_state_error_open:
        cstr_to_rcistr(RCI_ERROR, &rci->output.tag);
        add_numeric_attribute(&rci->output.attribute, RCI_ID, rci->shared.response.error_id);
        rci->output.type = rci_output_type_start_tag;

#if defined RCI_PARSER_USES_ERROR_DESCRIPTIONS
        rci->error.state = rci_error_state_error_description;
#else
        rci->error.state = (rci->shared.response.error_hint == NULL) ? rci_error_state_error_close : rci_error_state_error_hint;
#endif
        break;

#if defined RCI_PARSER_USES_ERROR_DESCRIPTIONS
    case rci_error_state_error_description:
        clear_attributes(&rci->output.attribute);

        cstr_to_rcistr(RCI_DESC, &rci->output.tag);
        cstr_to_rcistr(rci->error.description, &rci->output.content);

        rci->output.type = rci_output_type_three_tuple;

        rci->error.state = (rci->shared.response.element_data.error_hint == NULL) ? rci_error_state_error_close : rci_error_state_error_hint;
        break;
#endif

    case rci_error_state_error_hint:
        clear_attributes(&rci->output.attribute);

        cstr_to_rcistr(RCI_HINT, &rci->output.tag);
        str_to_rcistr(rci->shared.response.element_data.error_hint, &rci->output.content);

        rci->output.type = rci_output_type_three_tuple;
        break;

    case rci_error_state_error_close:
        cstr_to_rcistr(RCI_ERROR, &rci->output.tag);
        rci->output.type = rci_output_type_end_tag;

        switch (rci->input.command)
        {
        case rci_command_unseen:
        case rci_command_unknown:
            rci->error.state = rci_error_state_reply_close;
            break;
        default:
            if (rci->shared.request.element.id != INVALID_ID)
                rci->error.state = rci_error_state_element_close;
            else if (rci->shared.request.group.id != INVALID_ID)
                rci->error.state = rci_error_state_group_close;
            else
                rci->error.state = rci_error_state_command_close;
        }
        break;

    case rci_error_state_element_close:
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);
            idigi_group_element_t const * const group_element = (group->elements.data + rci->shared.request.element.id);

            cstr_to_rcistr(group_element->name, &rci->output.tag);
            rci->shared.request.element.id = INVALID_ID;

            rci->error.state = rci_error_state_group_close;
        }
        break;

    case rci_error_state_group_close:
        {
            idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
            idigi_group_t const * const group = (table->groups + rci->shared.request.group.id);

            trigger_rci_callback(rci, idigi_remote_config_group_end);

            cstr_to_rcistr(group->name, &rci->output.tag);
            rci->shared.request.group.id = INVALID_ID;

            rci->error.state = rci_error_state_command_close;
        }
        break;

    case rci_error_state_command_close:
        {
            trigger_rci_callback(rci, idigi_remote_config_action_end);

            set_rci_command_tag(rci->input.command, &rci->output.tag);
            rci->error.state = rci_error_state_reply_close;
        }
        break;

    case rci_error_state_reply_close:
        trigger_rci_callback(rci, idigi_remote_config_session_end);

        rci->input.command = rci_command_unseen;

        cstr_to_rcistr(RCI_REPLY, &rci->output.tag);
        rci->error.state = rci_error_state_none;
        state_call_return(rci, rci_parser_state_output, rci_parser_state_input);
        goto done;
        break;
    }

    state_call(rci, rci_parser_state_output);

done:
    return;
}


