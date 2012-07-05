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
#include <time.h>
#include <stdbool.h>
#include "idigi_api.h"
#include "platform.h"
#include <unistd.h>
#include <errno.h>
#include "application.h"
#include "idigi_config.h"

idigi_handle_t idigi_handle;

//clock_t        idigi_clock;
//clock_t        cb_clock = 0;

//    clockid_t clock_id = CLOCK_REALTIME;
clockid_t clock_id;

struct timespec callback_time = {0};
unsigned int callback_count = 0;
unsigned int startWritingCount = 1;

static FILE * file_fd = NULL;
static int idigi_count = 0;
static struct timespec idigi_time_max = {0};
static struct timespec idigi_time = {0};
static struct timespec idigi_time_total = {0};
static struct timespec idigi_time_overall_max = {0};
static long const nanosecond = 1000000000;

void get_time(struct timespec * the_time)
{
    if (clock_gettime(clock_id, the_time) != 0)
    {
        APP_DEBUG("get_time: clock_gettime failed errno = %d\n", errno);
    }
}

void total_elapsed_time(struct timespec * the_time, struct timespec const start, struct timespec const end)
{
    time_t elapsed_sec = end.tv_sec - start.tv_sec;
    long elapsed_nsec;

    if (end.tv_nsec >= start.tv_nsec)
    {
        elapsed_nsec = end.tv_nsec - start.tv_nsec;
    }
    else
    {
        elapsed_nsec = end.tv_nsec + nanosecond - start.tv_nsec;
        elapsed_sec--;
    }

    if (elapsed_nsec >= nanosecond)
    {
        APP_DEBUG("total_elapsed_time: ASSERT(elapsed_nsec < nanoseconde) %ld\n", elapsed_nsec);
        APP_DEBUG("total_elapsed_time: start tv_sec = %lu tv_nsec=%ld\n", start.tv_sec, start.tv_nsec);
        APP_DEBUG("total_elapsed_time: end tv_sec = %lu tv_nsec=%ld\n", end.tv_sec, end.tv_nsec);
    }

    ASSERT(elapsed_nsec < nanosecond);

    the_time->tv_nsec += elapsed_nsec;
    the_time->tv_sec += elapsed_sec;

}
void open_timing_table(char const * const label, char const * const desc)
{

    if (file_fd != NULL)
    {
        if (startWritingCount==0)
        {
            fprintf(file_fd, TIMING_TABLE_HTML_MAX_TIMING_ROW, idigi_time_max.tv_sec, idigi_time_max.tv_nsec);
            fprintf(file_fd, TIMING_TABLE_HTML_TOTAL_TIMING_ROW, idigi_time_total.tv_sec, idigi_time_total.tv_nsec);
            fprintf(file_fd, TIMING_TABLE_HTML_CLOSE_TABLE);
            fprintf(file_fd, TIMING_TABLE_HTML_OPEN_TABLE);


            writing_timing_description(label, desc);

            idigi_count = 0;
            idigi_time_max.tv_sec = 0;
            idigi_time_max.tv_nsec = 0;
            idigi_time_total.tv_sec = 0;
            idigi_time_total.tv_nsec = 0;
        }
        else
        {
            writing_timing_description(label, desc);
        }

        startWritingCount++;
    }
}

void close_timing_table(char const * const label, char const * const desc)
{
    if (file_fd != NULL)
    {
       writing_timing_description(label, desc);
        startWritingCount--;
    }
}

void writing_timing_description(char const * const desc, char const * const content)
{
    if (file_fd != NULL)
    {

        fprintf(file_fd, TIMING_TABLE_HTML_DESC_ROW_OPEN);

        fprintf(file_fd, "%s", desc);
        if (content != NULL)
        {
            fprintf(file_fd, "%s", content);
        }
        fprintf(file_fd, TIMING_TABLE_HTML_DESC_ROW_OPEN);

        fprintf(file_fd, TIMING_TABLE_HTML_DESC_ROW_CLOSE);
    }
}
#if 0
static unsigned int timing_percentage(struct timespec elapsed_time)
{
    unsigned int percentage;

    percentage = (elapsed_time.tv_nsec%nanosecond)/1000;

    if (elapsed_time.tv_sec > 0)
    {
        percentage = (unsigned int)elapsed_time.tv_sec * 100;
    }

    return percentage;
}

static void write_timing(struct timespec const elapsed)
{

    unsigned int percentage = timing_percentage(elapsed);

    if (startWritingCount > 0)
    {
        if (elapsed.tv_sec > 0)
            fprintf(file_fd, TIMING_TABLE_HTML_TIMING_GREATER_SECOND_ROW, ++idigi_count, 100, elapsed.tv_sec);
        else
            fprintf(file_fd, TIMING_TABLE_HTML_TIMING_ROW, ++idigi_count, percentage, elapsed.tv_nsec);
    }
}
#else

static void write_timing(struct timespec const elapsed_time)
{

    if (file_fd != NULL)
    {
        static struct timespec delta_elapsed_time = {0};

        /* We only want to log the timing if the difference between the current and previous time is > 1000 */
        bool const diff = ((elapsed_time.tv_nsec/1000) != (delta_elapsed_time.tv_nsec/1000)) || (elapsed_time.tv_sec != delta_elapsed_time.tv_sec);

        if (diff)
        {
    //        if (startWritingCount > 0)
            {
                if (elapsed_time.tv_sec > 0)
                    fprintf(file_fd, TIMING_TABLE_HTML_TIMING_GREATER_SECOND_ROW, ++idigi_count, elapsed_time.tv_sec);
                else
                    fprintf(file_fd, TIMING_TABLE_HTML_TIMING_ROW, ++idigi_count, elapsed_time.tv_nsec);
            }
            delta_elapsed_time = elapsed_time;
#if 0
            APP_DEBUG( "%-lu\t%-ld\t%-lu\t%-ld\t%-lu\t%-ld\t%-d\n", idigi_time.tv_sec, idigi_time.tv_nsec,
                                                             callback_time.tv_sec, callback_time.tv_nsec,
                                                             elapsed_time.tv_sec, elapsed_time.tv_nsec,
                                                             callback_count);
#endif
        }
        else
            /* Still keep track number of the API that has been called */
            idigi_count++;

        /* Record max timing */
        if (((elapsed_time.tv_sec == idigi_time_max.tv_sec) && (elapsed_time.tv_nsec > idigi_time_max.tv_nsec)) ||
            (elapsed_time.tv_sec > idigi_time_max.tv_sec))
        {
            idigi_time_max.tv_sec = elapsed_time.tv_sec;
            idigi_time_max.tv_nsec = elapsed_time.tv_nsec;
        }

        if (((elapsed_time.tv_sec == idigi_time_overall_max.tv_sec) && (elapsed_time.tv_nsec > idigi_time_overall_max.tv_nsec)) ||
            (elapsed_time.tv_sec > idigi_time_overall_max.tv_sec))
        {
            idigi_time_overall_max.tv_sec = elapsed_time.tv_sec;
            idigi_time_overall_max.tv_nsec = elapsed_time.tv_nsec;
        }

        /* Record total timing */
        idigi_time_total.tv_sec += elapsed_time.tv_sec;
        idigi_time_total.tv_nsec += elapsed_time.tv_nsec;
        if (idigi_time_total.tv_nsec >= nanosecond)
        {
            idigi_time_total.tv_nsec -= nanosecond;
            idigi_time_total.tv_sec++;
        }

    }
}
#endif

int main (void)
{
    int rc = EXIT_FAILURE;
    idigi_status_t status = idigi_success;
    struct timespec start_time;
    struct timespec end_time;
    bool timing_test_start = true;

    {
        struct timespec time_spec;

        pthread_t this_thread = pthread_self();

        if (pthread_getcpuclockid(this_thread, &clock_id) != 0)
        {
            APP_DEBUG("main: pthread_getcpucloackid failed errno = %d\n", errno);
            goto done;
        }

        if (clock_getres(clock_id, &time_spec) != 0)
        {
            APP_DEBUG("main: clock_getres failed errno = %d\n", errno);
            goto done;
        }

        APP_DEBUG("main: clock_getres tv_sec = %lu tv_nsec=%ld\n", time_spec.tv_sec, time_spec.tv_nsec);
    }

    if (timing_test_start)
    {
        char const * filename = TIMING_FILE;

        file_fd = fopen(filename, "w");
        if (file_fd == NULL)
        {
            APP_DEBUG("main: fopen failed on %s file\n", filename);
            goto done;
        }

        APP_DEBUG("main: fopen %s file\n", filename);
    }


    APP_DEBUG("main: Starting iDigi\n");
    get_time(&start_time);

    idigi_handle = idigi_init(app_idigi_callback);

    get_time(&end_time);
    if (idigi_handle == NULL)
    {
        APP_DEBUG("main: idigi_init() failed\n");
        goto done;
    }


    if (file_fd != NULL)
    {
        total_elapsed_time(&idigi_time, start_time, end_time);
        fprintf(file_fd, TIMING_TABLE_HTML_OPEN_BODY);
        fprintf(file_fd, TIMING_TABLE_HTML_TABLE_TITLE, "idigi_init() API Timing");
        fprintf(file_fd, TIMING_TABLE_HTML_OPEN_TABLE);

        {
            /* Need to subtract callback's time from idigi API time */
            struct timespec elapsed_time = {0};

            total_elapsed_time(&elapsed_time, callback_time, idigi_time);

            write_timing(elapsed_time);
        }

        startWritingCount = 0;

        fprintf(file_fd, TIMING_TABLE_HTML_MAX_TIMING_ROW, idigi_time_max.tv_sec, idigi_time_max.tv_nsec);
        fprintf(file_fd, TIMING_TABLE_HTML_TOTAL_TIMING_ROW, idigi_time_total.tv_sec, idigi_time_total.tv_nsec);
        fprintf(file_fd, TIMING_TABLE_HTML_CLOSE_TABLE);

        fprintf(file_fd, TIMING_TABLE_HTML_TABLE_TITLE, "idigi_step() API Timing");
        fprintf(file_fd, TIMING_TABLE_HTML_OPEN_TABLE);
        idigi_count = 0;
        idigi_time_max.tv_sec = 0;
        idigi_time_max.tv_nsec = 0;
        idigi_time_total.tv_sec = 0;
        idigi_time_total.tv_nsec = 0;
    }

  /* Sample program control loop */
    for (;;)
    {
        /*
         * Example of using idigi_step(), we run a portion of the iDigi Connector then run
         * a portion of the users application.
         */
        idigi_time.tv_sec = 0;
        idigi_time.tv_nsec = 0;
        callback_time.tv_sec = 0;
        callback_time.tv_nsec = 0;
        callback_count = 0;

        get_time(&start_time);

        status = idigi_step(idigi_handle);

        get_time(&end_time);

        if (file_fd != NULL)
        {
            struct timespec elapsed_time = {0};

            total_elapsed_time(&idigi_time, start_time, end_time);

            /* We need to subtract the callback's time */
            total_elapsed_time(&elapsed_time, callback_time, idigi_time);

            write_timing(elapsed_time);

        }

        if (status != idigi_success && status != idigi_receive_error &&
            status != idigi_send_error && status != idigi_connect_error)
        {
            /* We exclude idigi_receive_error, idigi_send_error, &
             * idigi_connect_error. We want to reconnect iDigi
             * even if idigi_step returns error in receive, send, or connect.
             */
            if (status != idigi_configuration_error)
                APP_DEBUG("main: idigi_step() failed\n");
            else
                APP_DEBUG("main: idigi_step aborted by callback\n");

            if (file_fd != NULL)
            {
                APP_DEBUG("main: done writing %s file\n", TIMING_FILE);

                fprintf(file_fd, TIMING_TABLE_HTML_MAX_TIMING_ROW, idigi_time_max.tv_sec, idigi_time_max.tv_nsec);
                fprintf(file_fd, TIMING_TABLE_HTML_TOTAL_TIMING_ROW, idigi_time_total.tv_sec, idigi_time_total.tv_nsec);
                fprintf(file_fd, TIMING_TABLE_HTML_CLOSE_BODY, idigi_time_overall_max.tv_sec, idigi_time_overall_max.tv_nsec);

            //    fflush(file_fd);
                fclose(file_fd);
                file_fd = NULL;
                APP_DEBUG("main: Complete timing test\n");
                timing_test_start = false;
            }
            usleep(10000);

        }

        application_step(idigi_handle);
    }

done:
    if (file_fd != NULL) fclose(file_fd);

    return rc;
}

