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
			if (lvglMutex.Take(pdMS_TO_TICKS(10)))
			{
				lv_label_set_text(handle, text.c_str());
				lvglMutex.Give();
			}
			
		}
		
		void SetAlignment(lv_align_t align)
		{
			if (lvglMutex.Take(pdMS_TO_TICKS(10)))
			{
				lv_obj_set_align(handle, align);
				lvglMutex.Give();
			}
		}
		
	private:		
		
		virtual void Create(lv_obj_t* parent) override
		{
			handle = lv_label_create(parent);
			lv_label_set_text(handle, "Some text"); /*Set the labels text*/
			lv_obj_center(handle);
		}
	};
}
