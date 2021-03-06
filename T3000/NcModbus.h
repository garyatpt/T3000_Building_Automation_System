#define OFFSET_MODBUS_ADDRESS	100 			// Offset for 2nd block of registers
#define SCHEDUAL_MODBUS_ADDRESS 200
enum {
	// MODBUS REGISTER				// MIN	MAX	DESCRIPTION
	// Address 0  
	MODBUS_SERIALNUMBER_LOWORD   ,          	// -	-	Lower 2 bytes of the serial number
	MODBUS_SERIALNUMBER_HIWORD  = 2  ,		// -	-	Upper 2 bytes of teh serial number
	MODBUS_VERSION_NUMBER_LO    = 4  ,		// -	-	Software version
	MODBUS_VERSION_NUMBER_HI,			// -	-	Software version
	MODBUS_ADDRESS,					// 1	254	Device modbus address
	MODBUS_PRODUCT_MODEL,				// -	-	Temco Product Model	1=Tstat5B, 2=Tstat5A, 4=Tstat5C, 12=Tstat5D, 
	MODBUS_HARDWARE_REV,				// -	-	Hardware Revision	 
	MODBUS_ADDRESS_PLUG_N_PLAY=10,			// -	-	Temporary address for plug-n-play addressing scheme
	
	MODBUS_TIME_ZONE,
	MODBUS_BAUDRATE,
	MODBUS_DAYLIGHT_ENABLE,                                     	// registers needed for updating status
	MODBUS_UPDATE_STATUS	= 16,			// reg 16 status for update_flash
	MODBUS_REFRESH_STATUS   = 90,
	
	MODBUS_SERINALNUMBER_WRITE_FLAG		= EEP_SERINALNUMBER_WRITE_FLAG ,	

	MODBUS_MAC_ADDRESS			= OFFSET_MODBUS_ADDRESS , //100 READ ONLY 
	MODBUS_IP_MODE         		= MODBUS_MAC_ADDRESS + 6 ,//0,static ip; 1,dhcp 106
	MODBUS_IPADDRESS_LOWORD ,										//107
	MODBUS_IPADDRESS_HIWORD 	= MODBUS_IPADDRESS_LOWORD + 2 ,		//109
	MODBUS_SUBNET_LOWORD    	= MODBUS_IPADDRESS_HIWORD + 2 ,		//111		
	MODBUS_SUBNET_HIWORD    	= MODBUS_SUBNET_LOWORD + 2 ,		//113
	MODBUS_GATEWAY_LOWORD   	= MODBUS_SUBNET_HIWORD + 2 ,		//115
	MODBUS_GATEWAY_HIWORD   	= MODBUS_GATEWAY_LOWORD + 2 ,		//117
	MODBUS_HOST_MODE   			= MODBUS_GATEWAY_HIWORD + 2 ,//0,tcps; 1,tcpc; 2,tcpsc 119
	MODBUS_LISTENING_PORT ,//120
	MODBUS_DESTINATION_ADDRESS_LOWORD ,//121
	MODBUS_DESTINATION_ADDRESS_HIWORD	= MODBUS_DESTINATION_ADDRESS_LOWORD + 2 ,//123
	MODBUS_DESTINATION_PORT    	= MODBUS_DESTINATION_ADDRESS_HIWORD + 2 ,//125
	MODBUS_CONNECTION_INTERVAL ,//126
	MODBUS_INACTIVITY_TIMEOUT  ,//127	
	MODBUS_SERIAL_BAUDRATE ,//0,9600; 1,19200   128
	MODBUS_SERIAL_INTERCHAR_TIMEOUT , //129
	MODBUS_FACTORY_DEFAULT ,//0,all parameters are default ; 1, all parameters except ip are default
	MODBUS_NEMO_SAVE , //0,normal work;1,save the nemo10 copfigure data
	MODBUS_NEMO_GET , //0,NORMAL WORK; 1,Does not save the configure data and disconnect the communication between NEMO10 and PC
	MODBUS_NEMO_REBOOT ,//  133

	
MODBUS_TEST  ,
MODBUS_TEST0 ,
MODBUS_TEST1  ,
MODBUS_TEST2  ,
MODBUS_TEST3  ,
MODBUS_TEST4  ,
MODBUS_TEST5  ,
MODBUS_TEST6  ,
MODBUS_TEST7  ,
 
	MODBUS_TSTAT_BAUDRATE		= 185,
	MODBUS_TIMER_ADDRESS		= SCHEDUAL_MODBUS_ADDRESS ,
	
	MODBUS_WR_DESCRIP_FIRST		= MODBUS_TIMER_ADDRESS + 8 ,
	MODBUS_WR_DESCRIP_LAST		= MODBUS_WR_DESCRIP_FIRST + WR_DESCRIPTION_SIZE * MAX_WR ,
	
	MODBUS_AR_DESCRIP_FIRST		= MODBUS_WR_DESCRIP_LAST ,
	MODBUS_AR_DESCRIP_LAST		= MODBUS_AR_DESCRIP_FIRST + AR_DESCRIPTION_SIZE * MAX_AR ,
	
	MODBUS_ID_FIRST				= MODBUS_AR_DESCRIP_LAST ,
	MODBUS_ID_LAST				= MODBUS_ID_FIRST + ID_SIZE * MAX_ID ,
	
	MODBUS_AR_TIME_FIRST		= MODBUS_ID_LAST ,
	MODBUS_AR_TIME_LAST			= MODBUS_AR_TIME_FIRST + AR_TIME_SIZE * MAX_AR ,
	
	MODBUS_WR_ONTIME_FIRST		= MODBUS_AR_TIME_LAST ,
	MODBUS_WR_ONTIME_LAST		= MODBUS_WR_ONTIME_FIRST + WR_TIME_SIZE * MAX_WR ,
	
	MODBUS_WR_OFFTIME_FIRST		= MODBUS_WR_ONTIME_LAST ,
	MODBUS_WR_OFFTIME_LAST		= MODBUS_WR_OFFTIME_FIRST + WR_TIME_SIZE * MAX_WR ,

	MODBUS_TOTAL_PARAMETERS		= MODBUS_WR_OFFTIME_LAST
	
	
} ;

  
