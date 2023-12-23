#ifndef POWER_H
#define POWER_H
#include <Arduino.h>
#include <Wire.h>

#if defined(ARDUINO_M5STICKV) || defined(ARDUINO_MAIXCUBE) || defined(ARDUINO_MAIXAMIGO)
#define AXP_ADDR 0x34
#else
#define AXP_ADDR -1
#ifndef SDA_PIN
#define SDA_PIN -1
#define SCL_PIN -1
#endif
#endif
//#if defined(ARDUINO_M5STICKV)
//#define POWER_DEVICE AXP192
//#define AXP_ADDR 0x34
//#define PIN_SDA 29
//#define PIN_SCL 28
//#elif defined(ARDUINO_MAIXCUBE)
//#define POWER_DEVICE AXP173
//#define AXP_ADDR 0x34
//#define PIN_SDA 31
//#define PIN_SCL 30
//#elif defined(ARDUINO_MAIXAMIGO)
//#define POWER_DEVICE AXP173
//#define AXP_ADDR 0x34
//#define PIN_SDA 27
//#define PIN_SCL 24
//#else
//#define POWER_DEVICE NONE
//#define AXP_ADDR -1
//#define PIN_SDA -1
//#define PIN_SCL -1
//#endif

class POWER {
    public:
        POWER();
        ~POWER(void);

        bool begin(void);
        bool isPmExist(void);
        void setBacklight(uint8_t level);
        void reset(void);

        int8_t getBatteryLevel(void);
        bool isCharging(void);
        void powerOFF(void);
        void setAdcState(bool enable);
        bool getEXTEN(void);
        bool isACIN(void);
        bool isVBUS(void);
        bool getBatState(void);
        uint8_t getPekPress(void);
        float getACINVoltage(void);
        float getACINCurrent(void);
        float getVBUSVoltage(void);
        float getVBUSCurrent(void);
        float getInternalTemperature(void);
        float getBatteryPower(void);
        float getBatteryVoltage(void);
        float getBatteryChargeCurrent(void);
        float getBatteryDischargeCurrent(void);
        float getAPSVoltage(void);

    private:
};

extern POWER Power;

#endif
