"""A simple main to fix the config.c file used for samples"""
from build_utils import setup_platform
from config import update_config_header
import sys

if __name__ == "__main__":
    setup_platform(*sys.argv[1:])
