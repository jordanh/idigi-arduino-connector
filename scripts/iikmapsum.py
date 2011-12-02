# finds all the sections in the map file and reports the sizes of each

from optparse import OptionParser

def print_kibibytes(text, value, type):

    value = value / 1000.0
    large = str(value);
    decimal = large.index('.')
    print '%10s %8s KB %s' % (text, large[:decimal+4], type)
    
parser = OptionParser(usage = "usage: %prog [options] mapfile");
(options, args) = parser.parse_args()

if len(args) != 1:
    leave('no mapfile specified')

mapfile = args[0]
print mapfile

print '----------------------------------------------------------------------------'

found_section_rodata_str1_1 = 0
read_only_data_str1_1 = 0
   
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
        
        if elements[0] =='.rodata' and object_file.endswith('idigi_api.o'):
            dec_size = read_only_data_str1_1 + int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '    constant, read-only data')
        
        if elements[0] =='.data' and object_file.endswith('idigi_api.o'):
            dec_size = int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '    initialized global and static variables')
        
        if elements[0] =='.bss' and object_file.endswith('idigi_api.o'):
            dec_size = int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '    uninitialized data zeroed by init code at startup')

print '----------------------------------------------------------------------------'
