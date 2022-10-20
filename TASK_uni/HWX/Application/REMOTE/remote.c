

#include "remote.h"
//#include "config.h"
//#include "math_support.h"

/**
  * @brief  ң����Ϣ����
  */
void rc_base_info_update(rc_base_info_t *info, uint8_t *rxBuf)
{
  info->ch0 = (rxBuf[0]      | rxBuf[1] << 8                 ) & 0x07FF;
  info->ch0 -= 1024;
  info->ch1 = (rxBuf[1] >> 3 | rxBuf[2] << 5                 ) & 0x07FF;
  info->ch1 -= 1024;
  info->ch2 = (rxBuf[2] >> 6 | rxBuf[3] << 2 | rxBuf[4] << 10) & 0x07FF;
  info->ch2 -= 1024;
  info->ch3 = (rxBuf[4] >> 1 | rxBuf[5] << 7                 ) & 0x07FF;
  info->ch3 -= 1024;
  info->s1.value = ((rxBuf[5] >> 4) & 0x000C) >> 2;
  info->s2.value = ( rxBuf[5] >> 4) & 0x0003;
	
	
  info->mouse_vx = rxBuf[6]  | (rxBuf[7 ] << 8);
  info->mouse_vy = rxBuf[8]  | (rxBuf[9 ] << 8);
  info->mouse_vz = rxBuf[10] | (rxBuf[11] << 8);
  info->mouse_btn_l.value = rxBuf[12] & 0x01;
  info->mouse_btn_r.value = rxBuf[13] & 0x01;
  info->W.value =   rxBuf[14]        & 0x01;
  info->S.value = ( rxBuf[14] >> 1 ) & 0x01;
  info->A.value = ( rxBuf[14] >> 2 ) & 0x01;
  info->D.value = ( rxBuf[14] >> 3 ) & 0x01;
  info->Shift.value = ( rxBuf[14] >> 4 ) & 0x01;
  info->Ctrl.value = ( rxBuf[14] >> 5 ) & 0x01;
  info->Q.value = ( rxBuf[14] >> 6 ) & 0x01 ;
  info->E.value = ( rxBuf[14] >> 7 ) & 0x01 ;
  info->R.value = ( rxBuf[15] >> 0 ) & 0x01 ;
  info->F.value = ( rxBuf[15] >> 1 ) & 0x01 ;
  info->G.value = ( rxBuf[15] >> 2 ) & 0x01 ;
  info->Z.value = ( rxBuf[15] >> 3 ) & 0x01 ;
  info->X.value = ( rxBuf[15] >> 4 ) & 0x01 ;
  info->C.value = ( rxBuf[15] >> 5 ) & 0x01 ;
  info->V.value = ( rxBuf[15] >> 6 ) & 0x01 ;
  info->B.value = ( rxBuf[15] >> 7 ) & 0x01 ;

  info->thumbwheel.value = ((int16_t)rxBuf[16] | ((int16_t)rxBuf[17] << 8)) & 0x07ff;
  info->thumbwheel.value -= 1024;
	
	
	
}

void rc_init(rc_t *rc, rc_info_t *info, rc_base_info_t *base_info)
{
	rc->base_info = base_info;
	rc->info      = info;
	
	
		/* ������Ϣ���� */
  memset(base_info,0,sizeof(rc_base_info_t));
	/* ������ť״̬��ʼ�� */
  base_info->s1.status = keep_R;
  base_info->s2.status = keep_R;
  base_info->thumbwheel.status = keep_R;
	
	
		/* ����ʧ������ */
  info->offline_cnt = REMOTE_OFFLINE_CNT_MAX;
  /* ����״̬ */
 // info->status = DEV_OFFLINE;
	
}




void rc_switch_status_interrupt_update(rc_base_info_t *info)
{
  /* �󲦸��ж� */
  if(info->s1.value != info->s1.value_last)
  {
    switch(info->s1.value)
    {
      case 1:
        info->s1.status = up_R;
        break;
      case 3:
        info->s1.status = mid_R;
        break;
      case 2:
        info->s1.status = down_R;
        break;
      default:
        break;
    }
    info->s1.value_last = info->s1.value;
  }
  else 
  {
    info->s1.status = keep_R;
  }
  /* �Ҳ����ж� */
  if(info->s2.value != info->s2.value_last)
  {
    switch(info->s2.value)
    {
      case 1:
        info->s2.status = up_R;
        break;
      case 3:
        info->s2.status = mid_R;
        break;
      case 2:
        info->s2.status = down_R;
        break;
      default:
        break;
    }
    info->s2.value_last = info->s2.value;
  }
  else 
  {
    info->s2.status = keep_R;
  }
}

/**
  * @brief  ң������ť״̬�����жϲ�����
  */
void rc_wheel_status_interrupt_update(rc_base_info_t *info)
{
  if(abs(info->thumbwheel.value_last) < WHEEL_JUMP_VALUE)
  {
    if(info->thumbwheel.value > WHEEL_JUMP_VALUE)
    {
      info->thumbwheel.status = up_R;
    }
    else if(info->thumbwheel.value < -WHEEL_JUMP_VALUE)
    {
      info->thumbwheel.status = down_R;
    }
    else 
    {
      info->thumbwheel.status = keep_R;
    }
  }
  else 
  {
    info->thumbwheel.status = keep_R;
  }
  info->thumbwheel.value_last = info->thumbwheel.value;
	
}


