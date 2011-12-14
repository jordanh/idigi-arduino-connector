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

BASE_SAMPLE_DIR='./public/run/samples/'
TEST_SCRIPT_DIR='./public/test/harness/cases/user_tests/'
DIR_ENTRY  = 0
TESTS_ENTRY = 1

#
# Modify this table when adding a new test.
# 
# Each test has an entry in this table.
# 1. Directory which contains the makefile and the idigi executable, the
#    makefile is build with 'make clean all' and the name of the executable
#    must be idigi
# 2. List of python test scripts to run
#
# Note: test scripts are located in the directory:
#                   public/test/harness/cases/user_tests
#              Directory.                                Test Scripts
test_table = [[BASE_SAMPLE_DIR+'connect_to_idigi',       ['test_discovery.py']],
              [BASE_SAMPLE_DIR+'firmware_download',      ['test_firmware.py']]
]

def run_tests():
    for test in test_table:
        dir    = test[DIR_ENTRY]
        tests  = test[TESTS_ENTRY]
        print '>>>Testing [%s]' % dir

        rc = os.system('cd %s; make clean all' % (dir))
        if rc != 0:
            print "+++FAIL: Build failed dir=[%s]" % dir
            exit(0)

        rc = os.system('cd %s;./idigi &' % (dir))
        if rc != 0:
            print "+++FAIL: Could not start idigi dir=[%s]" % dir
            exit(0)

        pid = commands.getoutput('pidof idigi')
        if pid == '':
            print "+++FAIL: idigi not running dir=[%s]" % dir
            exit(0)

        time.sleep(5) # Give the program time to start

        for test_script in tests:
            print '>>>Executing [%s]' % test_script
            rc = os.system('export PYTHONPATH=../;cd %s; nosetests --with-xunit %s' % (TEST_SCRIPT_DIR, test_script))

        p = int(pid)
        os.kill(p, signal.SIGKILL)
        os.system('cd ../../../../../')

        if rc != 0:
            print '+++FAIL: Test failed [%s] [%s]' % (dir, test_script)
            exit(0)

def setup_platform():
    f, filename, description = imp.find_module('config', ['./scripts'])
    config = imp.load_module('config', f, filename, description)
    config.remove_errors('./public/run/platforms/linux/config.c')
    config.update_config_files('./public/include/idigi_config.h', './public/test/harness/cases/user_tests/config.ini', './public/run/platforms/linux/config.c')

def main():
    setup_platform()
    run_tests()

if __name__ == '__main__':
    main()
