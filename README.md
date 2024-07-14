> * port.c:接口函数
> * port.h:接口函数有关的声明
> * test.c:测试案例

主程序参考test.c定义文件描述符数组，如下，顺序不能错，否则会**影响对比函数**

```c
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
```

使用接口函数需配合

1. **driver**文件：存放所有驱动文件，放在开发板根目录下
2. **rcS**文件：替换开发板根目录下的 `ect/init.d`，用来设置开机加载驱动，加载视频流服务器等
