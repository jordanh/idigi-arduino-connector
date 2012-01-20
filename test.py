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
import tempfile
import os, time
import StringIO
import uuid
import subprocess
import tempfile
import shutil
sys.path.append('./dvt/cases')
import idigi_ws_api
import argparse
import nose
import iik_plugin
sys.path.append('./dvt/scripts')
import config

from stat import * # ST_SIZE etc

TEMPLATE_TEST_DIR = 'dvt/samples/template_test'
TEMPLATE_SCRIPT_DIR = 'dvt/cases/dvt_tests/'
TEMPLATE_PLATFORM_DIR = 'public/run/platforms/template/'

SAMPLE_DIR='public/run/samples/'
SAMPLE_SCRIPT_DIR='dvt/cases/sample_tests/'
SAMPLE_PLATFORM_DIR = 'public/run/platforms/linux/'

BASE_SAMPLE_DIR='public/run/samples/'
BASE_DVT_SRC='dvt/samples/'
BASE_SCRIPT_DIR='dvt/cases/'

#indices of test_table
SRC_DIR  = 0
TEST_DIR = 1
TEST_LIST = 2

STDERR_FILE = 'stderr.txt'
STDOUT_FILE = 'stdout.txt'
MEMORY_USAGE_FILE = './dvt/memory_usage.txt'

DEVICE_ID_PROTOTYPE = '00000000-00000000-%sFF-FF%s'
MAC_ADDR_PROTOTYPE = '%s:%s'
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
              #[BASE_SAMPLE_DIR+'firmware_download', BASE_SCRIPT_DIR+'sample_tests', ['test_firmware.py']],
              #[BASE_SAMPLE_DIR+'send_data',         BASE_SCRIPT_DIR+'sample_tests', ['test_send_data.py']],
              #[BASE_SAMPLE_DIR+'device_request',    BASE_SCRIPT_DIR+'sample_tests', ['test_device_request.py']],
              #[BASE_DVT_SRC+'full_test',            BASE_SCRIPT_DIR+'dvt_tests',    ['test_firmware_errors.py', 
              #                                                                       'test_device_request.py', 
              #                                                                       'test_data_service.py']],
              #[BASE_DVT_SRC+'data_service',         BASE_SCRIPT_DIR+'admin_tests',  ['test_reboot.py', 
              #                                                                       'test_redirect.py']],
              #[BASE_DVT_SRC+'data_service',         BASE_SCRIPT_DIR+'admin_tests',  ['test_nodebug_redirect.py']],
]

def build_test(dir, stdout=subprocess.PIPE):

    process = subprocess.Popen(['make', 'clean', 'all'], cwd=dir, 
        stdout=stdout, stderr=subprocess.STDOUT)
    process.wait()
    if process.returncode != 0:
        print "+++FAIL: Build failed dir=[%s]" % dir
    
    return process.returncode

def generate_id(api):
    """
    Generate a Pseudo Random Device Id (low probability of duplication) and 
    provision device to account.
    """
    for _ in xrange(0,5):
        # Make up to 5 attempts to provision a unique device.
        base_id = str.upper(str(uuid.uuid4()))
        device_id = DEVICE_ID_PROTOTYPE % (base_id[:6], base_id[-6:])
        mac_addr = MAC_ADDR_PROTOTYPE % (base_id[:6], base_id[-6:])

        device_core = idigi_ws_api.RestResource.create('DeviceCore', 
            devConnectwareId=device_id)
        try:
            device_location = api.post(device_core)
            # break on successful post.
            return (device_id, mac_addr, device_location)
        except Exception, e:
            print e
    
    # If here, we couldn't provision a device, raise Exception.
    raise Exception("Failed to Provision Device using %s." % user_id)

def sandbox(directory):
    """
    Creates a temporary directory with contents of input directory.
    This can be used to compile code and execute tests in a separate
    place and will allow tests to run concurrently.
    """
    # Get a temporary directory name, then delete it.
    sandbox_directory = tempfile.mkdtemp()
    # Copy tree requires destination to not exist.
    os.removedirs(sandbox_directory)
    shutil.copytree(directory, sandbox_directory, symlinks=True)
    return sandbox_directory

def run_tests(description, base_dir, debug_on, api, replace_list=[], 
    update_config_header=False):

    sandbox_dir = sandbox(base_dir)

    try: 

        for (f, s, r) in replace_list:
            config.replace_string(os.path.join(sandbox_dir, f), s, r)

        if update_config_header:
            config.update_config_header(
                os.path.join(sandbox_dir, 'public/include/idigi_config.h'), 
                os.path.join(sandbox_dir, SAMPLE_SCRIPT_DIR+'config.ini'))

        (device_id, mac_addr, device_location) = generate_id(api)
        setup_platform(config, os.path.join(sandbox_dir, SAMPLE_SCRIPT_DIR), 
            os.path.join(sandbox_dir, SAMPLE_PLATFORM_DIR), mac_addr)

        for test in test_table:
            src_dir   = os.path.join(sandbox_dir, test[SRC_DIR])
            test_dir  = os.path.join(sandbox_dir, test[TEST_DIR])
            test_list = test[TEST_LIST]
     
            print '>>> Testing [%s]' % src_dir

            stdout_path = os.path.join(base_dir, '%s_%s' % (description, STDOUT_FILE))
            stdout_file = open(stdout_path, 'w')
            rc = build_test(src_dir, stdout_file)
            stdout_file.close()

            if rc != 0:
                raise Exception("Failed to Build from %s." % src_dir)
        
            #stdout_file = open(stdout_path, 'a')

            stderr_path = os.path.join(base_dir, '%s_%s' % (description, STDERR_FILE))

            print '>>> Starting idigi'
            iik_path = os.path.join(src_dir, 'idigi')

            process = subprocess.Popen('%s >> %s 2> %s &' % (iik_path, 
                stdout_path, stderr_path), shell=True)

            print '>>> Started idigi'
            time.sleep(5) # Give the program time to start
            try:
                for test_script in test_list:
                    # skip the test if script filename starts with 'test_nodebug'
                    if debug_on and test_script.find('test_nodebug') == 0:
                        print '>>> Skip [%s] since debug is on' % test_script
                    else:
                        print '>>> Executing [%s]' % test_script
                        
                        # Argument list to call nose with.  Generate a nosetest xml file in 
                        # current directory, pass in idigi / iik connection settings.
                        arguments = ['nosetests',
                                     '--with-xunit', 
                                     '--xunit-file=%s_%s.nxml' % (description, test_script),
                                     '--with-iik',
                                     '--idigi_username=%s'  % api.username,
                                     '--idigi_password=%s'  % api.password,
                                     '--idigi_hostname=%s'  % api.hostname,
                                     '--iik_device_id=%s' % device_id,
                                     '--iik_config=%s/config.ini' % test_dir ]
                        
                        test_to_run = os.path.join(test_dir, test_script)
                        nose.run(defaultTest=[test_to_run], argv=arguments,
                            addplugins=[iik_plugin.IIKPlugin()])
            except Exception, e:
                print e
            finally:
                print '>>> Killing Process with pid [%s]' % process.pid
                process.terminate()
                
                # Delete the Device after we're done with it.
                api.delete_location(device_location)
                # open standard error file to see any Error message
                try:
                    st = os.stat(stderr_path)
                    if st[ST_SIZE] != 0:
                        previous_line = ""
                        infile = open(stderr_path, "r")
                        for current_line in infile:
                            if 'Error:' in previous_line:
                                print "Error: %s" % current_line
                                rc = -1
                            previous_line = current_line
                        infile.close()
                except IOError:
                    print "failed to get information from %s" % errorfile
    finally:
        shutil.rmtree(sandbox_dir)

def setup_platform(config, config_dir, platform_dir, mac_addr):
    config.remove_errors(platform_dir+'config.c')
    config.update_config_source(platform_dir+'config.c', config_dir+'config.ini', mac_addr)

def clean_output(directory):
    for root, folders, files in os.walk(directory):
        for test_result in filter(lambda f: f.endswith('.nxml') \
            or f.endswith(STDOUT_FILE) or f.endswith(STDERR_FILE), files):
            file_path = os.path.join(root, test_result)
            print "Removing %s." % file_path
            os.remove(file_path)

def main():
    parser = argparse.ArgumentParser(description="IIK TestCase",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--username', action='store', type=str, default='iikdvt')
    parser.add_argument('--password', action='store', type=str, default='iik1sfun')
    parser.add_argument('--hostname', action='store', type=str, default='test.idigi.com')
    parser.add_argument('--descriptor', action='store', type=str, default='linux-x64')

    args = parser.parse_args()

    api = idigi_ws_api.Api(args.username, args.password, args.hostname)

    print "============ Cleaning Test Previous Output Files. ============"
    clean_output('.')

    # create empty memory usage file
    mem_usage_file = open(MEMORY_USAGE_FILE, 'w')
    mem_usage_file.close()

    print "============ Default ============="
    run_tests('%s_%s' % (args.descriptor, 'Default'), '.', True, api)

    print "============ No Debug ============="
    run_tests('%s_%s' % (args.descriptor, 'Release'), '.', False, api, 
        [('public/include/idigi_config.h', 'IDIGI_DEBUG', 'IDIGI_NO_DEBUG')])

    print "============ Compression On ============="
    run_tests('%s_%s' % (args.descriptor, 'Compression'), '.', False, api, 
        [('public/include/idigi_config.h', 'IDIGI_NO_COMPRESSION', 
         'IDIGI_COMPRESSION')])

    print "============ Debug On ============="
    run_tests('%s_%s' % (args.descriptor, 'Debug'), '.', True, api, 
        [('public/include/idigi_config.h', 'IDIGI_NO_DEBUG', 'IDIGI_DEBUG')])

    print "============ Configurations in idigi_config.h ============="
    run_tests('%s_%s' % (args.descriptor, 'idigiconfig'), '.', True, api, 
        update_config_header=True)

    print "============ Template platform ============="
    setup_platform(config, TEMPLATE_SCRIPT_DIR, TEMPLATE_PLATFORM_DIR, '000000:000000')
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

