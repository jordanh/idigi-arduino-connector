import ConfigParser
import os
import fileinput, string, sys
from optparse import OptionParser

new_lines = []

def update_field(config, line, field, isString):
  elements = line.split()
  if elements[0] != '/*':
    new_lines.append(line)
    return

  modified_line = elements[1] + '    ' + elements[2] + '    '
  if isString:
    modified_line += '"' + config.get("device", field) + '"' + '\n'  
  else:
    modified_line += config.get("device", field) + '\n'
  new_lines.append(modified_line)

def update_idigi_config_h(header_file, config):

  infile = open(header_file, "r")
  new_lines[:] = []

  for line in infile:
    if 'IDIGI_DEVICE_TYPE' in line:
      update_field(config, line, "device_type", True)
    elif 'IDIGI_CLOUD_URL' in line:
      update_field(config, line, "server_url", True)
    elif 'IDIGI_TX_KEEPALIVE_IN_SECONDS' in line:
      update_field(config, line, "tx_keepalive", False)
    elif 'IDIGI_RX_KEEPALIVE_IN_SECONDS' in line:
      update_field(config, line, "rx_keepalive", False)
    elif 'IDIGI_WAIT_COUNT' in line:
      update_field(config, line, "wait_count", False)
    elif 'IDIGI_VENDOR_ID' in line:
      update_field(config, line, "vendor_id", False)
    elif 'IDIGI_MSG_MAX_TRANSACTION' in line:
      update_field(config, line, "max_transaction", False)
    else:
      new_lines.append(line)

  infile.close()
  outfile = open(header_file, "wb")
  for line in new_lines:
    line = line.replace('\r\n', '\n')
    outfile.write(line)
  outfile.close()

def update_config_c(cnfg_file, config):
  old_mac = '{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}'

  infile = open(cnfg_file, "r")
  text = infile.read()
  infile.close()

  if text.find(old_mac) == -1:
    print 'warning: device_mac_addr in config.c is not %s\n' %old_mac
    raise NameError

  mac_str = config.get("device", "mac_addr")
  new_mac = '{' + '0x' + mac_str[0] + mac_str[1] + ', '
  new_mac += '0x' + mac_str[2] + mac_str[3] + ', '
  new_mac += '0x' + mac_str[4] + mac_str[5] + ', '
  new_mac += '0x' + mac_str[7] + mac_str[8] + ', '
  new_mac += '0x' + mac_str[9] + mac_str[10] + ', '
  new_mac += '0x' + mac_str[11] + mac_str[12] + '}'

  outfile = open(cnfg_file, "w")
  outfile.write(text.replace(old_mac, new_mac))
  outfile.close()

def update_config_files(header_file, ini_file, cnfg_file):
  config = ConfigParser.SafeConfigParser()
  config.read(ini_file)
  update_idigi_config_h(header_file, config)
  update_config_c(cnfg_file, config)

if __name__ == '__main__':
  parser = OptionParser(usage = "usage: %prog header_file ini_file cnfg_file")
  (options, args) = parser.parse_args()

  if len(args) != 3:
    print "Usage: python config.py <path>/idigi_config.h <path>/config.ini <path>/config.c"
    raise SyntaxError

  update_config_files(args[0], args[1], args[2])

