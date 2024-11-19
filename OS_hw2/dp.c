#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUMBER 5
#define MAX_SLEEP_TIME 5
//#define THINKING 0
//#define HUNGRY	1
//#define EATING	2

enum {THINKING,HUNGRY,EATING} state[NUMBER];
int thread_id[NUMBER];
pthread_cond_t	cond_vars[NUMBER];
pthread_mutex_t	mutex_lock;
pthread_t tid[NUMBER];

void thinking(int sleep_time){
	//printf("thinking for %d seconds",sleep_time);
	sleep(sleep_time);
}
void eating(int sleep_time){
	//printf("eating for %d seconds\n",sleep_time);
	sleep(sleep_time);
}
/* return the left neighbor */
int left_neighbor(int number){
	if(number==0)	return NUMBER-1;
	else	return number-1;
}

/* return the right neighbor */
int right_neighbor(int number){
	if(number==NUMBER-1)	return 0;
	else	return number+1;
}
void test(int i){
	//If I'm hungry, and my left and right neighbors aren't eating, then let me eat.
	if((state[left_neighbor(i)]!=EATING) && (state[i]==HUNGRY) && (state[right_neighbor(i)]!=EATING)){
		state[i] = EATING;
		pthread_cond_signal(&cond_vars[i]);
	}
}
void pickup_forks(int number){
	pthread_mutex_lock(&mutex_lock);

	state[number] = HUNGRY;
	test(number);

	while (state[number]!=EATING){
		pthread_cond_wait(&cond_vars[number], &mutex_lock);
	}

	pthread_mutex_unlock(&mutex_lock);
}
void return_forks(int number){
	pthread_mutex_lock(&mutex_lock);

	state[number] = THINKING;
	test(left_neighbor(number));
	test(right_neighbor(number));

	pthread_mutex_unlock(&mutex_lock);
}
void *philosopher(void *param)
{
	int *lnumber = (int *)param;
	int number = *lnumber;
	int sleep_time;
	int times_through_loop = 0;

	srand((unsigned)time(NULL));

	while(times_through_loop<5){
		sleep_time = (int)((rand() % MAX_SLEEP_TIME) + 1);
		thinking(sleep_time);

		pickup_forks(number);

		printf("Philosopher %d is eating\n",number);

		sleep_time = (int)((rand() % MAX_SLEEP_TIME) + 1);
		eating(sleep_time);

		printf("Philosopher %d is thinking\n",number);
		return_forks(number);
		
		times_through_loop++;
	}
}
int main(void){
	int i;
	
	for (i=0;i<NUMBER;i++){
		state[i] = THINKING;
		thread_id[i] = i;
		pthread_cond_init(&cond_vars[i],NULL);
	}

	pthread_mutex_init(&mutex_lock, NULL);

	for (i=0;i<NUMBER;i++){
		pthread_create(&tid[i], 0, philosopher, (void *)&thread_id[i]);
	}

	for(i=0;i<NUMBER;i++){
		pthread_join(tid[i],NULL);
	}

	return 0;
}







	
