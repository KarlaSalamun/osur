#include <stdio.h>
#include <api/prog_info.h>
#include <time.h>

extern size_t cnt;
size_t sec_cnt = 0;

static void alarm_func ( sigval_t param ) 
{
	sec_cnt++;
	printf ( "sekunda: %d	broj prekida: %d \n", sec_cnt, cnt);
	cnt = 0;
}

int timer_func (void) 
{
	timespec_t t;
	itimerspec_t t1;	
	sigevent_t evp;
	timer_t timer;

	printf ( "Example program: [%s:%s]\n%s\n\n", __FILE__, __FUNCTION__,
		timer_PROG_HELP );

	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function = alarm_func;
	evp.sigev_notify_attributes = NULL;

	t1.it_interval.tv_sec = 1;
	t1.it_interval.tv_nsec = 0;
	t1.it_value.tv_sec = 1;
	t1.it_value.tv_nsec = 0;

	evp.sigev_value.sival_int = t1.it_interval.tv_sec;
	timer_create ( CLOCK_REALTIME, &evp, &timer );
	timer_settime ( &timer, 0, &t1, NULL );

	t.tv_sec = 20;
	t.tv_nsec = 0;

	while ( TIME_IS_SET (&t) ) 
	{
		if ( clock_nanosleep ( CLOCK_REALTIME, 0, &t, &t ) )
		{
			printf ( "Interrupted sleep\n ");
		}
	}	
	
	timer_delete ( &timer );
	
	return 0;
}
		
