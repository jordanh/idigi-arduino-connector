import config_tool_validator

from com.digi.ic.config import Parser
from com.digi.ic.config import ConfigData
from java.io import IOException

import os
import tempfile

import nose
from nose.tools import *

def process_and_verify(test):
    config_file = tempfile.NamedTemporaryFile(delete=False)
    try:
        config_file.write(test.text)
        config_file.close()
        config_data = ConfigData()

        if test.error is not None:
            assert_raises_regexp(IOException, test.error, 
                Parser.processFile, config_file.name, config_data)
        else:
            try:
                Parser.processFile(config_file.name, config_data)
            except IOException, e:
                assert_equal(1, 2, e)
    finally:
        os.remove(config_file.name)


def test_configurations():
    for test in config_tool_validator.get_list():
        yield process_and_verify, test

if __name__ == "__main__":
    nose.run()
