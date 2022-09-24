#pragma once
#include <stdint.h>
#include <string>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "lib/rtos/freertos.h"
#include "alarms.h"
#include "errors.h"
#include "charactercounter.h"

namespace GBRL
{
		 
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
		CharacterCounter sendSize;
	
	
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
	
}