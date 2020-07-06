#include "ad7124.h"
#include "PD_DataProcess.h"

#define	AD7124_REF_VOLTAGE		(3300) //�ο���ѹ mV 3304
#define	DEFAULT_VDD						(float)(3.3)
_ad7124_t ad7124;

#define	AVER_MAX		15
#define	TEC_AVER_MAX		AVER_MAX
#define	PD_AVER_MAX		4
#define	TEC_DISCARD_NUM		4
#define	PD_DISCARD_NUM		1
#define	CH_AVERNUMS			6//��Ҫ��ƽ����ͨ������
struct _AdcCodeAver_t {
	u32 buf[AVER_MAX];
	u8 idx;
}AdcCodeAver[CH_AVERNUMS];//adc ���������� ��ֵ����

static float CalcADCVoltage(u32 adcode);
static void AD7124ChannelEnable(void);
static void CalcADCVolAverage(u8 ch, u32 advol, u8 aver_max, u8 discard);

u8 ad7124_id;
void AD7124Init(void)
{
//	u8 ad7124_id;

	ad7124.status = AD7124_IDLE;
	ad7124.busy = DEF_Idle;
	ad7124.channel = uCH_0;
	ad7124.channel_last = uCH_0;
	
	AdcCodeAver[uCH_0].idx = 0;
	AdcCodeAver[uCH_1].idx = 0;
	AdcCodeAver[uCH_2].idx = 0;
	AdcCodeAver[uCH_3].idx = 0;
	
	ad7124.pdev = bsp_ad7124_init(AD7124_ID1);
	ad7124_id = bsp_ad7124_id_get(ad7124.pdev);
	if(ad7124_id == AD7124_ID)	{
		SysError.Y1.bits.b2 = DEF_Active;
	}else	{
		SysError.Y1.bits.b2 = DEF_Inactive;//AD7124�쳣
	}
	bsp_ad7124_filterreg_set(ad7124.pdev);
	bsp_ad7124_control_set(ad7124.pdev);//��������ģʽ
	AD7124ChannelEnable();//ͨ������
}
//u32 ad7124reg_r;
static void AD7124ChannelEnable(void)
{
	ad7124_chcfg_t g_chcfg_tbl1=\
	{uCH_0, DEF_Enable, CONFIG_0,  UNIPOLAR, 0, AIN0, AIN_AVSS, 1, AD7124_GAIN_1_MUL};//channel_0 ���üĴ���0 ������ �ڲ�����64
	
	bsp_ad7124_cfg_set(ad7124.pdev, &g_chcfg_tbl1);//��ʼ�����üĴ���0 ʹ���ڲ��ο���ѹ �ڲ�����64 ����ѹ���ɼ�
//	ad7124reg_r = bsp_ad7124_cfg_get(ad7124.pdev, CONFIG_0);
	g_chcfg_tbl1.channel = uCH_0;
	g_chcfg_tbl1.config_idx = CONFIG_0;
	g_chcfg_tbl1.AINP = AIN0;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_0ʹ�����üĴ���0 ����

	g_chcfg_tbl1.channel = uCH_1;
	g_chcfg_tbl1.AINP = AIN1;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_1ʹ�����üĴ���0 ����
	g_chcfg_tbl1.channel = uCH_2;
	g_chcfg_tbl1.AINP = AIN2;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_2ʹ�����üĴ���0 ����
//	g_chcfg_tbl1.channel = uCH_3;
//	g_chcfg_tbl1.AINP = AIN3;
//	g_chcfg_tbl1.AINM = AIN_AVSS;
//	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_3ʹ�����üĴ���0 ����	
	g_chcfg_tbl1.channel = uCH_4;
	g_chcfg_tbl1.AINP = AIN4;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_3ʹ�����üĴ���0 ����	
//	g_chcfg_tbl1.channel = uCH_5;
//	g_chcfg_tbl1.AINP = AIN5;
//	g_chcfg_tbl1.AINM = AIN_AVSS;
//	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_3ʹ�����üĴ���0 ����	
	
	ad7124.channel = uCH_0;//��ʼͨ��
	ad7124.channel_last = uCH_5;//���һ��ͨ��		
	ad7124.status = AD7124_MEASURE_TEMP;		
}
//��/�رղ���PD��ADͨ��
void AD7124_PDChannelONOFF(u8 ch, u8 flag)
{
	ad7124_chcfg_t g_chcfg_tbl1=\
	{uCH_0, DEF_Enable, CONFIG_0,  UNIPOLAR, 0, AIN0, AIN_AVSS, 1, AD7124_GAIN_1_MUL};//channel_0 ���üĴ���0 ������ �ڲ�����64
	
	g_chcfg_tbl1.enable = flag;
	g_chcfg_tbl1.channel = ch;
	g_chcfg_tbl1.config_idx = CONFIG_0;
	g_chcfg_tbl1.AINP = ch;
	g_chcfg_tbl1.AINM = AIN_AVSS;
	bsp_ad7124_channel_set(ad7124.pdev, &g_chcfg_tbl1);//channel_0ʹ�����üĴ���0 ����
}
u8 r_channel;
u8 ad7124_err;
float ad_temp;
u32 ad_code;
//u32 calc_start=0,calc_time;
u8 StartADDataCollect(void)
{
//	u8 r_channel;
//	u8 ad7124_err;
//	float ad_temp;
//	u32 ad_code;
//	u8 delaycnt=0;
	
//	ad7124_cs_low();
//	while(AD7124_DATA_READY())	{//RDY����Ϊ��ʱ ��ʾת�����
//		delaycnt++;
//		if(delaycnt>10)	{
//			ad7124_cs_high();
//			return 0;
//		}
//	}
	r_channel = bsp_ad7124_conv_ready(ad7124.pdev, &ad7124_err);//�ض���ǰ����ͨ��
	if(ad7124.channel == r_channel)	{
		return 0;
	}
	ad7124.channel = r_channel;
//	HAL_GPIO_WritePin(GPIOC, Fluo_Green_Pin,GPIO_PIN_SET);
	if(ad7124_err==AD7124_ERR_NONE)	{
		if(r_channel <= ad7124.channel_last)	
		{
			ad_code = bsp_ad7124_value_get(ad7124.pdev); //��ȡADCת�����				
//			ad_temp = CalcADCVoltage(ad_code); //�����ѹ			
			switch(r_channel)	{
				case uCH_0:	//����ͨ�� ��ȷ��mv
					CalcADCVolAverage(uCH_0, ad_code, TEC_AVER_MAX, TEC_DISCARD_NUM);
					break;
				case uCH_1:	//����ͨ�� ��ȷ��mv
					CalcADCVolAverage(uCH_1, ad_code, TEC_AVER_MAX, TEC_DISCARD_NUM);
					break;
				case uCH_2://����ͨ�� ��ȷ��mv	 
					CalcADCVolAverage(uCH_2, ad_code, TEC_AVER_MAX, TEC_DISCARD_NUM);				
					break;
				case uCH_3:	//����ͨ�� ��ȷ��mv
					CalcADCVolAverage(uCH_3, ad_code, TEC_AVER_MAX, TEC_DISCARD_NUM);
					break;
				case uCH_4:	//PDͨ��
//					CalcADCVolAverage(uCH_4, ad_code, PD_AVER_MAX, PD_DISCARD_NUM);
					ad_temp = CalcADCVoltage(ad_code); //�����ѹ	
					PD_DataCollect(ad_temp, LED_BLUE);
					break;
				case uCH_5:	//PDͨ��
//					CalcADCVolAverage(uCH_5, ad_code, PD_AVER_MAX, PD_DISCARD_NUM);
					break;
				default:
					break;
			}
		}
	}
//	HAL_GPIO_WritePin(GPIOC, Fluo_Green_Pin,GPIO_PIN_RESET);
	return 1;
}

u32 GetADCVol(u8 ch)
{
	return ad7124.vol[ch];
}

//���ص�ѹֵmV
static float CalcADCVoltage(u32 adcode)
{
	float adtemp;
	
//	adtemp = (adcode*1.0/0x800000) - 1;//˫���Ե�ѹ���㹫ʽ
//	return (adtemp * AD7124_REF_VOLTAGE);
	adtemp = (adcode*1.0)/0x1000000;//�����Ե�ѹ���㹫ʽ
	return adtemp*AD7124_REF_VOLTAGE;
}

//����ÿ��ͨ����ѹ��ֵ���ȼ���adc���������ݾ�ֵ�� ȥ��1�����ֵ 1����Сֵ ȡƽ��
static void CalcADCVolAverage(u8 ch, u32 adcode, u8 aver_max, u8 discard)
{	
	u8 idx;
	
	idx = AdcCodeAver[ch].idx;
	AdcCodeAver[ch].buf[idx] = adcode;
	AdcCodeAver[ch].idx ++;
	if(AdcCodeAver[ch].idx >= aver_max)	{
		u32 temp;
		 /*---------------- ð������,��С�������� -----------------*/
    	s8 i,j,flag;
		i=0;
		idx = AdcCodeAver[ch].idx;

		do{
			flag=0;
			for (j=0;j<idx-i-1;j++)
			{
				if (AdcCodeAver[ch].buf[j] > AdcCodeAver[ch].buf[j+1])
				{
					temp = AdcCodeAver[ch].buf[j];
					AdcCodeAver[ch].buf[j]   = AdcCodeAver[ch].buf[j+1];
					AdcCodeAver[ch].buf[j+1] = temp;
					flag = 1;
				}
			}
			i++;
		}while ((i<idx) && flag);
		/*---------------- end -----------------*/
		/*----------- ȥ��1�����ֵ����Сֵ,��¼���ƫ����ݽ���ƽ�� --------------*/
		temp = 0;
		j = idx - discard;
		for (i=discard;i<j;i++)
		{
			temp += AdcCodeAver[ch].buf[i];
		}
		j = idx - discard*2;
		temp /= j;
		/*---------------- end -----------------*/
		ad7124.vol[ch] = CalcADCVoltage(temp)*100; //�����ѹ 0.01mv
		AdcCodeAver[ch].idx = 0;
	}
}

