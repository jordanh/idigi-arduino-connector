import os
import time
import iik_testcase

#indices of test_table
EXPECTED_RESPONSE = 0

#              EXPECTED_RESPONSE
test_table = [
              ["idigi_config_vendor_id,idigi_invalid_data_size,terminated\n"],
              ["idigi_config_device_type,idigi_invalid_data_size,terminated\n"],
              ["idigi_config_device_id,idigi_invalid_data_size,terminated\n"],
              ["idigi_config_server_url,idigi_invalid_data_range,terminated\n"],
              ["idigi_config_rx_keepalive,idigi_invalid_data_range,terminated\n"],
              ["idigi_config_ip_addr,idigi_invalid_data_size,terminated\n"],  
]

class ResponseToBadValuesDvtTestCase(iik_testcase.TestCase):
    
    def test_debug_response_to_bad_values(self):
    
        self.log.info("***** Beginning Response to Bad Values Test *****")

        filename = "%s.txt" % self.device_config.device_id

        errors = 0          

        # We should see the expected error reponses in filename
        try:
            f = open(filename, 'r')
            self.log.info("Successfully opened %s" % filename)
            lines = f.readlines()
            numLines = len(lines)
            f.close()
        except IOError:
            self.log.info("Failure opening file %s" % filename)  
            errors = errors + 1

        index = 0

        for test in test_table:
            line = lines[index] 
            if line == test[EXPECTED_RESPONSE]:
                self.log.info("Successfully compared %s" % test[EXPECTED_RESPONSE])
            else:
                self.log.info("Error comparing line %s with test_table %s" % (line, test[EXPECTED_RESPONSE]))
                errors = errors + 1
            index += 1

        os.unlink(filename)
    
        self.log.info("***** Ending Response to Bad Values Test *****")

        if errors != 0:
            self.fail("Failed %d" % errors)        
        
if __name__ == '__main__':
    unittest.main()
