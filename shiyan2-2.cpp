#include<sys/types.h>
#include<iostream>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdio.h>

using namespace std;
#define MAX 100

//此部分在头文件中丢失
union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};
union semun arg;
struct sembuf sem;
int a=0;
int semid; 

//p操作
void P(int semid,int index)
{	  
    struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op = -1;	
    sem.sem_flg = 0;	//操作标记：0或IPC_NOWAIT等	
    semop(semid,&sem,1);	//1:表示执行命令的个数
    return;
}

//V操作
void V(int semid,int index)
{	  
    struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op =  1;
    sem.sem_flg = 0;	
    semop(semid,&sem,1);	
    return;
}

void *cacuf(void*){
        for(int i=0;i<MAX;i++)
        {
               P(semid,0);
               a+=i;
		if(a%2==0){
			V(semid,1);
		}else{
			V(semid,2);
		}
	}	
}
void *print1_even(void*){
	while(a<=5150){
		P(semid,1);
		cout<<"phread1 print even num: "<<a<<endl;
		V(semid,0);
		if((a==MAX-1)&&(MAX%2)){
			break;
		}
	}
}
void *print2_odd(void*){
	while(a<=5050){
		P(semid,2);
		cout<<"phread2 print odd num: "<<a<<endl;
		V(semid,0);
		if((a==MAX-1)&&!(MAX%2)){
			break;
		}
	}
}
int main(){
	pthread_t cacu,p1,p2;
	int key=ftok("/tmp",0x66);
	if(key<0){
		cout<<"ftok error!";
		return 1;
	}
	semid = semget(key,3,IPC_CREAT|0666);  //使用3个信号量
	//0号：init=1 表计算进程的资源数
	//1号：init=0 偶数资源数
	//2号：init=0 奇数资源数
	if(semid==-1){
		cout<<"semget error!";
		return 1;
	}
	arg.val = 1;
	if(semctl(semid,0,SETVAL,arg)<0){
		cout<<"semctl error!";
		return 1;
	}
	arg.val=0;
	if(semctl(semid,1,SETVAL,arg)<0){
		cout<<"semctl error!";
		return 1;
	}
	if(semctl(semid,2,SETVAL,arg)<0){
		cout<<"semctl error!";
		return 1;
	}
	//创建信号灯

	pthread_create(&cacu,NULL,cacuf,NULL);
	pthread_create(&p1,NULL,print1_even,NULL);
	pthread_create(&p2,NULL,print2_odd,NULL);

	pthread_join(cacu,NULL);
	pthread_join(p1,NULL);
	pthread_join(p1,NULL);
       semctl(semid,0,IPC_RMID);
      	return 0;
}
