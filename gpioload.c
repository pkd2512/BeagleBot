/***********************************************************************************
 * Here the following GPIO pins have been enabled by the BONE DEVICE TREE
 * headerP8 ---> P8_11 to P8_18 || GPIO NOs. : 45,44,23,26,47,46,27,65
 * headerP9 ---> P9_11 to P9_18 || GPIO NOs. : 30,60,31,50,48,51,5,4
 ***********************************************************************************/


char setValue[4],GPIOString[4],GPIOValue[64],GPIODirection[64];
FILE *pinHandle=NULL;

/* 
 * Check if the GPIO DEvice tree is enabled, else, load the GPIO-Enable BoneDeviceTree to SLOTS
 */
void gpio()  
{	
	char temp[LINE_MAX];
	const char dtb[] = "GPIO-Enable"; 
	int flag = 1;
        if ((pinHandle=fopen("/sys/devices/bone_capemgr.9/slots","r+"))!=NULL)
        {
		printf("\nOverlaying GPIO-Enable...");
	/* Check if GPIO-Enable-OOAO.dtbo is already loaded */
	
	while (fgets(temp,sizeof(temp),pinHandle)!=NULL) 
	{			
		if ((strstr(temp,dtb))!=NULL) 
			flag = 0;
		}
	if(flag)			
	{	
		fputs(dtb,pinHandle);
		printf ("GPIO-Enable-00A0.dtbo loaded successfully to slots!\n");
               }
	else
		printf ("GPIO-Enable-00A0.dtbo already exists in slots!\n");
	}
	fclose(pinHandle);
}

/* Export a GPIO pin to sysfs */
int pin_export(int GPIOpin)
{
	sprintf(GPIOString,"%d",GPIOpin);
	if((pinHandle=fopen("/sys/class/gpio/export","ab"))==NULL)
	{
		printf("\nUnable to export GPIO pin %d\n",GPIOpin);
		return 1;
	}
	strcpy(setValue,GPIOString);
	fwrite(&setValue,sizeof(char),2,pinHandle);
	printf("\nGPIO Pin %d exported successfully!\n",GPIOpin);
	fclose(pinHandle);
return 0;
}

/* Set the direction of GPIO-pin (in/out) */
int pin_Direction(int GPIOpin)
{
	sprintf(GPIODirection,"/sys/class/gpio/gpio%d/direction",GPIOpin);
	
	if((pinHandle=fopen(GPIODirection,"rb+"))==NULL)
	{
		printf("\nUnable to open direction of GPIO pin %d\n",GPIOpin);
		return 1;
	}
	
	strcpy(setValue,"out");
	fwrite(&setValue,sizeof(char),3,pinHandle);
	printf("\nSetting GPIO Pin %d as OUTPUT pin...\n",GPIOpin);
	fclose(pinHandle);
return 0;
}

/* Set value of GPIO-Pin (0/1) */
int pin_Value(int GPIOpin,int val)
{
	sprintf(GPIOValue,"/sys/class/gpio/gpio%d/value",GPIOpin);
	sprintf(GPIOString,"%d",val);

	if((pinHandle=fopen(GPIOValue,"rb+"))==NULL)
	{
		printf("\nUnable to open value of GPIO pin %d\n",GPIOpin);
		return 1;
	}
	strcpy(setValue,GPIOString);
	fwrite(&setValue,sizeof(char),1,pinHandle);
	printf("\nGPIO Pin %d VALUE set to %d...\n",GPIOpin,val);
	fclose(pinHandle);
return 0;
}	

/* Un-Export a GPIO pin to sysfs */
int pin_unexport(int GPIOpin)
{
	sprintf(GPIOString,"%d",GPIOpin);
	if((pinHandle=fopen("/sys/class/gpio/unexport","ab"))==NULL)
	{
		printf("\nUnable to unexport GPIO pin %d\n",GPIOpin);
		return 1;
	}
	strcpy(setValue,GPIOString);
	fwrite(&setValue,sizeof(char),2,pinHandle);
	printf("\nGPIO Pin %d un-exported successfully!\n",GPIOpin);
	fclose(pinHandle);
return 0;
}

/* Get the part number for the PWM pins, as it varies on the sequence of overlays in the slots file */
inline int get_PWM_partNo(const char *type)
{
	const struct dirent *ent;
	int number, numstrlen;

	FILE *nameFile;
	DIR *dp;
	char thisname[64];
	char *filename;

	dp = opendir("/sys/devices/ocp.3/");
	if (dp == NULL) {
		printf("Failed to get PWM Part No.\n");
		return -ENODEV;
	}

	while (ent = readdir(dp), ent != NULL) 
	{
		if (strcmp(ent->d_name, ".") != 0 && 
			strcmp(ent->d_name, "..") != 0 &&
			strlen(ent->d_name) > strlen(type) &&
			strncmp(ent->d_name, type, strlen(type)) == 0)
			{
				numstrlen = sscanf(ent->d_name + strlen(type),"%d",&number);
		
			/* verify the next character is not a dot */
			if (strncmp(ent->d_name + strlen(type) + numstrlen,".",1) != 0) 
			{
				filename = malloc(strlen("/sys/devices/ocp.3/") + strlen(type) + numstrlen + 6);
				if (filename == NULL)
				{
					closedir(dp);
					return -ENOMEM;
				}
				sprintf(filename, "/sys/devices/ocp.3/%s%d",type, number);
				nameFile = fopen(filename, "r");
				if (!nameFile) 
				{
					free(filename);
					continue;
				}
				free(filename);
				fscanf(nameFile, "%s", thisname);
				fclose(nameFile);
				
				closedir(dp);
				return number;
			}
		}
	}
	closedir(dp);
	return -ENODEV;
}
	
/***************************************END_OF_GPIO-Enable***********************************/	
	
