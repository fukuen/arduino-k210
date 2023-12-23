#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

// UART
#define UART1_TX            (5)
#define UART1_RX            (4)

#define TFT_CSX_PIN         (22) //38
#define TFT_RST_PIN         (21) //39
#define TFT_DCX_PIN         (20) //37
#define TFT_CLK_PIN         (19) //36

#define SDCARD_CLK_PIN      (30) //28
#define SDCARD_MIS_PIN      (31) //29
#define SDCARD_MOS_PIN      (33) //27
#define SDCARD_CSX_PIN      (32) //26

#define CAMERA_PCLK_PIN     (47) //47
#define CAMERA_XCLK_PIN     (46) //46
#define CAMERA_HSYNC_PIN    (45) //44
#define CAMERA_VSYNC_PIN    (43) //42
#define CAMERA_PWDN_PIN     (44) //45
#define CAMERA_RST_PIN      (42) //43
#define CAMERA_SDA_PIN      (40) //40
#define CAMERA_SCL_PIN      (41) //41

// flash filesystem config
#define FLASH_FS_ADDR       (0xD00000)
#define FLASH_FS_SIZE       (0x300000)

// optional
#define TFT_WIDTH           (240)
#define TFT_HEIGHT          (280)
#define TFT_MOSI            (18)
#define TFT_MISO            (-1)
#define SDA_PIN             (29)
#define SCL_PIN             (28)
#define LCD_BL_PIN          (-1)
#define BOOT_PIN            (-1)
#define KEY_1_PIN           (36)
#define KEY_2_PIN           (37)
#define LED_R_PIN           (6)
#define LED_G_PIN           (9)
#define LED_B_PIN           (8)
#define LED_W_PIN           (7)

#endif /* Pins_Arduino_h */
