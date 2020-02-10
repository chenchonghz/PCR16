/*
H********************************************************************************************************
*                                         Novocyte System Firmware
*
*                           (c) Copyright 2010-2014 ACEA Biosciences, Inc.
*                           All rights reserved.  Protected by international copyright laws.
*
* File      :  tmc260.h
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

#ifndef BSP_TMC260_H
#define BSP_TMC260_H


#ifdef  TMC260_GLOBALS
#define TMC260_EXT
#else
#define TMC260_EXT extern
#endif


/*
*********************************************************************************************************
*                                          INCLUDE HEADER FILES
*********************************************************************************************************
*/

#include "bsp_spi.h"


/*
*********************************************************************************************************
*                                                  MACROS
*********************************************************************************************************
*/

#define TMC260_RB_MSTEP        0
#define TMC260_RB_STALL_GUARD  1
#define TMC260_RB_SMART_ENERGY 2

typedef enum _tmc260_id {
    TMC260_ID1 = 0,
   // TMC260_ID2 = 1,
}TMC260_ID;
#define TMC260_ID_MIN           TMC260_ID1
#define TMC260_ID_MAX           TMC260_ID1
#define TMC260_ID_NUMS         (TMC260_ID_MAX-TMC260_ID_MIN+1)
#define TMC260_FLAGS_INIT		1

/*
*********************************************************************************************************
*                                            MACROS FUNCTIONS
*********************************************************************************************************
*/




/*
*********************************************************************************************************
*                                             TYPE DEFINITION
*********************************************************************************************************
*/

typedef struct _step_dir_config {
    INT8U APolarity;
    INT8U BPolarity;
    INT8U NPolarity;
    INT8U NContinous;
    INT8U ClearOnN;
    INT8U Intpol;
    INT8U DEdge;
    INT8U MRes;
} step_dir_config_t;

typedef struct _chopper_config {
    INT8U BlankTime;
    INT8U ChopperMode;
    INT8U HysteresisDecay;
    INT8U RandomTOff;
    INT8U HysteresisEnd;
    INT8U HysteresisStart;
    INT8U TOff;
    INT8U DisableFlag;
} chopper_config_t;

typedef struct _smart_energy_control {
    INT8U SmartIMin;
    INT8U SmartDownStep;
    INT8U SmartStallLevelMax;
    INT8U SmartUpStep;
    INT8U SmartStallLevelMin;
} smart_energy_control_t;

typedef struct _stall_guard_config {
    INT8U FilterEnable;
    INT8S StallGuardThreshold;
    INT8U CurrentScale;
} stall_guard_config_t;

typedef struct _driver_config {
    INT8U TestMode;
    INT8U SlopeHighSide;
    INT8U SlopeLowSide;
    INT8U ProtectionDisable;
    INT8U ProtectionTimer;
    INT8U StepDirectionDisable;
    INT8U VSenseScale;
    INT8U ReadBackSelect;
} driver_config_t;

typedef struct _tmc260_dev {
    const INT8U id;
    CPU_INT16U          flags;
//    OS_EVENT           	*lock;
    spi_t 				*p_spi;
    void               	*priv;
    struct _step_dir_config      StepDirConfig;
    struct _chopper_config       ChopperConfig;
    struct _smart_energy_control SmartEnergyControl;
    struct _stall_guard_config   StallGuardConfig;
    struct _driver_config        DriverConfig;
} tmc260_dev_t;



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
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void    Set260StepDirMStepRes           (tmc260_dev_t *pdev, INT8U MicrostepResolution);
void    Set260StepDirInterpolation      (tmc260_dev_t *pdev, INT8U Interpolation);
void    Set260StepDirDoubleEdge         (tmc260_dev_t *pdev, INT8U DoubleEdge);
INT8U   Get260StepDirMStepRes           (tmc260_dev_t *pdev);
INT8U   Get260StepDirInterpolation      (tmc260_dev_t *pdev);
INT8U   Get260StepDirDoubleEdge         (tmc260_dev_t *pdev);
void    Set260ChopperBlankTime          (tmc260_dev_t *pdev, INT8U BlankTime);
void    Set260ChopperMode               (tmc260_dev_t *pdev, INT8U Mode);
void    Set260ChopperRandomTOff         (tmc260_dev_t *pdev, INT8U RandomTOff);
void    Set260ChopperHysteresisDecay    (tmc260_dev_t *pdev, INT8U HysteresisDecay);
void    Set260ChopperHysteresisEnd      (tmc260_dev_t *pdev, INT8U HysteresisEnd);
void    Set260ChopperHysteresisStart    (tmc260_dev_t *pdev, INT8U HysteresisStart);
void    Set260ChopperTOff               (tmc260_dev_t *pdev, INT8U TOff);
INT8U   Get260ChopperBlankTime          (tmc260_dev_t *pdev);
INT8U   Get260ChopperMode               (tmc260_dev_t *pdev);
INT8U   Get260ChopperRandomTOff         (tmc260_dev_t *pdev);
INT8U   Get260ChopperHysteresisDecay    (tmc260_dev_t *pdev);
INT8U   Get260ChopperHysteresisEnd      (tmc260_dev_t *pdev);
INT8U   Get260ChopperHysteresisStart    (tmc260_dev_t *pdev);
INT8U   Get260ChopperTOff               (tmc260_dev_t *pdev);
void    Set260SmartEnergyIMin           (tmc260_dev_t *pdev, INT8U SmartIMin);
void    Set260SmartEnergyDownStep       (tmc260_dev_t *pdev, INT8U SmartDownStep);
void    Set260SmartEnergyStallLevelMax  (tmc260_dev_t *pdev, INT8U StallLevelMax);
void    Set260SmartEnergyUpStep         (tmc260_dev_t *pdev, INT8U SmartUpStep);
void    Set260SmartEnergyStallLevelMin  (tmc260_dev_t *pdev, INT8U StallLevelMin);
INT8U   Get260SmartEnergyIMin           (tmc260_dev_t *pdev);
INT8U   Get260SmartEnergyDownStep       (tmc260_dev_t *pdev);
INT8U   Get260SmartEnergyStallLevelMax  (tmc260_dev_t *pdev);
INT8U   Get260SmartEnergyUpStep         (tmc260_dev_t *pdev);
INT8U   Get260SmartEnergyStallLevelMin  (tmc260_dev_t *pdev);
void    Set260StallGuardFilter          (tmc260_dev_t *pdev, INT8U Enable);
void    Set260StallGuardThreshold       (tmc260_dev_t *pdev, INT8S Threshold);
void    Set260StallGuardCurrentScale    (tmc260_dev_t *pdev, INT8U CurrentScale);
INT8U   Get260StallGuardFilter          (tmc260_dev_t *pdev);
INT8S   Get260StallGuardThreshold       (tmc260_dev_t *pdev);
INT8U   Get260StallGuardCurrentScale    (tmc260_dev_t *pdev);
void    Set260DriverSlopeHighSide       (tmc260_dev_t *pdev, INT8U SlopeHighSide);
void    Set260DriverSlopeLowSide        (tmc260_dev_t *pdev, INT8U SlopeLowSide);
void    Set260DriverDisableProtection   (tmc260_dev_t *pdev, INT8U DisableProtection);
void    Set260DriverProtectionTimer     (tmc260_dev_t *pdev, INT8U ProtectionTimer);
void    Set260DriverStepDirectionOff    (tmc260_dev_t *pdev, INT8U SDOff);
void    Set260DriverVSenseScale         (tmc260_dev_t *pdev, INT8U Scale);
void    Set260DriverReadSelect          (tmc260_dev_t *pdev, INT8U ReadSelect);
void    Set260DriverTestMode            (tmc260_dev_t *pdev, INT8U TestMode);
INT8U   Get260DriverSlopeHighSide       (tmc260_dev_t *pdev);
INT8U   Get260DriverSlopeLowSide        (tmc260_dev_t *pdev);
INT8U   Get260DriverDisableProtection   (tmc260_dev_t *pdev);
INT8U   Get260DriverProtectionTimer     (tmc260_dev_t *pdev);
INT8U   Get260DriverStepDirectionOff    (tmc260_dev_t *pdev);
INT8U   Get260DriverVSenseScale         (tmc260_dev_t *pdev);
INT8U   Get260DriverReadSelect          (tmc260_dev_t *pdev);
INT8U   Get260DriverTestMode            (tmc260_dev_t *pdev);
INT8U   TMC260_read_mstep               (tmc260_dev_t *pdev, INT8U *Phases, INT8U *MStep);
INT8U   TMC260_read_stallguard          (tmc260_dev_t *pdev, INT8U *StallGuard);
INT8U   TMC260_read_smartenergy         (tmc260_dev_t *pdev, INT32U *StallGuard, INT8U *SmartEnergy);
INT8U TMC260_read_status(tmc260_dev_t *pdev/*, INT8U *Status*/);
void    TMC260_disable                  (tmc260_dev_t *pdev);
void    TMC260_enable                   (tmc260_dev_t *pdev);
void    TMC260_install                  (tmc260_dev_t *pdev);
tmc260_dev_t* TMC260_get_dev(TMC260_ID id);

#endif    /* TMC260_H */

/*
*********************************************************************************************************
*                                                No More!
*********************************************************************************************************
*/
