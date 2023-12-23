#include "OV7740.h"
//#include "BusDVP.h"
#include "math.h"

static const uint8_t sensor_default_regs[][2] = {
	{0x47, 0x02}  ,
	{0x17, 0x27}  ,
	{0x04, 0x40}  ,
	{0x1B, 0x81}  ,
	{0x29, 0x17}  ,
	{0x5F, 0x03}  ,
	{0x3A, 0x09}  ,
	{0x33, 0x44}  ,
	{0x68, 0x1A}  ,
	{0x14, 0x38}  ,
	{0x5F, 0x04}  ,
	{0x64, 0x00}  ,
	{0x67, 0x90}  ,
	{0x27, 0x80}  ,
	{0x45, 0x41}  ,
	{0x4B, 0x40}  ,
	{0x36, 0x2f}  ,
	{0x11, 0x00}  ,  // 60fps
	{0x36, 0x3f}  ,
	// {0x0c, 0x12}  , // default YUYV
	{0x12, 0x00}  ,
	{0x17, 0x25}  ,
	{0x18, 0xa0}  ,
	{0x1a, 0xf0}  ,
	{0x31, 0x50}  ,
	{0x32, 0x78}  ,
	{0x82, 0x3f}  ,
	{0x85, 0x08}  ,
	{0x86, 0x02}  ,
	{0x87, 0x01}  ,
	{0xd5, 0x10}  ,
	{0x0d, 0x34}  ,
	{0x19, 0x03}  ,
	{0x2b, 0xf8}  ,
	{0x2c, 0x01}  ,
	{0x53, 0x00}  ,
	{0x89, 0x30}  ,
	{0x8d, 0x30}  ,
	{0x8f, 0x85}  ,
	{0x93, 0x30}  ,
	{0x95, 0x85}  ,
	{0x99, 0x30}  ,
	{0x9b, 0x85}  ,
	{0xac, 0x6E}  ,
	{0xbe, 0xff}  ,
	{0xbf, 0x00}  ,
	{0x38, 0x14}  ,
	{0xe9, 0x00}  ,
	{0x3D, 0x08}  ,
	{0x3E, 0x80}  ,
	{0x3F, 0x40}  ,
	{0x40, 0x7F}  ,
	{0x41, 0x6A}  ,
	{0x42, 0x29}  ,
	{0x49, 0x64}  ,
	{0x4A, 0xA1}  ,
	{0x4E, 0x13}  ,
	{0x4D, 0x50}  ,
	{0x44, 0x58}  ,
	{0x4C, 0x1A}  ,
	{0x4E, 0x14}  ,
	{0x38, 0x11}  ,
	{0x84, 0x70}  ,
	{0, 0},
};

#define OV7740_SET_MIRROR(r, x)   ((r&0xBF)|((x&1)<<6))
#define OV7740_SET_FLIP(r, x)     ((r&0x7F)|((x&1)<<7))
#define OV7740_SET_SP(r, x)       ((r&0xEE)|((x&1)<<4)|(x&1))

#define NUM_BRIGHTNESS_LEVELS (9)
#define NUM_CONTRAST_LEVELS (9)
#define NUM_SATURATION_LEVELS (9)

static const uint8_t brightness_regs[NUM_BRIGHTNESS_LEVELS][2] = {
    {0x06, 0x40}, /* -4 */
    {0x06, 0x30}, /* -3 */
    {0x06, 0x20}, /* -2 */
    {0x06, 0x10}, /* -1 */
    {0x0E, 0x00}, /*  0 */
    {0x0E, 0x10}, /* +1 */
    {0x0E, 0x20}, /* +2 */
    {0x0E, 0x30}, /* +3 */
    {0x0E, 0x40}, /* +4 */
};

static const uint8_t contrast_regs[NUM_CONTRAST_LEVELS][3] = {
    {0x20, 0x10, 0xD0}, /* -4 */
    {0x20, 0x14, 0x80}, /* -3 */
    {0x20, 0x18, 0x48}, /* -2 */
    {0x20, 0x1C, 0x20}, /* -1 */
    {0x20, 0x20, 0x00}, /*  0 */
    {0x20, 0x24, 0x00}, /* +1 */
    {0x20, 0x28, 0x00}, /* +2 */
    {0x20, 0x2C, 0x00}, /* +3 */
    {0x20, 0x30, 0x00}, /* +4 */
};

static const uint8_t saturation_regs[NUM_SATURATION_LEVELS][2] = {
    {0x00, 0x00}, /* -4 */
    {0x10, 0x10}, /* -3 */
    {0x20, 0x20}, /* -2 */
    {0x30, 0x30}, /* -1 */
    {0x40, 0x40}, /*  0 */
    {0x50, 0x50}, /* +1 */
    {0x60, 0x60}, /* +2 */
    {0x70, 0x70}, /* +3 */
    {0x80, 0x80}, /* +4 */
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OV7740::OV7740(int8_t sda, int8_t scl, int i2cNum)
:Camera(i2cNum, uint16_t(0x21))
,_sdaPin(sda)
,_sclPin(scl)
{

}

OV7740::~OV7740(void)
{

}

int OV7740::reset(framesize_t framesize, camera_buffers_t *buff)
{
    int w = K210::_camera_resolution[framesize][0];
    int h = K210::_camera_resolution[framesize][1];

    if ((0x00 == w) || (0x00 == h))
    {
        return -1;
    }

    if ((0 > _sdaPin) || (0 > _sclPin))
    {
        return -1;
    }

    if((NULL == _wire) || (false == _wire->begin(_sclPin, _sdaPin, 1000 * 400)))
    {
		rt_kprintf("begin wire failed\n");
        return -1;
    }

    camera_pins_t pin = {
        .pclk = CAMERA_PCLK_PIN,
        .xclk = CAMERA_XCLK_PIN,
        .hsync = CAMERA_HSYNC_PIN,
        .vsync = CAMERA_VSYNC_PIN,
        .rst = CAMERA_RST_PIN,
        .pwdn = CAMERA_PWDN_PIN,
    };
    if(0 != begin(pin, 22, w, h, buff))
//    if(0 != begin(w, h, buff))
    {
        rt_kprintf("dvp begin failed\n");
        return -1;
    }
    delay(10);

    write_reg(uint8_t(0x12), uint8_t(0x80));
    delay(10);

    for(size_t i = 0; sensor_default_regs[i][0]; i++)
    {
        if(0xFF == sensor_default_regs[i][0])
        {
            delay(sensor_default_regs[i][1]);
            continue;
        }
        write_reg(sensor_default_regs[i][0], sensor_default_regs[i][1]);
        delay(1);
    }

    if(0x00 != set_framesize(framesize))
    {
        rt_kprintf("set output size failed\n");
        return -1;
    }

    _imgWidth = w;
    _imgHeight = h;

    _sensorVflip = false;
    _sensorHmirror = false;

    dvp_set_image_format(DVP_CFG_YUV_FORMAT);

    return 0;
}

int OV7740::read_id(void)
{
    int t, id = 0;

    if(0x00 != write_reg(uint8_t(0xFF), uint8_t(0x01)))
    {
        return -1;
    }

    read_reg(0x0A, &t);
    id = t << 8;
    read_reg(0x0B, &t);
    id |= t;

    return id;
}

int OV7740::set_framesize(framesize_t framesize)
{
    int ret = 0;
    int w = K210::_camera_resolution[framesize][0];
    int h = K210::_camera_resolution[framesize][1];

    // VGA
    if ((w > 320) || (h > 240))
    {
        ret |= write_reg(uint8_t(0x31), uint8_t(0xA0));
        ret |= write_reg(uint8_t(0x32), uint8_t(0xF0));
        ret |= write_reg(uint8_t(0x82), uint8_t(0x32));
    }
    // QVGA
    else if( ((w <= 320) && (h <= 240)) && ((w > 160) || (h > 120)) )
    {
        ret |= write_reg(uint8_t(0x31), uint8_t(0x50));
        ret |= write_reg(uint8_t(0x32), uint8_t(0x78));
        ret |= write_reg(uint8_t(0x82), uint8_t(0x3F));
    }
    // QQVGA
    else
    {
        ret |= write_reg(uint8_t(0x31), uint8_t(0x28));
        ret |= write_reg(uint8_t(0x32), uint8_t(0x3c));
        ret |= write_reg(uint8_t(0x82), uint8_t(0x3F));
    }    

    return ret;
}

int OV7740::set_hmirror(int enable)
{
    uint8_t reg;
    int ret = read_reg(0x0C, &reg);
    ret |= write_reg(uint8_t(0x0C), (uint8_t)OV7740_SET_MIRROR(reg, enable));
    _sensorHmirror = enable;

    ret = read_reg(0x16, &reg);
    ret |= write_reg(uint8_t(0x16), (uint8_t)OV7740_SET_SP(reg, enable));

    return ret;
}

int OV7740::set_vflip(int enable)
{
    uint8_t reg;
    int ret = read_reg(0x0C, &reg);
    ret |= write_reg(uint8_t(0x0C), (uint8_t)OV7740_SET_FLIP(reg, enable));
    _sensorVflip = enable;

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OV7740::set_windowing(framesize_t framesize, int x, int y, int w, int h)
{
    return -1;
}

int OV7740::set_contrast(int level)
{
    int ret = 0;
    uint8_t tmp = 0;

    level += (NUM_CONTRAST_LEVELS / 2);
    if (level < 0 || level >= NUM_CONTRAST_LEVELS)
    {
        return -1;
    }

    ret |= read_reg(0x81, &tmp);
    tmp |= 0x20;
    ret |= write_reg(uint8_t(0x81), tmp);
    ret |= read_reg(0xDA, &tmp);
    tmp |= 0x04;
    ret |= write_reg(uint8_t(0xDA), tmp);
    ret |= write_reg(uint8_t(0xE1), contrast_regs[level][0]);
    ret |= write_reg(uint8_t(0xE2), contrast_regs[level][1]);
    ret |= write_reg(uint8_t(0xE3), contrast_regs[level][2]);
    ret |= read_reg(0xE4, &tmp);
    tmp &= 0xFB;
    ret |= write_reg(uint8_t(0xE4), tmp);

    return ret;
}

int OV7740::set_brightness(int level)
{
    int ret = 0;
    uint8_t tmp = 0;

    level += (NUM_BRIGHTNESS_LEVELS / 2 + 1);
    if (level < 0 || level > NUM_BRIGHTNESS_LEVELS)
    {
        return -1;
    }

    ret |= read_reg(0x81, &tmp);
    tmp |= 0x20;
    ret |= write_reg(uint8_t(0x81), tmp);
    ret |= read_reg(0xDA, &tmp);
    tmp |= 0x04;
    ret |= write_reg(uint8_t(0xDA), tmp);
    ret |= write_reg(uint8_t(0xE4), brightness_regs[level][0]);
    ret |= write_reg(uint8_t(0xE3), brightness_regs[level][1]);

    return ret;
}

int OV7740::set_saturation(int level)
{
    int ret = 0;
    uint8_t tmp = 0;

    level += (NUM_CONTRAST_LEVELS / 2);
    if (level < 0 || level > NUM_CONTRAST_LEVELS) {
        return -1;
    }

    ret |= read_reg(0x81, &tmp);
    tmp |= 0x20;
    ret |= write_reg(uint8_t(0x81), tmp);
    ret |= read_reg(0xDA, &tmp);
    tmp |= 0x02;
    ret |= write_reg(uint8_t(0xDA), tmp);
    ret |= write_reg(uint8_t(0xDD), saturation_regs[level][0]);
    ret |= write_reg(uint8_t(0xDE), saturation_regs[level][1]);

    return ret;
}

int OV7740::set_gainceiling(gainceiling_t gainceiling)
{
    int ret = 0;
    uint8_t tmp = 0;
    uint8_t ceiling = (uint8_t)gainceiling;
    if(ceiling > GAINCEILING_32X)
        ceiling = GAINCEILING_32X;

    tmp = (ceiling & 0x07) << 4;
    ret |= write_reg(uint8_t(0x14), tmp);

    return ret;
}

int OV7740::set_colorbar(int enable)
{
    int ret = 0;

    if(enable)
    {
        ret |= write_reg(uint8_t(0x38), uint8_t(0x07));
        ret |= write_reg(uint8_t(0x84), uint8_t(0x02));
    }
    else
    {
        ret |= write_reg(uint8_t(0x38), uint8_t(0x07));
        ret |= write_reg(uint8_t(0x84), uint8_t(0x00));
    }

    return ret;
}

int OV7740::set_auto_gain(int enable, float gain_db, float gain_db_ceiling)
{
    int ret = 0;
    uint8_t tmp = 0;
    uint16_t gain = (uint16_t)gain_db;
    uint8_t ceiling = (uint8_t)gain_db_ceiling;

    ret |= read_reg(0x13, &tmp);
    if(enable != 0)
    {
        ret |= write_reg(uint8_t(0x13), uint8_t(tmp | 0x04));
    }
    else
    {
        ret |= write_reg(uint8_t(0x13), uint8_t(tmp & 0xFB));
        if(gain != 0xFFFF && (uint16_t)gain_db_ceiling != 0xFFFF)
        {
            ret |= read_reg(0x15, &tmp);
            tmp = (tmp & 0xFC) | (gain>>8 & 0x03);
            ret |= write_reg(uint8_t(0x15), tmp);
            tmp = gain & 0xFF;
            ret |= write_reg(uint8_t(0x00), tmp);
            tmp = (ceiling & 0x07) << 4;
            ret |= write_reg(uint8_t(0x14), tmp);
        }
    }

    return ret;
}

int OV7740::get_gain_db(float *gain_db)
{
    int ret = 0;
    uint8_t tmp = 0;
    uint16_t gain;

    ret |= read_reg(0x00, &tmp);
    gain = tmp;
    ret |= read_reg(0x15, &tmp);
    gain |= ((uint16_t)(tmp & 0x03))<<8;
    *gain_db = (float)gain;

    return ret;
}

int OV7740::set_auto_exposure(int enable, int exposure_us)
{
    int ret = 0;
    uint8_t tmp = 0;

    ret |= read_reg(0x13, &tmp);
    if(enable != 0)
    {
        ret |= write_reg(uint8_t(0x13), uint8_t(tmp | 0x01));
    }
    else
    {
        ret |= write_reg(uint8_t(0x13), uint8_t(tmp & 0xFE));
        ret |= write_reg(uint8_t(0x0F), (uint8_t)(exposure_us>>8));
        ret |= write_reg(uint8_t(0x10), (uint8_t)exposure_us);
    }

    return ret;
}

int OV7740::get_exposure_us(int *exposure_us)
{
    int ret=0;
    uint8_t tmp = 0;

    ret |= read_reg(0x0F, &tmp);
    *exposure_us = tmp<<8 & 0xFF00;
    ret |= read_reg(0x10, &tmp);
    *exposure_us = tmp | *exposure_us;

    return ret;
}

int OV7740::set_auto_whitebal(int enable, uint8_t r_gain_db, uint8_t g_gain_db, uint8_t b_gain_db)
{
    int ret = 0;
    uint8_t tmp = 0;

    ret |= read_reg(0x80, &tmp);
    if(enable != 0)
    {
        ret |= write_reg(uint8_t(0x80), uint8_t(tmp | 0x14));
    }
    else
    {
        if((uint16_t)r_gain_db!= 0xFFFF && (uint16_t)g_gain_db!=0xFFFF && (uint16_t)b_gain_db!=0xFFFF)
        {
            ret |= write_reg(uint8_t(0x80), uint8_t(tmp & 0xEF));
            ret |= write_reg(uint8_t(0x01), (uint8_t)b_gain_db);
            ret |= write_reg(uint8_t(0x02), (uint8_t)r_gain_db);
            ret |= write_reg(uint8_t(0x03), (uint8_t)g_gain_db);
        }
        else
        {
            ret |= write_reg(uint8_t(0x80), uint8_t(tmp & 0xEB));
        }
    }

    return ret;
}

int OV7740::get_rgb_gain_db(uint8_t *r_gain_db, uint8_t *g_gain_db, uint8_t *b_gain_db)
{
    int ret = 0;
    uint8_t tmp = 0;

    ret |= read_reg(0x02, &tmp);
    *r_gain_db = (float)tmp;
    ret |= read_reg(0x03, &tmp);
    *g_gain_db = (float)tmp;
    ret |= read_reg(0x01, &tmp);
    *b_gain_db = (float)tmp;

    return ret;
}
