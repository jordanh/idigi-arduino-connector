/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees+
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

#include <pthread.h>
#include "idigi_config.h"
#include "idigi_api.h"
#include "platform.h"

extern void idigiThreadStackInit(void);

extern void print_remote_configurations(void);
extern int ethernet_configuration_init(void);

#if 0 /* already in header? */
extern idigi_callback_status_t app_idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id,
                                    void * const request_data, size_t const request_length,
                                    void * response_data, size_t * const response_length);
#endif

#if 0
void * idigi_run_thread(void * arg)
{
    idigi_status_t status = idigi_success;

    idigiThreadStackInit();

    UNUSED_ARGUMENT(arg);

    APP_DEBUG("idigi_run thread starts\n");

    while (status == idigi_success)
    {
        status = idigi_run((idigi_handle_t)arg);

        if (status == idigi_receive_error ||
            status == idigi_send_error ||
            status == idigi_connect_error)
        {
            /* server may disconnect us, so
             * let's try to reconnect.
             */
            status = idigi_success;
        }
        /* if status is not idigi_success, calling idigi_run
         * again will cause to re-connect to idigi.
         */

    }
    APP_DEBUG("idigi_run thread exits %d\n", status);

    pthread_exit(arg);

}

void * application_run_thread(void * arg)
{
    int status;

    APP_DEBUG("idigi_run thread starts\n");

    status = application_run((idigi_handle_t)arg);

    APP_DEBUG("application_run thread exits %d\n", status);

    pthread_exit(arg);
}

int main (void)
{
    pthread_t idigi_thread;
    pthread_t application_thread;

    idigi_handle_t idigi_handle;

    ethernet_configuration_init();

    APP_DEBUG("Start iDigi\n");
    idigi_handle = idigi_init((idigi_callback_t) app_idigi_callback);
    if (idigi_handle != NULL)
    {
        int ccode;
        ccode = pthread_create(&idigi_thread, NULL, idigi_run_thread, idigi_handle);
        if (ccode != 0)
        {
            APP_DEBUG("thread_create() error on idigi_process_thread %d\n", ccode);
            goto done;
        }

        ccode = pthread_create(&application_thread, NULL, application_run_thread, idigi_handle);
        if (ccode != 0)
        {
            APP_DEBUG("thread_create() error on idigi_process_thread %d\n", ccode);
            goto done;
        }

        pthread_join(idigi_thread, NULL);
        pthread_join(application_thread, NULL);
        APP_DEBUG("iDigi terminated\n");
    }
    else
    {
        APP_DEBUG("unable to initialize iDigi\n");
    }
done:
    return 0;
}

#else

#include "../../../../private/idigi_def.h"
#include "../../../../private/rci_parser.h"

static char parser_input_buffer[64];
static char parser_output_buffer[64];

static rci_service_data_t parser_data = {
    { parser_input_buffer, sizeof parser_input_buffer},
    { parser_output_buffer, sizeof parser_output_buffer}
};

int main (int argc, char * argv[])
{
    rci_status_t status = rci_status_more_input;
    FILE * fp;
    rci_session_t session = rci_session_start;

    if (argc != 2)
    {
        printf("Syntax: ./idigi <xml file>\n");
        goto done;
    }

    ethernet_configuration_init();

    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Unable to open %s file\n", argv[1]);
        goto done;
    }

    while (!feof(fp) && status != rci_status_complete)
    {
        if (status == rci_status_more_input)
        {
            parser_data.input.bytes = fread(parser_input_buffer, sizeof parser_input_buffer[0], asizeof(parser_input_buffer), fp);
        }

        if (session == rci_session_start)
        {
            status = rci_parser(session, &parser_data);
        }
        else
        {
            session = rci_session_active;
            status = rci_parser(session);
        }
            
        switch (status)
        {

        case rci_status_busy:
            break;

        case rci_status_more_input:
            parser_data.input.data = parser_input_buffer;
            parser_data.input.bytes = sizeof parser_input_buffer;
            break;

        case rci_status_complete:
        case rci_status_flush_output:
            printf("\"%.*s\"\n", parser_data.output.bytes, parser_data.output.data);
            parser_data.output.data = parser_output_buffer;
            parser_data.output.bytes = sizeof parser_output_buffer;
            break;

        case rci_status_internal_error:
            printf("Broken\n");
            goto done;
        case rci_status_error:
            printf("Cancel Message\n");
            goto done;
        default:
            assert(0);
            cstr_equals_buffer(NULL, NULL, 0);
            break;

        }

    }

done:
    return 0;
}
#endif
