/*********************************************************************************************
 * The code accesses the root files for PWM and GPIO and edits them to produce the results. 
 * On P9 header, PIN 2-> GND, PIN 21->pwm o/p.
 * On P9 header, PIN 12-> direction, PIN 14-> brake.
***********************************************************************************************/

unsigned long int rightPeriod = 50000000;	// 50 milliseconds period 
						// 50% duty cycle
/* Controlling the DIR Signal */
void motor_direction_right(int dr)
{	
	pin_Direction(60);
	pin_Value(60,dr); // 1-> Forward || 0-> Backward
}

/* Controlling the BRAKE Signal */
void brake_right(int tr)
{	
	pin_Direction(51); 
	pin_Value(51,tr); 
}

/* Enabling the PWM for the left motor */	
int motor_right()
{
	pin_export(60);		//direction control
	printf ("\nDirection control set for right motor on P9.12\n");
	pin_export(51);		//run control
	printf ("\nBrake control set for right motor on P9.16\n");
	pin_Direction(51); 
	pin_Value(51,1);
	motor_direction_right(1);

        FILE *pwm21_Handle=NULL;
        char period[64];
	char duty[64];
	
	char rperiod_ns[10], rduty_ns[10];
	
	/* Values of Period and Duty in nanoseconds */
	sprintf(rperiod_ns,"%lu",rightPeriod);
	sprintf(rduty_ns,"%lu",(rightPeriod/2));

	/* Getting the part number for the PWM pin P8.19
	 * Setting paths for the period and duty files
	 */
	sprintf(period,"/sys/devices/ocp.3/pwm_test_P9_21.%d/period",get_PWM_partNo("pwm_test_P9_21."));
	sprintf(duty,"/sys/devices/ocp.3/pwm_test_P9_21.%d/duty",get_PWM_partNo("pwm_test_P9_21."));
	
	if ((pwm21_Handle=fopen(period,"w"))==NULL)
	{
		printf("\nFailed to set P9.21 period");
		return -1;
	}
		fwrite(rperiod_ns,sizeof(rperiod_ns),1,pwm21_Handle);
		printf ("\nPWM PERIOD on P9.21 is %lu\n",rightPeriod);
		fclose(pwm21_Handle);
	

	if ((pwm21_Handle=fopen(duty,"w"))==NULL)
	{
		printf("\nFailed to set P9.21 duty");
		return -1;
	}
		fwrite(rduty_ns,sizeof(rduty_ns),1,pwm21_Handle);
		printf ("\nPWM PERIOD on P9.21 is 50%%  \n");
		fclose(pwm21_Handle);
return 0;		
}

void unexport_P9()
{
	pin_unexport(51);
	pin_unexport(60);
}

/********************************END_OF_MOTOR_RIGHT**********************************/

