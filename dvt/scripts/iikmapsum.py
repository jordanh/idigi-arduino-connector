# finds all the sections in the map file and reports the sizes of each

from optparse import OptionParser

# change this value if the size increase is not accidental
text_size_in_bytes   = 16664
rodata_size_in_bytes = 380
data_size_in_bytes   = 0
bss_size_in_bytes    = 0

both_enabled = 0
debug_disabled = 1
compression_disabled = 2
both_disabled = 3

buildOptions = both_disabled
buildTypeString = ""

parser = OptionParser(usage = "usage: %prog [options] mapfile");
(options, args) = parser.parse_args()

if len(args) != 1:
    leave('no mapfile specified')

mapfile = args[0]
print mapfile

print '----------------------------------------------------------------------------'

MEMORY_USAGE_FILE = '../../memory_usage.txt'

def check_release_build():
    global buildOptions
    global buildTypeString
    config_file_name = "../../../public/include/idigi_config.h"

    isDebug = False
    isCompression = False
    infile = open(config_file_name, "r")
    text = infile.read()
    infile.close()
    if text.find('IDIGI_NO_DEBUG') == -1:
        isDebug = True
    if text.find('IDIGI_NO_COMPRESSION') == -1:
        isCompression = True

    # can use bit operation also
    if isDebug and isCompression:
        buildOptions = both_enabled
    elif (not isDebug) and isCompression:
        buildOptions = debug_disabled
    elif isDebug and (not isCompression):
        buildOptions = compression_disabled
    else:
        buildOptions = both_disabled

    try:
        mem_usage_fd = open(MEMORY_USAGE_FILE, 'r')
    except IOError:
        mem_usage_fd = open(MEMORY_USAGE_FILE, 'w')
        mem_usage_fd.close()
        mem_usage_fd = open(MEMORY_USAGE_FILE, 'r')
    mem_usage_text = mem_usage_fd.read()
    mem_usage_fd.close()
    if (mem_usage_text.find('All Enabled') == -1) and (buildOptions == both_enabled):
        buildTypeString = 'All Enabled'
    elif (mem_usage_text.find('Debug Disabled') == -1) and (buildOptions == debug_disabled):
        buildTypeString = 'Debug Disabled'
    elif (mem_usage_text.find('Compression Disabled') == -1) and (buildOptions == compression_disabled):
        buildTypeString = 'Compression Disabled'
    elif mem_usage_text.find('All Disabled') == -1:
        buildTypeString = 'All Disabled'

check_release_build()
mem_usage_fd = open(MEMORY_USAGE_FILE, 'a')

def verify_size_limit (actual_size, expected_size):
    if buildOptions == debug_disabled and buildOptions == both_disabled:
        if actual_size > expected_size:
            raise AssertionError

def print_kibibytes(text, value, type):

    value = value / 1000.0
    large = str(value);
    decimal = large.index('.')
    print '%10s %8s KB %s' % (text, large[:decimal+4], type)
    if buildTypeString != "":
        mem_usage_fd.write('%10s %8s KB %s\n' % (text, large[:decimal+4], type))

found_section_rodata_str1_1 = 0
read_only_data_str1_1 = 0
   
if buildTypeString != "":
    mem_usage_fd.write('-------- %s --------\n' %buildTypeString)

for line in open(mapfile):
    
    elements = line.split()

# This code is needed because we need to add the size of section '.rodata.str1.1'
# which is on the next line

# --------------------------------------------------------------

    if found_section_rodata_str1_1 == 1:     
        found_section_rodata_str1_1 = 0
        if len(elements) == 3:
            elements = elements[:3]
            (hex_offset, hex_size, object_file) = elements        
            if object_file.endswith('idigi_api.o'):
                read_only_data_str1_1 += int(elements[1], 16)
                continue

    if len(elements) == 1:
        elements = elements[:1]

        (section) = elements

        if elements[0] == '.rodata.str1.1':
            found_section_rodata_str1_1 = 1
            continue

# --------------------------------------------------------------

    if len(elements) == 4:
        elements = elements[:4]

        (section, hex_offset, hex_size, object_file) = elements
        
        if elements[0] =='.text' and object_file.endswith('idigi_api.o'):
            dec_size = int(hex_size, 16)
            print_kibibytes(section[1:], dec_size, '    program instructions')
            verify_size_limit(dec_size, text_size_in_bytes)

        if elements[0] =='.rodata' and object_file.endswith('idigi_api.o'):
            dec_size = read_only_data_str1_1 + int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '    constant, read-only data')
            verify_size_limit(dec_size, rodata_size_in_bytes)
        
        if elements[0] =='.data' and object_file.endswith('idigi_api.o'):
            dec_size = int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '    initialized global and static variables')
            verify_size_limit(dec_size, data_size_in_bytes)
        
        if elements[0] =='.bss' and object_file.endswith('idigi_api.o'):
            dec_size = int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '    uninitialized data zeroed by init code at startup')
            verify_size_limit(dec_size, bss_size_in_bytes)

if buildTypeString != "":
    mem_usage_fd.write('--------------------------------\n')

mem_usage_fd.close()
print '----------------------------------------------------------------------------'
