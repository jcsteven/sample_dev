//dev_hello.h

#ifndef DEV_HELLO_H
#define DEV_HELLO_H
#include <linux/ioctl.h>

#define DEV_IOC_MAGIC 'k'
 
typedef struct
{
    unsigned char cmd1;
    unsigned char cmd2;
    unsigned char data1;	
    unsigned char data2;	
} dev_data_arg_t;
 
#define DEV_DATA_GET_VARIABLES _IOR(DEV_IOC_MAGIC, 1, dev_data_arg_t *)
#define DEV_DATA_CLR_VARIABLES _IOW(DEV_IOC_MAGIC, 2,dev_data_arg_t *)
#define DEV_DATA_SET_VARIABLES _IOW(DEV_IOC_MAGIC, 3, dev_data_arg_t *)
#define DEV_DATA_GET_COMMANDS _IOWR(DEV_IOC_MAGIC, 4, dev_data_arg_t *)
#define DEV_DATA_SET_COMMANDS _IOWR(DEV_IOC_MAGIC, 5, dev_data_arg_t *) 
#endif
