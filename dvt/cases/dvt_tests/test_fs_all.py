# -*- coding: utf-8 -*-
import time
import iik_testcase
import datetime
import re
import os
import base64
import xml.dom.minidom

from random import randint
from xml.dom.minidom import getDOMImplementation
impl = getDOMImplementation()

from utils import getText

MAX_TEST_FILE_SIZE = 23453
FileData = ""
FileList =  [
                ("dvt_fs_0.txt", 0),
                ("dvt_fs_1.txt", 1),
                ("dvt_fs_2.txt", 500),
                ("dvt_fs_4.txt", 1600),
                ("dvt_fs_5.bin", MAX_TEST_FILE_SIZE),
            ]

class FileSystemTestCase(iik_testcase.TestCase):

    def test_fs_basic_cases(self):
        """ Tests put, get and remove on different sized files """
        global FileData
        for i in xrange(MAX_TEST_FILE_SIZE):
            FileData += chr(randint(0, 255))

        for fname, fsize in FileList:
            putData = FileData[:fsize]
            outData = base64.encodestring(putData)
            self.put_a_file(fname, outData)

            inData = self.get_a_file(fname, fsize)
            getData = base64.decodestring(inData)
            self.assertEqual(putData, getData, "Mismatch in the sent and received file [%s, put:%s, get:%s]" %(fname, putData, getData))

            self.remove_a_file(fname)

    def put_a_file(self, filePath, fdata, offset = 0, truncate = False):
        putFileRequest = \
        """<sci_request version="1.0">
          <file_system>
            <targets>
              <device id="%s"/>
            </targets>
            <commands>
               <put_file path="%s" offset = "%d" truncate = "%r">
                  <data>%s</data>
               </put_file>
            </commands>
          </file_system>
        </sci_request>""" % (self.device_config.device_id, filePath, offset, truncate, fdata)

        self.log.info("Sending put file request for \"%s\" " % filePath)
        response = self.api.sci(putFileRequest)

        # Success response?
        try:
            dom = xml.dom.minidom.parseString(response)
            response_buffer = dom.getElementsByTagName("put_file")
        except:
            raise ValueError(response)
        self.log.info("Sent file \"%s\" " % filePath)

    def get_a_file(self, filePath, fileSize, offset = 0):
        fdata = ""
        getFileRequest = \
            """<sci_request version="1.0">
              <file_system>
                <targets>
                  <device id="%s"/>
                </targets>
                <commands>
                    <get_file path="%s" offset = "%d" length = "%d"/>
                </commands>
              </file_system>
              </sci_request>""" % (self.device_config.device_id, filePath, offset, fileSize)

        self.log.info("Sending get file request for \"%s\" " % filePath)
        response = self.api.sci(getFileRequest)

        # Success response?
        try:
            dom = xml.dom.minidom.parseString(response)
            response_buffer = dom.getElementsByTagName("get_file")
            fdata = getText(response_buffer[0].getElementsByTagName("data")[0])
        except:
            raise ValueError(response)

        self.log.info("Got file \"%s\" " % filePath)
        return fdata

    def remove_a_file(self, path):
        rmPathRequest = \
            """<sci_request version="1.0">
              <file_system>
                <targets>
                  <device id="%s"/>
                </targets>
                <commands>
                  <rm path="%s"/>
                </commands>
              </file_system>
            </sci_request>""" % (self.device_config.device_id, path)

        self.log.info("Sending rm request for \"%s\" " % path)
        response = self.api.sci(rmPathRequest)

        # Success response?
        try:
            dom = xml.dom.minidom.parseString(response)
            response_buffer = dom.getElementsByTagName("rm")
        except:
            raise ValueError(response)
        self.log.info("\"%s\" is removed" % path)

if __name__ == '__main__':
    unittest.main() 
