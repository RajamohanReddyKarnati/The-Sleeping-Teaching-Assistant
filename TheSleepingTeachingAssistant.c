#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

#define MAX_SLP_TIME  5
#define MAX_WAIT_STUDENTS    3
#define NUM_OF_STUDENT     5
#define NUMBER_OF_SEATS    3
pthread_mutex_t     mut_lock;
sem_t           *stu_sem;
sem_t           *ta_se;
int wait_std;
int std_num;
int std_id[NUM_OF_STUDENT];
void *students_loops(void *param);
void *ta_loops(void *param);
pthread_t ta;
pthread_t students[NUM_OF_STUDENT];

void init()
{
    int i;
 
    if ( pthread_mutex_init(&mut_lock, NULL) != 0)
	{
		printf("%s\n",strerror(errno));
	}
    if (sem_unlink("STUDENTS") == -1)
	{
		printf("%s\n",strerror(errno));
	}
    if (sem_unlink("TA") == -1)
	{
		printf("%s\n",strerror(errno));
	}
    if ( (stu_sem = sem_open("STUDENTS", O_CREAT, 0666, 0)) == SEM_FAILED)
	{
		printf("%s\n",strerror(errno));
	}
    if ( (ta_se = sem_open("TA", O_CREAT, 0666, 0)) == SEM_FAILED)
	{
		printf("%s\n",strerror(errno));
	}
	wait_std = 0;
    for (i = 0; i < NUM_OF_STUDENT; i++)
	{
		std_id[i] = i;
	}
}
void create_student()
{
    int i;
    for (i = 0; i < NUM_OF_STUDENT; i++) 
	{
        pthread_create(&students[i], 0, students_loops, (void *)&std_id[i]);
    }
}
void *students_loops(void *param)
{
    int *lnum = (int *)param;
    int number = *lnum;
    int sleeping_timeing;
    int times_through_loop = 0;
    srandom((unsigned)time(NULL));
    while (times_through_loop < 5) 
	{
        sleeping_timeing = (int)((random() % MAX_SLP_TIME) + 1);
        hang_out(*lnum,sleeping_timeing);
        if ( pthread_mut_lock(&mut_lock) != 0)
		{
			printf("StudentA %s\n",strerror(errno));
		}
        if (wait_std < NUMBER_OF_SEATS) 
		{
            ++wait_std;
            printf("\t\tStudent %d takes a seat waiting = %d\n",*lnum, wait_std);
             
            if (sem_post(stu_sem) != 0)
			{
				printf("StudentB %s\n",strerror(errno));
			}
 
            if (pthread_mutex_unlock(&mut_lock) != 0)
			{
				printf("StudentC %s\n",strerror(errno));
			}
 
            if (sem_wait(ta_se) != 0)
			{
				printf("StudentD %s\n",strerror(errno));
			}
            printf("Student %d receiving help\n",*lnum);
            ++times_through_loop;
        }
		else
		{
            printf("\t\t\tStudent %d will try later\n",*lnum);
            pthread_mutex_unlock(&mut_lock);
        }
    }
}


void create_ta()
{
    pthread_create(&ta, 0, ta_loops, 0);
}
void hang_out(int lnum, int sleeping_timeing) 
{
    printf("\tStudent %d hanging out for %d seconds\n",lnum,sleeping_timeing);
    sleep(sleeping_timeing);
}
void help_student(int sleeping_timeing)
{
        printf("Helping a student for %d seconds waiting students = %d\n",sleeping_timeing, wait_std);
        sleep(sleeping_timeing);
}
void *ta_loops(void *param)
{
    int sleeping_timeing;
    srandom((unsigned)time(NULL));
    while (1) {
        if ( sem_wait(stu_sem) != 0)
		{
			printf("%s\n",strerror(errno));
		}
        if (pthread_mut_lock(&mut_lock) != 0)
		{
			printf("%s\n",strerror(errno));
		}
        if (sem_post(ta_se) != 0)
		{
			printf("%s\n",strerror(errno));
		}
        if (pthread_mutex_unlock(&mut_lock) != 0)
		{
			printf("%s\n",strerror(errno));
		}
        sleeping_timeing = (int)((random() % MAX_SLP_TIME) + 1);
        help_student(sleeping_timeing);
    }
}
int main(void)
{
    int i;
    init();
    create_ta();
    create_student();
    for (i = 0; i < NUM_OF_STUDENT; i++)
	{
		pthread_join(students[i], NULL);
	}
    if (pthread_cancel(ta) != 0)
	{
		printf("%s\n",strerror(errno));
	}
    return 0;
}