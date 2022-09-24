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
#include "driver/gpio.h"
#include "driver/uart.h"
#include <string>
#include "lvglpp.h"

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

LVGL::Screen screen1;
LVGL::Screen screen2;

void ButtonNextClicked(LVGL::Button* sender)
{
	ESP_LOGI(TAG, "Next clicked");
	screen2.Show();
}



void ButtonBackClicked(LVGL::Button* sender)
{
	ESP_LOGI(TAG, "Back clicked");
	screen1.Show();
}

void InitScreen1(LVGL::Screen& screen)
{
	static LVGL::Button button;
	static LVGL::Label label;
	
	button.Init(screen);
	button.SetPos(0, 0);
	button.OnClicked.Bind(ButtonNextClicked);
	
	label.Init(button);
	label.SetText("Next");
}


void InitScreen2(LVGL::Screen& screen)
{
	static LVGL::Button button;
	static LVGL::Label label;
	
	button.Init(screen);
	button.SetPos(100, 0);
	button.OnClicked.Bind(ButtonBackClicked);
	
	label.Init(button);
	label.SetText("Back");
}


void app_main(void)
{
	nvs_flash_init();
	

	LVGL::Init();
	
	screen1 = LVGL::FirstScreen;
	screen2.Init();
	InitScreen1(screen1);
	InitScreen2(screen2);
	
	

	

	
	
	while (1)
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	
}










