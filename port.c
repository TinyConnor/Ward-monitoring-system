#include "port.h"

/**
 * @brief 将环境参数与阈值进行对比
 * 如果心跳、心氧过高或过低,打开报警系统(蜂鸣器),恢复正常则关闭警报
 * 如果光线过低,打开照明系统(led灯),过高则关系照明系统
 * 如果温度过高,打开通风系统(风扇),如果温度过低,则关闭通风系统
 * 如果湿度过低,打开加湿系统(马达),如果湿度过高,则关闭加湿系统
 * @param envdata    环境参数
 * @param limitset   阈值
 * @param fd         文件描述符组
 * @return
 *     成功 0
 *     失败 -1
 *typedef struct{
    float temp;
    unsigned char hume;
    unsigned short lux;
    unsigned char devstart;
} envdata_t;

typedef struct{
    float temp_up;
    float temp_down;
    unsigned char hume_up;
    unsigned char hume_down;
    unsigned short lux_up;
    unsigned short lux_down;

} limitset_t;
 *  */
int range_compare(envdata_t envdata, limitset_t limitset, int* fd)
{
    int ret;
    //1.心跳、心氧过高或过低,打开报警系统(蜂鸣器)

    //2.光线过低,打开照明系统(led灯),过高则关系照明系统
    if (envdata.lux < limitset.lux_down) {
        envdata.devstart = envdata.devstart | 0x01;
        if ((ret = set_led_state(fd[0], envdata.devstart)) == -1) {
            printf("set_led_state error\n");
            return -1;
        }
    } else if (envdata.lux > limitset.lux_up) {
        envdata.devstart = envdata.devstart & (~0x01);
        if ((ret = set_led_state(fd[0], envdata.devstart)) == -1) {
            printf("set_led_state error\n");
            return -1;
        }
    }
    //3.温度过高,打开通风系统(风扇),如果温度过低,则关闭通风系统
    if (envdata.temp < limitset.temp_down) {
        envdata.devstart = envdata.devstart | 0x08;//风扇开1档
        if ((ret = set_beep_state(fd[3], envdata.devstart)) == -1) {
            printf("set_beep_state error\n");
            return -1;
        }
    } else if (envdata.temp > limitset.temp_up) {
        envdata.devstart = envdata.devstart & (~0x1f);
        if ((ret = set_beep_state(fd[3], envdata.devstart)) == -1) {
            printf("set_beep_state error\n");
            return -1;
        }
    }
    //4.湿度过低,打开加湿系统(马达),如果湿度过高,则关闭加湿系统
    if (envdata.hume < limitset.hume_down) {
        envdata.devstart = envdata.devstart | 0x04;
        if ((ret = set_motor_state(fd[2], envdata.devstart)) == -1) {
            printf("set_motor_state error\n");
            return -1;
        }
    } else if (envdata.hume > limitset.hume_up) {
        envdata.devstart = envdata.devstart & (~0x04);
        if ((ret = set_motor_state(fd[2], envdata.devstart)) == -1) {
            printf("set_motor_state error\n");
            return -1;
        }
    }
    return 0;
}
/**
 * @brief 设置灯状态
 * @param fd    文件描述符
 * @param devstart 控制码
 * @return
 *     成功 0
 *     失败 -1
 */
int set_led_state(int fd, unsigned char devstart)
{

    int i = 1;
    int state = (int) devstart & 0x01;
    for (i = 0;i < 6;i++) {
        if (state == 1) {
            ioctl(fd, LED_ON, &i);
        } else if (state == 0) {
            ioctl(fd, LED_OFF, &i);
        } else {
            return -1;
        }
    }
    return 0;
}
/**
 * @brief 设置风扇状态
 * @param fd    文件描述符
 * @param devstart 控制码
 * @return
 *     成功 0
 *     失败 -1
 */
int set_beep_state(int fd, unsigned char devstart)
{
    int value;
    int state = ((int) devstart & 0x02) >> 1;
    if (state == 1) {
        value = 2000;
        write(fd, &value, sizeof(value));
    } else if (state == 0) {
        value = 0;
        write(fd, &value, sizeof(value));
    } else
        return -1;
    return 0;
}
/**
 * @brief 设置马达状态
 * @param fd    文件描述符
 * @param devstart 控制码
 * @return
 *     成功 0
 *     失败 -1
 */
int set_motor_state(int fd, unsigned char devstart)
{
    int value;
    int state = ((int) devstart & 0x04) >> 2;
    if (state == 1) {
        value = 2000;
        write(fd, &value, sizeof(value));
    } else if (state == 0) {
        value = 0;
        write(fd, &value, sizeof(value));
    } else
        return -1;
    return 0;
}
/**
 * @brief 设置风扇速度级转速
 * @param fd    文件描述符
 * @param devstart 控制码
 * @return
 *     成功 0
 *     失败 -1
 */
int set_fan_state(int fd, unsigned char devstart)
{
    int state = ((int) devstart & 0x18) >> 3;
    if (state < 0 || state > 3)
        return -1;
    int value[2] = { 8000,state };
    write(fd, &value, sizeof(value));
    return 0;
}
/**
 * @brief 获取温湿度数据
 * @param temp 温度
 * @param hume 湿度
 * @return
 *     成功 0
 *     失败 -1
 */
int get_temp_and_hume_data(float* temp, unsigned char* hume)
{
    int fd;
    int tmp, hum;
    if ((fd = open("/dev/si7006", O_RDWR)) == -1) {
        // PRINT_ERR("open error");
        printf("open error");
        return -1;
    }
    do {
        ioctl(fd, GET_SI7006_TMP, &tmp);
        ioctl(fd, GET_SI7006_HUM, &hum);

        *temp = 175.72 * tmp / 65536 - 46.85;
        *hume = (unsigned char) (125 * hum / 65536 - 6);

    } while (*temp < 0 && *temp > 100 && *hume < 0 && *hume > 100);
    close(fd);
    return 0;
}
/**
 * @brief 获取光照强度
 * @param light 环境光强度
 * @return
 *     成功 0
 *     失败 -1
 */
int get_light_data(unsigned short* light)
{
    int fd, ret;
    unsigned short data_buf[3] = { 0 };

    if ((fd = open("/dev/ap3216", O_RDWR)) == -1) {
        printf("open  ap3216 error\n");
        return -1;
    }
    // PRINT_ERR("open error");
    do {
        ret = read(fd, data_buf, sizeof(data_buf));
        if (ret < 0) {
            printf("read fail\n");
        } else {
            // printf("ir:%d,als:%d,ps:%d\n", data_buf[0], data_buf[1], data_buf[2]);
            *light = data_buf[1];
        }
    } while (*light < 0 && *light > 10000);
    close(fd);
    return 0;
}