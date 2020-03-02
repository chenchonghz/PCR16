#include "ad7124.h"

#define AD7124_DATA_READY()		HAL_GPIO_ReadPin(AD_MISO_GPIO_Port, AD_MISO_Pin)
#define	AD7124_REF_VOLTAGE		(2500) //参考电压 mV
#define	DEFAULT_VDD						(float)(3.3)
_ad7124_t ad7124;

//static float CalcADCVoltage(u32 adcode);
static void AD7124ChannelEnable(void);
//static void StartADDataCollect(void);

void AD7124Init(void)
{
	u8 ad7124_id;

	ad7124.status = AD7124_IDLE;
	ad7124.busy = DEF_Idle;
	ad7124.channel = uCH_0;
	ad7124.channel_last = uCH_0;
	
	ad7124.pdev = bsp_ad7124_init(AD7124_ID1);
	ad7124_id = bsp_ad7124_id_get(ad7124.pdev);
	if(ad7124_id == AD7124_ID)	{
//		sys_error.Y2.bits.b2 = SENSOR_OK;
	}else	{
//		sys_error.Y2.bits.b2 = SENSOR_ERROR;//AD7124异常
	}
	bsp_ad7124_filterreg_set(ad7124.pdev);
	bsp_ad7124_control_set(ad7124.pdev);//连续采样模式
	AD7124ChannelEnable();//通道配置
}

static void AD7124ChannelEnable(void)
{
	ad7124_chcfg_t g_chcfg_tbl1=\
	{uCH_0, DEF_Enable, CONFIG_0,  UNIPOLAR, 0, AIN0, AIN_AVSS, 1, AD7124_GAIN_64_MUL};//channel_0 配置寄存器0 单极性 内部增益64
	
	bsp_ad7124_cfg_set(ad7124.pdev, &g_chcfg_tbl1);//初始化配置寄存器0 使用内部参考电压 内部增益64 用于压力采集
	g_chcfg_tbl1.channel = uCH_0;
	g_chcfg_tbl1.config_idx = CONFIG_0;
	g_chcfg_tbl1.AINP = AIN0;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_0使用配置寄存器0 配置
	g_chcfg_tbl1.channel = uCH_1;
	g_chcfg_tbl1.AINP = AIN1;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_1使用配置寄存器0 配置
	g_chcfg_tbl1.channel = uCH_2;
	g_chcfg_tbl1.AINP = AIN2;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_2使用配置寄存器0 配置
	g_chcfg_tbl1.channel = uCH_3;
	g_chcfg_tbl1.AINP = AIN3;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_3使用配置寄存器0 配置	
	g_chcfg_tbl1.channel = uCH_4;
	g_chcfg_tbl1.AINP = AIN4;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_3使用配置寄存器0 配置	
	g_chcfg_tbl1.channel = uCH_5;
	g_chcfg_tbl1.AINP = AIN5;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_3使用配置寄存器0 配置	
	
	ad7124.channel = uCH_0;//起始通道
	ad7124.channel_last = uCH_5;//最后一个通道		
	ad7124.status = AD7124_MEASURE_TEMP;		
}

void StartADDataCollect(void)
{
	u8 r_channel;
	u8 ad7124_err;
	u32 ad_code;
	float temp;
	
	ad7124_cs_low();
	if(AD7124_DATA_READY())	{//RDY引脚为低时 表示转换完成
		ad7124_cs_high();
		return;
	}
	r_channel = bsp_ad7124_conv_ready(ad7124.pdev, &ad7124_err);//回读当前采样通道
	if(ad7124_err==AD7124_ERR_NONE)	{
		if(r_channel == ad7124.channel)	{
			ad_code = bsp_ad7124_value_get(ad7124.pdev); //读取ADC转换结果				
			temp = CalcADCVoltage(ad_code); //计算电压				
//			calc_time = HAL_GetTick() - calc_start;
//			calc_start = HAL_GetTick();
			switch(r_channel)	{
				case uCH_0:	//
					break;
				case uCH_1:	
					break;
				case uCH_2:	
					break;
				case uCH_3:	
					break;
				case uCH_4:	
					break;
				case uCH_5:	
					break;
			}
		}
	}
}
//返回电压值mV
float CalcADCVoltage(u32 adcode)
{
	float adtemp;
	
//	adtemp = (adcode*1.0/0x800000) - 1;//双极性电压计算公式
//	return (adtemp * AD7124_REF_VOLTAGE);
	adtemp = (adcode*1.0)/0x1000000;//单极性电压计算公式
	return adtemp*AD7124_REF_VOLTAGE;
}

