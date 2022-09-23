#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
#include <sys/mman.h>
#include <assert.h>
#include <linux/poll.h>
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
 
int main()
{
        int fd;
        char ch;
        int32_t value, number; 
        int N=5;
        char kernel_val[20];
	int ret;
	char buff[128];
	struct pollfd pfd;        
	int *ptr = mmap ( NULL, N*sizeof(int),PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
        printf("\nOpening Driver\n");
        fd = open("/dev/tayip_device", O_RDWR);
        printf("%d\n",fd);
        if(fd < 0) {
                printf("Device file açılamadı!\n");
                return 0;
        }
        printf("Emir kodu giriniz:\n");
        scanf("%c",&ch);
        switch(ch){
        	case 'i': 	
        		printf("Enter the Value to send\n");
        		scanf("%d",&number);
        		printf("Writing Value to Driver\n");
        		ioctl(fd, WR_VALUE, (int32_t*) &number); 
        		printf("Reading Value from Driver\n");
        		ioctl(fd, RD_VALUE, (int32_t*) &value);
        		printf("Value is %d\n", value);
 			break;
 		case 'w':
 			printf("Enter the Value to send\n");
        		scanf("%d",&number);
        		printf("Writing Value to Driver\n");
        		ioctl(fd, WR_VALUE, (int32_t*) &number); 
        		break;
        	case 'r':
        		printf("Reading Value from Driver\n");
        		ioctl(fd, RD_VALUE, (int32_t*) &value);
        		printf("Value is %d\n", value);
 			break;
 		case 'm':			
			printf("MMAP giriş\n");
			if(ptr == MAP_FAILED){
    				printf("Mapping Failed\n");
   			        return 1;
			}

			for(int i=0; i<N; i++)
 			        ptr[i] = i*10;

			for(int i=0; i<N; i++)
   				 printf("[%d] ",ptr[i]);
				 printf("\n");
			int err = munmap(ptr, 10*sizeof(int));
			if(err != 0){
   				 printf("UnMapping Failed\n");
  		        return 1;
			}
			break;
		case 'p':
			int fd = open("/dev/tayip_device", O_RDWR | O_NONBLOCK);
			if(fd == -1){
				perror("open");
				exit(EXIT_FAILURE);
			}
			pfd.fd = fd;
			pfd.events = (POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM);
			while(1)
			{	
				printf("Poll başlıyor...\n");
				ret = poll(&pfd, (unsigned long)1,500000); //wait for 5 secs
				printf("ret değeri=%d\n", ret);
					
				if(ret < 0){
					perror("poll");
					assert(0);
				}
				if((pfd.revents & POLLIN) == POLLIN){
					read(pfd.fd, &kernel_val, sizeof(kernel_val));
					printf("POLLIN: Kernel_val = %s\n", kernel_val);
				}
				if((pfd.revents & POLLOUT) == POLLOUT)
				{
					strcpy(kernel_val, "User Space");
					write(pfd.fd, &kernel_val, strlen(kernel_val));
					printf("POLLOUT : Kernel_value = %s\n", kernel_val);	
				}
			}					
			break;		
 		default:
 			printf("Hatalı giriş!\n");
 			break;	
 	}		
        printf("Closing Driver\n");
        close(fd);
        return 0;
}
