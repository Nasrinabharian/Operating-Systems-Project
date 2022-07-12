#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h" 
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int stdout = 1;
#define MAXCHILD 5
int T=10;

int main(int argc, char *argv[]){
	printf(stdout, "dpro2 is running (pid: %d) to make 5 children\n",(int)getpid());
	int Gid=(int)getpid();
	int child [MAXCHILD];
	int inChild=0;
	//int status=0;
	for (int i=0;i<MAXCHILD;i++){
		sleep(20);
		child[i]=fork();
 		if(child[i]==0){
			inChild=1;
			printf(stdout, "Parent (pid: %d) creating child %d (pid: %d)\n",Gid,(i+1), (int)getpid());
 			break;
		}
	}
	while (inChild==1){
 		//srand(getpid());
		//int r = 10000;
		//sleep(r+T);
		//T=T-3;
		printf(stdout, "Child (pid: %d) created\n",(int)getpid());
		int g=0;
		int i;
		for(i=0;i<5000000000000;i=i+1){
			int y;
			g++;
			y = y+1/5;
			y=g+5;
		}
		inChild=-1;
		exit();
	}
	while(inChild==0){
		sleep(1);
		int i=0;
		//for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
		for(i=0;i<5;i=i+1){
			//sleep(1000);
 			int child_d;
			//**comment from next line
			//child_d = waitpid(child[i],&status,0);
			child_d = wait();
			child_d =1; 
 			if(child_d>0){
				printf(stdout, "child[%d] is dead now \n", child[child_d]);
				kill(child[child_d]);
				continue;}
 			else if(child_d==-1 && i==4){
				exit();}
 				//printf("no child to wait for \n");
			//**comment till this line
 			 //child_d = waitpid(child[i],&status,0);
			// if(child_d==0)
 				// printf("child[%d] is still alive\n",child[i]);
			// else if(child_d>0)
				// printf("child[%d] is dead now \n",child[i]);
 			// else
				//exit();
				 //return 0;
				 // printf("no specified child to wait for \n");
		}
	}
	exit();
	//return 0;
}

