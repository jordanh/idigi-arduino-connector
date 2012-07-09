#!/usr/bin/env python
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
""" Generates Makefile for a sample project from Makefile.template.
Uses path to sample project to determine contents of Makefile. """
import os
import sys
import re
import logging

log = logging.getLogger('makegen')
log.setLevel(logging.WARNING)

if len(log.handlers) == 0:
    handler = logging.StreamHandler()
    handler.setLevel(logging.INFO)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    handler.setFormatter(formatter)
    log.addHandler(handler)

from string import Template

# Template file to use for basis of Makefile.
TEMPLATE = os.path.join(os.path.dirname(__file__), "Makefile.template")

# Samples that only link and don't require platform code.
LINK_SAMPLES = ['compileandlinkformetrics', 'compile_and_link']

# Default Substitution Dictionary.
DEFAULT_SUBS = {
    # Target Platform to build for.
    'TARGET_PLATFORM'         :  """
# Target Platform
PLATFORM = linux""",
    
    # Location of Platform Directory.
    'PLATFORM_DIR'            : """
# Location of Platform Src Code.
PLATFORM_DIR=../../platforms/$(PLATFORM)""",
    
    # Vpath directive to identify platform source files.
    'PLATFORM_VPATH'          : """
vpath $(PLATFORM_DIR)/%.c""",

    # Include Platform Directory header files.
    'PLATFORM_HEADER_INCLUDE' : """# Include Platform Header Files.
CFLAGS += -I$(PLATFORM_DIR)""",

    # Application sorce files to use
    'APP_SRCS'              : '',
    # Platform source files to use (will resolve based on sample name)
    'PLATFORM_SRCS'           : '',
    # Include all Application, Platform, and Private source.  
    #If Platform not needed, not included.
    'SRCS'                    : "SRCS = $(APP_SRCS) $(PLATFORM_SRCS) $(PRIVATE_SRCS)",
    # LIBS to include, -pthread will be added if 'run' type.  
    # connect_on_ssl adds '-lssl', file_system adds APP_ENABLE_MD5 if.
    'LIBS'                    : "LIBS = -lc -lz",
    # Where to find iDigi private code.
    'IDIGI_DIR'               : "../../../../private",
    # Where to find public header files.
    'PUBLIC_HEADER_DIR'       : "../../../include",
    # What to include in sample make process, if anything.
    'POST_SAMPLE'             : ""
}

def generate_makefile(path, make_template):
    """ Parses an input path to a sample directory and determines
    what libraries and platform files to include based on path.  Uses 
    make_template as an input Template object to make substitutes on.  
    See DEFAULT_SUBS for substitution patterns.

    :param path: Path to a sample directory.
    :param make_template: a Template object for a Makefile.
    """
    full_path = os.path.abspath(path)

    # List of *.c files in the directory.
    app_src = [ f for f in os.listdir(full_path) if f.endswith('.c') ]
    
    # Get the Mode and name of the Sample, this will be used for determining
    # what libraries and platform files to include.
    (_, mode, _, sample) = full_path.rsplit(os.sep, 3)

    subs = DEFAULT_SUBS.copy()

    dvt_test = path.find('dvt') != -1

    if dvt_test:
        subs['IDIGI_DIR'] = '../../../private'
        subs['PUBLIC_HEADER_DIR'] = '../../../public/include'
        subs['PLATFORM_DIR'] = """
# Location of Platform Src Code.
PLATFORM_DIR=../../../public/run/platforms/$(PLATFORM)"""

        # Change platform to 'template' if this is a template test.
        if sample == "template_test":
            subs["TARGET_PLATFORM"] = """
# Target Platform
PLATFORM = template"""

    # Treat compile and link as a special case, no platform used.
    if sample in LINK_SAMPLES:
        subs['TARGET_PLATFORM'] = ''
        subs['PLATFORM_DIR'] = ''
        subs['PLATFORM_VPATH'] = ''
        subs['PLATFORM_HEADER_INCLUDE'] = ''
        subs['SRCS'] = 'SRCS = $(APP_SRCS) $(PRIVATE_SRCS)'
    else:
        # Assume this is the base set of Platform files
        # Only include files who are also not in local directory.
        subs['PLATFORM_SRCS'] = 'PLATFORM_SRCS = '
        for f in ['os.c', 'config.c', 'debug.c', 'main.c']:
            if f not in app_src:
                subs['PLATFORM_SRCS'] += '$(PLATFORM_DIR)/%s ' % f

    # Add all *.c files in the directory.
    subs['APP_SRCS'] = 'APP_SRCS = ' + ' '.join([ re.sub(r'\.c$', '.c', f) \
        for f in app_src ])

    # Add -lpthread as a linked library if this is a run sample.
    if (dvt_test or mode == 'run') and sample not in LINK_SAMPLES:
        subs['LIBS'] += ' -lpthread' 

    if sample == 'connect_on_ssl' and 'network_ssl.c' not in app_src:
        # Add network_ssl.c to PLATFORM_SRCS and -lssl to LIBS.
        subs['PLATFORM_SRCS'] += ' $(PLATFORM_DIR)/network_ssl.c'
        subs['LIBS'] += ' -lssl'
    elif sample not in LINK_SAMPLES and 'network.c' not in app_src:
        #  Otherwise add network.c.
        subs['PLATFORM_SRCS'] += ' $(PLATFORM_DIR)/network.c'

    if sample == 'file_system' and 'file_system.c' not in app_src:
        # Add file_system.c to PLATFORM_SRCS.  -lcrypto if APP_ENABLE_MD5
        # passed.
        subs['PLATFORM_SRCS'] += " $(PLATFORM_DIR)/file_system.c"

    if sample == 'file_system':
        if dvt_test:
            subs['LIBS'] += """
APP_ENABLE_MD5=true"""

        subs['LIBS'] += """

ifeq ($(APP_ENABLE_MD5),true)
LIBS += -lcrypto
CFLAGS+= -DAPP_ENABLE_MD5=true
endif"""

    if sample == 'ic_timing':
        subs['LIBS'] += ' -lrt' 


    if dvt_test:
        subs['POST_SAMPLE'] = '$(AT)python ../../../dvt/scripts/iikmapsum.py $(SAMPLE).map | tee $(basename $@).stats'

    return make_template.substitute(**subs)

def make(path):
    # Read make template into a Template object.
    make_template = open(TEMPLATE, 'r')
    template_data = make_template.read()
    make_template.close()
    template = Template(template_data)

    directories = []

    # Generate Makefile for all samples assuming in base ic directory.
    if path == 'all':
        run_dir = os.path.abspath(os.path.join('.', 'public/run/samples'))
        step_dir = os.path.abspath(os.path.join('.', 'public/step/samples'))
        dvt_dir = os.path.abspath(os.path.join('.', 'dvt/samples'))

        for d in os.listdir(run_dir):
            run_d = os.path.join(run_dir, d)
            step_d = os.path.join(step_dir, d)

            if os.path.isdir(step_d):
                directories.append(step_d)
            if os.path.isdir(run_d):
                directories.append(run_d)

        for d in os.listdir(dvt_dir):
            sample_dir = os.path.join(dvt_dir, d)

            if os.path.isdir(sample_dir):
                directories.append(sample_dir)

    else:
        directories.append(os.path.abspath(path))

    for directory in directories:
        data = generate_makefile(directory, template)
        target_makefile = os.path.join(directory, 'Makefile')
        log.info("Generating Makefile in %s" % target_makefile)
        f = open(target_makefile, 'wb')
        f.write(data)
        f.close()

    log.info("Done")

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print "Usage: makegen.py [sample_directory|all]"
        sys.exit(-1)

    log.setLevel(logging.INFO)
    make(sys.argv[1])