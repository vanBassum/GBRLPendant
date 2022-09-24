#pragma once
#include "lib/rtos/freertos.h"

namespace GBRL
{

	class CharacterCounter
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
}