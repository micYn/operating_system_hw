#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
using namespace std;
#define N 5
#define MAX_SLEEP_TIME 5
//#define THINKING 0
//#define HUNGRY 1
//#define EATING 2
//#define LEFT (i+N-1)%N
//#define RIGHT (i+1)%N

pthread_t tid[N];
int thread_id[N];
class Monitor{
	private:
		enum {THINKING,HUNGRY,EATING} state[N];
		pthread_cond_t self[N];	//condition variable
		pthread_mutex_t mutex;	//make sure that the operations in this monitor can only be access by one thread at a time.
	public:
		void test(int i){
			if(state[i]==HUNGRY && state[(i+4)%5]!=EATING && state[(i+1)%5]!=EATING){      
	      			state[i]=EATING;
				pthread_cond_signal(&self[i]);
	    		}
	     		printf("philosopher %d is eating now...\n",i+1);
		}
		void pickup(int i){
			pthread_mutex_lock(&mutex);
				state[i] = HUNGRY;
				test(i);
				if(state[i] != EATING)
					pthread_cond_wait(&self[i],&mutex);
			pthread_mutex_unlock(&mutex);

		}
		void putdown(int i){
			pthread_mutex_lock(&mutex);
				state[i] = THINKING;
				printf("philosopher %d is thinking now...\n",i+1);
				test((i+4)%5);
				test((i+1)%5);
	       		pthread_mutex_unlock(&mutex);
		}
		void init(){	
			pthread_mutex_init(&mutex,NULL);	
	       		for(int i=0;i<N;i++){
				state[i]=THINKING;
				pthread_cond_init(&self[i],NULL);
			}
		}
}M;


void *philosopher(void *param){

	int *lnumber = (int *)param;
	int number = *lnumber;
	int sleep_time;
	int times_through_loop = 0;

	srand((unsigned)time(NULL));

	while(times_through_loop<5){
		sleep_time = (int)((rand() % MAX_SLEEP_TIME) + 1);
		sleep(sleep_time);	//simulates the thinking time

		M.pickup(number);


		sleep_time = (int)((rand() % MAX_SLEEP_TIME) + 1);
		sleep(sleep_time);	//simulates the eating time


		M.putdown(number);
		
		times_through_loop++;
	}
}

int main(){
	M.init();
	for(int i=0;i<N;i++){
		thread_id[i]=i;
	        pthread_create(&tid[i], 0, philosopher, (void*)&thread_id[i]);
	}    	
	for(int i=0;i<N;i++)
        	pthread_join(tid[i],0);
        
	return 0;
}

	
	
