#ifndef __MY_SHOOT
#define __MY_SHOOT


#include "stm32F4xx_hal.h"
#include "3508_motor.h"
#include "2006_motor.h"

/*基本参数*/
#define SHOOT_OFFLINE_CNT_MAX  50
#define SHOOT_DONE_SPEED_MAX   100
#define SHOOT_DONE_TIME_MAX    3 
#define SHOOT_SPEED            ((-1)*2000)
#define SHOOT_STRI_SPEED       ((-1)*900)
#define SHOOT_STRI_SPEED_ERR   100

/*摩擦轮速度环*/
#define PID_FIRC_OUTPUT_MAX    3000
#define PID_FIRC_INTEGRA_MAX   5000
#define PID_FIRC_KP            10
#define PID_FIRC_KI            0
#define PID_FIRC_KD            2

/*单发波胆轮*/
  /*速度*/
#define SPID_STRI_OUTPUT_MAX    8000
#define SPID_STRI_INTEGRA_MAX   3000
#define SPID_STRI_KP            0.1
#define SPID_STRI_KI            0.1
#define SPID_STRI_KD            0

  /*位置*/
#define PPID_STRI_OUTPUT_MAX    3000
#define PPID_STRI_INTEGRA_MAX   1000
#define PPID_STRI_KP            0.1
#define PPID_STRI_KI            0
#define PPID_STRI_KD            0

/*连发波胆轮速度环*/
#define TPID_STRI_OUTPUT_MAX    9000
#define TPID_STRI_INTEGRA_MAX   5000
#define TPID_STRI_KP            5
#define TPID_STRI_KI            0.01
#define TPID_STRI_KD            0



typedef enum 
{
	S_S_offline,  //失联
	S_S_oneshot,  //单发
	S_S_threeshot,//连发
	S_S_done,     //堵转
	
}shoot_work_status_e;

typedef struct shoot_structure
{
	motor_2006_t*        stir_wheel;
	motor_3508_t*        fric_left;
	motor_3508_t*        fric_right;
	shoot_work_status_e  work_sta;
	uint8_t              cnt_three_done;
	uint8_t              cnt_right;
	uint8_t              cnt_stri;	
	uint8_t              shoot_time;
	int16_t              shoot_speed;
	void                 (*init)  (struct shoot_structure* shoot);
  void                 (*check) (struct shoot_structure* shoot);  
	void                 (*update)(struct shoot_structure* shoot);  
	void                 (*data)(struct shoot_structure* shoot);  
	void                 (*work)(struct shoot_structure* shoot);  
	void                 (*she_ji)(struct shoot_structure* shoot);  
}shoot_t;


void SHOOT_INIT(shoot_t* shoot);
void DONE_CHECK(shoot_t* shoot);
void MODE_UPDATE(shoot_t* shoot);
void DATA_UPDATE(shoot_t* shoot);
void SHOOR_WORK(shoot_t* shoot);
void SHE_JI(shoot_t* shoot);

void shoot_task(void);

//uint16_t STRI_POSITION[8] = {0,0,0,0,0,0,0,0};



#endif

