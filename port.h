#ifndef __PORT_H__
#define __PORT_H__

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PRINT_ERR(msg)\
do\
{\
    perror(msg);\
    return -1;\
}while(0)

#define LED_ON _IOW('L',0,int)
#define LED_OFF _IOW('L',1,int)
#define GET_SI7006_TMP _IOR('r',0,int)
#define GET_SI7006_HUM _IOR('r',1,int)

int range_compare(envdata_t envdata,limitset_t limitset);//比较函数

int set_led_state(int fd, unsigned char devstart);//设置灯
int set_beep_state(int fd, unsigned char devstart);//设置蜂鸣器
int set_motor_state(int fd, unsigned char devstart);//设置马达
int set_fan_state(int fd, unsigned char devstart);//设置风扇
int get_temp_and_hume_data(float* temp, unsigned char* hume);//获取温湿度
int get_light_data(unsigned short* light);//获取光照强度
#endif