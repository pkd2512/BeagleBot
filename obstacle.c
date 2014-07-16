/*************************************************************************************
 * This program uses the sampler application to sample the ADC @ 1 sample/100uSec
 * It reads the ADC samples from sensor.txt file
 * Counts the no. of samples and calculates the distance from the obstacle
 *************************************************************************************/

#include <stdio.h>
#include <unistd.h>

/* 
 * To generate a PWM signal of period 10 msec. with a peak time of 10 uSec.
 * The PWM pin P8_34 has been used for trigerring the ultrasonic sensors.
 * Thus the sensor will get triggered 100 times in 1 second.
 */
void trigger(int r)
{
        FILE *pwm34_Handle=NULL;
	char pwm_path[64];
	int partNo = get_PWM_partNo("pwm_test_P8_34.");
	
	// Get the part no for P8.34 and set the file path

	sprintf(pwm_path,"/sys/devices/ocp.3/pwm_test_P8_34.%d/polarity",partNo);
	if ((pwm34_Handle=fopen("","w"))!=NULL)
	{
		fputs("0",pwm34_Handle);
		fclose(pwm34_Handle);
	}
	
	sprintf(pwm_path,"/sys/devices/ocp.3/pwm_test_P8_34.%d/period",partNo);
	if ((pwm34_Handle=fopen(period,"w"))!=NULL)
	{
		fputs("200000000",pwm34_Handle);
		fclose(pwm34_Handle);
	}

	sprintf(pwm_path,"/sys/devices/ocp.3/pwm_test_P8_34.%d/duty",partNo);
	if ((pwm34_Handle=fopen(duty,"w"))!=NULL)
	{
		fputs("10000",pwm34_Handle);
		fclose(pwm34_Handle);
	}

	sprintf(pwm_path,"/sys/devices/ocp.3/pwm_test_P8_34.%d/run",partNo);
	if ((pwm34_Handle=fopen(run,"w"))!=NULL)
	{
		if(r==1)
		fputs("1",pwm34_Handle);
		else
		fputs("0",pwm34_Handle);

		fclose(pwm34_Handle);
	}
}

/*
 * To enable the ADC channels for the respective sensors
 * sensorN-->AIN0 | sensorNW-->AIN1 | sensorNE-->AIN2 | sensorW-->AIN3 | sensorE-->AIN4 | sensorS-->AIN5
 */
void enable_ADC_channels(int channel_num)
{
	FILE *adc = NULL;
	char adc_path[64];
	// channel_num enable path
	sprintf (adc_path,"/sys/bus/iio/devices/iio:device0/scan_elements/in_voltage%d_en",channel_num);	
	
	if ((adc = fopen(adc_path,"w")==NULL)
		printf("\nFailed to enable ADC Channel%d",channel_num);
	
	fwrite("1",sizeof(char),1,adc_path);
	fclose(adc_path);
}


struct sensor
{
	int value;
	float distance;
} sensorN,sensorNW,sensorNE,sensorW,sensorE,sensorS;

float readSensor(char *buffer_path)		
{
/* This funtion reads the value of the voltage samples from buffer file.
 * Calculates the ECHO pulse width and distance.
 */
	FILE *handle=NULL;
	int i,count=0;
	float voltageRaw=0.0;
	if (handle=((fopen(buffer_path,"r+"))=!NULL)
	{
		while ((i=fgetc(buffer_path))!=EOF)
		{
			fscanf(handle,"%f",&voltageRaw);
			if (voltageRaw > 1000.0)
			count++;
		}
		fclose(handle);
	}
	// Distance in centimetres
return (count*0.1/58.0f);
}


float eastIsClear()
{
	int i=5,temp=1;
	trigger(1);
	while(i>0)
	{
		for (;temp>0;temp++)
		{
			if((east.value=readSensor(0))<2000);
			break;
			usleep(116);	// Min range is 2 cm.
		}
	i--;
	usleep(200000);
	}
	trigger(0);
	east.distance=(float) (temp/5)*2.0; 
	
	if(east.distance<5.0)
	return 0.0;
	else
	return east.distance;
}

float westIsClear()
{
	int i=5,temp=1;
	trigger(1);
	while(i>0)
	{
		for (;temp>0;temp++)
		{
			if((west.value=readSensor(1))<2000);
			break;
			usleep(116);	// Min range is 2 cm.
		}
	i--;
	usleep(200000);
	}
	trigger(0);
	west.distance=(float) (temp/5)*2.0; 
	
	if(west.distance<5.0)
	return 0.0;
	else
	return west.distance;
}

float northIsClear()
{
	int i=5,temp=1;
	trigger(1);
	while(i>0)
	{
		for (;temp>0;temp++)
		{
			if((north.value=readSensor(2))<2000);
			break;
			usleep(116);	// Min range is 2 cm.
		}
	i--;
	usleep(200000);
	}
	trigger(0);
	north.distance=(float) (temp/5)*2.0; 
	
	if(north.distance<5.0)
	return 0.0;
	else
	return north.distance;
}

float southIsClear()
{
	int i=5,temp=1;
	trigger(1);
	while(i>0)
	{
		for (;temp>0;temp++)
		{
			if((south.value=readSensor(3))<2000);
			break;
			usleep(116);	// Min range is 2 cm.
		}
	i--;
	usleep(200000);
	}
	trigger(0);
	south.distance=(float) (temp/5)*2.0; 
	
	if(south.distance<5.0)
	return 0.0;
	else
	return south.distance;
}





