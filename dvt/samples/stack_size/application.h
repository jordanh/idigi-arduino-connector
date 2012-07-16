
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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <stdarg.h>
#include "stdbool.h"
#include "idigi_api.h"

#define TERMINATE_TEST_FILE "terminate.test"
#define STACK_SIZE_FILE "./dvt/stacksize.html"
//#define STACK_SIZE_FILE "../../stacksize.html"

extern idigi_handle_t idigi_handle;
extern idigi_status_t idigi_run_thread_status;
extern int firmware_download_started;
extern size_t total_malloc_size;

#define STACKSIZE_HTML_OPEN_BODY "\
<html>\n\
  <head>\n\
    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"/>\n\
    <title>iDigi Connector API Timing</title>\n\
  </head>\n\
  <body>\n\
 \n\
    <h2>iDigi Connector Stack Size</h2>\n\
    <p>It calculates the maximum stack size used in idigi_run() excluding callback's stack size usage.</p>\n\
    "
#define STACKSIZE_HTML_TABLE   "\
<table border=\"2\">\n"

#define STACKSIZE_HTML_TABLE_ROW(format)   "\
    <tr><th align=\"left\">%s</th><td align=\"right\">" format "</td></tr>\n"

#define STACK_SIZE_HTML_OPTION_LIST_OPEN  "    <tr><th align=\"left\">Options</th><td align=\"right\"><dl>"
#define STACK_SIZE_HTML_OPTION_LIST       "    <dd>%s</dd>"
#define STACK_SIZE_HTML_OPTION_LIST_CLOSE "    </dl></td></tr></table>"
#define STACKSIZE_HTML_CLOSE_BODY     "       </body></html>\n"


extern idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
extern idigi_callback_status_t app_firmware_handler(idigi_firmware_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_callback_status_t app_remote_config_handler(idigi_remote_config_request_t const request,
                                                      void const * const request_data, size_t const request_length,
                                                      void * response_data, size_t * const response_length);

extern idigi_status_t send_put_request(idigi_handle_t handle);

extern void clear_stack_size(void);
extern void write_stack_info_action(char const * const action);

#endif /* APPLICATION_H_ */
