/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
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

typedef enum
{
    rci_parser_complete,      /* all done */
    rci_parser_busy,          /* user callback returned busy */
    rci_parser_more_input,    /* have output buffer space to process more input */
    rci_parser_flush_output,  /* need more output space, so send out the current buffer */
    rci_parser_error          /* error occured, RCI servrice should inform messaging layer to cancel the session */
} rci_parser_status_t;

typedef struct
{
    char * input_buf;
    size_t input_bytes;
    char * output_buf;
    size_t * output_bytes_ptr;
} rci_parser_data_t;

static rci_parser_status_t rci_parser(rci_parser_data_t * const rci_data);

