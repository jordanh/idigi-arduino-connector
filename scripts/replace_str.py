import fileinput, string, sys
from optparse import OptionParser

parser = OptionParser(usage = "usage: %prog file_name src_str dst_str")
(options, args) = parser.parse_args()

file_name = args[0]
src_str = args[1]
dst_str = args[2]

infile = open(file_name, "r")
text = infile.read()
infile.close()

outfile = open(file_name, "w")
outfile.write(text.replace(src_str, dst_str))
outfile.close()

