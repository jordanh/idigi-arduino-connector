#include <plib.h>
#include "idigi.h"
#include "tcpip_bsd_config.h"

#include <tcpip_bsd/tcpip_bsd.h>

#include "hardware_profile.h"
#include "system_services.h"

void TCPIPEventCB(eTCPIPEvent event);
// Toggle LEDs Functions
void ToggleRed(void);
void ToggleOrange(void);
void ToggleGreen(void);
void InitLEDs(void);
void DelayMs(unsigned int);

int new_stack_event = false;

idigi_callback_status_t idigi_callback(idigi_class_t class, idigi_request_t request, 
                                        void * const request_data, size_t request_length, 
                                        void * response_data, size_t * response_length)
{
    idigi_callback_status_t status = idigi_callback_continue;

    switch (class)
    {
    case idigi_class_config:
        status = idigi_config_callback(request.config_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_operating_system:
        status = idigi_os_callback(request.os_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_network:
        status = idigi_network_callback(request.network_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_firmware:
        status = idigi_firmware_callback(request.firmware_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_data_service:
        //status = idigi_data_service_callback(request.data_service_request, request_data, request_length, response_data, response_length);
        break;
    case idigi_class_rci:
        status = idigi_rci_callback(request.rci_request, request_data, request_length, response_data, response_length);
        break;
    default:
        /* not supported */
        break;
    }
    return status;
}

int main(void){
    // Configure the device for maximum performance, but do not change the PBDIV clock divisor.
    // Given the options, this function will change the program Flash wait states,
    // RAM wait state and enable prefetch cache, but will not change the PBDIV.
	// The PBDIV value is already set via the pragma FPBDIV option above.
    // SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    // System and PB Clock Frequency
    unsigned sys_clk, pb_clk;
    sys_clk = GetSystemClock();
    pb_clk = SYSTEMConfigWaitStatesAndPB(sys_clk);

    // Enable Systemwide Multi Vector Interrupts.
    INTEnableSystemMultiVectoredInt();

    // Turn ON the System Clock
    SystemTickInit(sys_clk, TICKS_PER_SECOND);
    
    // Read Timer to establish a start time.
    device_data.start_time = SystemTickGet() / TICKS_PER_SECOND;
    device_data.socket_fd = -1;

    // Configure the I/O port to drive the LEDs
    InitLEDs();
    
    // Initialize TCP/IP STack.
    TCPIPSetDefaultAddr(DEFAULT_IP_ADDR, DEFAULT_IP_MASK, DEFAULT_IP_GATEWAY, DEFAULT_MAC_ADDR);
    
    if(!TCPIPInit(sys_clk)){
        return 0;
    }
    
    TCPIPEventSetNotifyEvents(TCPIP_EV_STACK_PROCESSED|TCPIP_EV_ERRORS);
    TCPIPEventSetNotifyHandler(TCPIPEventCB);

    // IP Address Registered on the Ethernet Interface.
    IP_ADDR ip;
    // Get Automatic Private Ip.
    // Note: This code assumes DHCP is used instead of static IP.
    ip.Val = TCPIPGetIPAddr();

    // Whether or not an IP Address was assigned by dhcp.
    int ipAssigned = false;
    // Initialize DHCP
    DHCPInit();
    
    // Initialize iDigi Integration Kit.
    idigi_handle_t handle = idigi_init((idigi_callback_t) idigi_callback);
    idigi_status_t status = idigi_success;

    if(handle == 0){
        return 0;
    }
	
    while(1){ // status == idigi_success

        // Check to see if new IP Address assigned via DHCP.
        IP_ADDR new_ip;
        DHCPTask();
        if(ip.Val != (new_ip.Val = TCPIPGetIPAddr())){
            // IP Has Changed, Updated it.
            ip.Val = new_ip.Val;
            ipAssigned = true;
        }

        if(ipAssigned){
            // Process next iDigi step.
            status = idigi_step(handle);
        }
    }

    return 0;
}

void TCPIPEventCB(eTCPIPEvent event){
    eTCPIPEvent active_event;
    active_event = TCPIPEventGetPending();

    if(active_event & TCPIP_EV_STACK_PROCESSED){
        TCPIPEventProcess(active_event);
    }
}




void DelayMs(unsigned int msec){
    unsigned int tWait, tStart;

    tWait=(SYS_XTAL_FREQ/2000)*msec;
    tStart = ReadCoreTimer();
    while((ReadCoreTimer()-tStart)<tWait);
}

void InitLEDs(void){
    // configure PORTD pins RD0, RD1 and RD2 as digital outputs
    mPORTDSetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2);  /* same as TRISDCLR = 0x0007 */

    // initialize these pins low
    mPORTDClearBits(BIT_0 | BIT_1 | BIT_2); /* same as LATDCLR = 0x0007 */
}

void ToggleRed(void){
    mPORTDToggleBits(BIT_0);	/* toggle LED0 (same as LATDINV = 0x0001)*/
}

void ToggleOrange(void){
    mPORTDToggleBits(BIT_1);	/* toggle LED0 (same as LATDINV = 0x0002)*/
}

void ToggleGreen(void){
    mPORTDToggleBits(BIT_2);	/* toggle LED0 (same as LATDINV = 0x0004)*/
}