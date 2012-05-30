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

static char const * const rci_command[] = {
    RCI_SET_SETTING,
    RCI_SET_STATE,
    RCI_QUERY_SETTING,
    RCI_QUERY_STATE
};

static rci_command_t find_rci_command(rci_string_t const * const tag)
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

static void set_rci_command_tag(rci_command_t const command, rci_string_t * const tag)
{
    int const index = (command - rci_command_set_setting);
    
    ASSERT((command >= rci_command_set_setting) && (command <= rci_command_query_state));
    
    cstr_to_rci_string(rci_command[index], tag);
}

