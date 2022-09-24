# GBRLPendant
Creating a controller for GBRL around the WT32-sc01

Using ESP-IDF V4.4.
Using LVGL for the graphics.
Using my own libraries

Optional VisualGDB


# Hardware
I use the WT32-sc01 as the main controller module. The I/O is 3.3V while my GBRL board is 5V. So I use levelshifters as explained in this link: https://www.digikey.nl/nl/blog/logic-level-shifting-basics The WT32-sc01 can be supplied with 5V, the idea is to supply the pendant with 5V from the GBRL board. 

Pinning:
 - UART_PIN_TX	GPIO_NUM_26	J5_30	J6_30
 - UART_PIN_RX	GPIO_NUM_25	J5_28	J6_28


# TODO
- 