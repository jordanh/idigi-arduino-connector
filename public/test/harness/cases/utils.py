from __future__ import division
import datetime
import logging

log = logging.getLogger('utils')
log.setLevel(logging.INFO)

handler = logging.StreamHandler()
handler.setLevel(logging.DEBUG)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
log.addHandler(handler)

def convert_to_datetime(fd_time):

    """ Converts the date/time returned from iDigi to a datetime object"""
    
    dt = datetime.datetime(int(fd_time[0:4]), 
                                int(fd_time[5:7]), 
                                int(fd_time[8:10]),
                                int(fd_time[11:13]), 
                                int(fd_time[14:16]), 
                                int(fd_time[17:19]))
    return dt
    
def total_seconds(dt):

    """ Converts a datetime.delta to an int representing total seconds"""
    
    return (dt.microseconds + (dt.seconds + dt.days * 24 * 3600) * 10**6) / 10**6
    
def getText(elem):
    rc = []
    for node in elem.childNodes:
        if node.nodeType == node.TEXT_NODE:
            rc.append(node.data)
    return str(''.join(rc))
    
def clean_slate(api, file_location):
    
    """ Checks if a file exists. If it does, the file is deleted."""
    
    log.info("Checking to make sure file does not exist")
    try:
        response = api.get_raw(file_location)
        log.info("File already exists... deleting...")
        api.delete_location(file_location)
    except Exception:
        pass