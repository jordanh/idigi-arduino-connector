                      Release Notes for 40003007_C
                     iDigi Integration Kit vVERSION
                           January 27, 2012

ENHANCEMENTS

    v1.1

        Added support for device requests from the iDigi device cloud, the IIK
        now supports bidirectional data transfers.

        Updated the original User Guide with an online software centric 
        HTML-based User Guide with more hot links and tighter integration with 
        software distribution.

        Reduced the dynamic memory needed to run the IIK.  By adding support for 
        the idigi_config_max_transaction callback (or the compile time 
        IDIGI_MSG_MAX_TRANSACTION definition), a limited memory model is used 
        instead of a unbounded model.  For minimal use, the 
        idigi_config_max_transaction (or IDIGI_MSG_MAX_TRANSACTION) should be
        set to 1.   
       
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

    None
