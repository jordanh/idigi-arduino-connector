
import fileinput, string, sys
from optparse import OptionParser

def replace_string(file_name, src_str, dst_str):
  infile = open(file_name, "r")
  text = infile.read()
  infile.close()

  outfile = open(file_name, "w")
  outfile.write(text.replace(src_str, dst_str))
  outfile.close()

if __name__ == '__main__':
  parser = OptionParser(usage = "usage: %prog file_name src_str dst_str")
  (options, args) = parser.parse_args()

  replace_string(args[0], args[1], args[2])

