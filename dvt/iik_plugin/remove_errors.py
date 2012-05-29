"""A simple main to fix the config.c file used for samples"""
from build_utils import setup_platform
from config import update_config_header
import sys

if __name__ == "__main__":
    setup_platform(*sys.argv[1:])
    update_config_header('%s/%s' % (sys.argv[2], '../../../include/idigi_config.h'), '%s/%s' % (sys.argv[1], 'config.ini'))
