#include "port.h"

int main(int argc, char const* argv[])
{
#if 0   //测试灯光、蜂鸣器、马达、风扇
    int fd[4] = { 0 };
    if (-1 == (fd[0] = open("/dev/myleds", O_RDWR)))
        PRINT_ERR("open error\n");
    if (0 > (fd[1] = open("/dev/beeper_driver", O_RDWR))) {
        perror("open error");
        return -1;
    }
    if (0 > (fd[2] = open("/dev/motor_driver", O_RDWR))) {
        perror("open error");
        return -1;
    }
    if (0 > (fd[3] = open("/dev/fan_driver", O_RDWR))) {
        perror("open error");
        return -1;
    }
    while (1) {
        set_motor_state(fd[2], 0);
        set_led_state(fd[0], 1);
        set_fan_state(fd[3], 1);
        sleep(1);
        set_led_state(fd[0], 0);
        set_beep_state(fd[1], 1);
        set_fan_state(fd[3], 2);
        sleep(1);
        set_beep_state(fd[1], 0);
        set_motor_state(fd[2], 1);
        set_fan_state(fd[3], 3);
        sleep(1);
    }
    close(fd[0]);
    close(fd[1]);
    close(fd[2]);
    close(fd[3]);
#else   //测试温湿度、光强读取
    float temp;
    unsigned char hume;
    unsigned short light;
    while (1) {
        get_temp_and_hume_data(&temp, &hume);
        get_light_data(&light);
        printf("temp:%f hume:%d light:%d\n", temp, hume, light);
        sleep(1);
    }
#endif
    return 0;
}
