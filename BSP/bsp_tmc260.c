/*
C********************************************************************************************************
*                                         Novocyte System Firmware
*
*                           (c) Copyright 2010-2014 ACEA Biosciences, Inc.
*                           All rights reserved.  Protected by international copyright laws.
*
* File      :  tmc260.c
* By        :  AlexShi
* Email     :  shiweining123@gmail.com
* Version   :  V1.00.00
* Compiler  :  NIOS II 12.1 SBT
*********************************************************************************************************
* Note(s)   :
*
*
*
*********************************************************************************************************
*/

#define TMC260_GLOBALS


/*
*********************************************************************************************************
*                                          INCLUDE HEADER FILES
*********************************************************************************************************
*/
#include "includes.h"
#include "bsp_tmc260.h"

/*
*********************************************************************************************************
*                                              LOCAL MACROS
*********************************************************************************************************
*/

//#define TMC260_CS_RCC_EN()	__HAL_RCC_GPIOI_CLK_ENABLE()
//#define TMC260_ID1_CS_PORT	GPIOB
//#define TMC260_ID1_CS_PIN	GPIO_Pin_12
//#define TMC260_ID2_CS_PORT	GPIOG
//#define TMC260_ID2_CS_PIN	GPIO_Pin_11

/*
*********************************************************************************************************
*                                         LOCAL MACROS FUNCTIONS
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                              MACROS ERROR
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                            LOCAL DATA TYPES
*********************************************************************************************************
*/

struct _tmc260_port {
	struct _io_map	cs;
};

typedef struct _tmc260_priv {
	const struct _tmc260_port	*const pport;
} tmc260_priv_t;

static const struct _tmc260_port g_tmc260_port[TMC260_ID_NUMS] = {
    [TMC260_ID1] = {
        .cs = {TMC260_CS_GPIO_Port,  TMC260_CS_Pin}
    },
    /*[TMC260_ID2] = {
        .cs = {TMC260_ID2_CS_PORT,  TMC260_ID2_CS_PIN}
    }*/
};

static tmc260_priv_t    g_tmc260_priv   [TMC260_ID_NUMS] = {
    [TMC260_ID1] = {
        .pport  = &g_tmc260_port[TMC260_ID1],
    },
	/*[TMC260_ID2] = {
		.pport	= &g_tmc260_port[TMC260_ID2],
	}*/
};

static tmc260_dev_t     g_tmc260_dev    [TMC260_ID_NUMS] = {
    [TMC260_ID1] = {
        .id = TMC260_ID1,
            .StallGuardConfig = {   // SGCSCONF
            .FilterEnable           = 1,
            .StallGuardThreshold    = 0,
            .CurrentScale           = 12
        },
        .DriverConfig = {       // DRVCONF
            .SlopeHighSide          = 1,
            .SlopeLowSide           = 1,
            .ProtectionDisable      = 0,
            .ProtectionTimer        = 0,
            .StepDirectionDisable   = 0,
            .VSenseScale            = 0,
            .ReadBackSelect         = TMC260_RB_SMART_ENERGY
        },
        .SmartEnergyControl = { // SMARTEN
            .SmartIMin              = 1,
            .SmartDownStep          = 2,
            .SmartStallLevelMax     = 15,
            .SmartUpStep            = 3,
            .SmartStallLevelMin     = 5
        },
        .StepDirConfig = {      // DRVCTRL
            .Intpol = 1,
            .DEdge  = 0,
            .MRes   = 3// 32Ï¸·Ö
        },
        .ChopperConfig = {      // CHOPCONF
            .BlankTime          = 2,
            .ChopperMode        = 0,
            .RandomTOff         = 0,
            .HysteresisDecay    = 0,
            .HysteresisEnd      = 9,
            .HysteresisStart    = 3,
            .TOff               = 5
        }
    },
/*	[TMC260_ID2] = {
            .id = TMC260_ID2,
            .StallGuardConfig = {   // SGCSCONF
            .FilterEnable           = 1,
            .StallGuardThreshold    = 0,
            .CurrentScale           = 25
        },
        .DriverConfig = {       // DRVCONF
            .SlopeHighSide          = 1,
            .SlopeLowSide           = 1,
            .ProtectionDisable      = 0,
            .ProtectionTimer        = 0,
            .StepDirectionDisable   = 0,
            .VSenseScale            = 0,
            .ReadBackSelect         = TMC260_RB_SMART_ENERGY
        },
        .SmartEnergyControl = { // SMARTEN
            .SmartIMin              = 1,
            .SmartDownStep          = 2,
            .SmartStallLevelMax     = 5,
            .SmartUpStep            = 2,
            .SmartStallLevelMin     = 0
        },
        .StepDirConfig = {      // DRVCTRL
            .Intpol = 1,
            .DEdge  = 0,
            .MRes   = 4
        },
        .ChopperConfig = {      // CHOPCONF
            .BlankTime          = 2,
            .ChopperMode        = 0,
            .RandomTOff         = 0,
            .HysteresisDecay    = 0,
            .HysteresisEnd      = 9,
            .HysteresisStart    = 3,
            .TOff               = 5
        }
	}*/
};



/* DRVCTRL - Driver Control register */
typedef union _DRVCTRL_REG {
    /* DRVCTRL register value */
    INT32U value;
    /* driver control in SPI mode(SDOFF=1) */
    struct {
        /*
        Magnitude of current flow through coil B.
        The range is 0 to 248, if hysteresis or offset are used up to their full extent.
        The resulting value after applying hysteresis or offset must not exceed 255.
        */
        INT32U CB                   :8; // Bit0~7
        /*
        Sign of current flow through coil B:
            0: Current flows from OB1 pins to OB2 pins.
            1: Current flows from OB2 pins to OB1 pins.
        */
        INT32U PHB                  :1; // Bit8
        /*
        Magnitude of current flow through coil A.
        The range is 0 to 248, if hysteresis or offset are used up to their full extent.
        The resulting value after  applying hysteresis or offset must not exceed 255.
        */
        INT32U CA                   :8; // Bit9~16
        /*
        Sign of current flow through coil A:
            0: Current flows from OA1 pins to OA2 pins.
            1: Current flows from OA2 pins to OA1 pins.
        */
        INT32U PHA                  :1; // Bit17
        /* register address bit - ALWAYS:0b00 */
        INT32U ADDR                 :2; // Bit18~19
        /* reserved */
        INT32U                      :12;// Bit20~31
    } BitsSDOFFSet;
    /* driver control in STEP/DIR mode(SDOFF=0) */
    struct {
        /*
        Microstep resolution for STEP/DIR mode
        Microsteps per 90¡ã:
            0b0000: 256
            0b0001: 128
            0b0010: 64
            0b0011: 32
            0b0100: 16
            0b0101: 8
            0b0110: 4
            0b0110: 2 (halfstep)
            0b1000: 1 (fullstep)
        */
        INT32U MRES                 :4; // Bit0~3
        /* reserved */
        INT32U                      :4; // Bit4~7
        /*
        Enable double edge STEP pulses
            0: Rising STEP pulse edge is active, falling edge is inactive.
            1: Both rising and falling STEP pulse edges are active
        */
        INT32U DEDGE                :1; // Bit8
        /*
        Enable STEP pulse interpolation
            0: Disable STEP pulse interpolation.
            1: Enable STEP pulse multiplication by 16.
        */
        INT32U INTPOL               :1; // Bit9
        /* reserved */
        INT32U                      :8; // Bit10~17
        /* register address bit - ALWAYS:0b00 */
        INT32U Address              :2; // Bit18~19
        /* reserved */
        INT32U                      :12;// Bit20~31
    } BitsSDOFFClr;
} DRVCTRL_REG_t;
#define DRVCTRL_REG_ADDR        0x00

/* CHOPCONF - Chopper Configuration Register */
typedef union _CHOPCONF_REG {
    /* CHOPCONF register value */
    INT32U value;
    struct {
    /*
    Off time / MOSFET disable
    Duration of slow decay phase. If TOFF is 0, the MOSFETs are shut off.
    If TOFF is nonzero, slow decay time is a multiple of system clock periods:
    N(clk) = 12 + (32 * TOFF) (Minimum time is 64clocks.)
        0b0000: Driver disable, all bridges off
        0b0001: 1 (use with TBL of minimum 24 clocks)
        0b0010 ... 0b1111: 2 ... 15
    */
        INT32U TOff                 :4; // Bit0~3(TOFF)
    /*
    Hysteresis start value or Fast decay time setting
    CHM=0   Hysteresis start offset from HEND:
                0b000: 1    0b100: 5
                0b001: 2    0b101: 6
                0b010: 3    0b110: 7
                0b011: 4    0b111: 8
            Effective: HEND+HSTRT must be ¡Ü 15
    CHM=1   Three least-significant bits of the duration of the fast decay phase.
            The MSB is HDEC0. Fast decay time is a multiple of system clock
            periods: N(clk) = 32 * (HDEC0+HSTRT)
    */
        INT32U HysteresisStart      :3; // Bit4~6(HSTRT)
    /*
    Hysteresis end (low) value or Sine wave offset
    CHM=0   0b0000 ... 0b111:
            Hysteresis is -3, -2, -1, 0, 1, ..., 12
            (1/512 of this setting adds to current setting)
            This is the hysteresis value which becomes used for the hysteresis chopper
    CHM=1   0b0000 ... 0b1111:
            Offset is -3, -2, -1, 0, 1, ..., 12
            This is the sine wave offset and 1/512 of the
            value becomes added to the absolute value
            of each sine wave entry.
    */
        INT32U HysteresisEnd        :4; // Bit7~10(HEND)
    /*
    Hysteresis decrement interval or Fast decay mode
    CHM=0   Hysteresis decrement period setting, in system clock periods:
                0b00: 16
                0b01: 32
                0b10: 48
                0b11: 64
    CHM=1   HDEC1=0: current comparator can terminate the fast decay
                     phase before timer expires.
            HDEC1=1: only  the  timer  terminates  the  fast decay phase.
            HDEC0:   MSB of fast decay time setting.
    */
        INT32U HysteresisDecay      :2; // Bit11~12(HDEC)
    /*
    Random TOFF time
        Enable randomizing the slow decay phase duration:
            0: Chopper off time is fixed as set by bits t(OFF)
            1: Random mode, t(OFF) is random modulated by
               dN(clk) = -12 ... +3 clocks.
    */
        INT32U RandomTOff           :1; // Bit13(RNDTF)
    /*
    Chopper mode
        This mode bit affects the interpretation of the HDEC,
        HEND, and HSTRT parameters shown below.
            0: Standard mode (spreadCycle)
            1: Constant t(OFF) with fast decay time.
               Fast decay time is also terminated when the negative nominal
               current is reached. Fast decay is after on time.
    */
        INT32U ChopperMode          :1; // Bit14(CHM)
    /*
    Blanking time
        Blanking time interval, in system clock periods:
            0b00: 16
            0b01: 24
            0b10: 36
            0b11: 54
    */
        INT32U BlankTime            :2; // Bit15~16(TBL)
    /* register address bit - ALWAYS:0b001 */
        INT32U Address              :3; // Bit17~19
    /* reserved */
        INT32U                      :12;// Bit20~31
    } Bits;
} CHOPCONF_REG_t;
#define CHOPCONF_REG_ADDR       0x01

/* SMARTEN - coolStep (Smart Energy) Configuration Register */
typedef union _SMARTEN_REG {
    /* SMARTEN register value */
    INT32U value;
    struct {
    /*
    Lower coolStep threshold/coolStep disable
    If SEMIN is 0, coolStep is disabled.
    If SEMIN is nonzero and the stallGuard2 value SG falls below SEMIN * 32,
    the coolStep current scaling factor is increased
    */
        INT32U SmartStallLevelMin   :4; // Bit0~3(SEMIN)
    /* reserved */
        INT32U                      :1; // Bit4
    /*
    Current increment size
    Number of current increment steps for each time that the
    stallGuard2 value SG is sampled below the lower threshold:
        0b00: 1
        0b01: 2
        0b10: 4
        0b11: 8
    */
        INT32U SmartUpStep          :2; // Bit5~6(SEUP)
    /* reserved */
        INT32U                      :1; // Bit7
    /*
    Upper coolStep threshold as an offset from the lower threshold
    If the stallGuard2 measurement value SG is sampled
    equal to or above (SEMIN+SEMAX+1) * 32 enough times,
    then the coil current scaling factor is decremented
    */
        INT32U SmartStallLevelMax   :4; // Bit8~11(SEMAX)
    /* reserved */
        INT32U                      :1; // Bit12
    /*
    Current decrement speed
    Number of times that the stallGuard2 value must be
    sampled equal to or above the upper threshold for each
    decrement of the coil current:
        0b00: 32
        0b01: 8
        0b10: 2
        0b11: 1
    */
        INT32U SmartDownStep        :2; // Bit13~14(SEDN)
    /*
    Minimum coolStep current
        0: 1/2 CS current setting
        1: 1/4 CS current setting
    */
        INT32U SmartIMin            :1; // Bit15(SEIMIN)
    /* reserved */
        INT32U                      :1; // Bit16
    /* register address bit - ALWAYS:0b101 */
        INT32U Address              :3; // Bit17~19
    /* reserved */
        INT32U                      :12;// Bit20~31
    } Bits;
} SMARTEN_REG_t;
#define SMARTEN_REG_ADDR        0x05

/* SGSCONF - stallGuard2 and Current Setting Register */
typedef union _SGSCONF_REG {
    /* SGSCONF register value */
    INT32U value;
    struct {
    /*
    Current scale (scales digital currents A & B)
    Current scaling for SPI and step/direction operation.
    0b00000 ... 0b11111: 1/32, 2/32, 3/32, ... 32/32
    This value is biased by 1 and divided by 32, so the range is 1/32 to 32/32.
    */
        INT32U CurrentScale         :5; // Bit0~4(CS)
    /* reserved */
        INT32U                      :3; // Bit5~7
    /*
    stallGuard2 threshold value
    The stallGuard2 threshold value controls the optimum measurement range
    for readout. A lower value results in a higher sensitivity and requires
    less torque to indicate a stall. The value is a two¡¯s complement signed
    integer.
    Values below -10 are not recommended.
    Range: -64 to +63
    */
        INT32U StallGuardThreshold  :7; // Bit8~14(SGT)
    /* reserved */
        INT32U                      :1; // Bit15
    /*
    stallGuard2 filter enable
        0: Standard mode, fastest response time.
        1: Filtered mode, updated once for each four fullsteps to
           compensate  for  variation in motor construction, highest
           accuracy.
    */
        INT32U FilterEnable         :1; // Bit16(SFILT)
    /* register address bit - ALWAYS:0b011 */
        INT32U Address              :3; // Bit17~19
    /* reserved */
        INT32U                      :12;// Bit20~31
    } Bits;
} SGCSCONF_REG_t;
#define SGCSCONF_REG_ADDR       0x03

/* DRVCONF - Driver Configuration Register */
typedef union _DRVCONF_REG {
    /* DRVCONF register value */
    INT32U value;
    struct {
    /* reserved */
        INT32U                      :4; // Bit0~3
    /*
    Select value for read out (RD bits)
        0b00  Microstep position read back
        0b01  stallGuard2 level read back
        0b10  stallGuard2 and coolStep current level read back
        0b11  Reserved, do not use
    */
        INT32U ReadBackSelect       :2; // Bit4~5(RDSEL)
    /*
    Sense resistor voltage-based current scaling
        0: Full-scale sense resistor voltage is 305mV.
        1: Full-scale sense resistor voltage is 165mV.
        (Full-scale refers to a current setting of 31 and a DAC value of 255.)
    */
        INT32U VSenseScale          :1; // Bit6(VSENSE)
    /*
    STEP/DIR interface disable
        0: Enable STEP and DIR interface.
        1: Disable STEP and DIR interface. SPI interface is used to move motor.
    */
        INT32U StepDirectionDisable :1; // Bit7(SDOFF)
    /*
    Short to GND detection timer
        0b00: 3.2¦Ìs.
        0b01: 1.6¦Ìs.
        0b10: 1.2¦Ìs.
        0b11: 0.8¦Ìs.
    */
        INT32U ProtectionTimer      :2; // Bit8~9(TS2G)
    /*
    Short to GND protection disable
        0: Short to GND protection is enabled.
        1: Short to GND protection is disabled.
    */
        INT32U ProtectionDisable    :1; // Bit10(DISS2G)
    /* reserved */
        INT32U                      :1; // Bit11
    /*
    Slope control, low side
        0b00: Minimum.
        0b01: Minimum.
        0b10: Medium.
        0b11: Maximum.
    */
        INT32U SlopeLowSide         :2; // Bit12~13(SLPL)
    /*
    Slope control, high side
        0b00: Minimum
        0b01: Minimum temperature compensation mode.
        0b10: Medium temperature compensation mode.
        0b11: Maximum
    In temperature compensated mode (tc), the MOSFET gate
    driver strength is increased if the overtemperature
    warning temperature is reached. This compensates for
    temperature dependency of high-side slope control.
    */
        INT32U SlopeHighSide        :2; // Bit14~15(SLPH)
    /*
    Test mode
    Must be cleared for normal operation. When set, the
    SG_TST output exposes digital test values, and the
    TEST_ANA output exposes analog test values. Test value
    selection is controlled by SGT1 and SGT0:
    TEST_ANA:   0b00: anatest_2vth,
                0b01: anatest_dac_out,
                0b10: anatest_vdd_half.
    SG_TST:     0b00: comp_A,
                0b01: comp_B,
                0b10: CLK,
                0b11: on_state_xy
    */
        INT32U TestMode             :1; // Bit16(TST)
    /* register address bit - ALWAYS:0b111 */
        INT32U Address              :3; // Bit17~19
    /* reserved */
        INT32U                      :12;// Bit20~31
    } Bits;
} DRVCONF_REG_t;
#define DRVCONF_REG_ADDR        0x07

/* DRVSTATUS register */
typedef union _DRVSTATUS_REG {
    /* DRVSTATUS register value */
    INT32U value;
    struct {
    /*
    stallGuard2 status
        0: No motor stall detected.
        1: stallGuard2 threshold has been reached, and the SG_TST output is driven high.
    */
        INT32U SG        :1;         // Bit0
    /*
    Overtemperature shutdown
        0: No overtemperature shutdown condition.
        1: Overtemperature shutdown has occurred.
    */
        INT32U OT        :1;         // Bit1
    /*
    Overtemperature warning
        0: No overtemperature warning condition.
        1: Warning threshold is active.
    */
        INT32U OTPW      :1;         // Bit2
    /*
    Short to GND detection bits on high-side transistors
        0: No short to ground shutdown condition.
        1: Short to ground shutdown condition. The short counter is incremented
           by each short circuit and the chopper cycle is suspended. The counter
           is decremented for each phase polarity change. The MOSFETs are shut off
           when the counter reaches 3 and remain shut off until the shutdown
           condition is cleared by disabling and re-enabling the driver. The shutdown
           conditions reset by deasserting the ENN input or clearing the TOFF parameter.
    */
        INT32U S2GA      :1;         // Bit3
        INT32U S2GB      :1;         // Bit4
    /*
    Open load indicator
        0: No open load condition detected.
        1: No chopper event has happened during the last period with constant coil
           polarity. Only a current above 1/16 of the maximum setting can clear this bit!
        Hint: This bit is only a status indicator. The chip takes no other action
           when this bit is set. False indications may occur during fast motion
           and at standstill. Check this bit only during slow motion.
    */
        INT32U OLA       :1;         // Bit5
        INT32U OLB       :1;         // Bit6
    /*
    Standstill indicator
        0: No standstill condition detected.
        1: No active edge occurred on the STEP input during the last 2^20 system clock cycles.
    */
        INT32U STST      :1;         // Bit7
    /* reserved */
        INT32U           :2;         // Bit8~9
    /*
    RDSEL=0b00  Microstep counter for coil A
                Microstep position in sine table for coil A in STEP/DIR mode.
                RD.bit9 is the Polarity bit:
                    0: Current flows from OA1 pins to OA2 pins.
                    1: Current flows from OA2 pins to OA1 pins.
    RDSEL=0b01  stallGuard2 value RD.bit9:bit0
    RDSEL=0b10  stallGuard2 value RD.bit9:bit5 and the actual coolStep scaling value RD.bit4:bit0
    */
        INT32U RD        :10;        // Bit10~19
    /* reserved */
        INT32U           :12;        // Bit20~31
    } Bits;
} DRVSTATUS_REG_t;

/* TMC260 configure register */
typedef struct _TMC260_REG {
    DRVCTRL_REG_t   DRVCTRL;
    CHOPCONF_REG_t  CHOPCONF;
    SMARTEN_REG_t   SMARTEN;
    SGCSCONF_REG_t  SGCSCONF;
    DRVCONF_REG_t   DRVCONF;
    DRVSTATUS_REG_t DRVSTATUS;
} TMC260_REG_t;

typedef struct _TMC260_Status {
    INT8U   Phase;
    INT8U   MStep;
    INT8U   SmartEnergy;
    INT8U   Flags;
    INT32U  StallGuard;
} TMC260_Status_t;

/*
#define CFG_XXXXXX_DRVCTRL_MRES         0
#define CFG_XXXXXX_DRVCTRL_DEDGE        DEF_DISABLED
#define CFG_XXXXXX_DRVCTRL_INTPOL       1

#define CFG_XXXXXX_CHOPCONF_TOFF        5
#define CFG_XXXXXX_CHOPCONF_HSTRT       3
#define CFG_XXXXXX_CHOPCONF_HEND        9
#define CFG_XXXXXX_CHOPCONF_HDEC        0
#define CFG_XXXXXX_CHOPCONF_RNDTF       0
#define CFG_XXXXXX_CHOPCONF_CHM         0
#define CFG_XXXXXX_CHOPCONF_TBL         2

#define CFG_XXXXXX_SMARTEN_SEMIN        0
#define CFG_XXXXXX_SMARTEN_SEUP         2
#define CFG_XXXXXX_SMARTEN_SEMAX        5
#define CFG_XXXXXX_SMARTEN_SEDN         2
#define CFG_XXXXXX_SMARTEN_SEIMIN       1

#define CFG_XXXXXX_SGCSCONF_CS          31
#define CFG_XXXXXX_SGCSCONF_SGT         0
#define CFG_XXXXXX_SGCSCONF_SFILT       DEF_ENABLED

#define CFG_XXXXXX_DRVCONF_RDSEL        TMC260_RB_SMART_ENERGY
#define CFG_XXXXXX_DRVCONF_VSENSE       0
#define CFG_XXXXXX_DRVCONF_SDOFF        0
#define CFG_XXXXXX_DRVCONF_TS2G         0
#define CFG_XXXXXX_DRVCONF_DISS2G       0
#define CFG_XXXXXX_DRVCONF_SLPL         1
#define CFG_XXXXXX_DRVCONF_SLPH         1
#define CFG_XXXXXX_DRVCONF_TST          0


static TMC260_REG_t XXXXXX_tmc260_reg = {
    .DRVCTRL.BitsSDOFFClr = {
        .MRES                   = CFG_XXXXXX_DRVCTRL_MRES,
        .DEDGE                  = CFG_XXXXXX_DRVCTRL_DEDGE,
        .INTPOL                 = CFG_XXXXXX_DRVCTRL_INTPOL,
        .Address                = DRVCTRL_REG_ADDR
    },
    .CHOPCONF.Bits = {
        .TOff                   = CFG_XXXXXX_CHOPCONF_TOFF,
        .HysteresisStart        = CFG_XXXXXX_CHOPCONF_HSTRT,
        .HysteresisEnd          = CFG_XXXXXX_CHOPCONF_HEND,
        .HysteresisDecay        = CFG_XXXXXX_CHOPCONF_HDEC,
        .RandomTOff             = CFG_XXXXXX_CHOPCONF_RNDTF,
        .ChopperMode            = CFG_XXXXXX_CHOPCONF_CHM,
        .BlankTime              = CFG_XXXXXX_CHOPCONF_TBL,
        .Address                = CHOPCONF_REG_ADDR
    },
    .SMARTEN.Bits = {
        .SmartStallLevelMin     = CFG_XXXXXX_SMARTEN_SEMIN,
        .SmartUpStep            = CFG_XXXXXX_SMARTEN_SEUP,
        .SmartStallLevelMax     = CFG_XXXXXX_SMARTEN_SEMAX,
        .SmartDownStep          = CFG_XXXXXX_SMARTEN_SEDN,
        .SmartIMin              = CFG_XXXXXX_SMARTEN_SEIMIN,
        .Address                = SMARTEN_REG_ADDR
    },
    .SGCSCONF.Bits = {
        .CurrentScale           = CFG_XXXXXX_SGCSCONF_CS,
        .StallGuardThreshold    = CFG_XXXXXX_SGCSCONF_SGT,
        .FilterEnable           = CFG_XXXXXX_SGCSCONF_SFILT,
        .Address                = SGCSCONF_REG_ADDR
    },
    .DRVCONF.Bits = {
        .ReadBackSelect         = CFG_XXXXXX_DRVCONF_RDSEL,
        .VSenseScale            = CFG_XXXXXX_DRVCONF_VSENSE,
        .StepDirectionDisable   = CFG_XXXXXX_DRVCONF_SDOFF,
        .ProtectionTimer        = CFG_XXXXXX_DRVCONF_TS2G,
        .ProtectionDisable      = CFG_XXXXXX_DRVCONF_DISS2G,
        .SlopeLowSide           = CFG_XXXXXX_DRVCONF_SLPL,
        .SlopeHighSide          = CFG_XXXXXX_DRVCONF_SLPH,
        .TestMode               = CFG_XXXXXX_DRVCONF_TST,
        .Address                = DRVCONF_REG_ADDR
    }
}
*/

/*
*********************************************************************************************************
*                                             LOCAL CONSTANTS
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                             LOCAL VARIABLES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                            EXTERN VARIABLES
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                            GLOBAL CONSTANTS
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void     TMC260_init         (void);
static INT32U   TMC260_spi_rdwr         (tmc260_dev_t *pdev, INT32U dat);
static INT32U   WriteStepDirConfig      (tmc260_dev_t *pdev);
static INT32U   WriteChopperConfig      (tmc260_dev_t *pdev);
static INT32U   WriteSmartEnergyControl (tmc260_dev_t *pdev);
static INT32U   WriteStallGuardConfig   (tmc260_dev_t *pdev);
static INT32U   WriteDriverConfig       (tmc260_dev_t *pdev);
static void     TMC260_configs_init     (tmc260_dev_t *pdev);


/*
*********************************************************************************************************
*                                           FUNCTION DEFINITION
*********************************************************************************************************
*/

/*F******************************************************************************************************
*
********************************************************************************************************/
static void TMC260_init(void)
{
//    GPIO_InitTypeDef GPIO_InitStructure;

    tmc260_dev_t  *pdev = &g_tmc260_dev[TMC260_ID1];
    tmc260_priv_t *priv = &g_tmc260_priv[TMC260_ID1];

    if (pdev->flags & TMC260_FLAGS_INIT) return;

    /* Enable GPIOC clocks */

//    GPIO_InitStructure.GPIO_Pin   = TMC260_ID1_CS_PIN;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    //GPIO_InitStructure.Pull =  GPIO_PULLUP;              //ÉÏÀ­
//    GPIO_Init(TMC260_ID1_CS_PORT, &GPIO_InitStructure);
    //HAL_GPIO_WritePin(TMC260_ID1_CS_PORT, TMC260_ID1_CS_PIN,GPIO_PIN_SET);
//    GPIO_SetBits(TMC260_ID1_CS_PORT, TMC260_ID1_CS_PIN);

    pdev->priv   = priv;
//    pdev->lock   = OSSemCreate(1);
    pdev->p_spi   = bsp_spi_get(SPI_ID2);
    pdev->flags |= TMC260_FLAGS_INIT;
}


/*F******************************************************************************************************
*
********************************************************************************************************/
static INT32U TMC260_spi_rdwr(tmc260_dev_t *pdev, INT32U dat)
{
    INT32U rxdat;
    tmc260_priv_t *priv = (tmc260_priv_t*)pdev->priv;
    struct _io_map const *cs = &priv->pport->cs;
	
//    bsp_spi_lock(pdev->p_spi);

    /* ÉèÖÃÆ¬Ñ¡ÐÅºÅ */
    SET_L(cs);

    rxdat = bsp_spi_rw(pdev->p_spi, MSB_POSEDGE, 20, dat);
	
    SET_H(cs);
//    bsp_spi_unlock(pdev->p_spi);

    return rxdat;
}


/*F******************************************************************************************************
*                                           ÅäÖÃDRVCTRL¼Ä´æÆ÷
********************************************************************************************************/
static INT32U WriteStepDirConfig(tmc260_dev_t *pdev)
{
    INT32U value;

    value = 0;

    if (pdev->StepDirConfig.Intpol) {
        value |= DEF_BIT09_MASK;
    }

    if (pdev->StepDirConfig.DEdge) {
        value |= DEF_BIT08_MASK;
    }

    if (pdev->StepDirConfig.MRes > 15) {
        pdev->StepDirConfig.MRes = 15;
    }
    value |= pdev->StepDirConfig.MRes;

    return TMC260_spi_rdwr(pdev, value);
}


/*F******************************************************************************************************
*                                               ÅäÖÃCHOPCONF¼Ä´æÆ÷
********************************************************************************************************/
static INT32U WriteChopperConfig(tmc260_dev_t *pdev)
{
    INT32U value;

    if (pdev->ChopperConfig.BlankTime > 3) {
        pdev->ChopperConfig.BlankTime = 3;
    }

    if (pdev->ChopperConfig.HysteresisDecay > 3) {
        pdev->ChopperConfig.HysteresisDecay = 3;
    }

    if (pdev->ChopperConfig.HysteresisEnd > 15) {
        pdev->ChopperConfig.HysteresisEnd = 15;
    }

    if (pdev->ChopperConfig.HysteresisStart > 7) {
        pdev->ChopperConfig.HysteresisStart = 7;
    }

    if (pdev->ChopperConfig.TOff > 15) {
        pdev->ChopperConfig.TOff = 15;
    }

    value  = 0;
    value |= DEF_BIT19_MASK;   // Register addresse CHOPCONF;
    value |= ((INT32U) pdev->ChopperConfig.BlankTime) << 15;

    if (pdev->ChopperConfig.ChopperMode) {
        value |= DEF_BIT14_MASK;
    }

    if (pdev->ChopperConfig.RandomTOff) {
        value |= DEF_BIT13_MASK;
    }

    value |= ((INT32U) pdev->ChopperConfig.HysteresisDecay) << 11;
    value |= ((INT32U) pdev->ChopperConfig.HysteresisEnd)   <<  7;
    value |= ((INT32U) pdev->ChopperConfig.HysteresisStart) <<  4;

    if (!pdev->ChopperConfig.DisableFlag) {
        value |= ((INT32U) pdev->ChopperConfig.TOff);
    }

    return TMC260_spi_rdwr(pdev, value);
}


/*F******************************************************************************************************
*                                               ÅäÖÃSMARTEN¼Ä´æÆ÷
********************************************************************************************************/
static INT32U WriteSmartEnergyControl(tmc260_dev_t *pdev)
{
    INT32U value;

    if (pdev->SmartEnergyControl.SmartIMin > 1) {
        pdev->SmartEnergyControl.SmartIMin = 1;
    }

    if (pdev->SmartEnergyControl.SmartDownStep > 3) {
        pdev->SmartEnergyControl.SmartDownStep = 3;
    }

    if (pdev->SmartEnergyControl.SmartStallLevelMax > 15) {
        pdev->SmartEnergyControl.SmartStallLevelMax = 15;
    }

    if (pdev->SmartEnergyControl.SmartUpStep > 3) {
        pdev->SmartEnergyControl.SmartUpStep = 3;
    }

    if (pdev->SmartEnergyControl.SmartStallLevelMin > 15) {
        pdev->SmartEnergyControl.SmartStallLevelMin = 15;
    }

    value  = 0;
    value |= DEF_BIT19_MASK | DEF_BIT17_MASK;   // Register adresse SMARTEN
    value |= ((INT32U) pdev->SmartEnergyControl.SmartIMin)          << 15;
    value |= ((INT32U) pdev->SmartEnergyControl.SmartDownStep)      << 13;
    value |= ((INT32U) pdev->SmartEnergyControl.SmartStallLevelMax) <<  8;
    value |= ((INT32U) pdev->SmartEnergyControl.SmartUpStep)        <<  5;
    value |= ((INT32U) pdev->SmartEnergyControl.SmartStallLevelMin);

    return TMC260_spi_rdwr(pdev, value);
}


/*F******************************************************************************************************
*                                               ÅäÖÃSGCSCONF¼Ä´æÆ÷
********************************************************************************************************/
static INT32U WriteStallGuardConfig(tmc260_dev_t *pdev)
{
    INT32U value;

    if (pdev->StallGuardConfig.StallGuardThreshold > 63) {
        pdev->StallGuardConfig.StallGuardThreshold = 63;
    }

    if (pdev->StallGuardConfig.StallGuardThreshold < -63) {
        pdev->StallGuardConfig.StallGuardThreshold = -63;
    }

    if (pdev->StallGuardConfig.CurrentScale > 31) {
        pdev->StallGuardConfig.CurrentScale = 31;
    }

    value  = 0;
    value |= DEF_BIT19_MASK | DEF_BIT18_MASK;   // Register adresse SGCSCONF

    if (pdev->StallGuardConfig.FilterEnable == 1){
        value |= DEF_BIT16_MASK;
    }

    value |= ((INT32U) pdev->StallGuardConfig.StallGuardThreshold & 0x7F) << 8;
    value |= ((INT32U) pdev->StallGuardConfig.CurrentScale);

    return TMC260_spi_rdwr(pdev, value);
}


/*F******************************************************************************************************
*                                               ÅäÖÃDRVCONF¼Ä´æÆ÷
********************************************************************************************************/
static INT32U WriteDriverConfig(tmc260_dev_t *pdev)
{
    INT32U value;

    value  = 0;
    value |= DEF_BIT19_MASK | DEF_BIT18_MASK | DEF_BIT17_MASK;   // Register adresse DRVCONF
    value |= ((INT32U) pdev->DriverConfig.SlopeHighSide & 0x03) << 14;
    value |= ((INT32U) pdev->DriverConfig.SlopeLowSide  & 0x03) << 12;

    if (pdev->DriverConfig.ProtectionDisable == 1) {
        value |= DEF_BIT10_MASK;
    }

    value |= ((INT32U) pdev->DriverConfig.ProtectionTimer & 0x03) << 8;

    if (pdev->DriverConfig.StepDirectionDisable == 1) {
        value |= DEF_BIT07_MASK;
    }

    if (pdev->DriverConfig.VSenseScale == 1) {
        value |= DEF_BIT06_MASK;
    }

    value |= ((INT32U) pdev->DriverConfig.ReadBackSelect & 0x03) << 4;

    if (pdev->DriverConfig.TestMode == 1) {
        value |= DEF_BIT16_MASK;
    }

    return TMC260_spi_rdwr(pdev, value);
}


/*F******************************************************************************************************
*
********************************************************************************************************/
static void TMC260_configs_init(tmc260_dev_t *pdev)
{
    #if 0
    INT8U TMC260Flags;

    do {
        TMC260Flags = TMC26x_read_mstep(pdev, NULL, NULL);
    } while ((TMC260Flags & 0x80) == 0x00);
    #endif

    WriteSmartEnergyControl(pdev); // SMARTEN
    WriteStallGuardConfig(pdev);   // SGCSCONF
    WriteDriverConfig(pdev);       // DRVCONF
    WriteStepDirConfig(pdev);      // DRVCTRL
    WriteChopperConfig(pdev);      // CHOPCONF
}


/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260StepDirMStepRes(tmc260_dev_t *pdev, INT8U MicrostepResolution)
{
    pdev->StepDirConfig.MRes = MicrostepResolution;

    if (MicrostepResolution != 4) {
        pdev->StepDirConfig.Intpol = 0;
    }

    WriteStepDirConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260StepDirInterpolation(tmc260_dev_t *pdev, INT8U Interpolation)
{
    pdev->StepDirConfig.Intpol = Interpolation;

    if (Interpolation) {
        pdev->StepDirConfig.MRes = 4;
    }

    WriteStepDirConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260StepDirDoubleEdge(tmc260_dev_t *pdev, INT8U DoubleEdge)
{
    pdev->StepDirConfig.DEdge = DoubleEdge;
    WriteStepDirConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260StepDirMStepRes(tmc260_dev_t *pdev)
{
    return pdev->StepDirConfig.MRes;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260StepDirInterpolation(tmc260_dev_t *pdev)
{
    return pdev->StepDirConfig.Intpol;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260StepDirDoubleEdge(tmc260_dev_t *pdev)
{
    return pdev->StepDirConfig.DEdge;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260ChopperBlankTime(tmc260_dev_t *pdev, INT8U BlankTime)
{
    pdev->ChopperConfig.BlankTime = BlankTime;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260ChopperMode(tmc260_dev_t *pdev, INT8U Mode)
{
    pdev->ChopperConfig.ChopperMode = Mode;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260ChopperRandomTOff(tmc260_dev_t *pdev, INT8U RandomTOff)
{
    pdev->ChopperConfig.RandomTOff = RandomTOff;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260ChopperHysteresisDecay(tmc260_dev_t *pdev, INT8U HysteresisDecay)
{
    pdev->ChopperConfig.HysteresisDecay = HysteresisDecay;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260ChopperHysteresisEnd(tmc260_dev_t *pdev, INT8U HysteresisEnd)
{
    pdev->ChopperConfig.HysteresisEnd = HysteresisEnd;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260ChopperHysteresisStart(tmc260_dev_t *pdev, INT8U HysteresisStart)
{
    pdev->ChopperConfig.HysteresisStart = HysteresisStart;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260ChopperTOff(tmc260_dev_t *pdev, INT8U TOff)
{
    pdev->ChopperConfig.TOff = TOff;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260ChopperBlankTime(tmc260_dev_t *pdev)
{
    return pdev->ChopperConfig.BlankTime;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260ChopperMode(tmc260_dev_t *pdev)
{
    return pdev->ChopperConfig.ChopperMode;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260ChopperRandomTOff(tmc260_dev_t *pdev)
{
    return pdev->ChopperConfig.RandomTOff;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260ChopperHysteresisDecay(tmc260_dev_t *pdev)
{
    return pdev->ChopperConfig.HysteresisDecay;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260ChopperHysteresisEnd(tmc260_dev_t *pdev)
{
    return pdev->ChopperConfig.HysteresisEnd;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260ChopperHysteresisStart(tmc260_dev_t *pdev)
{
    return pdev->ChopperConfig.HysteresisStart;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260ChopperTOff(tmc260_dev_t *pdev)
{
    return pdev->ChopperConfig.TOff;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260SmartEnergyIMin(tmc260_dev_t *pdev, INT8U SmartIMin)
{
    pdev->SmartEnergyControl.SmartIMin = SmartIMin;
    WriteSmartEnergyControl(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260SmartEnergyDownStep(tmc260_dev_t *pdev, INT8U SmartDownStep)
{
    pdev->SmartEnergyControl.SmartDownStep = SmartDownStep;
    WriteSmartEnergyControl(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260SmartEnergyStallLevelMax(tmc260_dev_t *pdev, INT8U StallLevelMax)
{
    pdev->SmartEnergyControl.SmartStallLevelMax = StallLevelMax;
    WriteSmartEnergyControl(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260SmartEnergyUpStep(tmc260_dev_t *pdev, INT8U SmartUpStep)
{
    pdev->SmartEnergyControl.SmartUpStep = SmartUpStep;
    WriteSmartEnergyControl(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260SmartEnergyStallLevelMin(tmc260_dev_t *pdev, INT8U StallLevelMin)
{
    pdev->SmartEnergyControl.SmartStallLevelMin = StallLevelMin;
    WriteSmartEnergyControl(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260SmartEnergyIMin(tmc260_dev_t *pdev)
{
    return pdev->SmartEnergyControl.SmartIMin;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260SmartEnergyDownStep(tmc260_dev_t *pdev)
{
    return pdev->SmartEnergyControl.SmartDownStep;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260SmartEnergyStallLevelMax(tmc260_dev_t *pdev)
{
    return pdev->SmartEnergyControl.SmartStallLevelMax;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260SmartEnergyUpStep(tmc260_dev_t *pdev)
{
    return pdev->SmartEnergyControl.SmartUpStep;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260SmartEnergyStallLevelMin(tmc260_dev_t *pdev)
{
    return pdev->SmartEnergyControl.SmartStallLevelMin;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260StallGuardFilter(tmc260_dev_t *pdev, INT8U Enable)
{
    pdev->StallGuardConfig.FilterEnable = Enable;
    WriteStallGuardConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260StallGuardThreshold(tmc260_dev_t *pdev, INT8S Threshold)
{
    pdev->StallGuardConfig.StallGuardThreshold = Threshold;
    WriteStallGuardConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260StallGuardCurrentScale(tmc260_dev_t *pdev, INT8U CurrentScale)
{
    pdev->StallGuardConfig.CurrentScale = CurrentScale;
    WriteStallGuardConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260StallGuardFilter(tmc260_dev_t *pdev)
{
    return pdev->StallGuardConfig.FilterEnable;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8S Get260StallGuardThreshold(tmc260_dev_t *pdev)
{
    return pdev->StallGuardConfig.StallGuardThreshold;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260StallGuardCurrentScale(tmc260_dev_t *pdev)
{
    return pdev->StallGuardConfig.CurrentScale;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260DriverSlopeHighSide(tmc260_dev_t *pdev, INT8U SlopeHighSide)
{
    pdev->DriverConfig.SlopeHighSide = SlopeHighSide;
    WriteDriverConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260DriverSlopeLowSide(tmc260_dev_t *pdev, INT8U SlopeLowSide)
{
    pdev->DriverConfig.SlopeLowSide = SlopeLowSide;
    WriteDriverConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260DriverDisableProtection(tmc260_dev_t *pdev, INT8U DisableProtection)
{
    pdev->DriverConfig.ProtectionDisable = DisableProtection;
    WriteDriverConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260DriverProtectionTimer(tmc260_dev_t *pdev, INT8U ProtectionTimer)
{
    pdev->DriverConfig.ProtectionTimer = ProtectionTimer;
    WriteDriverConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260DriverStepDirectionOff(tmc260_dev_t *pdev, INT8U SDOff)
{
    pdev->DriverConfig.StepDirectionDisable = SDOff;
    WriteDriverConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260DriverVSenseScale(tmc260_dev_t *pdev, INT8U Scale)
{
    pdev->DriverConfig.VSenseScale = Scale;
    WriteDriverConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260DriverReadSelect(tmc260_dev_t *pdev, INT8U ReadSelect)
{
    pdev->DriverConfig.ReadBackSelect = ReadSelect;
    WriteDriverConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void Set260DriverTestMode(tmc260_dev_t *pdev, INT8U TestMode)
{
    pdev->DriverConfig.TestMode = TestMode;
    WriteDriverConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260DriverSlopeHighSide(tmc260_dev_t *pdev)
{
    return pdev->DriverConfig.SlopeHighSide;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260DriverSlopeLowSide(tmc260_dev_t *pdev)
{
    return pdev->DriverConfig.SlopeLowSide;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260DriverDisableProtection(tmc260_dev_t *pdev)
{
    return pdev->DriverConfig.ProtectionDisable;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260DriverProtectionTimer(tmc260_dev_t *pdev)
{
    return pdev->DriverConfig.ProtectionTimer;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260DriverStepDirectionOff(tmc260_dev_t *pdev)
{
    return pdev->DriverConfig.StepDirectionDisable;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260DriverVSenseScale(tmc260_dev_t *pdev)
{
    return pdev->DriverConfig.VSenseScale;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260DriverReadSelect(tmc260_dev_t *pdev)
{
    return pdev->DriverConfig.ReadBackSelect;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U Get260DriverTestMode(tmc260_dev_t *pdev)
{
    return pdev->DriverConfig.TestMode;
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U TMC260_read_mstep(tmc260_dev_t *pdev, INT8U *Phases, INT8U *MStep)
{
    INT32U data;

    if (pdev->DriverConfig.ReadBackSelect != TMC260_RB_MSTEP) {
        Set260DriverReadSelect(pdev, TMC260_RB_MSTEP);
    }

    data = WriteDriverConfig(pdev);
    if (Phases != NULL) { *Phases = data >> 18; }
    if (MStep  != NULL) { *MStep  = data >> 10; }

    return (data & 0xFF);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U TMC260_read_stallguard(tmc260_dev_t *pdev, INT8U *StallGuard)
{
    INT32U data;

    if (pdev->DriverConfig.ReadBackSelect != TMC260_RB_STALL_GUARD) {
        Set260DriverReadSelect(pdev, TMC260_RB_STALL_GUARD);
    }

    data = WriteDriverConfig(pdev);
    if (StallGuard != NULL) { *StallGuard = data >> 10; }

    return (data & 0xFF);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
INT8U TMC260_read_smartenergy(tmc260_dev_t *pdev, INT32U *StallGuard, INT8U *SmartEnergy)
{
    INT32U data;

    if (pdev->DriverConfig.ReadBackSelect != TMC260_RB_SMART_ENERGY) {
        Set260DriverReadSelect(pdev, TMC260_RB_SMART_ENERGY);
    }

    data = WriteDriverConfig(pdev);
    if (StallGuard != NULL) {
        *StallGuard = data >> 15;
        *StallGuard <<= 5;
    }
    if (SmartEnergy != NULL) {
        *SmartEnergy = (data >> 10) & 0x1F;
    }

    return (data & 0xFF);
}

INT8U TMC260_read_status(tmc260_dev_t *pdev/*, INT8U *Status*/)
{
    INT32U data;

    if (pdev->DriverConfig.ReadBackSelect != TMC260_RB_STALL_GUARD) {
        Set260DriverReadSelect(pdev, TMC260_RB_STALL_GUARD);
    }

    data = WriteDriverConfig(pdev);
    /*if (Status != NULL) { 
      *Status = data & 0xff; 
    }*/

    return (data & 0xFF);
}
/*F******************************************************************************************************
*
********************************************************************************************************/
void TMC260_disable(tmc260_dev_t *pdev)
{
    pdev->ChopperConfig.DisableFlag = TRUE;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void TMC260_enable(tmc260_dev_t *pdev)
{
    pdev->ChopperConfig.DisableFlag = FALSE;
    WriteChopperConfig(pdev);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
void TMC260_install(tmc260_dev_t *pdev)
{
    TMC260_init();
    TMC260_configs_init(pdev);
}

tmc260_dev_t* TMC260_get_dev(TMC260_ID id)
{
    return &g_tmc260_dev[id];
}

/*
*********************************************************************************************************
*                                               No More!
*********************************************************************************************************
*/
