#pragma once
#include "lvgl/lvgl.h"
#include "widget.h"

namespace LVGL
{
		 	
	class Button : public Widget
	{
	public:
		Callback<void, Button*> OnClicked;
		
	private:
		static void StaticCallback(lv_event_t * e)
		{
			Button* button = (Button*)e->user_data;
			lv_event_code_t code = lv_event_get_code(e);
			
			switch (code)
			{
			case LV_EVENT_CLICKED:
				if (button->OnClicked.IsBound())
					button->OnClicked.Invoke(button);
				break;
			default:
				break;
			}
		}
		
	public:		
		void SetPos(int x, int y)
		{
			if (lvglMutex.Take(pdMS_TO_TICKS(10)))
			{
				lv_obj_set_pos(handle, x, y);
				lvglMutex.Give();
			}
		}
		
		void SetSize(int width, int height)
		{
			if (lvglMutex.Take(pdMS_TO_TICKS(10)))
			{
				lv_obj_set_size(handle, width, height);
				lvglMutex.Give();
			}
		}
		
		void Init(Widget& parent)
		{
			if (handle != NULL)
				return;
			
			if (lvglMutex.Take())
			{
				handle = lv_btn_create(parent.handle);
				lv_obj_set_pos(handle, 0, 0);
				lv_obj_set_size(handle, 120, 50);
				lv_obj_add_event_cb(handle, StaticCallback, LV_EVENT_ALL, this); /*Assign a callback to the button*/
				handle->user_data = this;
				lvglMutex.Give();
			}
		}
	};
}
