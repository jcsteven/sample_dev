
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
 
#include "dev_hello.h"
 
void show_dev_Data(dev_data_arg_t *pData)
{
    printf("Show Dev Data: cmd1=%02X, cmd2=%02X  data1=%02X data2=%02X\n",pData->cmd1,pData->cmd2,pData->data1,pData->data2);	
}
 
void get_dev_data(int fd,dev_data_arg_t *pdev_data)
{
    dev_data_arg_t dev_data;
    dev_data.cmd1=pdev_data->cmd1;
     dev_data.cmd1=pdev_data->cmd2;
    if (ioctl(fd, DEV_DATA_GET_VARIABLES, &dev_data) == -1)
    {
        printf(" Fail to get_dev_data !!");
    }
    else
    {
        printf("OK to get_dev_data !!\n");
		show_dev_Data(&dev_data);
    }
}

void set_dev_data(int fd,dev_data_arg_t *pdev_data)
{
    dev_data_arg_t dev_data;
    dev_data.cmd1=pdev_data->cmd1;
    dev_data.cmd2=pdev_data->cmd2;	
    if (ioctl(fd, DEV_DATA_SET_VARIABLES, &dev_data) == -1)
    {
        printf(" Fail to Set_dev_data !!");
    }
    else
    {
        printf("OK to get_dev_data !!\n");
		show_dev_Data(&dev_data);
		memcpy(pdev_data,&dev_data,sizeof(dev_data_arg_t));
    }
}

void print_uasge(char *cmd)
{
 printf("Usage: %s \n", cmd);
 printf("     option: ");
 printf("           -h  : help message\n");
 printf("           -1 [ <cmd1>  ] :  \n");
 printf("           -2 [ <cmd2>  ] :  \n");
 printf("           -g  :  ioctl get\n");
 printf("           -s  :  ioctl set\n"); 
 printf("\n") ;
}

enum{
    o_help,
	o_ioctl_get,
	o_ioctl_set,
	o_read,
	o_write
    } option;


	

int main (int argc, char **argv)
{
  char *file_name = "/dev/dev_hello";
  int fd;	
  int rc=0,result;
  long ret_value;
  opterr = 0;    
  dev_data_arg_t opt_dev_data={0x01,0x02,0x00,0x00};
  option=o_ioctl_get;

    
  if (argc == 1)
  {
		print_uasge( argv[0]);
		goto addr_end;
  }
  else
  {
	  
    while( (result = getopt(argc, argv, "gs1:2:3:")) != -1 )
    {
           switch(result)
          {
               case '1':
                   printf("option=1, optopt=%c, optarg=%s\n", optopt, optarg);
				   opt_dev_data.cmd1=(unsigned char)strtol(optarg, NULL, 16); 				   
                   break;
              case '2':
                   printf("option=2, optopt=%c, optarg=%s\n", optopt, optarg);
				   opt_dev_data.cmd2=(unsigned char)strtol(optarg, NULL, 16); 				   
                   break;
              case '3':
                   printf("option=1, optopt=%c, optarg=%s\n", optopt, optarg);
				   ret_value=0;
				   ret_value = strtol(optarg, NULL, 16);
				   opt_dev_data.data1=(unsigned char)ret_value; 
                   break;
              case 'g':
                    printf("result=g, optopt=%c, optarg=%s\n", optopt, optarg);
					option=o_ioctl_get;
                    break;
              case 's':
                    printf("result=s, optopt=%c, optarg=%s\n", optopt, optarg);
					option=o_ioctl_set;
                    break;
			default:
                   printf("default, result=%c\n",result);
                   break;
           }
        printf("argv[%d]=%s\n", optind, argv[optind]);
    }	  
	  
  }	 

  printf("Show before call commmand: option: %d\n",option); 
  show_dev_Data(&opt_dev_data);
  
  fd = open(file_name, O_RDWR);
  if (fd == -1)
    {
        printf("[%s]:fail to open",argv[0]);
        rc=2;
		goto addr_end;
    }  

  switch (option)
  {
	case o_ioctl_get:
            get_dev_data(fd,&opt_dev_data);
            break;
	case o_ioctl_set:
            set_dev_data(fd,&opt_dev_data);
            break;
    default:
            break;
    }	
	
  close (fd);
	
addr_end: 
  return rc;
  
  
}

