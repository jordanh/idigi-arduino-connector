import ConfigParser
import logging
import os
import idigi_ws_api

log = logging.getLogger('configuration')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)



class DeviceConfiguration:
    
    def __init__(self, config_file = "config.ini"):
        # Retrieve configuration from file
        log.info("Reading configuration file.")
        config = ConfigParser.SafeConfigParser()
        config.read(config_file)
    
        # Parse device section of configuration file
        self.tx_keepalive = config.getint("device", "tx_keepalive")
        self.rx_keepalive = config.getint("device", "rx_keepalive")
        self.wait_count = config.getint("device", "wait_count")
 
        self.vendor_id = config.get("device", "vendor_id")
  
        self.mac_addr = config.get("device", "mac_addr")
        self.device_id = config.get("device", "device_id")
        self.device_type = config.get("device", "device_type")
    
        host = config.get("device", "server_url")
        self.ip_address = config.get("device", "ip_address")
    
        # Parse firmware targets
        self.num_firmware_targets = config.getint("firmware", "num_targets")
        self.firmware_target = []
        self.firmware_target_file = []
        self.data_service_target = {}
        
        for i in range(self.num_firmware_targets):
            try:
                file = config.get("firmware.target.%d" % i, "file")
                self.firmware_target_file.append(file)
            except ConfigParser.NoOptionError:
                self.firmware_target_file.append(None)
                
            self.firmware_target.append([config.get("firmware.target.%d" % i, "description"),
                            config.get("firmware.target.%d" % i, "name_spec"),
                            config.get("firmware.target.%d" % i, "version")])
            try:
                data_service = config.get("firmware.target.%d" % i, "data_service")
                if data_service == "True":
                    try:
                        description = config.get("firmware.target.%d" % i, "description")
                        self.data_service_target[description] = {"target":i,}
                        
                        try:
                            data_flag = config.get("firmware.target.%d" % i, "data_flag")
                            self.data_service_target[description]['data_flag'] = data_flag
                        except ConfigParser.NoOptionError:
                            pass
                        try:
                            file_name = config.get("firmware.target.%d" % i, "path")
                            self.data_service_target[description]['file_name'] = file_name
                        except ConfigParser.NoOptionError:
                            pass
                        try:
                            interval = config.get("firmware.target.%d" % i, "interval")
                            duration = config.get("firmware.target.%d" % i, "duration")
                            self.data_service_target[description]['interval'] = interval
                            self.data_service_target[description]['duration'] = duration
                        except ConfigParser.NoOptionError:
                            pass
                        try:
                            compression_flag = config.get("firmware.target.%d" % i, "compression")
                            self.data_service_target[description]['compression'] = compression_flag
                        except ConfigParser.NoOptionError:
                            pass
                    except ConfigParser.NoOptionError:
                        log.info("Data Service Firmware Target %d is improperly formatted" % i)
            except ConfigParser.NoOptionError:
                pass
                    
        
        if self.firmware_target[0]:
            self.firmware_version = self.firmware_target[0][2]
        else:
            self.firmware_version = None
 
        user_id = config.get("credentials", "user")
        password = config.get("credentials", "password")
        self.api = idigi_ws_api.Api(user_id, password, host)
        
if __name__ == '__main__':
    config = DeviceConfiguration("configTest.ini")
    