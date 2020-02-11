#include "motor.h"

struct _motor_port{
	struct _io_map en;
	struct _io_map dir;
	struct _io_map step;
};

typedef struct _motor_priv{
	struct _motor_port *pport;
}motor_priv_t;

/*
*********************************************************************************************************
*                                       PRIVATE FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void     UpdateMotorTimer        (MOTOR_ID id, INT16U val);
static void     CalcSpedingProcedure    (MOTOR_ID id,INT8U if_acc);
//static void     MotorSpeedUp            (TMotor *pMotor);
//static void     MotorSpeedConst         (TMotor *pMotor);
//static void     MotorSpeedDn            (TMotor *pMotor);

/*
*********************************************************************************************************
*                                  PRIVATE GLOBAL CONSTANTS & VARIABLES
*********************************************************************************************************
*/

static struct _motor_port g_motor_port[MOTOR_ID_NUMS]={
	[MOTOR_ID1] = {
		.en ={MOTOR_ID1_EN_PORT ,MOTOR_ID1_EN_PIN},
		.dir={MOTOR_ID1_DIR_PORT,MOTOR_ID1_DIR_PIN},
		.step={MOTOR_ID1_STEP_PORT,MOTOR_ID1_STEP_PIN}
	},
/*	[MOTOR_ID2] = {
		.en ={MOTOR_ID2_EN_PORT ,MOTOR_ID2_EN_PIN},
		.dir={MOTOR_ID2_DIR_PORT,MOTOR_ID2_DIR_PIN},
		.step={MOTOR_ID2_STEP_PORT,MOTOR_ID2_STEP_PIN}
	},*/
};

static motor_priv_t g_motor_priv[MOTOR_ID_NUMS]={
	[MOTOR_ID1] = {
		.pport = &g_motor_port[MOTOR_ID1]
	},
/*	[MOTOR_ID2] = {
		.pport = &g_motor_port[MOTOR_ID2]
	},*/
};
TMotor tMotor[MOTOR_ID_NUMS];

void Motor_init(void)
{
	TMC260_install(tMotor[MOTOR_ID1].tmc260dev);
}

