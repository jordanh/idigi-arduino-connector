import ConfigParser
import logging
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
    
        self.tx_keepalive = config.getint("device", "tx_keepalive")
        self.rx_keepalive = config.getint("device", "rx_keepalive")
        self.wait_count = config.getint("device", "wait_count")
 
        self.vendor_id = config.get("device", "vendor_id")
  
        self.mac_addr = config.get("device", "mac_addr")
        self.device_id = config.get("device", "device_id")
        self.device_type = config.get("device", "device_type")
    
        host = config.get("device", "server_url")
        self.ip_address = config.get("device", "ip_address")
    
        self.num_firmware_targets = config.getint("firmware", "num_targets")
        self.firmware_target = []
        self.firmware_target_file = []
        for i in range(self.num_firmware_targets):
            self.firmware_target_file.append(config.get("firmware.target.%d" % i, "file"))
            self.firmware_target.append([config.get("firmware.target.%d" % i, "description"),
                            config.get("firmware.target.%d" % i, "name_spec"),
                            config.get("firmware.target.%d" % i, "version"),
                            "4294967295"])
    #                       os.stat(firmware_target_file[i])[ST_SIZE])
    
        if self.firmware_target[0]:
            self.firmware_version = self.firmware_target[0][2]
        else:
            self.firmware_version = None
 
        user_id = config.get("credentials", "user")
        password = config.get("credentials", "password")
        self.api = idigi_ws_api.Api(user_id, password, host)
        