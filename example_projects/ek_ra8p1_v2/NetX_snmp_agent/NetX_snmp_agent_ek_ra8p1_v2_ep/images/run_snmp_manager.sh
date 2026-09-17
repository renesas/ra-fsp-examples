sudo service snmpd restart
sudo service snmptrapd restart
sudo killall -9 snmptrapd
sudo snmptrapd -f -Lo
