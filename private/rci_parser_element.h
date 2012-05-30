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

#define ELEMENT_VARIABLE(rci)       ((rci)->shared.request.element.id)
#define set_element_id(rci, value)  (ELEMENT_VARIABLE(rci) = (value))
#define get_element_id(rci)         (ELEMENT_VARIABLE(rci))
#define have_element_id(rci)        (get_element_id(rci) != INVALID_ID)

static unsigned int find_element_id_in_group(idigi_group_t const * const group, rci_string_t const * const tag)
{
    int result = INVALID_ID;
    size_t i;
    
    for (i = 0; i < group->elements.count; i++)
    {
        if (cstr_equals_rcistr(group->elements.data[i].name, tag))
        {
            result = i;
            break;
        }
    }
    
    return result;
}

static unsigned int find_element_id(rci_t const * const rci, rci_string_t const * const tag)
{
    ASSERT(have_group_id(rci));
 
    return find_element_id_in_group(get_current_group(rci), tag);
}

static idigi_group_element_t const * get_element_in_group(idigi_group_t const * const group, unsigned int const id)
{
    ASSERT(id < group->elements.count);

    return (group->elements.data + id);
}

static idigi_group_element_t const * get_current_element(rci_t const * const rci)
{
    ASSERT(have_group_id(rci));
    ASSERT(have_element_id(rci));
    
    return get_element_in_group(get_current_group(rci), get_element_id(rci));
}

