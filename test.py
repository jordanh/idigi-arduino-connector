# ***************************************************************************
# Copyright (c) 2011, 2012 Digi International Inc.,
# All rights not expressly granted are reserved.
# 
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.
# 
# Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
#
# ***************************************************************************
#
# Build the iDigi Connector test harness
#
import commands
import sys
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
import gcov as gcov_config

from stat import * # ST_SIZE etc
from threading import Thread

BASE_SCRIPT_DIR= 'dvt/cases/'
SAMPLE_SCRIPT_DIR='dvt/cases/sample_tests/'
SAMPLE_PLATFORM_RUN_DIR = 'public/run/platforms/linux/'
SAMPLE_PLATFORM_STEP_DIR = 'public/step/platforms/linux/'

BUILD_TEST='dvt/cases/build_tests/test_build.py'

MEMORY_USAGE_FILE = './dvt/memory_usage.txt'

DEVICE_ID_PROTOTYPE = '00000000-00000000-%sFF-FF%s'
MAC_ADDR_PROTOTYPE = '%s:%s'

GCOV_FLAGS = " -g -pg -fprofile-arcs -ftest-coverage"

class TestType(object):

    def __init__(self, name, src_dir, script_dir, tests):
        self.name = name
        self.src_dir = src_dir
        self.script_dir = script_dir
        self.tests = tests

sample_tests = {
    'compile_and_link'  : [],
    'connect_to_idigi'  : ('test_discovery.py',),
    'connect_on_ssl'    : ('test_ssl.py',),
    'firmware_download' : ('test_firmware.py',),
    'send_data'         : ('test_send_data.py',),
    'device_request'    : ('test_device_request.py',),
    'file_system'       : ('test_file_system.py',),
}
run_sample  = TestType('run_sample', 'public/run/samples/', 
                       'dvt/cases/sample_tests', sample_tests)
step_sample = TestType('step_sample', 'public/step/samples/', 
                       'dvt/cases/sample_tests', sample_tests)

dvt_tests = {
    'full_test'                   : ('test_firmware_errors.py', 
                                     'test_device_request.py', 
                                     'test_put_request.py',
                                     'test_reboot.py',
                                     'test_disconnect.py'),
    'file_system'                 : ('test_fs_positive.py',
                                     'test_fs_negative.py'),
    'reboot_test'                 : ('test_delay_reboot.py',
                                     'test_disconnect.py'),
    'terminate_test'              : ('test_ds_terminate.py',),
    'response_to_bad_values_test' : ('test_debug_response_to_bad_values.py',),
    'compile_remote_config'       : (),
    'base_remote_config'          : ('test_rci_descriptors.py',)
}
dvt_test    = TestType('dvt_test', 'dvt/samples/', 'dvt/cases/dvt_tests',
                       dvt_tests)

keepalive_tests = {
    'keep_alive_test'             : ('test_keep_alive.py',),
}
keepalive_test = TestType('keepalive_test', 'dvt/samples/', 
                          'dvt/cases/keep_alive', keepalive_tests)

admin_tests = {
    'full_test'                   : ('test_redirect.py', 
                                     'test_nodebug_redirect.py')
}
admin_test  = TestType('admin_test',  'dvt/samples', 
                       'dvt/cases/admin_tests', admin_tests)

malloc_tests = {'malloc_test'     : ('test_malloc.py',) 
}
malloc_test  = TestType('malloc_test',  'dvt/samples', 
                       'dvt/cases/malloc_tests', malloc_tests)

# Dictionary mapping Test Type name to it's instance.
TESTS = dict((test.name,test) for test in [run_sample, step_sample,
                                           malloc_test,  
                                           dvt_test, admin_test, 
                                           keepalive_test])
SAMPLE_TESTS = dict((test.name,test) for test in [run_sample, step_sample])

DVT_TESTS = dict((test.name,test) for test in [malloc_test,
                                           dvt_test, admin_test, 
                                           keepalive_test])

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
    raise Exception("Failed to Provision Device using %s." % api.username)

def start_iik(executable, tty=False):
    """
    Starts an iDigi Connector session in given path with given executable name.
    """
    if tty:
        os.system('/usr/bin/script -q -f -c "%s"' % (executable))
    else:
        os.system('%s 2>&1' % (executable))

def run_test(test, test_list, execution_type, base_src_dir, base_script_dir, 
    description, base_dir, debug_on, api, cflags, replace_list=[], 
    update_config_header=False, tty=False, gcov=False, sample=False,
    config_tool_jar='ConfigGenerator.jar', keystore=None):
    device_location = None
    try:
        sandbox_dir = sandbox(base_dir)
        src_dir        = os.path.join(sandbox_dir, base_src_dir)
        test_dir       = os.path.join(sandbox_dir, base_script_dir)
        idigi_config   = os.path.join(src_dir, 'idigi_config.h')

        for (s, r) in replace_list:
            config.replace_string(idigi_config, s, r)

        (device_id, mac_addr, device_location) = generate_id(api)
        if test =='compile_and_link':
            config.replace_string(os.path.join(src_dir, 'Makefile'), 
                'c99', 'c89')

        # Use config.c in the local directory if it exists
        try:
            filename = os.path.join(src_dir, "config.c")
            f = open(filename, 'r')
            f.close()
            setup_platform(test_dir, src_dir, mac_addr)
        except IOError:
            setup_platform(test_dir, os.path.join(sandbox_dir, 
                SAMPLE_PLATFORM_RUN_DIR), mac_addr)
            setup_platform(test_dir, os.path.join(sandbox_dir, 
                SAMPLE_PLATFORM_STEP_DIR), mac_addr)

        if update_config_header:
            config.update_config_header(idigi_config, 
                os.path.join(test_dir, 'config.ini'))
 
        if gcov is True and test != 'compile_and_link':
            cflags += GCOV_FLAGS
            # Resolve the main.c file.  If it exists inthe src_dir assume
            # that is what is used, otherwise autoresolve to 
            # ../../platforms/linux/main.c
            main = None
            local_main = os.path.join(src_dir, "main.c")
            if os.path.isfile(local_main):
                main = local_main
            else:
                platform_main = os.path.join(src_dir, 
                    "../../platforms/linux/main.c")
                if os.path.isfile(platform_main):
                    main = platform_main
                else:
                    platform_main = os.path.join(src_dir,
                            "../../../public/run/platforms/linux/main.c")
                    if os.path.isfile(platform_main):
                        main = platform_main
                    else:
                        print "Error:  Could not resolve main.c from %s." % src_dir
                        return
            # Instrument the main file with a __gcov_flush USR1 signal hook.
            gcov_config.instrument(main)
            # Add -lgcov to libraries.
            gcov_config.add_lib(os.path.join(src_dir, "Makefile"))

        print '>>> [%s] Testing [%s]-[%s]' % (description, execution_type, test)

        build_args = ['nosetests',
                      '--with-xunit',
                      '-s', # Don't Capture STDOUT
                      '--xunit-file=%s_%s_%s_build.nxml' % (description, execution_type, test),
                      '--with-build',
                      '--build_cflags=%s' % cflags,
                      '--build_src=%s' % src_dir,
                      '--build_username=%s' % api.username,
                      '--build_password=%s' % api.password,
                      '--build_hostname=%s' % api.hostname,
                      '--build_config_tool_jar=%s' % config_tool_jar,
                      '--build_keystore=%s' % keystore]

        rc = nose.run(defaultTest=[BUILD_TEST], argv=build_args)

        # If False is returned, Fail this build
        if rc == False:
            raise Exception("Failed to Build from %s." % src_dir)
    
        if test == 'compile_and_link':
            print '>>> [%s] Finished [%s]-[%s]' % (description, src_dir, 'c89')
            return

        print '>>> [%s] Starting idigi [%s]-[%s]' % (description, execution_type, src_dir)

        # Move idigi executable to a unique file name to allow us to isolate
        # the pid.
        old_idigi_path = os.path.join(src_dir, 'idigi')
        idigi_executable = str(uuid.uuid4())
        idigi_path = os.path.join(src_dir, idigi_executable)
        shutil.move(old_idigi_path, idigi_path)

        # Spawn in separate thread rather than shelling off in background.
        # Will allow capture of STDOUT/STDERR easier, save to separate file.
        iik_thread = Thread(group=None, target=start_iik, args=(idigi_path,tty))
        iik_thread.start()

        connected = False
        for _ in xrange(1,10):
            device_core = api.get_first('DeviceCore', 
                    condition="devConnectwareId='%s'" % device_id)
            if hasattr(device_core, 'dpConnectionStatus') \
                and device_core.dpConnectionStatus == '1':
                print ">>> [%s] Device %s Connected." % (description, device_id)
                connected = True
                break
            else:
                time.sleep(1)

        # Sleep 5 seconds to allow device to do it's initialization (push data for example)
        time.sleep(5)
        test_script = None
        try:
            pid = commands.getoutput('pidof -s %s' % idigi_path)
            if pid == '':
                raise Exception(">>> [%s] idigi [%s] not running dir=[%s]" % (description, execution_type, src_dir))

            if not connected:
                raise Exception("Device %s was not connected after 10 seconds." % device_id)

            print '>>> [%s] Started idigi [%s]-[%s]' % (description, execution_type, src_dir)
            for test_script in test_list:
                # skip the test if script filename starts with 'test_nodebug'
                if debug_on and test_script.find('test_nodebug') == 0:
                    print '>>> [%s] Skip [%s]-[%s] since debug is on' % (description, execution_type, test_script)
                else:
                    # skip the test if script filename starts with 'test_debug'
                    if (not debug_on and test_script.find('test_debug') == 0) or (update_config_header and test_script.find('test_debug') == 0):
                        print '>>> [%s] Skip [%s]-[%s] since debug is off or update_config_header' % (description, execution_type, test_script)
                        filename1 = "%s.txt" % device_id
                        print '>>> deleting file [%s]' % filename1
                        os.unlink(filename1)
                    else:
                        print '>>> [%s] Executing [%s]-[%s]' % (description, execution_type, test_script)
                    
                        # Argument list to call nose with.  Generate a nosetest xml file in 
                        # current directory, pass in idigi / iik connection settings.
                        arguments = ['nosetests',
                                     '--with-xunit',
                                     '-s', # Don't capture STDOUT (allow everything else to print)
                                     '--xunit-file=%s_%s_%s_%s.nxml' % (description, execution_type, test, test_script),
                                     '--with-iik',
                                     '--idigi_username=%s'  % api.username,
                                     '--idigi_password=%s'  % api.password,
                                     '--idigi_hostname=%s'  % api.hostname,
                                     '--iik_device_id=%s' % device_id,
                                     '--iik_config=%s/config.ini' % test_dir]
                    
                        test_to_run = os.path.join(test_dir, test_script)
                        nose.run(defaultTest=[test_to_run], argv=arguments)
                        print '>>> [%s] Finished [%s]-[%s]' % (description, execution_type, test_script)
        finally:
            # Killing the process should also cause the thread to complete.
            if gcov and test != 'compile_and_link' and test_script is not None:
                print '>>> [%s] Flushing gcov coverage data for pid [%s] and exiting.' % (description, pid)
                os.kill(int(pid), signal.SIGUSR1)
                # Only include Coverage data for files in private directory unless this is a sample test.
                inclusion = '-f ".*%s/private.*"' % sandbox_dir
                if sample:
                    # Otherwise, explicitly filter private
                    inclusion = '-e ".*%s/private.*"' % sandbox_dir
                cwd = os.getcwd()   
                cmd = 'cd %s; %s/dvt/scripts/gcovr %s --root %s -d --xml %s > "%s/%s_%s_%s_%s_coverage.xml"' % (src_dir, sandbox_dir, sandbox_dir, sandbox_dir, inclusion, cwd, description, execution_type, test, test_script)
                print "Command is %s" % cmd
                os.system(cmd)
            else:
                print '>>> [%s] Killing Process with pid [%s]' % (description, pid)
                os.kill(int(pid), signal.SIGKILL)
    except Exception, e:
        print ">>> [%s] Error: %s" % (description, e)
    finally:
        # Delete the Device after we're done with it.
        if device_location is not None:
            try:
                pass#api.delete_location(device_location)
            except:
                # If we get a failure deleting the device, proceed
                # as the device was already removed.
                pass
        shutil.rmtree(sandbox_dir)


def run_tests(description, base_dir, debug_on, api, cflags, replace_list=[], 
    update_config_header=False, tty=False, gcov=False, test_type=None, 
    test_name=None, config_tool_jar='ConfigGenerator.jar', keystore=None):

    test_modes = ([('IDIGI_FILE_SYSTEM', 'IDIGI_NO_FILE_SYSTEM'), 
                   ('IDIGI_FIRMWARE_SERVICE', 'IDIGI_NO_FIRMWARE_SERVICE')],
                  [('IDIGI_FILE_SYSTEM', 'IDIGI_NO_FILE_SYSTEM'), 
                   ('IDIGI_FIRMWARE_SERVICE', 'IDIGI_NO_FIRMWARE_SERVICE'),
                   ('DS_MAX_USER 1', 'DS_MAX_USER 0')],
                  [('IDIGI_DATA_SERVICE', 'IDIGI_NO_DATA_SERVICE'), 
                   ('IDIGI_FILE_SYSTEM', 'IDIGI_NO_FILE_SYSTEM')],
                  [('IDIGI_DATA_SERVICE', 'IDIGI_NO_DATA_SERVICE'), 
                   ('IDIGI_FIRMWARE_SERVICE', 'IDIGI_NO_FIRMWARE_SERVICE')])

    tests = TESTS
    # If test_type is defined, filter tests executed to the test_type.
    if test_type is not None:
        # If 'sample' is provided as the test_type, include all samples.
        if test_type == 'sample':
            print "Executing sample tests."
            tests = SAMPLE_TESTS
        elif test_type == 'dvt':
            print "Executing dvt tests."
            tests = DVT_TESTS
        elif test_type not in TESTS:
            print "Error:  test_type (%s) is not valid (%s)."\
                % (test_type, TESTS.keys())
            return
        else:
            print "Executing %s tests." % test_type
            tests = { test_type : TESTS[test_type] }

    for test in tests:
            test_type = tests[test]
            test_list = test_type.tests

            # If test_name is specified, filter test list to the test_name.
            if test_name is not None:
                if test_list.has_key(test_name):
                    test_list = { test_name : test_list[test_name] }
                else:
                    print "Warning: no tests found for %s with this test type (%s)."\
                        % (test_name, test_type.name)
                    continue

            for test_set in test_list:
                sample = test_type.name.find('sample') != -1
                more_modes = test_type.name.find('malloc') != -1

                run_test(test_set, test_list[test_set], test, 
                    os.path.join(test_type.src_dir, test_set), 
                    test_type.script_dir, description, base_dir, debug_on, 
                    api, cflags, replace_list, update_config_header, tty, 
                    gcov, sample, config_tool_jar, keystore)
                if more_modes:
                    for test_mode in test_modes:
                        new_replace_list = replace_list + test_mode
                        run_test(test_set, test_list[test_set], test, 
                        os.path.join(test_type.src_dir, test_set), 
                        test_type.script_dir, description, base_dir, debug_on, 
                        api, cflags, new_replace_list, update_config_header, tty, 
                        gcov, sample, config_tool_jar, keystore)

def clean_output(directory):
    for root, folders, files in os.walk(directory):
        for test_result in filter(lambda f: f.endswith('.nxml') or f.endswith('coverage.xml'), files):
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
    parser = argparse.ArgumentParser(description="iDigi Connector TestCase",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--username', action='store', type=str, default='iikdvt')
    parser.add_argument('--password', action='store', type=str, default='iik1sfun')
    parser.add_argument('--hostname', action='store', type=str, default='test.idigi.com')
    parser.add_argument('--descriptor', action='store', type=str, default='linux-x64')
    parser.add_argument('--architecture', action='store', type=str, default='x64')
    parser.add_argument('--test_name', action='store', type=str, default=None)
    parser.add_argument('--test_type', action='store', type=str, default=None)
    parser.add_argument('--configuration', action='store', type=str, 
        default='all', choices=['default', 'nodebug', 'compression', 
                                    'debug', 'config_header', 'template', 
                                    'all'])
    parser.add_argument('--tty', action='store_true',dest='tty', default=False)
    parser.add_argument('--gcov', action='store_true', dest='gcov', default=False)
    parser.add_argument('--config_tool_jar', action='store', 
        dest='config_tool_jar', type=argparse.FileType('r'))
    parser.add_argument('--keystore', action='store', 
        dest='keystore', type=argparse.FileType('r'))

    args = parser.parse_args()

    config_tool_jar = os.path.abspath(args.config_tool_jar.name) \
        if args.config_tool_jar is not None else None

    keystore = os.path.abspath(args.keystore.name) \
        if args.keystore is not None else None

    if args.hostname == "idigi-e2e.sa.digi.com":
        keystore = os.path.abspath('dvt/conf/e2e.keystore')

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
        run_tests('%s_%s' % (args.descriptor, 'Default'), '.', True, api, cflags, 
        tty=args.tty, test_name=args.test_name, test_type=args.test_type,
        config_tool_jar = config_tool_jar, keystore = keystore)

    if args.configuration == 'nodebug' or args.configuration == 'all':
        print "============ No Debug ============="
        run_tests('%s_%s' % (args.descriptor, 'Release'), '.', False, api, cflags, 
        [('IDIGI_DEBUG', 'IDIGI_NO_DEBUG')], 
         tty=args.tty, test_name=args.test_name, test_type=args.test_type,
        config_tool_jar = config_tool_jar, keystore = keystore)

    if args.configuration == 'compression' or args.configuration == 'all':
        print "============ Compression On ============="
        run_tests('%s_%s' % (args.descriptor, 'Compression'), '.', False, api, cflags,
        [('/* #define IDIGI_COMPRESSION */', '#define IDIGI_COMPRESSION'), 
         ('IDIGI_DEBUG', 'IDIGI_NO_DEBUG')], 
         tty=args.tty, test_name=args.test_name, test_type=args.test_type,
         config_tool_jar = config_tool_jar, keystore = keystore)

    if args.configuration == 'debug' or args.configuration == 'all':
        print "============ Debug On ============="
        run_tests('%s_%s' % (args.descriptor, 'Debug'), '.', True, api, cflags, 
        [('/* #define IDIGI_DEBUG */', '#define IDIGI_DEBUG'),], 
         tty=args.tty, gcov=args.gcov, test_name=args.test_name, 
         test_type=args.test_type,
         config_tool_jar = config_tool_jar, keystore = keystore)

    if args.configuration == 'config_header' or args.configuration == 'all':
        print "============ Configurations in idigi_config.h ============="
        run_tests('%s_%s' % (args.descriptor, 'idigiconfig'), '.', True, api, cflags,
        update_config_header=True, tty=args.tty, test_name=args.test_name, 
        test_type=args.test_type,
        config_tool_jar = config_tool_jar, keystore = keystore)

    if args.configuration == 'template' or args.configuration == 'all':
        print "============ Template platform ============="
        build_template(args.descriptor, cflags)

if __name__ == '__main__':
    main()

