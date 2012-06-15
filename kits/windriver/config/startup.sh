#!/bin/bash
# Script used to get the MAC address from the WindRiver device
# Execute a program to register the device
# Then update the configuration file on the device
#set -v

rc=0
# Add route to the static address of the remote host
sudo route add -net 172.31.255.1 netmask 255.255.255.255 dev eth2
REMOTE_HOST=172.31.255.1 

mac_address=

ping -q -c 1 $REMOTE_HOST
rc=$?
if [[ ${rc} != 0 ]]; then
    echo "Could not ping device, please verify that the ethernet cable of the device is connected."
    read -p "Press [Enter] key to exit"
    exit ${rc}
fi

# Get the MAC address from the arp cache, search for the first colon in the MAC address
arp_string=`arp -a $REMOTE_HOST`
b=`expr index "$arp_string" :`   

start=$(($b-3)) # Back up to the beginning of the MAC address

if [ $b -eq 0 ]; then
    echo "Error could not locate mac address of device."
    read -p "Press [Enter] key to exit"
    exit -1
fi

# Add a a sanity test to verify this looks like a mac address.

mac_address=${arp_string:$start:17} 
echo "Mac address of device:" $mac_address

# Execute the process to register the device ID, this will create a file the idigi.conf
java -classpath /home/wruser/idigi Register $mac_address
if [[ ${rc} != 0 ]]; then
    echo "Could not register device"
    exit ${rc}
fi

# scp the configuration file to the device
echo "Enter the root password of the device"
scp idigi.conf root@$REMOTE_HOST:/etc/idigi.conf

mac_address=${mac_address//[:]/}
device_id=00000000-00000000-${mac_address:0:6}FF-FF${mac_address:7:13}

echo "Device ID device (record for later):" $device_id
read -p "Provisioning complete; press [Enter] to exit"
