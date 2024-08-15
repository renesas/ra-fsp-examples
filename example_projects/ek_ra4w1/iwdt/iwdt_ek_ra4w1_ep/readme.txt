/**********************************************************************************************************************
* File Name    : readme.txt
* Description  : Contains general information about Example Project and detailed instructions
**********************************************************************************************************************/

Project Overview:
	This example project demonstrates the typical use of the IWDT HAL module.
	User input initializes the IWDT and start GPT timer. IWDT counter is refreshed
	periodically every 1 second when the GPT timer expires. On entering 2 from RTTViewer,
	IWDT counter stops refreshing and IWDT resets the MCU.
   