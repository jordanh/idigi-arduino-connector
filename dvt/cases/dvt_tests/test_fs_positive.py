# -*- coding: utf-8 -*-
import time
import iik_testcase
import datetime
import re
import os
import base64
import xml.dom.minidom
import hashlib

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

    def test_fs1_basic_cases(self):
        """ Tests put, get and remove on different sized files """
        global FileData

        for i in xrange(MAX_TEST_FILE_SIZE):
            FileData += chr(randint(0, 255))

        for fname, fsize in FileList:
            putData = FileData[:fsize]
            outData = base64.encodestring(putData)
            self.put_a_file(fname, outData, fsize)

            inData = self.get_a_file(fname, fsize)
            getData = base64.decodestring(inData)
            self.assertEqual(putData, getData, "Mismatch in the sent and received file [%s, put:%s, get:%s]" %(fname, putData, getData))

    def test_fs2_offset(self):
        """ Tests put and get with different offset and size on a file """
        fname = "dvt_fs_5.bin"

        offsetList =  [
            (100, 0),
            (511, 1),
            (8192, 512),
            (12345, 1580),
            (20000, MAX_TEST_FILE_SIZE-20000),
            ]

        for offset, fsize in offsetList:
            putData = FileData[:fsize]
            outData = base64.encodestring(putData)
            self.put_a_file(fname, outData, fsize, offset)

            inData = self.get_a_file(fname, fsize, offset)
            getData = base64.decodestring(inData)
            self.assertEqual(putData, getData, "Mismatch in the sent and received file [%s, put:%s, get:%s]" %(fname, putData, getData))

    def test_fs3_truncate(self):
        """ Tests put with different offset, size and truncate ON on list of files """
        global FileList

        truncateList =  [
            (0, 4),
            (0, 1),
            (500, 83),
            (1111, 580),
            (4321, 16037),
            ]

        for i in xrange(len(FileList)):
            offset, fsize = truncateList[i]
            fname, total = FileList[i]

            putData = FileData[:fsize]
            outData = base64.encodestring(putData)
            self.put_a_file(fname, outData, fsize, offset, True)

            FileList[i] = (fname, offset + fsize)

    def test_fs4_ls(self):
        """ Tests file system ls command """
        HashAlgoList = ['none', 'any', 'crc32', 'md5']

        FILE_NAME = 0
        FILE_TSTAMP = 1
        FILE_SIZE = 2
        FILE_HASH = 3
        my_path = './'
        for hashAlgo in HashAlgoList:
            fList, dList = self.get_file_list(my_path, hashAlgo)
            for fname, fsize in FileList:
                fileFound = False
                fname = my_path + fname
                for eachFile in fList:
                    if eachFile[FILE_NAME] == fname:
                        fileFound = True
                        if hashAlgo == 'md5' or hashAlgo == 'any':
                            hash_value = self.md5Checksum(fname)
                            hash_value = hash_value.upper();
                            eachFile[FILE_HASH] = eachFile[FILE_HASH].upper()
                            self.assertEqual(eachFile[FILE_HASH], hash_value , "Mismatch file hash [%s, expected:%s, actual:%s]" %(fname, hash_value, eachFile[3]))
                        self.assertEqual(eachFile[FILE_SIZE], str(fsize), "Mismatch file size [%s, expected:%d, actual:%s]" %(fname, fsize, eachFile[2]))
                self.assertEqual(fileFound, True, "File %s not in the file list" %fname)

            dirNames = ""
            my_public = my_path + "public"
            my_private = my_path + "private"
            for eachDir in dList:
                if eachDir[FILE_NAME] == my_private:
                    dirNames += eachDir[0] + ", "
                if eachDir[FILE_NAME] == my_public:
                    dirNames += eachDir[0]
            self.assertEqual(dirNames, my_private + ", " + my_public, "expected: private, public, found: %s" %dirNames)

        for fname, fsize in FileList:
            self.remove_a_file(fname)

    def put_a_file(self, fpath, fdata, fsize, offset = 0, truncate = False):
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
        response = self.api.sci(putRequest)

        # Success response?
        try:
            dom = xml.dom.minidom.parseString(response)
            response_buffer = dom.getElementsByTagName("put_file")
        except:
            raise ValueError(response)
        self.log.info("sent file \"%s\" " % fpath)

    def get_a_file(self, fpath, fsize, offset = 0):
        fdata = ""
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
        response = self.api.sci(getRequest)

        # Success response?
        try:
            dom = xml.dom.minidom.parseString(response)
            response_buffer = dom.getElementsByTagName("get_file")
            fdata = getText(response_buffer[0].getElementsByTagName("data")[0])
        except:
            raise ValueError(response)

        self.log.info("got file \"%s\" " % fpath)
        return fdata

    def remove_a_file(self, path):
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
        response = self.api.sci(rmRequest)

        # Success response?
        try:
            dom = xml.dom.minidom.parseString(response)
            response_buffer = dom.getElementsByTagName("rm")
        except:
            raise ValueError(response)
        self.log.info("\"%s\" is removed" % path)

    def get_file_list(self, path, hashAlgo):
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

        # Success response?
        try:
            dom = xml.dom.minidom.parseString(response)
            response_buffer = dom.getElementsByTagName("ls")
        except:
            raise ValueError(response)

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

    def md5Checksum(self, filePath):
        fh = open(filePath, 'rb')
        m = hashlib.md5()
        while True:
            data = fh.read(8192)
            if not data:
                break
            m.update(data)
        return m.hexdigest()

if __name__ == '__main__':
    unittest.main() 
