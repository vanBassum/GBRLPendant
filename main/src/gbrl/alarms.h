#pragma once

namespace GBRL
{
		 
	//https://github.com/terjeio/grblHAL/blob/master/drivers/ESP32/main/grbl/alarms.h
	enum class Alarms
	{
		None                           = 0,
		HardLimit                      = 1,
		SoftLimit                      = 2,
		AbortCycle                     = 3,
		ProbeFailInitial               = 4,
		ProbeFailContact               = 5,
		HomingFailReset                = 6,
		HomingFailDoor                 = 7,
		FailPulloff                    = 8,
		HomingFailApproach             = 9,
		EStop                          = 10,
		HomingRequried                 = 11,
		LimitsEngaged                  = 12,
		ProbeProtect                   = 13,
		Spindle                        = 14,
		HomingFailAutoSquaringApproach = 15,
		SelftestFailed                 = 16,
		MotorFault                     = 17
	};
}