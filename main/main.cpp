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
#include "lib/rtos/freertos.h"
#include "lvglpp/lvgl.h"
#include "driver/gpio.h"
#include "driver/uart.h"



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


void Uart()
{

	uart_config_t uart_config = {
		.baud_rate = UART_BAUD_RATE,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_APB,
	};

	int intr_alloc_flags = 0;
	char *data = (char *) malloc(UART_BUF_SIZE);
	
	ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
	ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_PIN_TX, UART_PIN_RX, -1, -1));
	
	ESP_LOGI("DATA", "Uart started");
	
	while (1) {
		// Read data from the UART
		int len = uart_read_bytes(UART_PORT_NUM, data, (UART_BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
		
		if(len > 0)
			ESP_LOGI("DATA", "data= %s", (char*)data);
		
		

	}
	
}


void ButtonClicked(LVGL::Button* sender)
{
	// Write data back to the UART
	char data[32];
	int len = snprintf(data, 32, "$$\r\n");
	uart_write_bytes(UART_PORT_NUM, (const char *) data, len);
}

void app_main(void)
{
	nvs_flash_init();
	

	LVGL::Init();
	
	LVGL::Label label;
	LVGL::Button button;
	LVGL::Button button2;

	LVGL::ActScreen.AddWidget(button);
	button.OnClicked.Bind(ButtonClicked);
	
	LVGL::ActScreen.AddWidget(button2);
	button2.SetPos(0, 100);
	
	button.AddWidget(label);
	//label.SetAlignment(LV_ALIGN_CENTER);
	//label.SetText("Yeah");
	
	Uart();
	while (1)
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	
}










