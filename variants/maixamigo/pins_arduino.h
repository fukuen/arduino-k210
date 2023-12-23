#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

// UART
#define UART1_TX            (5)
#define UART1_RX            (4)

#define TFT_CSX_PIN         (36)
#define TFT_RST_PIN         (37)
#define TFT_DCX_PIN         (38)
#define TFT_CLK_PIN         (39)

#define SDCARD_CLK_PIN      (11)
#define SDCARD_MIS_PIN      (6)
#define SDCARD_MOS_PIN      (10)
#define SDCARD_CSX_PIN      (26)

#define CAMERA_PCLK_PIN     (47)
#define CAMERA_XCLK_PIN     (46)
#define CAMERA_HSYNC_PIN    (45)
#define CAMERA_VSYNC_PIN    (43)
#define CAMERA_PWDN_PIN     (44)
#define CAMERA_RST_PIN      (42)
#define CAMERA_SDA_PIN      (40)
#define CAMERA_SCL_PIN      (41)

// flash filesystem config
#define FLASH_FS_ADDR       (0xD00000)
#define FLASH_FS_SIZE       (0x300000)

// optional
#define TFT_WIDTH           (320)
#define TFT_HEIGHT          (480)
#define SDA_PIN             (27)
#define SCL_PIN             (24)
#define LCD_BL_PIN          (-1)
#define BOOT_PIN            (16)
#define KEY_1_PIN           (23)
#define KEY_2_PIN           (20)
#define KEY_3_PIN           (31)
#define LED_R_PIN           (14)
#define LED_G_PIN           (15)
#define LED_B_PIN           (17)
#define LED_W_PIN           (32)

#endif /* Pins_Arduino_h */
