#!/bin/bash
# Script used to get the MAC address from the WindRiver device
# Execute a program to register the device
# Then update the configuration file on the device

# Ping the device to get the IP address

REMOTE_HOST=10.52.18.76
$macAddr=false;

$arp=`arp -a $ipAddress`;
$lines=explode("\n", $arp);

#look for the output line describing our IP address
foreach($lines as $line)
{
   $cols=preg_split('/\s+/', trim($line));
   if ($cols[0]==$ipAddress)
   {
       $macAddr=$cols[1];
   }
}
