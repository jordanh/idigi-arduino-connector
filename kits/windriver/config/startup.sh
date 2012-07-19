#!/bin/bash
# Script used to get the MAC address from the WindRiver device
# Execute a program to register the device
# Then update the configuration file on the device
#set -v

rc=0

REMOTE_HOST=172.31.255.1 
#REMOTE_HOST=10.52.18.100
# Add route to the static address of the remote host

# If the route to the device does not already exist add it
check_route=`route | grep $REMOTE_HOST | wc -l`

if [[ $check_route == 0 ]]; then

    num_eths=`ifconfig | grep eth | wc -l`

    # Add the route to the default interface
    eth=`route | grep default | grep -o 'eth[0-9]'`

    if [[ $num_eths != 1 ]]; then
        echo "Warning: Multiple ethernets were detected: using default interface" $eth
        read -p "Press [Enter] key to continue"
    fi
    sudo route add -net $REMOTE_HOST netmask 255.255.255.255 dev $eth
fi

mac_address=

ping -q -c 1 $REMOTE_HOST
rc=$?
if [[ ${rc} != 0 ]]; then
    echo "Could not ping device: Verify that the ethernet cable of the device is connected."
    read -p "Press [Enter] key to exit"
    exit ${rc}
fi

# Get the MAC address from the arp cache, search for the first colon in the MAC address
arp_string=`arp -a $REMOTE_HOST`
b=`expr index "$arp_string" :`   

start=$(($b-3)) # Back up to the beginning of the MAC address

if [ $b -eq 0 ]; then
    echo "Error could not locate mac address: Verify that the ethernet cable of the device is connected."
    read -p "Press [Enter] key to exit"
    exit -1
fi

# Add a a sanity test to verify this looks like a mac address.

mac_address=${arp_string:$start:17} 
echo "Mac address of device:" $mac_address

# Execute the process to register the device ID, this will create a file the idigi.conf
java -classpath /home/wruser/idigi Register $mac_address
rc=$?
if [[ ${rc} != 0 ]]; then
    echo "Could not register device: Verify login credentials and try again."
    read -p "Press [Enter] key to exit"
    exit ${rc}
fi

# scp the configuration file to the device
echo "Enter the root password of the device"
scp idigi.conf root@$REMOTE_HOST:/etc/idigi.conf
rc=$?
if [[ ${rc} != 0 ]]; then
    echo "Failed to transfer configuration file to device, please try again."
    read -p "Press [Enter] key to exit"
    exit ${rc}
fi

mac_address=${mac_address//[:]/}
device_id=00000000-00000000-${mac_address:0:6}ff-ff${mac_address:6:6}

echo "Device ID device (record for later):" $device_id
read -p "Provisioning complete; press [Enter] to exit"
