#pragma once


/* Littlevgl specific */
#include "lvgl/lvgl.h"
#include "widget.h"
#include "esp_log.h"


namespace LVGL
{
	class Screen : public Widget
	{

	public:
		Screen()
		{

		}
		
		void Init()
		{
			if (handle != NULL)
				return;
			
			if (lvglMutex.Take())
			{
				handle = lv_obj_create(NULL);
				handle->user_data = this;
				lvglMutex.Give();
			}
		}

		
		void Show()
		{
			ESP_LOGI("Screen", "Show screen a");
			if (lvglMutex.Take())
			{
				ESP_LOGI("Screen", "Show screen b");
				lv_scr_load(handle);
				lvglMutex.Give();
			}
		}
		
		void InitActualScreen()
		{
			if (lvglMutex.Take(pdMS_TO_TICKS(10)))
			{
				if (handle == NULL)
					handle = lv_scr_act();
				lvglMutex.Give();
			}
		}
	};	
}







