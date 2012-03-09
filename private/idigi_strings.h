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

#include "idigi_remote.h"



char const idigi_all_strings[] = {
    ON_STRING_LENGTH, 'o', 'n',
    OFF_STRING_LENGTH, 'o', 'f','f',
    TRUE_STRING_LENGTH, 't', 'r','u', 'e',
    FALSE_STRING_LENGTH, 'f', 'a','l','s','e',

    ERROR_FIELD_NOT_EXIT_LENGTH, 'F', 'i', 'e', 'l', 'd', ' ', 's', 'p', 'e', 'c', 'i', 'f', 'i', 'e', 'd', ' ', 'd', 'o', 'e', 's', ' ', 'n', 'o', 't', ' ', 'e', 'x', 'i', 's', 't',
    ERROR_LOAD_FAILED_LENGTH, 'L', 'o', 'a', 'd', ' ', 'f', 'a', 'i', 'l', 'e', 'd',
    ERROR_SAVE_FAILED_LENGTH, 'S', 'a', 'v', 'e', ' ', 'f', 'a', 'i', 'l', 'e', 'd',
    ERROR_UNKNOWN_VALUE_LENGTH, 'U', 'n', 'k', 'n', 'o', 'w', 'n', ' ', 'v', 'a', 'l', 'u', 'e',
    IDIGI_RCI_REPLY_LENGTH, 'r', 'c', 'i', '_', 'r', 'e', 'p', 'l', 'y',
    IDIGI_VERSION_LENGTH, 'v', 'e', 'r', 's', 'i', 'o', 'n',
    IDIGI_RCI_VERSION_LENGTH, '1', '.', '1',
    IDIGI_QUERY_SETTING_LENGTH, 'q', 'u', 'e', 'r', 'y','_', 's', 'e', 't', 't', 'i', 'n', 'g',
    IDIGI_QUERY_STATE_LENGTH, 'q', 'u', 'e', 'r', 'y', '_', 's', 't', 'a', 't', 'e',
    IDIGI_SET_SETTING_LENGTH, 's', 'e', 't', '_', 's', 'e', 't', 't', 'i', 'n', 'g',
    IDIGI_SET_STATE_LENGTH, 's', 'e', 't', '_', 's', 't', 'a', 't', 'e'
};

