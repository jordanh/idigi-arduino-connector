import ConfigParser
import os
import fileinput, string, sys
from optparse import OptionParser

new_lines = []

def replace_string(file_name, src_str, dst_str):
  infile = open(file_name, "r")
  text = infile.read()
  infile.close()
  outfile = open(file_name, "w")
  outfile.write(text.replace(src_str, dst_str))
  outfile.close()

def remove_errors(file_name):
    infile = open(file_name, "r")
    text = infile.read()
    infile.close()
    if text.find('//#error') == -1: # Don't run if #error is already commented out.
        #print "Replacing #error in [%s]" % file_name
        outfile = open(file_name, "w")
        outfile.write(text.replace('#error', '//#error'))
        outfile.close()

def update_field(config, line, field, isString):
  elements = line.split()
  if elements[0] != '#define':
    new_lines.append(line)
    return

  modified_line = elements[0] + '    ' + elements[1] + '    '
  if isString:
    modified_line += '"' + config.get("device", field) + '"' + '\n'  
  else:
    modified_line += config.get("device", field) + '\n'
  new_lines.append(modified_line)

def update_idigi_config_h(header_file, config):

  found_enable = False
  infile = open(header_file, "r")
  new_lines[:] = []

  for line in infile:
    if 'ENABLE_COMPILE_TIME_DATA_PASSING' in line:
      if found_enable == False:
        found_enable = True
        elements = line.split()
        if elements[0] == '#if':
          new_lines.append("#define   ENABLE_COMPILE_TIME_DATA_PASSING   1\n")
      new_lines.append(line)
    elif 'IDIGI_DEVICE_TYPE' in line:
      update_field(config, line, "device_type", True)
    elif 'IDIGI_CLOUD_URL' in line:
      update_field(config, line, "proxy_url", True)
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


def update_idigi_config_c(src_file, config):

  infile = open(src_file, "r")
  new_lines[:] = []

  for line in infile:
    if 'DEVICE_TX_KEEPALIVE_INTERVAL_IN_SECONDS' in line:
      update_field(config, line, "tx_keepalive", False)
    elif 'DEVICE_RX_KEEPALIVE_INTERVAL_IN_SECONDS' in line:
      update_field(config, line, "rx_keepalive", False)
    elif 'DEVICE_WAIT_COUNT' in line:
      update_field(config, line, "wait_count", False)
    elif 'IDIGI_MAX_MSG_TRANSACTIONS' in line:
      update_field(config, line, "max_transaction", False)
    else:
      new_lines.append(line)

  infile.close()
  outfile = open(src_file, "wb")
  for line in new_lines:
    line = line.replace('\r\n', '\n')
    outfile.write(line)
  outfile.close()

def update_config_c(cnfg_file, config, mac_addr):

  old_mac = '{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}'
  old_vendor_id = '{0x00, 0x00, 0x00, 0x00}'
  old_device_type = '"Linux Application"'
  old_server_url = '"developer.idigi.com"'

  new_mac = '{' + '0x' + mac_addr[0] + mac_addr[1] + ', '
  new_mac += '0x' + mac_addr[2] + mac_addr[3] + ', '
  new_mac += '0x' + mac_addr[4] + mac_addr[5] + ', '
  new_mac += '0x' + mac_addr[7] + mac_addr[8] + ', '
  new_mac += '0x' + mac_addr[9] + mac_addr[10] + ', '
  new_mac += '0x' + mac_addr[11] + mac_addr[12] + '}'

  vendor_id = config.getint("device", "vendor_id")
  hex_string = hex(vendor_id)
  new_vendor_id = '{' + '0x0' + hex_string[2] + ', ' 
  new_vendor_id += '0x' + hex_string[3:5] + ', '
  new_vendor_id += '0x' + hex_string[5:7] + ', '
  new_vendor_id += '0x' + hex_string[7:9] + '}'

  new_device_type = '"' + config.get("device", "device_type") + '"'
  new_server_url  = '"' + config.get("device", "proxy_url") + '"'

  replace_string(cnfg_file, old_mac, new_mac)
  replace_string(cnfg_file, old_vendor_id, new_vendor_id)
  replace_string(cnfg_file, old_device_type, new_device_type)
  replace_string(cnfg_file, old_server_url, new_server_url)

def update_config_header(header_file, ini_file):
  config = ConfigParser.SafeConfigParser()
  config.read(ini_file)
  update_idigi_config_h(header_file, config)

def update_config_source(cnfg_file, ini_file, mac_addr):
  config = ConfigParser.SafeConfigParser()
  config.read(ini_file)
  update_idigi_config_c(cnfg_file, config)
  update_config_c(cnfg_file, config, mac_addr)

