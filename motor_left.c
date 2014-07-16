/*********************************************************************************************
 * The code accesses the root files for PWM and GPIO and edits them to produce the results. 
 * On P8 header, PIN 2-> GND, PIN 19->pwm o/p.
 * On P8 header, PIN 12-> direction, PIN 16-> brake.
***********************************************************************************************/

unsigned long int leftPeriod = 50000000;	// 50 milliseconds period 
						// 50% duty cycle
/* Controlling the DIR Signal */
void motor_direction_left(int dl)
{
	pin_Direction(44);
	pin_Value(44,dl); // 1-> Forward || 0-> Backward
}

/* Controlling the BRAKE Signal */
void brake_left(int tl)
{	
	pin_Direction(46);
	pin_Value(46,tl); 
}

/* Enabling the PWM for the left motor */	
int motor_left()
{
	pin_export(44);		//direction control 
	printf ("\nDirection control set for left motor on P8.12\n");
	pin_export(46);		//run control
	printf ("\nBrake control set for left motor on P8.16\n");
	pin_Direction(46);
	pin_Value(46,1);
	motor_direction_left(1);
	
        FILE *pwm19_Handle=NULL;
        char period[64];
	char duty[64];

	char lperiod_ns[10], lduty_ns[10];
	
	/* Values of Period and Duty in nanoseconds */
	sprintf(lperiod_ns,"%lu",leftPeriod);
	sprintf(lduty_ns,"%lu",(leftPeriod/2));
	
	/* Getting the part number for the PWM pin P8.19
	 * Setting paths for the period and duty files
	 */
	sprintf(period,"/sys/devices/ocp.3/pwm_test_P8_19.%d/period",get_PWM_partNo("pwm_test_P8_19."));
	sprintf(duty,"/sys/devices/ocp.3/pwm_test_P8_19.%d/duty",get_PWM_partNo("pwm_test_P8_19."));
	
	if ((pwm19_Handle=fopen(period,"w"))==NULL)
	{
		printf("\nFailed to set P8.19 period");
		return -1;
	}
		fwrite(lperiod_ns,sizeof(lperiod_ns),1,pwm19_Handle);
		printf ("\nPWM PERIOD on P8.19 is %lu\n",leftPeriod);
		fclose(pwm19_Handle);		
	
	if ((pwm19_Handle=fopen(duty,"w"))==NULL)
	{
		printf("\nFailed to set P8.19 duty");
		return -1;
	}
		fwrite(lduty_ns,sizeof(lduty_ns),1,pwm19_Handle);
		printf ("\nPWM DUTY on P8.19 is 50%% \n");
		fclose(pwm19_Handle);
return 0;			
}

void unexport_P8()
{
	pin_unexport(44);
	pin_unexport(46);
}

/********************************END_OF_MOTOR_LEFT**********************************/

