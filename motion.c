/* This file contains the funtions that will let the bot keep track of its DISTANCE and POSITION.
   Based on calculations, the bot will MOVE or ROTATE
*/

/* The motors are running at 20 Hz, 50% duty cycle at 12 volts.
   Distance moved in 1 full rotation = 33.31 cm
   Average velocity of bot = 40 cm/sec
*/

#include <unistd.h>

#define TRACK_WIDTH 23
#define DIAMETER 10.6
#define SPEED 40
#define TRY_LIMIT 1

/* Array to read the data from the file containing position records of the BOT.
0=Serial No.	1=runTime	2=X-coordinate	3=Y-coordinate		4=Heading
*/
float trackRecord [5] = {1.0,10.0,5.0,10.0,80.0}; 
char *rec = "/mnt/home/bot.txt";
/* Declaring global variable to hold tracking data for the BOT */
float posX, posY, heading;		// current coordinates
float deltaX, deltaY, deltaHeading;	// difference in position values
float distance;
long unsigned int runTime=0;
int hops;



/* TO ROTATE THE BOT BY SPECIFIED ANGLE, RIGHT AND LEFT */

int right (float angle)	// angle is in degrees
{
	distance = (TRACK_WIDTH/2.0f)*tanf(angle*M_PI/180.0f);
	runTime = distance*1000000/SPEED;
	printf("\nRotating RIGHT by %.2f degrees. Running motors for %lu milliseconds",angle,runTime/1000);

	motor_direction_right(0);
	motor_direction_left(1);

	brake_right(1);
	brake_left(0);
	printf("Rotating for %lu milliseconds\n",runTime/1000);
	usleep(runTime);
	
	brake_left(1);
return 0;
}
int rotate_right(float degrees)
{
	if(degrees<0.0)
	degrees = degrees*(-1);	// to get positive value of the angle
		
	if (degrees<90.0)
	{
		right(degrees);
	}
	else
	{
		right(45.0);
		right(45.0);
	}
return 0;	
}
int left (float angle)	// angle is in degrees
{	
	distance = (TRACK_WIDTH/2.0f)*tan(angle*M_PI/180.0f);
	runTime = distance*1000000/SPEED;	
	printf("\nRotating LEFT by %.2f degrees. Running motors for %lu milliseconds",angle,runTime/1000);

	motor_direction_right(1);
	motor_direction_left(0);

	brake_left(1);
	brake_right(0);
	printf("Rotating for %lu milliseconds\n",runTime/1000);
	usleep(runTime);
	
	brake_right(1);
return 0;
}

int rotate_left (float degrees)
{	
	if(degrees<0.0)
	degrees = degrees*(-1);	// to get positive value of the angle
		
	if (degrees<90.0)
	{
		left(degrees);
	}
	else
	{
		left(45.0);
		left(45.0);
	}
return 0;
}

/* 
 * TO READ AND WRITE FROM THE RECORD FILE OF THE BOTs
 * mount point "/mnt/home"
 */

int readRecord()
{
	FILE *bot = NULL;
	char *rec = "/mnt/home/bot.txt";
	int i;
	char record[30];

	for (i=0;i<TRY_LIMIT;i++)
	{
		if ((bot = fopen(rec,"a+"))!=NULL) 	// try to open the file, till it opens.
		{
		printf("\n Record file opened successfully!\n");
		break;
		}	
				
		else
		{
		printf("\nRetry %d to open the Record file...\n",i);
		continue;
		}
	}
/*
 * The record file will have numbers separated by blank spaces.  e.g. "\n0 0 0 0 0 "
 */

	fseek(bot,0,SEEK_END);

	while ((i=fgetc(bot))!='\n')
	{		
		fseek(bot,-2,SEEK_CUR);
	}
	fgets(record,30,bot);
	sscanf(record,"%f%f%f%f%f",&trackRecord[0],&trackRecord[1],&trackRecord[2],&trackRecord[3],&trackRecord[4]);
		
	for(i=0;i<5;i++)
		printf("\t%.2f",trackRecord[i]);		// To display the acquired values successfully
		
	
fclose(bot);

return 0;
}

int writeRecord()
{
	FILE *bot = NULL;
	char *rec = "/mnt/home/bot.txt";
	int i;
	
	for (i=0;i<TRY_LIMIT;i++)
	{
		if ((bot = fopen(rec,"a+"))!=NULL) 	// try to open the file, till it opens.
		{
		printf("\n Record file opened successfully!\n");
		break;
		}	
		else
		{
		printf("\nRetry %d to open the Record file...\n",i);
		continue;
		}
	}
/*
 * The record file will have numbers separated by blank spaces.  e.g. "\n0 1 2 3 4  "
 */
	
	fprintf(bot,"\n%.0f %.0f %.2f %.2f %.2f ",trackRecord[0],trackRecord[1],trackRecord[2],trackRecord[3],trackRecord[4]);

fclose(bot);
return 0;
}

/* TO TRACK THE POSITION OF BOT AND MAKE IT MOVE ACCORDINGLY */

int track()
{
	// Will get the readings of current position and store from trackRecord[]

	posX = trackRecord[2];
	posY = trackRecord[3];
	heading = trackRecord[4];
	
	// Will get the readings for future position from the record file of other BOT and store in trackRecord[]

	readRecord();
	hops = trackRecord[0];
	// Calculate the average of the bot positions to determine next hop
	
	trackRecord[2] = (trackRecord[2]+posX)/2;
	trackRecord[3] = (trackRecord[3]+posY)/2;

	// Calculate adjustment values to move the BOT
	
	deltaX = trackRecord[2]-posX;
	deltaY = trackRecord[3]-posY;
	deltaHeading = (180.0f/M_PI)*(atanf(deltaX/deltaY))-heading;
/*
 * To measure the distance to the destination and move the bot accordingly
 */

	distance = sqrt((deltaX*deltaX) + (deltaY*deltaY));
	runTime = (unsigned long int) distance*1000000/SPEED;
	trackRecord[1] = runTime/1000;
	

/* The BOT will first rotate in its own position to adjust the heading, then will translate in that direction. 
 * The rotation will be guided by various cases to find the minimum angle of rotation and ease of motion
 */

	// CASE 1: When both lie on Y-axis	
	if (deltaX==0.0 && deltaY!=0.0)
	{
		if (deltaY>0.0)
		{
			rotate_left(90.0f-heading);
			trackRecord[4]=90.0f;
		}
		else
		{
			rotate_left(90.0f+heading);
			trackRecord[4]=(-90.0f);
		}
	}

	// CASE 2: When both lie on X-axis
	else if (deltaY==0.0 && deltaX!=0.0)
	{
		if (deltaX>0.0)
		{
			rotate_right(heading);
			trackRecord[4]=0.0;
		}
		else
		{
			rotate_left(180.0f-heading);
			trackRecord[4]=180.0;
		}
	}

	// CASE 3: When the angle is negative
	 
	else if (deltaHeading<0.0)
	{
		if (deltaX<0.0 && deltaY>0.0)
		{
			rotate_left(180.0f-(deltaHeading));
			trackRecord[4]=heading;
		}
		if (deltaX>0.0 && deltaY<0.0)
		{
			rotate_right(abs(deltaHeading));
			trackRecord[4]=heading;
		}
	}
	
	// CASE 4: When the angle is positive
	 
	else if (deltaHeading>0.0)
	{
		if (deltaX>0.0 && deltaY>0.0)
		{
			rotate_left(deltaHeading);
			trackRecord[4]=heading;
		}
		if (deltaX<0.0 && deltaY<0.0)
		{
			rotate_right(180.0f-(deltaHeading));
			trackRecord[4]=heading;
		}
	}
	
	// CASE 5: When the BOTS lie along same line
	
	else 
	{
		if (heading == trackRecord[4])	// Both in same direction
		{
			rotate_left(180.0f);
			trackRecord[4]=abs(heading-180.0f);
		}
		else				// Both facing each other
		{
			printf("\nHeaded in right direction\n");
			trackRecord[4]=(trackRecord[4]<0.0f)?(180.0f-abs(trackRecord[4])):((-1)*(180.0f-trackRecord[4]));
		}
	}
		
	
	// Will write the data of the current BOT from the trackRecord[] into its own file
	
	trackrecord[0] = hops;
	writeRecord();
		
return 0;
}
