#include "TempCtrl.h"

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
{0,	2208},
{100,	2201},
{200,	2194},
{300,	2187},
{400,	2180},
{500,	2172},
{600,	2163},
{700,	2155},
{800,	2146},
{900,	2137},
{1000,	2128},
{1100,	2118},
{1200,	2108},
{1300,	2097},
{1400,	2086},
{1500,	2075},
{1600,	2064},
{1700,	2052},
{1800,	2040},
{1900,	2027},
{2000,	2014},
{2100,	2001},
{2200,	1987},
{2300,	1973},
{2400,	1958},
{2500,	1944},
{2600,	1928},
{2700,	1913},
{2800,	1897},
{2900,	1880},
{3000,	1864},
{3100,	1847},
{3200,	1829},
{3300,	1811},
{3400,	1793},
{3500,	1775},
{3600,	1756},
{3700,	1737},
{3800,	1717},
{3900,	1697},
{4000,	1677},
{4100,	1657},
{4200,	1636},
{4300,	1615},
{4400,	1594},
{4500,	1572},
{4600,	1551},
{4700,	1529},
{4800,	1507},
{4900,	1484},
{5000,	1462},
{5100,	1439},
{5200,	1416},
{5300,	1393},
{5400,	1370},
{5500,	1347},
{5600,	1323},
{5700,	1300},
{5800,	1277},
{5900,	1253},
{6000,	1229},
{6100,	1206},
{6200,	1182},
{6300,	1159},
{6400,	1135},
{6500,	1111},
{6600,	1088},
{6700,	1064},
{6800,	1041},
{6900,	1018},
{7000,	995},
{7100,	971},
{7200,	949},
{7300,	926},
{7400,	903},
{7500,	881},
{7600,	858},
{7700,	836},
{7800,	814},
{7900,	792},
{8000,	771},
{8100,	750},
{8200,	728},
{8300,	708},
{8400,	687},
{8500,	667},
{8600,	646},
{8700,	626},
{8800,	607},
{8900,	587},
{9000,	568},
{9100,	549},
{9200,	531},
{9300,	513},
{9400,	494},
{9500,	477},
{9600,	459},
{9700,	442},
{9800,	425},
{9900,	408},
{10000,	392},
{10100,	376},
{10200,	360},
{10300,	344},
{10400,	329},
{10500,	314},
{10600,	299},
{10700,	285},
{10800,	271},
{10900,	257},
{11000,	243},
{11100,	230},
{11200,	216},
{11300,	204},
{11400,	191},
{11500,	178},
{11600,	166},
{11700,	154},
{11800,	143},
{11900,	131},
{12000,	120},

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

