#include "car.h"
#include "REMOTE.h"

extern car_t    car_structure;
extern rc_t     rc_structure;

int8_t          s2_down_flag = 1;
int8_t          s2_up_flag = 1;



void MODE_CHECK()
{
    /*ң����ģʽ*/
    car_structure.ctrl_mode = RC_CAR;

    /*״̬����*/
    rc_wheel_status_interrupt_update(rc_structure.base_info);

    if(rc_structure.base_info->s1.value == 2)//�²���
    {
        /*��еģʽ*/
        car_structure.move_mode_status = machine_CAR;

    }
    else if(rc_structure.base_info->s1.value == 3)//�в���
    {
        /*С����ģʽ*/
        if(rc_structure.base_info->thumbwheel.value >= 600)
        {
            car_structure.move_mode_status = spin_CAR;

        }
        else
        {
            /*����ģʽ*/
            car_structure.move_mode_status =follow_CAR;
        }

    }
    else
    {
        /*ң��������ģʽģʽ*/
        car_structure.move_mode_status =offline_CAR;
    }

    /*�������(������²���)*/

    /*����Ҳ�������*/
    if(rc_structure.base_info->s2.value_last != rc_structure.base_info->s2.value \
            && rc_structure.base_info->s2.value == 2)
    {
        s2_down_flag = s2_down_flag*(-1);
    }

    /*����Ҳ�������*/
    if(rc_structure.base_info->s2.value_last != rc_structure.base_info->s2.value \
            && rc_structure.base_info->s2.value == 1)
    {
        s2_up_flag = s2_up_flag*(-1);
    }








}






