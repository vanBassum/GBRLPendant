#pragma once
#include "lib/rtos/freertos.h"
#include "lib/misc/callback.h"

namespace LVGL
{
	static FreeRTOS::RecursiveMutex lvglMutex;
	class Widget
	{

	public:
		lv_obj_t* handle = NULL;
		
		Widget()
		{
		}
		
		~Widget()
		{
			LVGL::lvglMutex.Take();
			lv_obj_del_async(handle);
			lvglMutex.Give();
		}
	};
}