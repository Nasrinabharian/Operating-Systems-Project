#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_LENGTH 256               ///< The buffer length (crude but fine)

static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM

void routine(){

 int x,ret,fd;
 char str[10];
 fd = open("/dev/ebbchar", O_RDWR|O_NONBLOCK);             // Open the device with read/write access
 if (fd < 0){
       perror("Failed to open the device...");
       return errno;
 }
 else {
 srand(getpid());
 for(int i=0;i<1000000;i++){
    x = (rand()%100)+1;
    sprintf(str,"e,%d,0,1",x);
    ret = write(fd, str, strlen(str));
    if (ret < 0){
        perror("Failed to write the message to the device.");
        return errno;
   }
 } 
 }   
}
int main(){
   int ret, fd,inChild=0,status=1;
   pid_t pid[2];
      
   for (int i = 0; i < 2; i++) {
        pid[i]=fork();
        if (pid[i]==0){
          inChild=1;
          routine();
          break;
        }
      }
  if(inChild==0){
   while(wait(&status)>0);
    fd = open("/dev/ebbchar", O_RDWR|O_NONBLOCK);             // Open the device with read/write access
     if (fd < 0){
       perror("Failed to open the device...");
       return errno;
      }
     printf("Reading from the device...\n");
     ret = read(fd, receive, BUFFER_LENGTH);      
      if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("The received message is: [%s]\n", receive);
   printf("End of the program\n");
   }
  
   return 0;
}
