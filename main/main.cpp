#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_sntp.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/apps/sntp.h"
#include "lwip/apps/sntp_opts.h"
#include <string.h>
#include "esp_flash_partitions.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "lib/rtos/freertos.h"
#include "lvglpp/lvgl.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include <string>


#define TAG "MAIN"

#define UART_PIN_TX			GPIO_NUM_26
#define UART_PIN_RX			GPIO_NUM_25
#define UART_PORT_NUM		UART_NUM_1
#define UART_BAUD_RATE		115200
#define UART_BUF_SIZE		(1024)

/*
 * For this to work, use menuconfig to define the right screen and add following defines to lvgl_helpers.h
 * #define LV_HOR_RES_MAX 320
 * #define LV_VER_RES_MAX 480
 * #define SPI_HOST_MAX 3
 */



extern "C" {
   void app_main();
}

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


class IntCircleBuffer
{
	FreeRTOS::Mutex mutex;
	static const int bufSize = 64;
	int wPtr = 0;
	int rPtr = 0;
	int sendCount[bufSize];
	int sum = 0;
public:
	
	void Enqueue(int i)
	{
		if (mutex.Take(0))
		{
			sum += i;
			sendCount[wPtr++] = i;
			if (wPtr >= bufSize)
				wPtr = 0;
		
			if (rPtr == wPtr)
				rPtr++;
		
			if (rPtr >= bufSize)
				rPtr = 0;
			mutex.Give();
		}
	}
	
	int Dequeue()
	{
		int result = -1;
		if (mutex.Take(0))
		{
			if (rPtr != wPtr)
			{
				result = sendCount[rPtr++];
				if (rPtr >= bufSize)
					rPtr = 0;
				sum -= result;
			}
			mutex.Give();
		}
		return result;
	}
	
	int Sum()
	{
		int result = -1;
		if (mutex.Take(0))
		{
			result = sum;
			mutex.Give();
		}
		return result;
	}
	void Clear()
	{
		if (mutex.Take(0))
		{
			sum = 0;
			wPtr = rPtr = 0;
			mutex.Give();
		}
	}
};

//https://github.com/terjeio/grblHAL/blob/master/drivers/ESP32/main/grbl/alarms.h
enum class Alarms
{
	None = 0,
    HardLimit = 1,
    SoftLimit = 2,
    AbortCycle = 3,
    ProbeFailInitial = 4,
    ProbeFailContact = 5,
    HomingFailReset = 6,
    HomingFailDoor = 7,
    FailPulloff = 8,
    HomingFailApproach = 9,
    EStop = 10,
    HomingRequried = 11,
    LimitsEngaged = 12,
    ProbeProtect = 13,
    Spindle = 14,
    HomingFailAutoSquaringApproach = 15,
    SelftestFailed = 16,
    MotorFault = 17
};

//https://github.com/bdring/Grbl_Esp32/blob/main/Grbl_Esp32/src/Error.h
enum class Errors
{
	Ok                          = 0,
    ExpectedCommandLetter       = 1,
    BadNumberFormat             = 2,
    InvalidStatement            = 3,
    NegativeValue               = 4,
    SettingDisabled             = 5,
    SettingStepPulseMin         = 6,
    SettingReadFail             = 7,
    IdleError                   = 8,
    SystemGcLock                = 9,
    SoftLimitError              = 10,
    Overflow                    = 11,
    MaxStepRateExceeded         = 12,
    CheckDoor                   = 13,
    LineLengthExceeded          = 14,
    TravelExceeded              = 15,
    InvalidJogCommand           = 16,
    SettingDisabledLaser        = 17,
    HomingNoCycles              = 18,
    GcodeUnsupportedCommand     = 20,
    GcodeModalGroupViolation    = 21,
    GcodeUndefinedFeedRate      = 22,
    GcodeCommandValueNotInteger = 23,
    GcodeAxisCommandConflict    = 24,
    GcodeWordRepeated           = 25,
    GcodeNoAxisWords            = 26,
    GcodeInvalidLineNumber      = 27,
    GcodeValueWordMissing       = 28,
    GcodeUnsupportedCoordSys    = 29,
    GcodeG53InvalidMotionMode   = 30,
    GcodeAxisWordsExist         = 31,
    GcodeNoAxisWordsInPlane     = 32,
    GcodeInvalidTarget          = 33,
    GcodeArcRadiusError         = 34,
    GcodeNoOffsetsInPlane       = 35,
    GcodeUnusedWords            = 36,
    GcodeG43DynamicAxisError    = 37,
    GcodeMaxValueExceeded       = 38,
    PParamMaxExceeded           = 39,
    FsFailedMount               = 60, // SD Failed to mount
    FsFailedRead                = 61, // SD Failed to read file
    FsFailedOpenDir             = 62, // SD card failed to open directory
    FsDirNotFound               = 63, // SD Card directory not found
    FsFileEmpty                 = 64, // SD Card directory not found
    FsFileNotFound              = 65, // SD Card file not found
    FsFailedOpenFile            = 66, // SD card failed to open file
    FsFailedBusy                = 67, // SD card is busy
    FsFailedDelDir              = 68,
    FsFailedDelFile             = 69,
    BtFailBegin                 = 70, // Bluetooth failed to start
    WifiFailBegin               = 71, // WiFi failed to start
    NumberRange                 = 80, // Setting number range problem
    InvalidValue                = 81, // Setting string problem
    MessageFailed               = 90,
    NvsSetFailed                = 100,
    NvsGetStatsFailed           = 101,
    AuthenticationFailed        = 110,
    Eol                         = 111,
    AnotherInterfaceBusy        = 120,
    JogCancelled                = 130,
};


class UartClient
{	
public:
	Callback<void, UartClient*, Alarms> OnAlarm;
	Callback<void, UartClient*, Errors> OnError;
	
private:
	uart_port_t portNum = NULL;
	static const size_t rxBufSize = 128;
	static const size_t grblBufSize = 128;
	FreeRTOS::Task receiveTask;
	FreeRTOS::Mutex sendMutex;
	IntCircleBuffer sendSize;
	
	
	void HandleFrame(std::string frame)
	{
		//Beware, this is called from RX task, don't do anything extensive
		if (frame.rfind("ALARM", 0) == 0) //hmm casing, this will be annoying
		{
			Alarms alarm = std::stoi(frame, 6);
			if (OnAlarm.IsBound())
				OnAlarm.Invoke(this, alarm);
			sendSize.Clear();
		}
		else if (frame.rfind("error", 0) == 0) 
		{
			Errors error = std::stoi(frame, 6);
			if (OnError.IsBound())
				OnError.Invoke(this, error);
			sendSize.Clear();
		}
		else if (frame == "ok")
		{
			sendSize.Dequeue();
		}
		else
		{
			switch
						
		}
	}

	void RxTask(FreeRTOS::Task* task, void* args)
	{
		char rxBuffer[rxBufSize];
		size_t wrPtr = 0;
		while (1)
		{
			wrPtr += uart_read_bytes(portNum, &rxBuffer[wrPtr], (rxBufSize - 1), 20 / portTICK_PERIOD_MS); //Timeout was 20 / portTICK_PERIOD_MS)
			if (wrPtr > 0)
			{
				char* start = rxBuffer;
				char* end = strchr(rxBuffer, '\n');
				int length = end - start + 1; //Also consume the \n char
				if (length > 0)
				{
					std::string frame;
					std::copy(start, end, frame);
					wrPtr -= length;
					HandleFrame(frame);
				}
			}
		}
	}
	
	
	bool DirectCommand(char firstChar)
	{
		switch (firstChar)
		{
		case '?': return true;
		default: return false;
		}
	}
	
	
	bool Write(std::string frame)
	{
		
	}
	
	
public:
	
	
	void Init(const uart_config_t config, gpio_num_t rx, gpio_num_t tx, uart_port_t portNum)
	{
		this->portNum = portNum;
		int intr_alloc_flags = 0;
		ESP_ERROR_CHECK(uart_driver_install(portNum, rxBufSize * 2, 0, 0, NULL, intr_alloc_flags));
		ESP_ERROR_CHECK(uart_param_config(portNum, &config));
		ESP_ERROR_CHECK(uart_set_pin(portNum, rx, tx, -1, -1));
		receiveTask.SetCallback(this, &UartClient::RxTask);
		receiveTask.Run("UartClient RX", 7, 1024, NULL);
	}
	

	
	bool TrySend(std::string frame)
	{
		int length = frame.length();
		if (length == 0)
			return false;
		
		int space = grblBufSize - sendSize.Sum();
		bool fits = space >= length;
		bool direct = DirectCommand(frame[0]);

		if (fits || direct)
		{
			if (sendMutex.Take())
			{
				if (!direct)
					sendSize.Enqueue(length);
				uart_write_bytes(portNum, frame.c_str(), length);
				sendMutex.Give();
				return true;
			}
		}
		return false;
	}
	
	
	
};


void Uart()
{

	uart_config_t uart_config = {
		.baud_rate = UART_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};

	int intr_alloc_flags = 0;
	char *data = (char *) malloc(UART_BUF_SIZE);
	
	ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
	ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_PIN_TX, UART_PIN_RX, -1, -1));
	
	ESP_LOGI("DATA", "Uart started");
	
	while (1) {
		// Read data from the UART
		int len = uart_read_bytes(UART_PORT_NUM, data, (UART_BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
		
		if(len > 0)
			ESP_LOGI("DATA", "data= %s", (char*)data);
		
		

	}
	
}


void ButtonClicked(LVGL::Button* sender)
{
	// Write data back to the UART
	char data[32];
	int len = snprintf(data, 32, "$$\r\n");
	uart_write_bytes(UART_PORT_NUM, (const char *) data, len);
}

void app_main(void)
{
	nvs_flash_init();
	

	LVGL::Init();
	
	LVGL::Label label;
	LVGL::Button button;
	LVGL::Button button2;

	LVGL::ActScreen.AddWidget(button);
	button.OnClicked.Bind(ButtonClicked);
	
	LVGL::ActScreen.AddWidget(button2);
	button2.SetPos(0, 100);
	
	button.AddWidget(label);
	//label.SetAlignment(LV_ALIGN_CENTER);
	//label.SetText("Yeah");
	
	Uart();
	while (1)
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	
}










