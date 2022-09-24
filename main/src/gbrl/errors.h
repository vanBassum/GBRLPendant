#pragma once

namespace GBRL
{
		 
	//https://github.com/bdring/Grbl_Esp32/blob/main/Grbl_Esp32/src/Error.h
	enum class Errors
	{
		Ok = 0,
		ExpectedCommandLetter = 1,
		BadNumberFormat = 2,
		InvalidStatement = 3,
		NegativeValue = 4,
		SettingDisabled = 5,
		SettingStepPulseMin = 6,
		SettingReadFail = 7,
		IdleError = 8,
		SystemGcLock = 9,
		SoftLimitError = 10,
		Overflow = 11,
		MaxStepRateExceeded = 12,
		CheckDoor = 13,
		LineLengthExceeded = 14,
		TravelExceeded = 15,
		InvalidJogCommand = 16,
		SettingDisabledLaser = 17,
		HomingNoCycles = 18,
		GcodeUnsupportedCommand = 20,
		GcodeModalGroupViolation = 21,
		GcodeUndefinedFeedRate = 22,
		GcodeCommandValueNotInteger = 23,
		GcodeAxisCommandConflict = 24,
		GcodeWordRepeated = 25,
		GcodeNoAxisWords = 26,
		GcodeInvalidLineNumber = 27,
		GcodeValueWordMissing = 28,
		GcodeUnsupportedCoordSys = 29,
		GcodeG53InvalidMotionMode = 30,
		GcodeAxisWordsExist = 31,
		GcodeNoAxisWordsInPlane = 32,
		GcodeInvalidTarget = 33,
		GcodeArcRadiusError = 34,
		GcodeNoOffsetsInPlane = 35,
		GcodeUnusedWords = 36,
		GcodeG43DynamicAxisError = 37,
		GcodeMaxValueExceeded = 38,
		PParamMaxExceeded = 39,
		FsFailedMount = 60,
		// SD Failed to mount
		FsFailedRead = 61,
		// SD Failed to read file
		FsFailedOpenDir = 62,
		// SD card failed to open directory
		FsDirNotFound = 63,
		// SD Card directory not found
		FsFileEmpty = 64,
		// SD Card directory not found
		FsFileNotFound = 65,
		// SD Card file not found
		FsFailedOpenFile = 66,
		// SD card failed to open file
		FsFailedBusy = 67,
		// SD card is busy
		FsFailedDelDir = 68,
		FsFailedDelFile = 69,
		BtFailBegin = 70,
		// Bluetooth failed to start
		WifiFailBegin = 71,
		// WiFi failed to start
		NumberRange = 80,
		// Setting number range problem
		InvalidValue = 81,
		// Setting string problem
		MessageFailed = 90,
		NvsSetFailed = 100,
		NvsGetStatsFailed = 101,
		AuthenticationFailed = 110,
		Eol = 111,
		AnotherInterfaceBusy = 120,
		JogCancelled = 130,
	};
}