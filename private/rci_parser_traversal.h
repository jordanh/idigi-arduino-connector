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

/* We are only called on set_setting and set_state for a single element (with no content). */
static idigi_bool_t one_element_set(rci_t const * const rci)
{
    idigi_bool_t const is_set = is_set_command(rci->input.command);

    if (is_set)
    {
        ASSERT((rci->traversal.state == rci_traversal_state_one_element_start) || (rci->traversal.state == rci_traversal_state_one_element_data));
    }

    return is_set;
}
static void rci_traverse_data(rci_t * const rci)
{
    idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);

    if (pending_rci_callback(rci))
    {
        idigi_remote_group_response_t const * const response = &rci->shared.response;

        if (!rci_callback(rci))
            goto done;

        if (response->error_id != idigi_success)
        {
            rci_group_error(rci, response->error_id, response->element_data.error_hint);
            goto done;
        }

        {
            idigi_remote_config_request_t const remote_config_request = rci->callback.request.remote_config_request;

            switch (remote_config_request)
            {
            UNHANDLED_CASES_ARE_NEEDED;
            case idigi_remote_config_session_start:
            case idigi_remote_config_session_end:
                rci->status = rci_status_internal_error;
                ASSERT(idigi_false);
                goto done;
                break;
            case idigi_remote_config_action_start:
                rci->traversal.state = (table->count != 0) ? rci_traversal_state_all_groups_group_start : rci_traversal_state_all_groups_end;
                break;
            case idigi_remote_config_group_start:
                set_element_id(rci, 0);

                switch (rci->traversal.state)
                {
                UNHANDLED_CASES_ARE_INVALID
                case rci_traversal_state_all_groups_group_start:    rci->traversal.state = rci_traversal_state_all_groups_element_start;    break;
                case rci_traversal_state_one_group_start:           rci->traversal.state = rci_traversal_state_one_group_element_start;     break;
                case rci_traversal_state_indexed_group_start:       rci->traversal.state = rci_traversal_state_indexed_group_element_start; break;
                }
                break;
            case idigi_remote_config_group_process:
                switch (rci->traversal.state)
                {
                UNHANDLED_CASES_ARE_INVALID
                case rci_traversal_state_all_groups_element_start:      rci->traversal.state = rci_traversal_state_all_groups_element_data;     break;
                case rci_traversal_state_one_group_element_start:       rci->traversal.state = rci_traversal_state_one_group_element_data;      break;
                case rci_traversal_state_indexed_group_element_start:   rci->traversal.state = rci_traversal_state_indexed_group_element_data;  break;
                case rci_traversal_state_one_element_start:             rci->traversal.state = rci_traversal_state_one_element_data;            break;
                case rci_traversal_state_one_element_data:              rci->traversal.state = rci_traversal_state_one_element_end;             break;
                }
                break;

            case idigi_remote_config_group_end:
                {
                    idigi_group_t const * const group = (table->groups + get_group_id(rci));

                    rci->output.type = rci_output_type_end_tag;
                    cstr_to_rcistr(group->name, &rci->output.tag);
                }
                switch (rci->traversal.state)
                {
                UNHANDLED_CASES_ARE_INVALID
                case rci_traversal_state_all_groups_group_end:  rci->traversal.state = rci_traversal_state_all_groups_group_advance;    break;
                case rci_traversal_state_one_group_end:         rci->traversal.state = rci_traversal_state_one_group_advance;           break;
                case rci_traversal_state_indexed_group_end:     rci->traversal.state = rci_traversal_state_indexed_group_advance;       break;
                }
                state_call(rci, rci_parser_state_output);
                goto done;
                break;
            case idigi_remote_config_action_end:
                rci->output.type = rci_output_type_end_tag;
                set_rci_command_tag(rci->input.command, &rci->output.tag);

                rci->input.command = rci_command_header;
                state_call_return(rci, rci_parser_state_output, rci_parser_state_input);
                goto action_complete;
                break;
            case idigi_remote_config_session_cancel:
                rci->status = rci_status_error;
                goto done;
                break;
            }
        }
    }

    switch (rci->traversal.state)
    {
    UNHANDLED_CASES_ARE_NEEDED

    case rci_traversal_state_none:
        rci_global_error(rci, idigi_rci_error_parser_error, RCI_NO_HINT);
        goto done;
        break;

    case rci_traversal_state_all_groups_start:

        ASSERT(!have_group_id(rci));
        ASSERT(!have_group_index(rci));
        ASSERT(!have_element_id(rci));

        rci->output.type = rci_output_type_start_tag;
        set_rci_command_tag(rci->input.command, &rci->output.tag);

        trigger_rci_callback(rci, idigi_remote_config_action_start);
        break;

    case rci_traversal_state_all_groups_group_start:
        if (!have_group_id(rci))
        {
            set_group_id(rci, 0);
        }
        /* no break; */

    case rci_traversal_state_one_group_start:
        if (!have_group_index(rci))
        {
            set_group_index(rci, 1);
        }
        /* no break; */

    case rci_traversal_state_indexed_group_start:
        {
            idigi_group_t const * const group = (table->groups + get_group_id(rci));

            rci->output.type = rci_output_type_start_tag;
            cstr_to_rcistr(group->name, &rci->output.tag);
            set_numeric_attribute(&rci->output.attribute, RCI_INDEX, get_group_index(rci));
        }

        trigger_rci_callback(rci, idigi_remote_config_group_start);
        break;

    case rci_traversal_state_all_groups_element_start:
    case rci_traversal_state_one_group_element_start:
    case rci_traversal_state_indexed_group_element_start:
    case rci_traversal_state_one_element_start:
        {
            idigi_group_element_t const * const element = get_current_element(rci);

            if (is_query_command(rci->input.command) && (element->access == idigi_element_access_write_only))
            {
                switch (rci->traversal.state)
                {
                UNHANDLED_CASES_ARE_INVALID
                case rci_traversal_state_all_groups_element_start:      rci->traversal.state = rci_traversal_state_all_groups_element_end;     break;
                case rci_traversal_state_one_group_element_start:       rci->traversal.state = rci_traversal_state_one_group_element_end;      break;
                case rci_traversal_state_indexed_group_element_start:   rci->traversal.state = rci_traversal_state_indexed_group_element_end;  break;
                case rci_traversal_state_one_element_start:             rci->traversal.state = rci_traversal_state_one_element_end;            break;
                }
                goto done;
            }

            rci->output.type = rci_output_type_start_tag;
            cstr_to_rcistr(element->name, &rci->output.tag);

            rci->shared.request.element.type = element->type;
            rci->shared.request.element.value = NULL;

            if (one_element_set(rci))
            {
                /* Doing a set, do callback after we have issued the start tag. */
                rci->traversal.state = rci_traversal_state_one_element_data;
            }
            else
            {
                trigger_rci_callback(rci, idigi_remote_config_group_process);
            }
        }
        break;

    case rci_traversal_state_all_groups_element_data:
    case rci_traversal_state_one_group_element_data:
    case rci_traversal_state_indexed_group_element_data:
    case rci_traversal_state_one_element_data:
        if (one_element_set(rci))
        {
            static char content;

            content = '<';
            init_rcistr(&rci->shared.string.content, &content, 0);
            rci_handle_content(rci);
        }
        else
        {
            rci_output_content(rci);

            switch (rci->traversal.state)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_traversal_state_all_groups_element_data:       rci->traversal.state = rci_traversal_state_all_groups_element_end;      break;
            case rci_traversal_state_one_group_element_data:        rci->traversal.state = rci_traversal_state_one_group_element_end;       break;
            case rci_traversal_state_indexed_group_element_data:    rci->traversal.state = rci_traversal_state_indexed_group_element_end;   break;
            case rci_traversal_state_one_element_data:              rci->traversal.state = rci_traversal_state_one_element_end;             break;
            }
        }
        goto done;
        break;

    case rci_traversal_state_all_groups_element_end:
    case rci_traversal_state_one_group_element_end:
    case rci_traversal_state_indexed_group_element_end:
    case rci_traversal_state_one_element_end:
        {
            idigi_group_element_t const * const element = get_current_element(rci);
            idigi_bool_t const reading_write_only = idigi_bool(is_query_command(rci->input.command) && (element->access == idigi_element_access_write_only));

            rci->output.type = rci_output_type_end_tag;
            cstr_to_rcistr(element->name, &rci->output.tag);

            switch (rci->traversal.state)
            {
            UNHANDLED_CASES_ARE_INVALID
            case rci_traversal_state_all_groups_element_end:
            case rci_traversal_state_one_group_element_end:
            case rci_traversal_state_indexed_group_element_end:
                {
                    idigi_group_t const * const group = (table->groups + get_group_id(rci));
                    unsigned int const next_element = (get_element_id(rci) + 1);

                    if (next_element == group->elements.count)
                    {
                        invalidate_element_id(rci);

                        switch (rci->traversal.state)
                        {
                        UNHANDLED_CASES_ARE_INVALID
                        case rci_traversal_state_all_groups_element_end:    rci->traversal.state = rci_traversal_state_all_groups_group_end;    break;
                        case rci_traversal_state_one_group_element_end:     rci->traversal.state = rci_traversal_state_one_group_end;           break;
                        case rci_traversal_state_indexed_group_element_end: rci->traversal.state = rci_traversal_state_indexed_group_end;       break;
                        }
                    }
                    else
                    {
                        set_element_id(rci, next_element);

                        switch (rci->traversal.state)
                        {
                        UNHANDLED_CASES_ARE_INVALID
                        case rci_traversal_state_all_groups_element_end:    rci->traversal.state = rci_traversal_state_all_groups_element_start;    break;
                        case rci_traversal_state_one_group_element_end:     rci->traversal.state = rci_traversal_state_one_group_element_start;     break;
                        case rci_traversal_state_indexed_group_element_end: rci->traversal.state = rci_traversal_state_indexed_group_element_start; break;
                        }
                    }
                }
                break;
            case rci_traversal_state_one_element_end:
                invalidate_element_id(rci);
                if (reading_write_only)
                {
                    state_call(rci, rci_parser_state_input);
                }
                else
                {
                    state_call_return(rci, rci_parser_state_output, rci_parser_state_input);
                }
                goto element_complete;
                break;
            }

            if (reading_write_only)
            {
                goto done;
            }
        }
        break;

    case rci_traversal_state_all_groups_group_end:
    case rci_traversal_state_one_group_end:
    case rci_traversal_state_indexed_group_end:
        trigger_rci_callback(rci, idigi_remote_config_group_end);
        goto done;
        break;

    case rci_traversal_state_all_groups_group_advance:
    case rci_traversal_state_one_group_advance:
    case rci_traversal_state_indexed_group_advance:
        switch (rci->traversal.state)
        {
        UNHANDLED_CASES_ARE_INVALID
        case rci_traversal_state_all_groups_group_advance:
        case rci_traversal_state_one_group_advance:
            {
                idigi_group_t const * const group = (table->groups + get_group_id(rci));

                /* group indicies are 1-based */
                if (get_group_index(rci) == group->instances)
                {
                    if (rci->traversal.state == rci_traversal_state_one_group_advance)
                    {
                        goto group_complete;
                    }

                    ASSERT(rci->traversal.state == rci_traversal_state_all_groups_group_advance);
                    {
                        unsigned int const next_group = (get_group_id(rci) + 1);

                        if (next_group == table->count)
                        {
                            rci->traversal.state = rci_traversal_state_all_groups_end;
                        }
                        else
                        {
                            set_group_id(rci, next_group);
                            set_group_index(rci, 1);
                            rci->traversal.state = rci_traversal_state_all_groups_group_start;
                        }
                    }
                }
                else
                {
                    increment_group_index(rci);
                    rci->traversal.state = (rci->traversal.state == rci_traversal_state_all_groups_group_advance) ? rci_traversal_state_all_groups_group_start : rci_traversal_state_one_group_start;
                }
            }
            goto done;
            break;

        case rci_traversal_state_indexed_group_advance:
            goto group_complete;
            break;
        }
        break;

    case rci_traversal_state_all_groups_end:
        invalidate_group_id(rci);
        invalidate_group_index(rci);
        invalidate_element_id(rci);

        trigger_rci_callback(rci, idigi_remote_config_action_end);
        goto done;
        break;
    }

    state_call(rci, rci_parser_state_output);
    goto done;

group_complete:
    invalidate_group_id(rci);
    invalidate_group_index(rci);
    state_call(rci, rci_parser_state_input);

action_complete:
    ASSERT(!have_group_id(rci));
    ASSERT(!have_group_index(rci));

element_complete:
    ASSERT(!have_element_id(rci));

    rci->traversal.state = rci_traversal_state_none;

done:
    return;
}


