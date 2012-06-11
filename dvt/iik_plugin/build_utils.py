import os
import shutil
import subprocess
import tempfile
import sys
sys.path.append('./dvt/scripts')
import config

TEMPLATE_TEST_DIR = 'dvt/samples/template_test'
TEMPLATE_SCRIPT_DIR = 'dvt/cases/dvt_tests/'
TEMPLATE_PLATFORM_DIR = 'public/run/platforms/template/'

def build(dir, cflags):
    process = subprocess.Popen(['make', 'clean', 'all', cflags], cwd=dir, 
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    data = process.communicate()[0]
    if process.returncode != 0:
        print "+++FAIL: Build failed dir=[%s]" % dir
        if data is not None:
            print data
    
    return (process.returncode, data)

def setup_platform(config_dir, platform_dir, mac_addr):
    config.remove_errors(os.path.join(platform_dir, 'config.c'))
    config.update_config_source(os.path.join(platform_dir, 'config.c'), os.path.join(config_dir, 'config.ini'), mac_addr)

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

def get_template_dirs():
    sandbox_dir = sandbox('.')
    template_script_dir = os.path.join(sandbox_dir, TEMPLATE_SCRIPT_DIR)
    template_platform_dir = os.path.join(sandbox_dir, TEMPLATE_PLATFORM_DIR)
    template_test_dir = os.path.join(sandbox_dir, TEMPLATE_TEST_DIR)
    idigi_config_path = os.path.join(template_test_dir, './idigi_config.h')

    return (sandbox_dir, template_script_dir, template_platform_dir, template_test_dir, idigi_config_path)
