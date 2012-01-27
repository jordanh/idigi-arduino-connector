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
import idigi_ws_api
import iik_plugin
import build_plugin
import nose
from build_utils import get_template_dirs, setup_platform, build, sandbox
sys.path.append('./dvt/scripts')
import config
import argparse

from stat import * # ST_SIZE etc
from threading import Thread

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
              [BASE_SAMPLE_DIR+'firmware_download', BASE_SCRIPT_DIR+'sample_tests', ['test_firmware.py']],
              [BASE_SAMPLE_DIR+'send_data',         BASE_SCRIPT_DIR+'sample_tests', ['test_send_data.py']],
              [BASE_SAMPLE_DIR+'device_request',    BASE_SCRIPT_DIR+'sample_tests', ['test_device_request.py']],
              [BASE_DVT_SRC+'full_test',            BASE_SCRIPT_DIR+'dvt_tests',    ['test_firmware_errors.py', 
                                                                                     'test_device_request.py',
                                                                                     'test_data_service.py']],
              [BASE_DVT_SRC+'data_service',         BASE_SCRIPT_DIR+'admin_tests',  ['test_reboot.py']],
              [BASE_DVT_SRC+'data_service',         BASE_SCRIPT_DIR+'admin_tests',  ['test_redirect.py']],
              [BASE_DVT_SRC+'data_service',         BASE_SCRIPT_DIR+'admin_tests',  ['test_nodebug_redirect.py']],
]

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

def start_iik(executable):
    """
    Starts an IIK session in given path with given executable name.
    """
    os.system('/usr/bin/script -q -f -c "%s"' % (executable))

def run_tests(description, base_dir, debug_on, api, cflags, replace_list=[], 
    update_config_header=False):

    for test in test_table:
        sandbox_dir = sandbox(base_dir)
        try:
            for (f, s, r) in replace_list:
                config.replace_string(os.path.join(sandbox_dir, f), s, r)

            if update_config_header:
                config.update_config_header(
                    os.path.join(sandbox_dir, 'public/include/idigi_config.h'), 
                    os.path.join(sandbox_dir, SAMPLE_SCRIPT_DIR+'config.ini'))

            (device_id, mac_addr, device_location) = generate_id(api)
            setup_platform(os.path.join(sandbox_dir, SAMPLE_SCRIPT_DIR), 
                os.path.join(sandbox_dir, SAMPLE_PLATFORM_DIR), mac_addr)
            src_dir   = os.path.join(sandbox_dir, test[SRC_DIR])
            test_dir  = os.path.join(sandbox_dir, test[TEST_DIR])
            test_list = test[TEST_LIST]
     
            print '>>> [%s] Testing [%s]' % (description, src_dir)

            (rc,output) = build(src_dir, cflags)

            if rc != 0:
                raise Exception("Failed to Build from %s." % src_dir)
        
            print '>>> [%s] Starting idigi' % description

            # Move idigi executable to a unique file name to allow us to isolate
            # the pid.
            old_idigi_path = os.path.join(src_dir, 'idigi')
            idigi_executable = str(uuid.uuid4())
            idigi_path = os.path.join(src_dir, idigi_executable)
            shutil.move(old_idigi_path, idigi_path)

            # Spawn in separate thread rather than shelling off in background.
            # Will allow capture of STDOUT/STDERR easier, save to separate file.
            iik_thread = Thread(group=None, target=start_iik, args=(idigi_path,))
            iik_thread.start()

            connected = False
            for _ in xrange(1,10):
                device_core = api.get_first('DeviceCore', 
                        condition="devConnectwareId='%s'" % device_id)
                if device_core.dpConnectionStatus == '1':
                    print ">>> [%s] Device %s Connected." % (description, device_id)
                    connected = True
                    break
                else:
                    time.sleep(1)

            # Sleep 5 seconds to allow device to do it's initialization (push data for example)
            time.sleep(5)
            pid = commands.getoutput('pidof -s %s' % idigi_path)
            if pid == '':
                raise Exception(">>> [%s] idigi not running dir=[%s]" % (description, src_dir))

            if not connected:
                raise Exception("Device %s was not connected after 10 seconds." % device_id)

            print '>>> [%s] Started idigi' % description

            try:
                for test_script in test_list:
                    # skip the test if script filename starts with 'test_nodebug'
                    if debug_on and test_script.find('test_nodebug') == 0:
                        print '>>> [%s] Skip [%s] since debug is on' % (description, test_script)
                    else:
                        print '>>> [%s] Executing [%s]' % (description, test_script)
                        
                        # Argument list to call nose with.  Generate a nosetest xml file in 
                        # current directory, pass in idigi / iik connection settings.
                        arguments = ['nosetests',
                                     '--with-xunit',
                                     '-s', # Don't capture STDOUT (allow everything else to print)
                                     '--xunit-file=%s_%s.nxml' % (description, test_script),
                                     '--with-iik',
                                     '--idigi_username=%s'  % api.username,
                                     '--idigi_password=%s'  % api.password,
                                     '--idigi_hostname=%s'  % api.hostname,
                                     '--iik_device_id=%s' % device_id,
                                     '--iik_config=%s/config.ini' % test_dir]
                        
                        test_to_run = os.path.join(test_dir, test_script)
                        nose.run(defaultTest=[test_to_run], argv=arguments)
                        print '>>> [%s] Finished [%s]' % (description, test_script)
            finally:
                # Killing the process should also cause the thread to complete.
                print '>>> [%s] Killing Process with pid [%s]' % (description, pid)
                os.kill(int(pid), signal.SIGKILL)
        except Exception, e:
            print ">>> [%s] Error: %s" % (description, e)
        finally:
            # Delete the Device after we're done with it.
            if device_location is not None:
                api.delete_location(device_location)
            shutil.rmtree(sandbox_dir)

def clean_output(directory):
    for root, folders, files in os.walk(directory):
        for test_result in filter(lambda f: f.endswith('.nxml'), files):
            file_path = os.path.join(root, test_result)
            print "Removing %s." % file_path
            os.remove(file_path)

def build_template(description, cflags):

    test_script = "test_build.py"
    test_dir = BASE_SCRIPT_DIR+'template_tests'

    print '>>> [%s] Executing [%s]' % (description, test_script)
    
    # Argument list to call nose with.  Generate a nosetest xml file in 
    # current directory, pass in idigi / iik connection settings.
    arguments = ['nosetests',
                 '-s',
                 '--with-xunit', 
                 '--xunit-file=%s_%s.nxml' % (description, test_script),
                 '--with-build',
                 '--build_cflags=%s' % (cflags)]
    
    test_to_run = os.path.join(test_dir, test_script)
    nose.run(defaultTest=[test_to_run], argv=arguments)
    print '>>> [%s] Finished [%s]' % (description, test_script)

def main():
    parser = argparse.ArgumentParser(description="IIK TestCase",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--username', action='store', type=str, default='iikdvt')
    parser.add_argument('--password', action='store', type=str, default='iik1sfun')
    parser.add_argument('--hostname', action='store', type=str, default='test.idigi.com')
    parser.add_argument('--descriptor', action='store', type=str, default='linux-x64')
    parser.add_argument('--architecture', action='store', type=str, default='x64')
    parser.add_argument('--configuration', action='store', type=str, 
        default='all', choices=['default', 'nodebug', 'compression', 
                                    'debug', 'config_header', 'template', 
                                    'all'])

    args = parser.parse_args()

    # If 64-bit not passed in, assume 32-bit
    if args.architecture == 'x64':
        cflags='DFLAGS=-m64'
    else:
        cflags='DFLAGS=-m32'

    api = idigi_ws_api.Api(args.username, args.password, args.hostname)

    print "============ Cleaning Test Previous Output Files. ============"
    clean_output('.')

    # create empty memory usage file
    mem_usage_file = open(MEMORY_USAGE_FILE, 'w')
    mem_usage_file.close()

    if args.configuration == 'default' or args.configuration == 'all':
        print "============ Default ============="
        run_tests('%s_%s' % (args.descriptor, 'Default'), '.', True, api, cflags)

    if args.configuration == 'nodebug' or args.configuration == 'all':
        print "============ No Debug ============="
        run_tests('%s_%s' % (args.descriptor, 'Release'), '.', False, api, cflags, 
        [('public/include/idigi_config.h', 'IDIGI_DEBUG', 'IDIGI_NO_DEBUG')])

    if args.configuration == 'compression' or args.configuration == 'all':
        print "============ Compression On ============="
        run_tests('%s_%s' % (args.descriptor, 'Compression'), '.', False, api, cflags,
        [('public/include/idigi_config.h', 'IDIGI_NO_COMPRESSION', 
         'IDIGI_COMPRESSION'), 
         ('public/include/idigi_config.h', 'IDIGI_DEBUG', 'IDIGI_NO_DEBUG')])

    if args.configuration == 'debug' or args.configuration == 'all':
        print "============ Debug On ============="
        run_tests('%s_%s' % (args.descriptor, 'Debug'), '.', True, api, cflags, 
        [('public/include/idigi_config.h', 'IDIGI_NO_DEBUG', 'IDIGI_DEBUG')])

    if args.configuration == 'config_header' or args.configuration == 'all':
        print "============ Configurations in idigi_config.h ============="
        run_tests('%s_%s' % (args.descriptor, 'idigiconfig'), '.', True, api, cflags,
            update_config_header=True)

    if args.configuration == 'template' or args.configuration == 'all':
        print "============ Template platform ============="
        build_template(args.descriptor, cflags)

if __name__ == '__main__':
    main()

