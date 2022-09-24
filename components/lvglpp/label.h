#pragma once
#include "lvgl/lvgl.h"
#include "widget.h"
#include <string>

namespace LVGL
{
		 	
	class Label : public Widget
	{
	public:
		void SetText(std::string text)
		{
			if (lvglMutex.Take())
			{
				lv_label_set_text(handle, text.c_str());
				lvglMutex.Give();
			}
			
		}
		
		void SetAlignment(lv_align_t align)
		{
			if (lvglMutex.Take())
			{
				lv_obj_set_align(handle, align);
				lvglMutex.Give();
			}
		}
		
		void Init(Widget& parent)
		{
			if (handle != NULL)
				return;
			
			if (lvglMutex.Take())
			{
				handle = lv_label_create(parent.handle);
				lv_label_set_text(handle, "Some text"); /*Set the labels text*/
				lv_obj_center(handle);
				handle->user_data = this;
				lvglMutex.Give();
			}
		}
	};
}
