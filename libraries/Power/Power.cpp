#include "Power.h"

POWER Power = POWER();

POWER::POWER()
{
}

POWER::~POWER(void)
{
}

bool POWER::begin()
{
    sysctl_set_power_mode(SYSCTL_POWER_BANK3,SYSCTL_POWER_V33);

    Wire.begin((uint8_t) SCL_PIN, (uint8_t) SDA_PIN, 400000);
    Wire.beginTransmission(AXP_ADDR);
    int err = Wire.endTransmission();
    if (err) {
        Serial.printf("Power management ic not found.\n");
        return false;
    }

#if defined(ARDUINO_M5STICKV)
    // Clear the interrupts
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x46);
    Wire.write(0xFF);
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x23);
    Wire.write(0x08); //K210_VCore(DCDC2) set to 0.9V
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x33);
    Wire.write(0xC1); //190mA Charging Current
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x36);
    Wire.write(0x6C); //4s shutdown
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x91);
    Wire.write(0xF0); //LCD Backlight: GPIO0 3.3V
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x90);
    Wire.write(0x02); //GPIO LDO mode
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x28);
    Wire.write(0xF0); //VDD2.8V net: LDO2 3.3V,  VDD 1.5V net: LDO3 1.8V
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x27);
    Wire.write(0x2C); //VDD1.8V net:  DC-DC3 1.8V
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x12);
    Wire.write(0xFF); //open all power and EXTEN
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x23);
    Wire.write(0x08); //VDD 0.9v net: DC-DC2 0.9V
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x31);
    Wire.write(0x03); //Cutoff voltage 3.2V
    Wire.endTransmission();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x39);
    Wire.write(0xFC); //Turnoff Temp Protect (Sensor not exist!)
    Wire.endTransmission();

    fpioa_set_function(23, (fpioa_function_t)(FUNC_GPIOHS0 + 26));
    gpiohs_set_drive_mode(26, GPIO_DM_OUTPUT);
    gpiohs_set_pin(26, GPIO_PV_HIGH); //Disable VBUS As Input, BAT->5V Boost->VBUS->Charing Cycle

#elif defined(ARDUINO_MAIXCUBE)
    // Clear the interrupts
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x46);
    Wire.write(0xFF);
    Wire.endTransmission();
    // set target voltage and current of battery(axp173 datasheet PG.)
    // charge current (default)780mA -> 190mA
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x33);
    Wire.write(0xC1);
    Wire.endTransmission();
    // REG 10H: EXTEN & DC-DC2 control
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x10);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x10);
    Wire.write(reg & 0xFC);
    Wire.endTransmission();

    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x36);
    Wire.write(0x6C); //4s shutdown
    Wire.endTransmission();

    fpioa_set_function(LCD_BL_PIN, FUNC_TIMER0_TOGGLE1);
    pwm_init(PWM_DEVICE_0);
    pwm_set_frequency(PWM_DEVICE_0, PWM_CHANNEL_0, 1000, 0.5);
    pwm_set_enable(PWM_DEVICE_0, PWM_CHANNEL_0, 1);

#elif defined(ARDUINO_MAIXAMIGO)
    // Clear the interrupts
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x46);
    Wire.write(0xFF);
    Wire.endTransmission();
    //LDO4 - 0.8V (default 0x48 1.8V)
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x27);
    Wire.write(0x20);
    Wire.endTransmission();
    //LDO2/3 - LDO2 1.8V / LDO3 3.0V
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x28);
    Wire.write(0x0C);
    Wire.endTransmission();
    // REG 10H: EXTEN & DC-DC2 control
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x10);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x10);
    Wire.write(reg & 0xFC);
    Wire.endTransmission();

    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x36);
    Wire.write(0x6C); //4s shutdown
    Wire.endTransmission();

    fpioa_set_function(LCD_BL_PIN, FUNC_TIMER0_TOGGLE1);
    pwm_init(PWM_DEVICE_0);
    pwm_set_frequency(PWM_DEVICE_0, PWM_CHANNEL_0, 1000.0, 0.5);
    pwm_set_enable(PWM_DEVICE_0, PWM_CHANNEL_0, 1);

#elif defined(ARDUINO_MAIXDUINO)
    fpioa_set_function(LCD_BL_PIN, FUNC_TIMER0_TOGGLE1);
    pwm_init(PWM_DEVICE_0);
    pwm_set_frequency(PWM_DEVICE_0, PWM_CHANNEL_0, 1000.0, 0.5);
    pwm_set_enable(PWM_DEVICE_0, PWM_CHANNEL_0, 1);

#endif

    delay(20);

    return true;
}

bool POWER::isPmExist()
{
#if defined(ARDUINO_M5STICKV) || defined(ARDUINO_MAIXCUBE) || defined(ARDUINO_MAIXAMIGO)
    return true;
#else
    return false;
#endif
}

// backlight level 0 - 255;
void POWER::setBacklight(uint8_t level)
{
#if defined(ARDUINO_M5STICKV)
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x91);
    Wire.write(0xF0 & level); //LCD Backlight: GPIO0 3.3V
    Wire.endTransmission();
#elif defined(ARDUINO_MAIXCUBE) || defined(ARDUINO_MAIXAMIGO) || defined(ARDUINO_MAIXDUINO)
    float duty = (255 - level) / 255.0f;
    pwm_set_frequency(PWM_DEVICE_0, PWM_CHANNEL_0, 1000.0, duty);
#else
    return;
#endif
}

void POWER::reset()
{
    sysctl_reset(SYSCTL_RESET_SOC);
}

int8_t POWER::getBatteryLevel()
{
    uint8_t buf[4];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x78);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 4, 1);
    int siz = Wire.readBytes(buf, 4);
    if (siz < 0) return -1;

    uint16_t voltage = (buf[0] << 4) + buf[1];
    uint16_t current = (buf[2] << 5) + buf[3];
    int16_t res = (voltage > 3150) ? ((voltage - 3075) * 0.16f)
                : (voltage > 2690) ? ((voltage - 2690) * 0.027f)
                : 0;
    if (current > 16) res -= 16;

    return (res < 100) ? res : 100;
}

bool POWER::isCharging()
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x04);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    return reg & 0x04;
}

void POWER::powerOFF()
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x32);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x32);
    Wire.write(reg | 0x80);
    Wire.endTransmission();
}

void POWER::setAdcState(bool enable)
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x82);
    Wire.write(enable ? 0xff : 0x00);
    Wire.endTransmission();
}

bool POWER::getEXTEN()
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write((uint8_t)0x12);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    return reg & (1 << 6);
}

bool POWER::isACIN()
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write((uint8_t)0x00);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    return reg & 0x80;
}

bool POWER::isVBUS()
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write((uint8_t)0x00);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    return reg & 0x20;
}

bool POWER::getBatState()
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write((uint8_t)0x01);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    return reg & 0x20;
}

uint8_t POWER::getPekPress()
{
    Wire.beginTransmission(AXP_ADDR);
    Wire.write((uint8_t)0x46);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 1, 1);
    int reg = Wire.read();
    return reg & 0x03;
}

float POWER::getACINVoltage()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x56);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    int16_t voltage = (buf[0] << 4) + buf[1];

    return voltage * (1.7f / 1000.0f);
}

float POWER::getACINCurrent()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x58);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    uint16_t current = (buf[0] << 4) + buf[1];

    return current * 0.625f;
}

float POWER::getVBUSVoltage()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x5a);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    uint16_t voltage = (buf[0] << 4) + buf[1];

    return voltage * (1.7f / 1000.0f);
}

float POWER::getVBUSCurrent()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x5c);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    uint16_t current = (buf[0] << 4) + buf[1];

    return current * 0.375f;
}

float POWER::getInternalTemperature()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x5e);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    uint16_t voltage = (buf[0] << 4) + buf[1];

    return voltage * 0.1f - 144.7f;
}

float POWER::getBatteryPower()
{
    uint8_t buf[3];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x70);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 3, 1);
    int siz = Wire.readBytes(buf, 3);
    if (siz < 0) return -1;

    uint32_t power = (buf[0] << 16) + (buf[1] << 8) + buf[2];

    return power * (1.1f * 0.5f / 1000.0f);
}

float POWER::getBatteryVoltage()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x78);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    uint16_t voltage = (buf[0] << 4) + buf[1];

    return voltage * (1.1f / 1000.0f);
}

float POWER::getBatteryChargeCurrent()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x7a);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    uint16_t current = (buf[0] << 5) + buf[1];

    return current * 0.5f;
}

float POWER::getBatteryDischargeCurrent()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x7c);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    uint16_t current = (buf[0] << 5) + buf[1];

    return current * 0.5f;
}

float POWER::getAPSVoltage()
{
    uint8_t buf[2];
    Wire.beginTransmission(AXP_ADDR);
    Wire.write(0x7e);
    Wire.endTransmission();
    Wire.requestFrom(AXP_ADDR, 2, 1);
    int siz = Wire.readBytes(buf, 2);
    if (siz < 0) return -1;

    uint16_t voltage = (buf[0] << 4) + buf[1];

    return voltage * (1.4f / 1000.0f);
}
