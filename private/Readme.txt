                      Release Notes for 40003007_E
                        iDigi Connector vVERSION
                             _RELEASE_DATE_ 

ENHANCEMENTS

    v1.2
     
        Support for Remote configuration (using the rci_request payload mechanism
        within the send_message operation) has been added.  This allows iDigi 
        Connector applications to develop configurations that can be remotely 
        managed (read/write) through iDigi Manager Pro or a web services client.  
        Note this feature requires a C99 Compiler.
        
        Support for Remote File Systems (using the file_system operation) has 
        been added.  This allows remotely connected devices to easily connect
        to standard file system commands (i.e., open, close, read, write, stat) 
        and have the file system contents displayed through iDigi Manager Pro or 
        through a web services client. 
        
        Additional examples demonstrating secure connections using OpenSSL.  This 
        allows sensitive applications to easily include greater levels of network 
        security.  
        
    v1.1

        Added support for device requests from the iDigi device cloud, the IIK
        now supports bidirectional data transfers.

        Updated the original User Guide with an online software centric 
        HTML-based User Guide with more hot links and tighter integration with 
        software distribution.

        Reduced the amount of dynamic memory required to execute the IIK.  With 
        added support for the idigi_config_max_transaction callback (or the 
        compile time IDIGI_MSG_MAX_TRANSACTION definition), a limited memory 
        approach is employed instead of an unbounded model.  For minimal memory 
        usage, idigi_config_max_transaction (or IDIGI_MSG_MAX_TRANSACTION) 
        should be set to 1.   
       
DEPRECATED FUNCTIONS AND CHANGES

    v1.1
     
        Deprecated the v1.0 Data Service call routine methodology, which used
        several idigi_initiate_action() calls, with the request argument cast to 
        an idigi_data_request_t structure and the flag field set to 
        IDIGI_DATA_REQUEST_START, IDIGI_DATA_REQUEST_LAST, etc.  This now uses
        a single idigi_initiate_action() call, with a IDIGI_DATA_PUT_ARCHIVE or
        IDIGI_DATA_PUT_APPEND flag, along with a several
        idigi_data_service_device_request application callbacks.
        
        Note legacy applications can choose to compile the IIK library without
        the IDIGI_VERSION number defined, which will compile in the required
        compatibility sections.

KNOWN ISSUES

    None 
    
BUG FIXES

    v1.2
     
        Corrected all platforms to include handling for unrecognized iDigi 
        Callback cases with an idigi_callback_unrecognized return value.  This 
        was missing in v1.1 and will allow v1.2 applications to port correctly 
        to v1.3 and beyond. 
