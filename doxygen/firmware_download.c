/*! @page firmware_download Firmware Download
 *
 * @htmlinclude nav.html
 *
 * @section fw_overview1 Firmware Download
 *
 *  -# @ref fw_overview
 *  -# @ref fw_num_targets
 *  -# @ref fw_version_number
 *  -# @ref fw_code_size
 *  -# @ref fw_description
 *  -# @ref fw_namespec
 *  -# @ref fw_download
 *  -# @ref fw_image_data
 *  -# @ref fw_complete
 *  -# @ref fw_abort
 *  -# @ref fw_reset
 *
 * @section fw_overview Overview
 *
 * The firmware access facility is an optional facility for applications to update their 
 * firmware. The IIK invokes the application-defined callback to query whether this 
 * firmware access facility is supported or not.  The firmware access facility uses a target 
 * number to distinguish application-dependent firmware images.  For example, a
 * stand-alone bootloader may be loaded separate from an application image, or a default file system
 * could be maintained as well.
 * Applications define an image to each target except target 0 (target 0 must be the firmware 
 * image that is running the IIK). Only one firmware upgrade can be in progress at any given 
 * time. The IIK will send a firmware target list to the iDigi server to identify the number 
 * of target applications and the version number of each target. 
 *
 * The firmware portion of the application-defined callback is used for the following:
 *  -# To obtain the number of firmware downloadable images.
 *  -# To obtain firmware image information and descriptions.
 *  -# To process firmware image upgrades.
 *  -# To reset firmware images.
 *
 * A typical application-defined callback sequence for downloading a firmware image would include:
 *  -# IIK calls application-defined callback to return firmware information which 
 *     includes the firmware version number, maximum size of an image, firmware description, and file name spec.
 *  -# IIK calls application-defined callback to initiate firmware download.
 *  -# IIK calls application-defined callback for firmware binary data.
 *  -# Repeat step 3 until the entire firmware image data is completed.
 *  -# calls application-defined callback to complete firmware download.
 *  -# calls application-defined callback to reset and reboot the target to begin executing the new firmware.
 *  -# IIK calls application-defined callback to disconnect the current connection
 *
 * Applications may choose to begin writing their downloaded image to Flash either 
 * during the Firmware Binary or Firmware Download Complete message. This is an application 
 * level decision with clear tradeoffs (memory needs versus power loss protection). 
 * If you write to Flash while the Firmware Binary data is being received, you could lose communication
 * or power at some later time.  If protection is built in (i.e., back up images) this would not be
 * problem. If protections are not built in, applications might choose to place the entire 
 * image contents into RAM before writing to Flash. Back up image protection would still 
 * protect against power loss after your write occurred, but the communication loss or corruption 
 * could be avoided by having a checksum test before starting your Flash write.
 *
 * @note See @ref firmware_support under Configuration to enable or
 * disable firmware download.
 * 
 * @section fw_num_targets Number of Firmware Targets
 *
 * Return the number of targets for firmware download.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_target_count @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to timeout value which callback must return control back to IIK in seconds</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of unsigned timeout value</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>Pointer to 2-byte integer memory where callback writes the number of supported targets</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Successfully returned the number of targets supported</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIKArguments</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_target_count)
 *     {
 *          /* return total number of firmware update targets */
 *          *((uint16_t *)response_data) = firmware_list_count;
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 * @section fw_version_number Version Number of Firmware Target
 *
 * Return the version number of the target.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_version @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>Pointer to 4-byte integer memory where callback writes version number</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned version number</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIKArguments</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_version)
 *     {
 *          idigi_fw_config_t * config = (idigi_fw_config_t *)request_data;
 *          uint32_t * version = (uint32_t *)response_data;
 *          /* return the target version number */
 *          *version = fimware_list[config->target].version;
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 * @section fw_code_size Code Size of Firmware Target
 *
 * Return the code size that is currently stored for the target.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_code_size @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>Pointer to 4-byte integer memory where callback writes code size. If size is unknown, returns 0xFFFFFFFF.</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned image size</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_code_size)
 *     {
 *          idigi_fw_config_t * config = (idigi_fw_config_t *)request_data;
 *          /* Return the target code size */
 *          uint32_t * code_size = (uint32_t *)response_data;
 *          *code_size = fimware_list[config->target].code_size;
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 * @section  fw_description Description of Firmware Target
 *
 * Return the description of the target.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_description @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>Return pointer to address of an ASCII description string</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the length of the description string. The total length of description and name spec must not exceed 127 bytes.</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned description</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_description)
 *     {
 *          /* return pointer to firmware target description */
 *          char ** description = (char **)response_data;
 *          *description = fimware_list[config->target].description;
 *          *response_length = strlen(fimware_list[config->target].description);
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 * @section fw_namespec File Name Spec of Firmware Target
 *
 * Return the file name spec as a regular expression.  No file name spec can be returned
 * if only a single target (target 0) is supported.  If an application wants to match the 
 * file name "image.bin", it must return "image\.bin" for regular expression. Regular 
 * expressions are case sensitive. If file names for the given target are to be case insensitive,
 * it must be specified as case insensitive in a regular expression such as: "[iI][mM][aA][gG][eE]\.[bB][iI][nN]".
 * The maximum length of the firmware description and file name spec combined is 127 bytes.
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_name_spec @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>Return pointer to address of regular expression string</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>Pointer to memory where callback writes the length of the description string. The total length of description and name spec must not exceed 127 bytes.</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully returned file name spec</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_name_spec)
 *     {
 *           /* return pointer to firmware target description */
 *          char ** name_spec = (char **)response_data;
 *          *name_spec = fimware_list[config->target].name_spec;
 *          *response_length = strlen(fimware_list[config->target].name_spec);
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 * @section fw_download File Firmware Download Request
 *
 * Callback is called to start firmware download when IIK receives a firmware download request message.
 * IIK parses the information in the firmware download request and passes the information to the callback:
 *  -# A target number which target the firmware is intended for.
 *  -# The name of the file to be sent. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_download_request @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_download_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_download_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>Pointer to memory where callback writes the @endhtmlonly @ref idigi_fw_status_t @htmlonly to</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully set appropriate status in response_data</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_download_request)
 *     {
 *          idigi_fw_download_request_t  * const download_info = request_data;
 *          idigi_fw_status_t * download_status = response_data;
 *          if (firmware_download_started)
 *          {   /* already started */
 *              *download_status = idigi_fw_device_error;
 *              return idigi_callback_continue;
 *          }
 *
 *          APP_DEBUG("target = %d\n",         download_info->target);
 *          APP_DEBUG("filename = %s\n",       download_info->filename);
 *          /* initialize & prepare for firmware update */
 *          total_image_size = 0;
 *          firmware_download_started = 1;
 *          *download_status = idigi_fw_success;
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 *
 * @section fw_image_data Binary Image Data for Firmware Download
 *
 * Callback is called to process image data when IIK receives a firmware binary block 
 * message. IIK calls this callback for each block of data received from the server.
 * IIK parses the information in the firmware binary block and passes the information to the callback:
 *  -# 4-octet offset that determines where the block of binary data fits into the download.
 *  -# Binary data.
 *  -# Length of the binary data. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_binary_block @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_image_data_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_image_data_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>Pointer to memory where callback writes the @endhtmlonly @ref idigi_fw_status_t @htmlonly to</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully set appropriate status in response_data</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Callback is busy and will be called again with same binary image data</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_binary_block)
 *     {
 *          idigi_fw_image_data_t * const image_data = request_data;
 *          idigi_fw_status_t * data_status = response_data;
 *
 *          fwStoreImage(image_data->target, image_data->data, image_data->length, image_data->offset);
 *          *data_status = idigi_fw_success;
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 * @section fw_complete Firmware Download Complete
 *
 * Callback is called when server is done sending all image data. This callback tells IIK 
 * when target has been completely updated.
 * If this callback returns:
 *  -# BUSY status indicating the firmware download is still in process, IIK will 
 * call this callback again. This usually indicates that image data is still being written onto flash.
 *  -# CONTINUE status indicating the firmware download has been successfully completed, IIK will 
 * send a firmware download complete response to the server.
 *  -# ABORT status, IIK will send a firmware download abort message to the server, stop and exit with error status. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_download_complete @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_download_complete_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_download_complete_request_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>Pointer to memory where callback writes the @endhtmlonly idigi_fw_download_complete_response_t @htmlonly to</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback successfully set appropriate status in response_data</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_busy @htmlonly</td>
 * <td>Callback is busy and will be called again</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_download_complete)
 *     {
 *         idigi_fw_download_complete_request_t * const complete_request = request_data;
 *         idigi_fw_download_complete_response_t * complete_response = response_data;
 *
 *         if (complete_request->code_size != total_image_size)
 *         {
 *             complete_response->status = idigi_fw_download_not_complete;
 *         }
 *         else
 *         {
 *            complete_response->status = idigi_fw_download_success;
 *         }
 *
 *         fwCloseImage(complete_request->target, complete_response->status);
 *         firmware_download_started = 0;
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 * @section fw_abort Firmware Download Abort
 *
 * Called to abort firmware download when IIK receives a firmware download abort message. *
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_download_abort @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_download_abort_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_download_abort_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback acknowledged that server aborted download process</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == digi_firmware_abort)
 *     {
 *         idigi_fw_download_abort_t * const abort_data = request_data;
 *         fwCloseImage(abort_data->target, abort_data->status);
 *         firmware_download_started = 0;
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 * @section fw_reset Firmware Target Reset
 *
 * Callback is called when server resets target. The callback should not return if 
 * it's resetting itself. It may return and continue. However, server may disconnect the device. 
 *
 * @htmlonly
 * <table class="apitable">
 * <tr> <th colspan="2" class="title">Arguments</th> </tr> 
 * <tr><th class="subtitle">Name</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>class_id</td>
 * <td>@endhtmlonly @ref idigi_class_firmware @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_id</td>
 * <td>@endhtmlonly @ref idigi_firmware_target_reset @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_data</td>
 * <td>Pointer to @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>request_length</td>
 * <td>Size of @endhtmlonly idigi_fw_config_t @htmlonly</td>
 * </tr>
 * <tr>
 * <td>response_data</td>
 * <td>N/A</td>
 * </tr>
 * <tr>
 * <td>response_length</td>
 * <td>N/A</td>
 * </tr>
 * <tr> <th colspan="2" class="title">Return Values</th> </tr> 
 * <tr><th class="subtitle">Values</th> <th class="subtitle">Description</th></tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_continue @htmlonly</td>
 * <td>Callback acknowledged that server reset the target</td>
 * </tr>
 * <tr>
 * <td>@endhtmlonly @ref idigi_callback_abort @htmlonly</td>
 * <td>Callback aborted IIK</td>
 * </tr>
 * </table>
 * @endhtmlonly
 *
 * Example:
 *
 * @code
 *
 * idigi_callback_status_t idigi_callback(idigi_class_t const class_id, idigi_request_t const request_id
 *                              void * const request_data, size_t const request_length,
 *                              void * response_data, size_t * const response_length)
 * {
 *
 *     if (class_id = idigi_class_firmware && request_id.firmware_request == idigi_firmware_target_reset)
 *     {
 *         idigi_fw_config_t * config = (idigi_fw_config_t *)request_data;
 *         fwResetImage(config->target);
 *     }
 *     return idigi_callback_continue;
 * }
 * @endcode
 *
 */
