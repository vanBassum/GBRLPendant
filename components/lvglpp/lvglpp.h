#pragma once
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "esp_log.h"
#include "lib/rtos/freertos.h"

#include "screen.h"
#include "widget.h"
#include "button.h"
#include "label.h"


namespace LVGL
{
	static Screen FirstScreen;
	
	
	static void GuiTask(FreeRTOS::Task* task, void* args)
	{
		while (1) 
		{
			//5ms intervals, https://docs.lvgl.io/latest/en/html/porting/task-handler.html
			vTaskDelay(pdMS_TO_TICKS(5));
			if (lvglMutex.Take(pdMS_TO_TICKS(10)))
			{
				lv_task_handler();
				lvglMutex.Give();
			}
			else
			{
				ESP_LOGE("LVGLPP", "Mutex taken, skipped lv_task_handler");
			}
		}
	}
	
	static void Tick(FreeRTOS::Timer* t)
	{
		//https://docs.lvgl.io/latest/en/html/porting/tick.html Should be done differently, see if we can find a tick hook in freertos
		if (lvglMutex.Take(pdTICKS_TO_MS(10)))
		{
			lv_tick_inc(pdTICKS_TO_MS(t->GetPeriod()));
			lvglMutex.Give();
		}
	}
		
	static void Init()
	{
		static FreeRTOS::Timer lvglTimer;
		static FreeRTOS::Task lvglTask;
		static const uint16_t screenWidth = LV_HOR_RES_MAX;
		static const uint16_t screenHeight = LV_VER_RES_MAX;
		static lv_disp_draw_buf_t draw_buf;
		static lv_color_t buf[screenWidth * 10];
	
		lvgl_driver_init();
		lv_init(); // Initialize lvgl
	

		/* LVGL : Setting up buffer to use for display */
		lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

		/*** LVGL : Setup & Initialize the display device driver ***/
		static lv_disp_drv_t disp_drv;
		lv_disp_drv_init(&disp_drv);
		disp_drv.hor_res = screenWidth;
		disp_drv.ver_res = screenHeight;
		disp_drv.flush_cb = disp_driver_flush;
		disp_drv.draw_buf = &draw_buf;
		lv_disp_drv_register(&disp_drv);

		/*** LVGL : Setup & Initialize the input device driver ***/
		static lv_indev_drv_t indev_drv;
		lv_indev_drv_init(&indev_drv);
		indev_drv.type = LV_INDEV_TYPE_POINTER;
		indev_drv.read_cb = touch_driver_read;
		lv_indev_drv_register(&indev_drv);
	
		/* Fetch first screen */
		
		
		/* Create and start a periodic timer interrupt to call lv_tick_inc */
		lvglTimer.Init("lvgl", pdMS_TO_TICKS(1), true);
		lvglTimer.SetCallback(&Tick);
		lvglTimer.Start();
	
		lvglTask.SetCallback(&GuiTask);
		lvglTask.RunPinned("LVGL", 5, 1024 * 8, 0, NULL);
		

		FirstScreen.InitActualScreen();
	}


	
	
}
