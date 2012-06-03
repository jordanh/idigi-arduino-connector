/*
* Copyright (c) 2011, 2012 Digi International Inc.,
* All rights not expressly granted are reserved.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
* =======================================================================
*/
#include <mqx.h>
#include "idigi_api.h"
#include "platform.h"
#include "firmware_mqx.h"
#include "main.h"

extern int FlashWriteInProgress;
extern char *download_buffer;
extern uint_32 image_data_length;

#define FLASH_NAME "flashx:bank1"

/************************************************************************
*     Definitions.
*************************************************************************/
#define FAPP_TFTP_FAILED            "\nFailed!"
#define FAPP_TFTP_ERR               "\nTFTP Error: Code %d \"%s\"."
#define FAPP_TFTP_CHECKSUM_ERR      "\nChecksum error."

#define FAPP_TFTP_RX_HEADER_STR     "TFTP downloading \'%s\' (%s) from %s:  "
#define FAPP_TFTP_TX_HEADER_STR     "TFTP uploading \'%s\' (%s) to %s:  "  
#define FAPP_TFTP_ENTRYPOINT_STR    "\nEntry point set to 0x%08X\n"
#define FAPP_TFTP_COMPLETED_STR     "\nTFTP completed (%d bytes)"

#define FAPP_MEM_ERROR_WRITEFAILED  "\n Writing %d bytes to 0x%08X failed!"

struct fapp_tftp_rx_handler_srec srec;

MQX_FILE_PTR   flash_file;

void idigi_flash_task(unsigned long initial_data)
{
    int result, i = 0;

    uint_32 current_address, num_sectors, sector_size, ioctl_param;
    
    /* Open the flash device */
    flash_file = fopen(FLASH_NAME, NULL);
    if (flash_file == NULL) {
    	APP_DEBUG("\nUnable to open file %s\n", FLASH_NAME);
       _task_block();
    } else {
    	APP_DEBUG("\nFlash file %s opened\n", FLASH_NAME);
    }

    /* Get the size of the flash file */
    fseek(flash_file, 0, IO_SEEK_END);
    APP_DEBUG("\nSize of the flash file: 0x%x Bytes", ftell(flash_file));
    
    /* Get the number of sectors in the Flash File */    
    ioctl(flash_file, FLASH_IOCTL_GET_NUM_SECTORS, &num_sectors);
    APP_DEBUG("\nThe number of sectors in the flash file: %d\n", num_sectors);
    
    /* Get the sector size of the current sector (assuming all sectors are the same size) */
    fseek(flash_file, -PFLASH_BLOCK1_BASE, IO_SEEK_END);    
    ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_SIZE, &sector_size);
    APP_DEBUG("\nThe sector size is: %d\n", sector_size);

    /* Enable sector cache */
    ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    printf("\nFlash sector cache enabled.");

    /* Unprotecting the the FLASH might be required */
    ioctl_param = 0;
    ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);
       
    /*
     * Erase the 2nd half of Flash a sector at a time except for the last 2 sectors
     * which are used by the bootloader for configuration data
     */
    
    for (i = 0; i < num_sectors - 2; i ++)
    {
        /* Set the current address of Flash to the beginning of the 2nd bank of the flash file */
        fseek(flash_file, -PFLASH_BLOCK1_BASE + (sector_size * i), IO_SEEK_END);    
        ioctl(flash_file, FLASH_IOCTL_GET_SECTOR_BASE, &current_address);
        APP_DEBUG("\nCurrent address of the flash file: 0x%x\n", current_address);
    
        /* Erase Current Sector */
        ioctl(flash_file, FLASH_IOCTL_ERASE_SECTOR, NULL);
    }
    
    for (;;)
    {        
    	if (FlashWriteInProgress)
    	{
            idigi_rx_handler_srec((unsigned char*)download_buffer, (uint_32)image_data_length);
            FlashWriteInProgress = 0;
    	}
       
        _time_delay(10);
    }
}

/************************************************************************
* NAME: fapp_tftp_rx_handler_srec
*
* DESCRIPTION: 
************************************************************************/
int idigi_rx_handler_srec (unsigned char* data, unsigned long n)
{
	int result = FNET_OK, result2;
    unsigned short result1;
    char tmp[2];
    char tmp_data;
    char *p = 0;
    int i;
    unsigned char checksum;
	unsigned long address, current_address;
	static unsigned long flash_address_offset = 0;
	static int first_time = 1;
    
    tmp[1]='\0';
    
    APP_DEBUG("srec.state [%d]\n", srec.state);
    
    while(n && (result == FNET_OK))
    {
        switch(srec.state)
        {
            default:
            case FAPP_TFTP_RX_HANDLER_SREC_STATE_INIT:
                srec.state = FAPP_TFTP_RX_HANDLER_SREC_STATE_GETSTART;
                break;
            case FAPP_TFTP_RX_HANDLER_SREC_STATE_GETSTART:
                if(*data++ == 'S')
                {
                    srec.state = FAPP_TFTP_RX_HANDLER_SREC_STATE_GETTYPE;
                }
                n--;
                break;
            case FAPP_TFTP_RX_HANDLER_SREC_STATE_GETTYPE:
                srec.type = *data++;
                n--;
                srec.record_hex_index=0; /* Reset hex index. */
                srec.record.count = 0xFF; /* Trick. */
                srec.state = FAPP_TFTP_RX_HANDLER_SREC_STATE_GETDATA;
                break;
            case FAPP_TFTP_RX_HANDLER_SREC_STATE_GETDATA:
                tmp[0] = (char)*data++;
                n--;
                tmp_data = (char)strtoul(tmp,&p,16); /* Char to integer.*/
                if ((tmp_data == 0) && (p == tmp))
                {
                    result = FNET_ERR;
                    break;
                }
                
                srec.record_bytes[srec.record_hex_index>>1] = (unsigned char)((srec.record_bytes[srec.record_hex_index>>1] & (0xF<<(4*(srec.record_hex_index%2))))
                                                          + (tmp_data<<(4*((srec.record_hex_index+1)%2))));
                 
                if(srec.record_hex_index > ((srec.record.count<<1)))
                {
                    char type;
                    char *addr;
                    
                    /* Check checksum. */
                    checksum = 0;
                    for(i=0; i< srec.record.count; i++)
                    {
                        checksum+=srec.record_bytes[i];    
                    }
                   
                    if(srec.record_bytes[srec.record.count] != (unsigned char)~checksum)
                    {
                        result = FNET_ERR;
                        APP_DEBUG(FAPP_TFTP_CHECKSUM_ERR);
                        break;     
                    }
                    
                    /* Handle S[type].*/
                    type = (char)(srec.type - '0'); /* Convert from character to number.*/
                    if((type == 1)|| (type == 9))
                    {
                        addr = (char *)( ((unsigned long)((srec.record.data[0])&0xFFL)<< 8 ) + (unsigned long)((srec.record.data[1])&0xFFL));
                    
                    }
                    
                    if((type == 2)|| (type == 8))
                    {
                        addr = (char *)( ((unsigned long)((srec.record.data[0])&0xFFL)<< 16) + 
                             ((unsigned long)((srec.record.data[1])&0xFFL)<< 8 ) + (unsigned long)((srec.record.data[2])&0xFFL));
                    
                    }
                    
                    if((type == 3)|| (type == 7))
                    {
                        addr = (char *)(((unsigned long)((srec.record.data[0])&0xFFL)<< 24) + ((unsigned long)((srec.record.data[1])&0xFFL)<< 16) + 
                             ((unsigned long)((srec.record.data[2])&0xFFL)<< 8 ) + (unsigned long)((srec.record.data[3])&0xFFL));
                    }
                    
                    if((type > 0) && (type < 4)) /* Data sequence. */
                    {
                    	unsigned long *ptr;
                    	int len;
                        
                        ptr = (unsigned long *)((unsigned long)(&srec.record.data[0] + (1+type)));
                        
                        address = (unsigned long)addr;
                        
                        if (first_time == 1)
                        {
                        	first_time = 0;
                        	if (address > 0)
                        	{
                        		flash_address_offset = 0xc000; /* 48k bootloader offset */
                        	}
                        	
                        }
#ifdef DEBUG_FLASH
                        APP_DEBUG("Writing to Flash [0x%x] bytes [%d]\n", PFLASH_BLOCK1_IMAGE_BASE + (address - flash_address_offset), (unsigned int)(srec.record.count - (2+type)));
#endif
                        /* Setup address to write to */
                        fseek(flash_file, -PFLASH_BLOCK1_BASE + 4 + (address - flash_address_offset), IO_SEEK_END);
                        len = write(flash_file, ptr, (srec.record.count - (2+type)));
                        if (len != (srec.record.count - (2+type)))
                        {
                 	       APP_DEBUG("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
                        }
                    }
                    
                    if((type > 6) && (type < 10)) /* End of block. */
                    {
                    	int len;
                    	
                        /* Set entry point. */
/*                        fapp_params_boot_config.go_address = (unsigned long)addr; */

                        /* Setup address to write to */
                        fseek(flash_file, -PFLASH_BLOCK1_BASE, IO_SEEK_END);
                        len = write(flash_file, &addr, 4);
                        if (len != 4)
                        {
                 	       APP_DEBUG("\nError writing to the file. Error code: %d", _io_ferror(flash_file));
                        }
                        
                        APP_DEBUG(FAPP_TFTP_ENTRYPOINT_STR, addr);
                    }
 
                    srec.state = FAPP_TFTP_RX_HANDLER_SREC_STATE_GETSTART;
                }
                
                srec.record_hex_index++;
                break;
        }
    }
    
    return result;
}









