/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
	The example project demonstrates the typical use of the FLASH_LP HAL module APIs. The example project demonstrate 
	the code flash and data flash operations once user provides the RTT input from the available commands list. After 
	selecting the flash, user has to give input for the block number on which the operations are to be performed.
	
NOTE  : If block number less than 10 is given for code flash operations, the board may get locked. 
	This happens due to R_FLASH_LP_AccessWindowSet API.
	User is expected to enter data of size not exceeding 15 bytes.
	Operation is not guaranteed for any user input value other than integer(i.e. float, char, special char) through RTT.