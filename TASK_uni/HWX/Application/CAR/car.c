#include "car.h"
#include "REMOTE.h"

extern car_t    car_structure;
extern rc_t     rc_structure;

int8_t          s2_down_flag = 1;
int8_t          s2_up_flag = 1;



void MODE_CHECK()
{
    /*遥控器模式*/
    car_structure.ctrl_mode = RC_CAR;

    /*状态更新*/
    rc_wheel_status_interrupt_update(rc_structure.base_info);

    if(rc_structure.base_info->s1.value == 2)//下拨码
    {
        /*机械模式*/
        car_structure.move_mode_status = machine_CAR;

    }
    else if(rc_structure.base_info->s1.value == 3)//中拨码
    {
        /*小陀螺模式*/
        if(rc_structure.base_info->thumbwheel.value >= 600)
        {
            car_structure.move_mode_status = spin_CAR;

        }
        else
        {
            /*跟随模式*/
            car_structure.move_mode_status =follow_CAR;
        }

    }
    else
    {
        /*遥控器掉线模式模式*/
        car_structure.move_mode_status =offline_CAR;
    }

    /*检测跳变(检测向下拨动)*/

    /*检测右拨杆向下*/
    if(rc_structure.base_info->s2.value_last != rc_structure.base_info->s2.value \
            && rc_structure.base_info->s2.value == 2)
    {
        s2_down_flag = s2_down_flag*(-1);
    }

    /*检测右拨杆向上*/
    if(rc_structure.base_info->s2.value_last != rc_structure.base_info->s2.value \
            && rc_structure.base_info->s2.value == 1)
    {
        s2_up_flag = s2_up_flag*(-1);
    }








}






