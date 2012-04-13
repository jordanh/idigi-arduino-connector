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

MAX_TEST_FILE_SIZE = 4321
FileData = ""
TEST_ERROR_BUSY = 0
TEST_ERROR_AT_START = 1
TEST_ERROR_AT_MIDDLE = 2
TEST_ERROR_AT_END = 3
TEST_ERROR_TIMEOUT = 4

class FileSystemErrorTestCase(iik_testcase.TestCase):

    def test_fs1_invalid_path(self):
        """ Tests get, put, rm and ls on a path which is not found """
        fname = "dummy/test_file.error"
        errorExpected = True
        putData = "abcd"
        fsize = len(putData)

        self.verify_get_file(fname, fsize, errorExpected)
        outData = base64.encodestring(putData)
        self.verify_put_file(fname, outData, fsize, errorExpected)
        self.verify_remove_file(fname, errorExpected)
        self.verify_file_list(fname, errorExpected)

    def test_fs2_invalid_offset(self):
        """ Tests get, put with invalid offset """
        fname = "test_file.error"
        putData = "abcd"
        fsize = len(putData)

        outData = base64.encodestring(putData)
        self.verify_put_file(fname, outData, fsize, errorExpected=True, offset=2)
        self.verify_put_file(fname, outData, fsize, errorExpected=False)
        self.verify_put_file(fname, outData, fsize, errorExpected=True, offset=fsize+1)
        self.verify_get_file(fname, fsize, errorExpected=True, offset=fsize+1)
        self.verify_remove_file(fname, errorExpected=False)

    def test_fs3_get_error(self):
        """ Tests file get with error returned by the connector at start, middle and end of read """
        fname = "dvt_fs_get_error.test"
        global FileData

        for i in xrange(MAX_TEST_FILE_SIZE):
            FileData += chr(randint(0, 255))

        fsize = len(FileData)    #idigi connector reads around 500 bytes at once
        outData = base64.encodestring(FileData)
        self.verify_put_file(fname, outData, len(FileData), errorExpected=False)
        self.verify_get_file(fname, fsize, errorExpected=False, offset=TEST_ERROR_BUSY)
        self.verify_get_file(fname, fsize, errorExpected=True, offset=TEST_ERROR_AT_START)
        self.verify_get_file(fname, fsize, errorExpected=True, offset=TEST_ERROR_AT_MIDDLE)
        self.verify_get_file(fname, fsize, errorExpected=True, offset=TEST_ERROR_AT_END)
        #self.verify_get_file(fname, fsize, errorExpected=True, offset=TEST_ERROR_TIMEOUT)
        self.verify_remove_file(fname, errorExpected=False)

    def verify_put_file(self, fpath, fdata, fsize, errorExpected, offset = 0, truncate = False):
        putRequest = \
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
          </sci_request>""" % (self.device_config.device_id, fpath, offset, truncate, fdata)

        self.log.info("put request file:\"%s\", offset:%d, size:%d" % (fpath, offset, fsize))
        self.verify_device_response(self.api.sci(putRequest), errorExpected)

    def verify_get_file(self, fpath, fsize, errorExpected, offset = 0):
        getRequest = \
            """<sci_request version="1.0">
              <file_system>
                <targets>
                  <device id="%s"/>
                </targets>
                <commands>
                    <get_file path="%s" offset = "%d" length = "%d"/>
                </commands>
              </file_system>
              </sci_request>""" % (self.device_config.device_id, fpath, offset, fsize)

        self.log.info("get request file:\"%s\", offset:%d, size:%d" % (fpath, offset, fsize))
        self.verify_device_response(self.api.sci(getRequest), errorExpected)

    def verify_device_response(self, response, errorExpected):
        # validate response?

        if (response.find('<error') == -1):
            errorActual = False
            errorString = "Expected error, but got success"
        else:
            errorActual = True
            errorString = "Expected success, but got error"

        self.assertTrue(errorExpected == errorActual, "%s, response[%s]" %(errorString, response))
        self.log.info("Test success!")

    def verify_remove_file(self, path, errorExpected):
        rmRequest = \
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
        self.verify_device_response(self.api.sci(rmRequest), errorExpected)

    def verify_file_list(self, path, errorExpected, hashAlgo="none"):
        lsRequest = \
            """<sci_request version="1.0">
              <file_system>
                <targets>
                  <device id="%s"/>
                </targets>
                <commands>
                  <ls path="%s" hash="%s"/>
                </commands>
              </file_system>
            </sci_request>""" % (self.device_config.device_id, path, hashAlgo)

        self.log.info("ls for path:\"%s\" and hash:%s" % (path, hashAlgo))
        response = self.api.sci(lsRequest)
        self.verify_device_response(response, errorExpected)

        return self.parse_ls(response)

    def get_obj_from_slice(self, segment):
        obj = []

        path_re = re.compile('path="([\.\w/-]+)"')
        last_re = re.compile('last_modified="([\.\w/-]+)"')
        size_re = re.compile('size="([\.\w/-]+)"')
        hash_re = re.compile('hash="([\.\w/-]+)"')

        pm = path_re.search(segment)
        obj.append((pm and pm.group(1)) or None)

        lm = last_re.search(segment)
        obj.append((lm and lm.group(1)) or None)

        sm = size_re.search(segment)
        obj.append((sm and sm.group(1)) or None)

        hm = hash_re.search(segment)
        obj.append((hm and hm.group(1)) or None)

        return obj

    def get_line_indexes(self, resp, starter):
        index = resp.find(starter)
        if index == -1:
            return None, None
        end_index = resp[index:].find('/>')
        self.assertTrue(end_index != -1, "Failed to parse out file from current response: %s" %resp)

        end_index += index

        return index, end_index

    def parse_ls(self, resp):
        file_list = []
        dir_list = []
        tmp_resp = resp
        while 1:
            index, end_index = self.get_line_indexes(tmp_resp, '<file ')
            if index is None:
                break

            segment = tmp_resp[index:end_index]
            obj = self.get_obj_from_slice(segment)
            file_list.append(obj)
            tmp_resp = tmp_resp[end_index:]

        tmp_resp = resp
        while 1:
            index, end_index = self.get_line_indexes(tmp_resp, '<dir')
            if index is None:
                break

            segment = tmp_resp[index:end_index]
            obj = self.get_obj_from_slice(segment)
            dir_list.append(obj)
            tmp_resp = tmp_resp[end_index:]

        self.log.info("Files: %d Dirs: %d parsed" %(len(file_list), len(dir_list)))
        return file_list, dir_list

if __name__ == '__main__':
    unittest.main() 
