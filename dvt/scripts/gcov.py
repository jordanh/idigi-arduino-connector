POST_INCLUDE_DATA = """#include <signal.h>
#include <stdlib.h>

void __gcov_flush(void);
void gcov_handler(int signum)
{
    APP_DEBUG("received signal %d to dump coverage data.\\n", signum);
    __gcov_flush(); /* dump coverage data on receiving SIGUSR1 */
    exit(0); /* exit cleanly after flushing */
}

void add_gcov_signal(void)
{
    struct sigaction new_action, old_action;
    
    /* setup signal hander */
    new_action.sa_handler = gcov_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGUSR1, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction (SIGUSR1, &new_action, NULL);
}
"""

SIGNAL_INJECT_DATA = """    add_gcov_signal();
"""

def add_lib(makefile):
    """ Adds -lgcov to all lines starting with 'LIB' in a Makefile to link 
    gcov. 

    :param makefile: Makefile to add -lgcov to."""
    f = open(makefile, 'r')
    lines = f.readlines()
    f.close()

    i_data = ""
    for line in lines:
        try:
            if line.find('LIB') == 0:
                # LIB found, lets add -lgcov to it.
                line = line[:-1] + ' -lgcov\n'
        finally:
            i_data += line

    f = open(makefile, 'w')
    f.write(i_data)
    f.close()

def instrument(source_file):
    """Instruments a C source file containing a main method with a signal 
    handler for USR1 to trigger a gcov flush.  This is done in place.

    :param source_file: The file to instrument.
    """
    f = open(source_file, 'r')
    lines = f.readlines()
    f.close()

    i_data = ""
    include_found = False
    include_processed = False
    main_found = False
    main_processed = False
    for line in lines:
        try:
            if not include_processed:
                if not include_found:
                    if line.startswith("#include"):
                        include_found = True
                        continue
                if include_found:
                    # If we find another include, continue.
                    if line.startswith("#include"):
                        continue
                    # Otherwise, add the signal handler code inline.
                    else:
                        i_data += POST_INCLUDE_DATA
                        include_processed = True
                        continue
            elif not main_processed:
                if not main_found:
                    if line.find("main") != -1:
                        main_found = True
                    else:
                        continue
                if main_found and not main_processed:
                    if line.find("main") == -1 and line.find("{") == -1:
                        i_data += SIGNAL_INJECT_DATA
                        main_processed=True
                        continue
        finally:
            i_data += line

    f = open(source_file, 'w')
    f.write(i_data)
    f.close()
