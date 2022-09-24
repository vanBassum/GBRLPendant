#pragma once


/* Littlevgl specific */
#include "lvgl/lvgl.h"
#include "widget.h"


namespace LVGL
{
	class Screen : public Widget
	{
	protected:
		virtual void Create(lv_obj_t* parent) override
		{
			
		}
		
	public:
		Screen()
		{

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
		
		void InitNewScreen()
		{
			if (lvglMutex.Take(pdMS_TO_TICKS(10)))
			{
				if (handle == NULL)
					handle = lv_obj_create(NULL);
				lvglMutex.Give();
			}

		}

		void Show()
		{
			if (lvglMutex.Take(pdMS_TO_TICKS(10)))
			{
				lv_scr_load(handle);
				lvglMutex.Give();
			}
		}
	};	
}







