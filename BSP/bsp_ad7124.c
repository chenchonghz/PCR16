/*
********************************************************************************
* bsp_ad7124.c
*
*   Copyright (C) 2010-2016 ACEA Biosciences, Inc. All rights reserved.
*   Author: AlexShi <shiweining123@163.com>
*
********************************************************************************
*/

/*
********************************************************************************
* Included Files
********************************************************************************
*/

#include "ad7124.h"

/*
********************************************************************************
* Pre-processor Definitions
********************************************************************************
*/

#define AD7124_CS_LOW()          	 HAL_GPIO_WritePin(AD7124_CS_GPIO_Port, AD7124_CS_Pin, GPIO_PIN_RESET)//AD7124_SCLK_GPIO_Port
#define AD7124_CS_HIGH()            HAL_GPIO_WritePin(AD7124_CS_GPIO_Port, AD7124_CS_Pin, GPIO_PIN_SET)

/*
********************************************************************************
* Shared Types
********************************************************************************
*/



/*
********************************************************************************
* Private Types
********************************************************************************
*/



/*
********************************************************************************
* Private Function Prototypes
********************************************************************************
*/

static void ad7124_write(ad7124_dev_t *pdev, CPU_INT08U nbits, CPU_INT32U data);
static CPU_INT32U ad7124_read(ad7124_dev_t *pdev, CPU_INT08U nbits);
static void ad7124_reg_write(ad7124_dev_t *pdev, CPU_INT08U reg, CPU_INT08U nbits, CPU_INT32U data);
static CPU_INT32U ad7124_reg_read(ad7124_dev_t *pdev, CPU_INT08U reg, CPU_INT08U nbits);

/*
********************************************************************************
* Shared Data
********************************************************************************
*/



/*
********************************************************************************
* Private Data
********************************************************************************
*/

static ad7124_dev_t g_ad7124_dev[AD7124_ID_NUMS] = {
    [AD7124_ID1] = {
        .id     = AD7124_ID1,
        //.spix   = CONFIG_AD7124_ID1_SPI,
        .clksrc = AD7124_CLKSRC_ExtClock,
        .vref   = 3.065f,
       // .cs_mask= AD7124_ID1_CS
    }
};

/*
********************************************************************************
* Private Functions
********************************************************************************
*/

/*
 * @brief
 *
 * @param
 *
 * @return
 *
 * @notes
 */
#if CONFIG_AD7124_LOCK_EN
static void __unused bsp_ad7124_lock(ad7124_dev_t *pdev)
{
    CPU_INT08U os_err;

    OSSemPend(pdev->lock, 0, &os_err);
    OSErrorCheck(os_err);
}

static void __unused bsp_ad7124_unlock(ad7124_dev_t *pdev)
{
    CPU_INT08U os_err;

    os_err = OSSemPost(pdev->lock);
    OSErrorCheck(os_err);
}
#endif

void ad7124_cs_low()
{
	AD7124_CS_LOW();
}

void ad7124_cs_high()
{
	AD7124_CS_HIGH();
}
//extern void ad7124_spi_write(spi_t *pdev, CPU_INT08U nbits, CPU_INT32U txdat);
static void ad7124_write(ad7124_dev_t *pdev, CPU_INT08U nbits, CPU_INT32U data)
{
    //spi_t *pspi = bsp_spi_get(pdev->spix);

    //bsp_spi_lock(pdev->p_spi);
    //bsp_spi_sck_high(pdev->p_spi);
//    ad7124_cs_low();

    //bsp_spi_rw(pdev->p_spi, MSB_POSEDGE, nbits, data);
    ad7124_spi_write(pdev->p_spi, nbits, data);

//    ad7124_cs_high();
   // bsp_spi_sck_low(pdev->p_spi);
    //bsp_spi_unlock(pdev->p_spi);
}

//extern CPU_INT32U ad7124_spi_read(spi_t *pdev, CPU_INT08U nbits);
static CPU_INT32U ad7124_read(ad7124_dev_t *pdev, CPU_INT08U nbits)
{
    //spi_t *pspi = bsp_spi_get(pdev->spix);
    CPU_INT32U ret;

    //bsp_spi_lock(pdev->p_spi);
    //bsp_spi_sck_high(pdev->p_spi);
    //ad7124_cs_low();

    //ret = bsp_spi_rw(pdev->p_spi, MSB_POSEDGE, nbits, 0xffff);
    ret = ad7124_spi_read(pdev->p_spi, nbits);

    //ad7124_cs_high();
    //bsp_spi_sck_low(pdev->p_spi);
    //bsp_spi_unlock(pdev->p_spi);

    return ret;
}

static void ad7124_reg_write(ad7124_dev_t *pdev, CPU_INT08U reg, CPU_INT08U nbits, CPU_INT32U data)
{
    ad7124_reg_comm_t r;

    r.ubyte = 0;
    r.bits.RW = AD7124_W;
    r.bits.RS = reg;
	
	ad7124_cs_low();
    ad7124_write(pdev, DEF_INT_08_BITS, r.ubyte);
    ad7124_write(pdev, nbits, data);
	ad7124_cs_high();
}

static CPU_INT32U ad7124_reg_read(ad7124_dev_t *pdev, CPU_INT08U reg, CPU_INT08U nbits)
{
    ad7124_reg_comm_t r;
	CPU_INT32U ret;

    r.ubyte = 0;
    r.bits.RW = AD7124_R;
    r.bits.RS = reg;
	
	ad7124_cs_low();
    ad7124_write(pdev, DEF_INT_08_BITS, r.ubyte);
    ret = ad7124_read(pdev, nbits);
	ad7124_cs_high();
	return ret;
}


/*
********************************************************************************
* Public Functions
********************************************************************************
*/

/*
 * @brief
 *
 * @param
 *
 * @return
 *
 * @notes
 */
void bsp_ad7124_reset(ad7124_dev_t *pdev)
{
	ad7124_cs_low();
    ad7124_write(pdev, 32, 0xFFFFFFFF);
    ad7124_write(pdev, 32, 0xFFFFFFFF);
	ad7124_cs_high();
	HAL_Delay(1);
}

CPU_INT08U bsp_ad7124_status_get(ad7124_dev_t *pdev)
{
    CPU_INT08U ret;

    ret = (CPU_INT08U)ad7124_reg_read(pdev, AD7124_REG_STATUS, DEF_INT_08_BITS);
    return ret;
}

CPU_INT08U bsp_ad7124_id_get(ad7124_dev_t *pdev)
{
    CPU_INT08U ret;

    ret = (CPU_INT08U)ad7124_reg_read(pdev, AD7124_REG_ID, DEF_INT_08_BITS);
    return ret;
}
#if 0
CPU_INT32U bsp_ad7124_mode_get(ad7124_dev_t *pdev)
{
    CPU_INT32U ret;

    ret = (CPU_INT32U)ad7124_reg_read(pdev, AD7124_REG_MODE, DEF_INT_24_BITS);
    return ret;
}

CPU_INT32U bsp_ad7124_cfg_get(ad7124_dev_t *pdev)
{
    CPU_INT32U ret;

    ret = (CPU_INT32U)ad7124_reg_read(pdev, AD7124_REG_CONFIGURATION, DEF_INT_24_BITS);
    return ret;
}

CPU_INT08U bsp_ad7124_gpocon_get(ad7124_dev_t *pdev)
{
    CPU_INT08U ret;

    ret = (CPU_INT08U)ad7124_reg_read(pdev, AD7124_REG_GPOCON, DEF_INT_08_BITS);
    return ret;
}

CPU_INT32U bsp_ad7124_offset_get(ad7124_dev_t *pdev)
{
    CPU_INT32U ret;

    ret = (CPU_INT32U)ad7124_reg_read(pdev, AD7124_REG_OFFSET, DEF_INT_24_BITS);
    return ret;
}

CPU_INT32U bsp_ad7124_fullscale_get(ad7124_dev_t *pdev)
{
    CPU_INT32U ret;

    ret = (CPU_INT32U)ad7124_reg_read(pdev, AD7124_REG_FULL_SCALE, DEF_INT_24_BITS);
    return ret;
}
#endif
CPU_INT32U bsp_ad7124_cfg_get(ad7124_dev_t *pdev, u8 config_x)
{
    CPU_INT32U ret;

    ret = (CPU_INT32U)ad7124_reg_read(pdev, AD7124_REG_CONFIG + config_x, DEF_INT_16_BITS);
    return ret;
}

CPU_INT32U bsp_ad7124_channel_get(ad7124_dev_t *pdev, u8 channel)
{
	CPU_INT32U ret;

    ret = (CPU_INT32U)ad7124_reg_read(pdev, AD7124_REG_CHANNEL + channel, DEF_INT_16_BITS);
    return ret;
}

CPU_INT32U bsp_ad7124_control_get(ad7124_dev_t *pdev)
{
	CPU_INT32U ret;

    ret = (CPU_INT32U)ad7124_reg_read(pdev, AD7124_REG_CONTROL, DEF_INT_16_BITS);
    return ret;
}

CPU_INT32U bsp_ad7124_errorreg_get(ad7124_dev_t *pdev)
{
	CPU_INT32U ret;
	
	ret = (CPU_INT32U)ad7124_reg_read(pdev, AD7124_REG_ERROR, DEF_INT_24_BITS);
	return ret;
}

void bsp_ad7124_channel_set(ad7124_dev_t *pdev, const ad7124_chcfg_t *pcfg)
{
    ad7124_reg_channel_t r;

    r.uword = 0;
    r.bits.EANBLE = pcfg->enable;
    r.bits.SETUP = pcfg->config_idx;
    r.bits.O = 0;
    r.bits.AINP = pcfg->AINP;
    r.bits.AINM = pcfg->AINM;
	ad7124_reg_write(pdev, AD7124_REG_CHANNEL + pcfg->channel, DEF_INT_16_BITS, r.uword);
}

void bsp_ad7124_control_set(ad7124_dev_t *pdev)
{
    ad7124_reg_ctrl_t r;

    r.uword = 0;
    r.bits.DOUT_RDY_DEL = 0;
    r.bits.CONT_READ = 0;
    r.bits.DATA_STATUS = 1;
    r.bits.CS_EN = 1;//1-cs上升沿 DOUT引脚切换为RDY引脚
    r.bits.REF_EN = 0;
    r.bits.POWER_MODE  = 2;//01 = mid power 02 = full power
	r.bits.Mode = AD7124_MODE_CONTINUOUS_CNV;//*/AD7124_MODE_SINGLE_CNV; //单次触发模式
	r.bits.CLK_SEL = 0;//00 = internal 614.4 kHz clock. The internal clock is not available at the CLK pin
    ad7124_reg_write(pdev, AD7124_REG_CONTROL, DEF_INT_16_BITS, r.uword);
}
//u32 cfgwordset;
void bsp_ad7124_cfg_set(ad7124_dev_t *pdev, const ad7124_chcfg_t *pcfg)
{
    ad7124_reg_cfg_t r;

    r.uword = 0;
	r.bits.U_B    = pcfg->U_B;
	r.bits.BURNOUT = pcfg->burnout;
	r.bits.AINBUFP    = 1;
	r.bits.AINBUFM    = 1;
	r.bits.REFSEL = 0x0;//使用内部参考电压
    r.bits.GAIN   = pcfg->intgain;
	//cfgwordset = r.uword;
    ad7124_reg_write(pdev, AD7124_REG_CONFIG + pcfg->config_idx, DEF_INT_16_BITS, r.uword);
}

void bsp_ad7124_errorreg_set(ad7124_dev_t *pdev, u8 flag)
{
	ad7124_reg_error_t r;
	
	r.uword = 0;
	r.bits.SPI_CRC_ERR = flag;
	ad7124_reg_write(pdev, AD7124_REG_ERROR, DEF_INT_16_BITS, r.uword);
}

void bsp_ad7124_filterreg_set(ad7124_dev_t *pdev)
{
	ad7124_reg_filter_t r;
	
	r.uword = 0;
	r.bits.FS = 30;
	ad7124_reg_write(pdev, 0x21, DEF_INT_24_BITS, r.uword);
}
ad7124_dev_t* bsp_ad7124_find(CPU_INT08U id)
{
    if (id < AD7124_ID_NUMS) {
        return &g_ad7124_dev[id];
    }

    return NULL;
}

CPU_INT08U bsp_ad7124_conv_ready(ad7124_dev_t *pdev, CPU_INT08U *perr)
{
//    CPU_INT32U ret = 0;
    CPU_INT08U status, errint;

    status = bsp_ad7124_status_get(pdev);
    if (status == 0xFF) {
        errint = AD7124_ERR_INACTIVE;
        goto _exit;
    }

    if (status & AD7124_STATUS_ERR) {
        if (status & AD7124_STATUS_NOREF) {
            errint = AD7124_ERR_NOREF;
        } else {
            errint = AD7124_ERR_ACTIVE;
        }
    } else if (!(status & AD7124_STATUS_RDY)) {
        //ret = ad7124_reg_read(pdev, AD7124_REG_DATA, DEF_INT_24_BITS);
        errint = AD7124_ERR_NONE;
				
    } else {
        errint = AD7124_ERR_BUSY;
    }

_exit:
    *perr = errint;
		//*pchannel = status&0x0f;;
    return status&0x0f;
}

CPU_INT32U bsp_ad7124_value_get(ad7124_dev_t *pdev)
{
	return ad7124_reg_read(pdev, AD7124_REG_DATA, DEF_INT_24_BITS);
}

ad7124_dev_t* bsp_ad7124_init(CPU_INT08U id)
{
    ad7124_dev_t *pdev = NULL;

    if (id < AD7124_ID_NUMS) {
        pdev = &g_ad7124_dev[id];
    } else return pdev;

    if (pdev->flags & DEF_FLAGS_AD7124_INIT) return pdev;

    pdev->p_spi   = bsp_spi_get(SPI_ID1);
    bsp_ad7124_reset(pdev);

    pdev->flags |= DEF_FLAGS_AD7124_INIT;
    return pdev;
}

/*
********************************************************************************
* No More!
********************************************************************************
*/
