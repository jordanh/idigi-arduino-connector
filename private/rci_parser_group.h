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
 
#define GROUP_VARIABLE(rci)         ((rci)->shared.request.group.id)
#define set_group_id(rci, value)    (GROUP_VARIABLE(rci) = (value))
#define get_group_id(rci)           (GROUP_VARIABLE(rci))
#define have_group_id(rci)          (get_group_id(rci) != INVALID_ID)

static void assign_group_id(rci_t * const rci, rci_string_t const * const tag)
{
    idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
    size_t const count = table->count;
    int id = INVALID_ID;
    size_t i;
    
    for (i = 0; i < count; i++)
    {
        if (cstr_equals_rcistr(table->groups[i].name, tag))
        {
            id = i;
            break;
        }
    }
    
    set_group_id(rci, id);
}

static idigi_group_t const * get_current_group(rci_t const * const rci)
{
    idigi_group_table_t const * const table = (idigi_group_table + rci->shared.request.group.type);
    unsigned int const group_id = get_group_id(rci);
    
    ASSERT(have_group_id(rci));
    ASSERT(group_id < table->count);
    
    return (table->groups + group_id);
}
  
 

