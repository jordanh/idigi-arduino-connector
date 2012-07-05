
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

#define VALUE_TO_STRING(value)   # value
#define MACRO_TO_STRING(macro)   VALUE_TO_STRING(macro)

#define TERMINATE_TEST_FILE "terminate_test.txt"
#define TIMING_FILE     "./dvt/timing_usage.html"
//#define TIMING_FILE     "../../timing_usage.html"

#define TIMING_TABLE_HTML_OPEN_BODY "\
<html>\n\
  <head>\n\
    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\"/>\n\
    <title>iDigi Connector API Timing</title>\n\
  </head>\n\
  <body>\n\
 \n\
    <h1>iDigi Connector API Timing</h1>\n\
    <p>It uses clock_gettime() to obtain the iDigi Connector API execution time.\n\
       It calculates the amount of time that API takes. It excludes the time spent in the callbacks.</p>\n\
    <p>The following tables show the API call index and its timing.\n\
       <ul><li>API call index = Sequence order/index of calling the API. Note that it only displays the time only if \
the difference between the time and the prevous time is greater than 1000 nsec</li>\n\
           <li>Timing = Amount of time that it takes to execute the iDigi Connector API in nanoseconds</li></ul></p>\n\
    "
#define TIMING_TABLE_HTML_TABLE_TITLE   "    <hr><p><h2>%s</h2></p>\n"

#define TIMING_TABLE_HTML_OPEN_TABLE    "\
    <table border=\"2\" cellspacing=\"1\" cellpadding=\"0\" width=\"500\">\n\
      <thead>\n\
        <tr bgcolor=\"#C8C8C8\"><th>API call index</th><th>Timing</th></tr>\n\
      </thead>\n\
      <tbody>\n"

#define TIMING_TABLE_HTML_CLOSE_TABLE "\
      </tbody>\n\
    </table><br>\n"


#define TIMING_TABLE_HTML_MAX_OVERALL_TIMING_ROW        "       <p style=\"color:red;font-size:20px;\">The maximum idigi_step API execution time = %lu sec %ld nsec</p>\n"

#define TIMING_TABLE_HTML_TIMING_ROW                    "         <tr><td>%d</td><td align=\"right\">%ld</td></tr>\n"
#define TIMING_TABLE_HTML_TIMING_GREATER_SECOND_ROW     "         <tr bgcolor=\"#8A0808\" ><td>%d</td><tdalign=\"right\">%lu (sec)</td></tr>\n"
#define TIMING_TABLE_HTML_DESC_ROW_OPEN                 "         <tr><td bgcolor=\"#C0C0C0\" colspan=\"2\">"
#define TIMING_TABLE_HTML_DESC_ROW_CLOSE                "         </td></tr>\n"
#define TIMING_TABLE_HTML_CLOSE_BODY                    TIMING_TABLE_HTML_CLOSE_TABLE TIMING_TABLE_HTML_MAX_OVERALL_TIMING_ROW "       </body></html>\n"
#define TIMING_TABLE_HTML_MAX_TIMING_ROW                "         <tr bgcolor=\"#FFFF33\"><td>Max timing</td> <td align=\"right\">%lu sec %ld nsec</td></tr>\n"
#define TIMING_TABLE_HTML_TOTAL_TIMING_ROW              "         <tr bgcolor=\"#FFFF33\"><td>Total timing</td> <td align=\"right\">%lu sec %ld nsec</td></tr>\n"

//#define TIMING_TABLE_HTML_NEW_TABLE     TIMING_TABLE_HTML_CLOSE_TABLE TIMING_TABLE_HTML_OPEN_TABLE

extern bool start_put_file;

extern idigi_handle_t idigi_handle;
extern int firmware_download_started;
extern struct timespec callback_time;
extern unsigned int callback_count;
extern unsigned int startWritingCount;


extern idigi_callback_status_t app_data_service_handler(idigi_data_service_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);
extern idigi_callback_status_t app_firmware_handler(idigi_firmware_request_t const request,
                                                  void const * const request_data, size_t const request_length,
                                                  void * response_data, size_t * const response_length);

extern idigi_callback_status_t app_remote_config_handler(idigi_remote_config_request_t const request,
                                                      void const * const request_data, size_t const request_length,
                                                      void * response_data, size_t * const response_length);

extern void get_time(struct timespec * the_time);

extern void total_elapsed_time(struct timespec * the_time, struct timespec const start, struct timespec const end);

extern void send_put_request(idigi_handle_t handle);

extern int application_step(idigi_handle_t handle);

extern void open_timing_table(char const * const label, char const * const desc);
extern void close_timing_table(char const * const label, char const * const desc);
extern void writing_timing_description(char const * const desc, char const * const content);


#endif /* APPLICATION_H_ */
