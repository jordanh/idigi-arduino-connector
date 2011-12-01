# finds all the sections in the map file and reports the sizes of each

from optparse import OptionParser

def print_kibibytes(text, value, type):

    value = value / 1000.0
    large = str(value);
    decimal = large.index('.')
    print '%10s %8s KiB %s' % (text, large[:decimal+4], type)
    
parser = OptionParser(usage = "usage: %prog [options] mapfile");
(options, args) = parser.parse_args()

if len(args) != 1:
    leave('no mapfile specified')

mapfile = args[0]
print mapfile
   
for line in open(mapfile):
    
    elements = line.split()

    if len(elements) == 3:
        elements = elements[:3]

        (section, hex_offset, hex_size) = elements
        
        if elements[0] =='.text':
            dec_size = int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '     program instructions')
        
        if elements[0] =='.rodata':
            dec_size = int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '     constant, read-only data')
        
        if elements[0] =='.data':
            dec_size = int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '     initialized static variables')
        
        if elements[0] =='.bss':
            dec_size = int(hex_size, 16)        
            print_kibibytes(section[1:], dec_size, '     uninitialized data')

print '-----------------------'
