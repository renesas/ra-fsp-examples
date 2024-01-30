#!/bin/bash

#snmp commands

SNMP_GET='snmpget'
SNMP_SET='snmpset'
SNMP_GETNEXT='snmpgetnext'


V1_2C_GET_OPTS=' -c public'
V1_2C_SET_OPTS=' -c private'

V3_OPTS=' -n \"\" -u username -a MD5 -A authpassword -x DES -X privpassword -l authPriv'


AGENT_ADDR='192.168.0.153'    


oid_var='1.3.6.1.4.1.51000.1.4.0' 


echo "\n ###### SNMP V1 test             ##################\n "
echo "\nGET"
$SNMP_GET -v1 $V1_2C_GET_OPTS $AGENT_ADDR $oid_var
echo "\nGETNEXT"
echo "\nSET"
$SNMP_SET -v1 $V1_2C_SET_OPTS $AGENT_ADDR $oid_var i 2
echo "\n ################## END V1 #########################\n "

echo "\n ###### SNMP V2 test             ##################\n "
echo "\nGET"
$SNMP_GET -v2c $V1_2C_GET_OPTS $AGENT_ADDR $oid_var
echo "\nSET"
$SNMP_SET -v2c $V1_2C_SET_OPTS $AGENT_ADDR $oid_var i 40
echo "\n ################## END V2 #########################\n "



echo "\n ###### SNMP V3 test             ##################\n "
$SNMP_SET -v3  -n public -u username -a MD5 -A authpassword -x DES -X privpassword -l authPriv $AGENT_ADDR $oid_var i 678
$SNMP_GET -v3  -n public -u username -a MD5 -A authpassword -x DES -X privpassword -l authPriv $AGENT_ADDR $oid_var 
$SNMP_SET -v3  -n public -u username -a MD5 -A authpassword -x DES -X privpassword -l authPriv $AGENT_ADDR $oid_var i 123  
$SNMP_GET -v3  -n public -u username -a MD5 -A authpassword -x DES -X privpassword -l authPriv $AGENT_ADDR $oid_var 


echo "\n ################## END V3 #########################\n "
echo "\nScript ends Thank you\n"

