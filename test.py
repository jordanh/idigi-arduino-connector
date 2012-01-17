# Copyright (c) 2011 Digi International Inc., All Rights Reserved
# 
# This software contains proprietary and confidential information of Digi
# International Inc.  By accepting transfer of this copy, Recipient agrees
# to retain this software in confidence, to prevent disclosure to others,
# and to make no use of this software other than that for which it was
# delivered.  This is an published copyrighted work of Digi International
# Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
# prohibited.
# 
# Restricted Rights Legend
#
# Use, duplication, or disclosure by the Government is subject to
# restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
# Technical Data and Computer Software clause at DFARS 252.227-7031 or
# subparagraphs (c)(1) and (2) of the Commercial Computer Software -
# Restricted Rights at 48 CFR 52.227-19, as applicable.
#
# Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# Build the IIK test harness
#
import commands
import sys
import os
import time
import getopt
import signal
import imp

import os, time
from stat import * # ST_SIZE etc

TEMPLATE_TEST_DIR = './dvt/samples/template_test'
TEMPLATE_SCRIPT_DIR = './dvt/cases/dvt_tests/'
TEMPLATE_PLATFORM_DIR = './public/run/platforms/template/'

SAMPLE_DIR='./public/run/samples/'
SAMPLE_SCRIPT_DIR='./dvt/cases/sample_tests/'
SAMPLE_PLATFORM_DIR = './public/run/platforms/linux/'

BASE_SAMPLE_DIR='./public/run/samples/'
BASE_DVT_SRC='./dvt/samples/'
BASE_SCRIPT_DIR='./dvt/cases/'

#indices of test_table
SRC_DIR  = 0
TEST_DIR = 1
TEST_LIST = 2

test_case = 0

STDERR_FILE = 'stderr.txt'
MEMORY_USAGE_FILE = './dvt/memory_usage.txt'

#
# Modify this table when adding a new test.
# 
# Each test has an entry in this table.
# 1. Directory which contains the makefile and the idigi executable, the
#    makefile is build with 'make clean all' and the name of the executable
#    must be idigi
# 2. Directory of the python test script
# 3. List of python test scripts to run
#
#              Directory.                            Script directory                Test Scripts
test_table = [
              [BASE_SAMPLE_DIR+'connect_to_idigi',  BASE_SCRIPT_DIR+'sample_tests', ['test_discovery.py']],
              [BASE_SAMPLE_DIR+'firmware_download', BASE_SCRIPT_DIR+'sample_tests', ['test_firmware.py']],
              [BASE_SAMPLE_DIR+'send_data',         BASE_SCRIPT_DIR+'sample_tests', ['test_send_data.py']],
              [BASE_SAMPLE_DIR+'device_request',    BASE_SCRIPT_DIR+'sample_tests', ['test_device_request.py']],
              [BASE_DVT_SRC+'full_test',            BASE_SCRIPT_DIR+'dvt_tests',    ['test_firmware_errors.py', 'test_device_request.py', 'test_data_service.py']],
              [BASE_DVT_SRC+'data_service',         BASE_SCRIPT_DIR+'admin_tests',  ['test_reboot.py', 'test_redirect.py']]
]

def build_test(dir):
    print '>>>Testing [%s]' % dir

    rc = os.system('cd %s; make clean all' % (dir))
    if rc != 0:
        print "+++FAIL: Build failed dir=[%s]" % dir
        exit(0)

def run_tests(debug_on):
    global test_case    

    for test in test_table:
        src_dir   = test[SRC_DIR]
        test_dir  = test[TEST_DIR]
        test_list = test[TEST_LIST]

        print '>>>Testing [%s]' % src_dir

        build_test(src_dir)

        print '>>>Starting idigi'
        rc = os.system('cd %s;./idigi 2>%s &' % (src_dir, STDERR_FILE))
        if rc != 0:
            print "+++FAIL: Could not start idigi dir=[%s]" % src_dir
            exit(0)

        print '>>>Started idigi'
        time.sleep(1) # Give the program time to start

        pid = commands.getoutput('pidof -s idigi')
        if pid == '':
            print "idigi not running dir=[%s]" % src_dir

        time.sleep(5) # Give the program time to start

        for test_script in test_list:
            if debug_on and test_script == 'test_redirect.py':
                print '>>>Skip [%s] since debug is on' % test_script
            else:
                print '>>>Executing [%s]' % test_script
                rc = os.system('export PYTHONPATH=../;cd %s; nosetests --with-xunit --xunit-file=nosestest%1d.xml %s' % (test_dir, test_case, test_script))
                test_case += 1

        print '>>>pid [%s]' % pid
        if pid != '':
            p = int(pid)
            try:
                os.kill(p, signal.SIGKILL)
            except OSError as ex:
                idigi_pid = commands.getoutput('pidof -s idigi')
                if idigi_pid == '':
                    print "idigi process not exist"
                elif p != int(idigi_pid):
                    print "multiple idigi processes pid [%s, %s]" % (pid, idigi_pid)
                else: 
                    print"OSError as ex"

        # open standard error file to see any Error message
        errorfile = "%s/%s" % (src_dir, STDERR_FILE)
        try:
            st = os.stat(errorfile)
        except IOError:
            print "failed to get information from %s" % errorfile
        else:
            if st[ST_SIZE] != 0:
                previous_line = ""
                infile = open(errorfile, "r")
                for current_line in infile:
                    if 'Error:' in previous_line:
                        print "Error: %s" % current_line
                        rc = -1
                    previous_line = current_line
                infile.close()
            os.system('rm %s' % errorfile);

        os.system('cd ../../../../../')

        if rc != 0:
            print '+++FAIL: Test failed [%s] [%s]' % (src_dir, test_script)
            exit(0)

def setup_platform(config, config_dir, platform_dir):
    config.remove_errors(platform_dir+'config.c')
    config.update_config_source(platform_dir+'config.c', config_dir+'config.ini')

def main():
    f, filename, description = imp.find_module('config', ['./dvt/scripts'])
    config = imp.load_module('config', f, filename, description)

    # create empty memory usage file
    mem_usage_file = open(MEMORY_USAGE_FILE, 'w')
    mem_usage_file.close()

    debug_on = True
    setup_platform(config, SAMPLE_SCRIPT_DIR, SAMPLE_PLATFORM_DIR)
    run_tests(debug_on)

    debug_on = False
    config.replace_string('./public/include/idigi_config.h', 'IDIGI_DEBUG', 'IDIGI_NO_DEBUG')
    run_tests(debug_on)

    config.replace_string('./public/include/idigi_config.h', 'IDIGI_NO_COMPRESSION', 'IDIGI_COMPRESSION')
    run_tests(debug_on)

    debug_on = True
    config.replace_string('./public/include/idigi_config.h', 'IDIGI_NO_DEBUG', 'IDIGI_DEBUG')
    run_tests(debug_on)

    config.update_config_header('./public/include/idigi_config.h', SAMPLE_SCRIPT_DIR+'config.ini')
    run_tests(debug_on)

    setup_platform(config, TEMPLATE_SCRIPT_DIR, TEMPLATE_PLATFORM_DIR)
    build_test(TEMPLATE_TEST_DIR)

    config.replace_string('./public/include/idigi_config.h', 'IDIGI_COMPRESSION', 'IDIGI_NO_COMPRESSION')
    config.replace_string('./public/include/idigi_config.h', 'IDIGI_FIRMWARE_SERVICE', 'IDIGI_NO_FIRMWARE_SERVICE')
    build_test(TEMPLATE_TEST_DIR)

    config.replace_string('./public/include/idigi_config.h', 'IDIGI_DATA_SERVICE', 'IDIGI_NO_DATA_SERVICE')
    build_test(TEMPLATE_TEST_DIR)

    config.replace_string('./public/include/idigi_config.h', 'IDIGI_NO_FIRMWARE_SERVICE', 'IDIGI_FIRMWARE_SERVICE')
    build_test(TEMPLATE_TEST_DIR)

    config.replace_string('./public/include/idigi_config.h', 'IDIGI_NO_DATA_SERVICE', 'IDIGI_DATA_SERVICE')


if __name__ == '__main__':
    main()

