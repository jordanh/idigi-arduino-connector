#!/bin/bash
# Script used to get the MAC address from the WindRiver device
# Execute a program to register the device
# Then update the configuration file on the device
#set -v

rc=0
# Note: we need a route to the remote host:
# sudo route add net 172.31.255.1 netmask 255.255.255.255 dev eth0
#REMOTE_HOST=172.31.255.1 
REMOTE_HOST=10.52.18.111

mac_address=

ping -q -c 1 $REMOTE_HOST
rc=$?
if [[ ${rc} != 0 ]]; then
    echo "Could not ping device, please verify that the ethernet cable of the device is connected."
    exit ${rc}
fi

# Get the MAC address from the arp cache, search for the first colon in the MAC address
arp_string=`arp -a $REMOTE_HOST`
b=`expr index "$arp_string" :`   

start=$(($b-3)) # Back up to the beginning of the MAC address

if [ $b -eq 0 ]; then
    echo "Error could not locate mac address of device."
    exit -1
fi

# Add a a sanity test to verify this looks like a mac address.

mac_address=${arp_string:$start:17} 
echo "Mac address of device:" $mac_address

# Execute the process to register the device ID, this will create a file the idigi.conf
# configuration file which we the scp to the device.
java Register $mac_address
if [[ ${rc} != 0 ]]; then
    echo "Could not register device"
    exit ${rc}
fi

# scp the configuration file to the device
echo "You will be prompted for the root password of the device"
scp idigi.conf root@$REMOTE_HOST:/etc/idigi.conf 
