#include "TempCalc.h"

#define TEMP_TABLE_LIST_SIZE   121
#define REFERENCE_RESISTOR     50000
//#define SUPPLY_VDD             3300
//_tempctrl_t tempctrl[TEMP_ID_NUMS];

typedef struct _TEMP_TABLE_T
{
    short  	    temp;
    uint32_t		resistance;
} TEMP_TABLE_T;
//温度与电压对照表
const static  TEMP_TABLE_T temp_table[TEMP_TABLE_LIST_SIZE] =
{
{0,	220859},
{100,	220186},
{200,	219486},
{300,	218758},
{400,	218001},
{500,	217214},
{600,	216397},
{700,	215549},
{800,	214669},
{900,	213758},
{1000,	212814},
{1100,	211836},
{1200,	210825},
{1300,	209779},
{1400,	208698},
{1500,	207583},
{1600,	206431},
{1700,	205243},
{1800,	204019},
{1900,	202757},
{2000,	201459},
{2100,	200124},
{2200,	198751},
{2300,	197340},
{2400,	195892},
{2500,	194406},
{2600,	192882},
{2700,	191321},
{2800,	189723},
{2900,	188088},
{3000,	186415},
{3100,	184707},
{3200,	182962},
{3300,	181181},
{3400,	179366},
{3500,	177516},
{3600,	175632},
{3700,	173716},
{3800,	171767},
{3900,	169786},
{4000,	167775},
{4100,	165735},
{4200,	163666},
{4300,	161570},
{4400,	159447},
{4500,	157299},
{4600,	155128},
{4700,	152933},
{4800,	150718},
{4900,	148482},
{5000,	146228},
{5100,	143957},
{5200,	141670},
{5300,	139369},
{5400,	137055},
{5500,	134729},
{5600,	132394},
{5700,	130051},
{5800,	127701},
{5900,	125345},
{6000,	122986},
{6100,	120624},
{6200,	118262},
{6300,	115900},
{6400,	113540},
{6500,	111184},
{6600,	108833},
{6700,	106489},
{6800,	104152},
{6900,	101823},
{7000,	99506},
{7100,	97199},
{7200,	94906},
{7300,	92626},
{7400,	90361},
{7500,	88112},
{7600,	85880},
{7700,	83667},
{7800,	81472},
{7900,	79297},
{8000,	77142},
{8100,	75009},
{8200,	72898},
{8300,	70810},
{8400,	68746},
{8500,	66705},
{8600,	64689},
{8700,	62698},
{8800,	60733},
{8900,	58793},
{9000,	56880},
{9100,	54993},
{9200,	53133},
{9300,	51301},
{9400,	49495},
{9500,	47717},
{9600,	45967},
{9700,	44244},
{9800,	42549},
{9900,	40882},
{10000,	39243},
{10100,	37631},
{10200,	36046},
{10300,	34490},
{10400,	32960},
{10500,	31458},
{10600,	29983},
{10700,	28535},
{10800,	27114},
{10900,	25719},
{11000,	24351},
{11100,	23008},
{11200,	21691},
{11300,	20400},
{11400,	19134},
{11500,	17892},
{11600,	16676},
{11700,	15483},
{11800,	14315},
{11900,	13170},
{12000,	12048},

};

static u32 config_temp_calculate_decimal(uint32_t max, uint32_t mix, uint32_t resistance)
{
    int i;
    uint32_t step, resistance_1;

    step = (max - mix) / 10;
    for(i = 0; i < 10 + 1; i++)
    {
        resistance_1 = max - i * step;
        if(resistance_1 < resistance )
            return i * 10;
    }

    return 0;
}

static u8 config_lookup_temp_tables(uint32_t resistance, s32 *outTemp)
{
    uint8_t i = 0;
    u32 difference;
    int table_size = sizeof(temp_table) / sizeof( TEMP_TABLE_T);
	
    if((resistance > temp_table[table_size - 1].resistance) && (resistance < temp_table[0].resistance))
    {
        for(i = 0; i < table_size; i++)
        {
            if(resistance == temp_table[i].resistance)
            {
                *outTemp = temp_table[i].temp;
                return 0;
            }
            if(temp_table[i].resistance < resistance )
            {
                difference = config_temp_calculate_decimal(temp_table[i - 1].resistance, temp_table[i].resistance, resistance);
                *outTemp = temp_table[i - 1].temp + difference;
                return 0;
            }
        }
    }		
	return 1;
}

//计算温度
u8 CalcTemperature(u32 Rx, s32 *ptemp)
{
	return config_lookup_temp_tables(Rx, ptemp);
}

