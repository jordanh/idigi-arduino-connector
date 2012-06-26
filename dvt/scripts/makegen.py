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

from string import Template

# Template file to use for basis of Makefile.
TEMPLATE = os.path.join(os.path.dirname(__file__), "Makefile.template")

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

    # Application Objects to use (will resolve to *.c in sample dir to *.o)
    'APP_OBJS'              : '',
    # Platform Objects to use (will resolve based on sample name)
    'PLATFORM_OBJS'           : '',
    # Include all Application, Platform, and Private Objects.  
    #If Platform not needed, not included.
    'OBJS'                    : "OBJS = $(APP_OBJS) $(PLATFORM_OBJS) $(PRIVATE_OBJS)",
    # LIBS to include, -pthread will be added if 'run' type.  
    # connect_on_ssl adds '-lssl', file_system adds APP_ENABLE_MD5 if.
    'LIBS'                    : "LIBS = -lc -lz"
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
    
    # Get the Mode and name of the Sample, this will be used for determining
    # what libraries and platform files to include.
    (_, mode, _, sample) = full_path.rsplit(os.sep, 3)

    subs = DEFAULT_SUBS.copy()

    # Treat compile and link as a special case, no platform used.
    if sample == 'compile_and_link':
        subs['TARGET_PLATFORM'] = ''
        subs['PLATFORM_DIR'] = ''
        subs['PLATFORM_VPATH'] = ''
        subs['PLATFORM_HEADER_INCLUDE'] = ''
        subs['OBJS'] = 'OBJS = $(APP_OBJS) $(PRIVATE_OBJS)'
    else:
        # Assume this is the base set of Platform Objects 
        subs['PLATFORM_OBJS'] = 'PLATFORM_OBJS = $(PLATFORM_DIR)/os.o $(PLATFORM_DIR)/config.o $(PLATFORM_DIR)/debug.o $(PLATFORM_DIR)/main.o'

    # Resolve Local Objs to all *.c files in the directory.
    subs['APP_OBJS'] = 'APP_OBJS = ' + ' '.join([ re.sub(r'\.c$', '.o', f) \
        for f in os.listdir(full_path) if f.endswith('.c') ])

    # Add -lpthread as a linked library if this is a run sample.
    if mode == 'run' and sample != 'compile_and_link':
        subs['LIBS'] += ' -lpthread' 

    if sample == 'connect_on_ssl':
        # Add network_ssl.o to PLATFORM_OBJS and -lssl to LIBS.
        subs['PLATFORM_OBJS'] += ' $(PLATFORM_DIR)/network_ssl.o'
        subs['LIBS'] += ' -lssl'
    elif sample != 'compile_and_link':
        #  Otherwise add network.o.
        subs['PLATFORM_OBJS'] += ' $(PLATFORM_DIR)/network.o'

    if sample == 'file_system':
        # Add file_system.o to PLATFORM_OBJS.  -lcrypto if APP_ENABLE_MD5 
        # passed.
        subs['PLATFORM_OBJS'] += " $(PLATFORM_DIR)/file_system.o"
        subs['LIBS'] += """

ifeq ($(APP_ENABLE_MD5),true)
LIBS += -lcrypto
endif"""

    return make_template.substitute(**subs)

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print "Usage: makegen.py [sample_directory|all]"
        sys.exit(-1)

    # Read make template into a Template object.
    make_template = open(TEMPLATE, 'r')
    template_data = make_template.read()
    make_template.close()
    template = Template(template_data)

    directories = []

    # Generate Makefile for all samples assuming in base ic directory.
    if sys.argv[1] == 'all':
        run_dir = os.path.abspath(os.path.join('.', 'public/run/samples'))
        step_dir = os.path.abspath(os.path.join('.', 'public/step/samples'))

        for d in os.listdir(run_dir):
            run_d = os.path.join(run_dir, d)
            step_d = os.path.join(step_dir, d)

            if os.path.isdir(step_d):
                directories.append(step_d)
            if os.path.isdir(run_d):
                directories.append(run_d)
    else:
        directories.append(os.path.abspath(sys.argv[1]))

    for directory in directories:
        data = generate_makefile(directory, template)
        target_makefile = os.path.join(directory, 'Makefile')
        print "Generating Makefile in %s" % target_makefile
        f = open(target_makefile, 'wb')
        f.write(data)
        f.close()

    print "Done"
