#include <stdio.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	/* Variables to store arguments provided by the user upon
	   start of program. Also includes a temporary string variable
	   used to store strings when reading files. */
	 
	char *file_to_read;
	int data_periodicity;
	char *output_file;
	int interrupt_number;
	int DSMS_process;
	char temp[255];


	/* Checks to see if there are correct number of arguments provided
	   when initiating the program. Otherwise the program will display
	   a reference to what arguments are required for the program to run
	   and then closes the program. */
	   
	if(argc != 6)
	{
		printf("Incorrect amount of arguments. Please try again.\n");
		printf("'NewsStream file_to_read data_periodicity output_file interrupt_number DSMS_process'\n\n");
		return 1;
	}
  

	/* Stores the arguments provided by the user and stores it in their
	   respective variables defined above. */
	 
	file_to_read = argv[1];
	data_periodicity = atoi(argv[2]);
	output_file = argv[3];
	interrupt_number = atoi(argv[4]);
	DSMS_process = atoi(argv[5]);

	
	/* If the user specifies that this is the second interrupt, then use
	   the SIGUSR1 signals. */
	
	if(interrupt_number == 1)
	{
	
		/* Open a file for reading and a file for writing. The reading file
		   is the stream that is being read from and the file name is provided
		   by the user. The writing file is written to the file name that is
		   also provided by the user. */
		   
		FILE *fp = fopen(file_to_read, "r");
		FILE *fw = fopen(output_file, "w");
		if(fp == NULL || fw == NULL)
		{
			printf("File could not be opened\n\n");
			return 1;
		}

		
		/* Reads a line every time it loops and stores it in the temp variable.
		   Then it writes it into the another file and sends a SIGUSR1 signal
		   to the DSMS to notify it that there is new data to read. It also
		   flushes when writing to make sure the data shows up right after
		   writing it. After the signal is sent, the program will pause for 
		   the user-defined time before resuming to another line. */
		   
		while (fgets(temp, 1000, fp)!=NULL)
		{
			fprintf(fw, "%s", temp);
			fflush(fw);

			if (kill(DSMS_process, SIGUSR1) != 0)
				printf("Error killing process\n");

			sleep(data_periodicity);
		}

		
		/* Closes both the writing and reading files opened before. */
		
		fclose(fp);
		fclose(fw);
	}
	
	
	/* If the user specifies that this is the second interrupt, then use
	   the SIGUSR2 signals. */
	
	if(interrupt_number == 2)
	{
	
		/* Open a file for reading and a file for writing. The reading file
		   is the stream that is being read from and the file name is provided
		   by the user. The writing file is written to the file name that is
		   also provided by the user. */
		   
		FILE *fp = fopen(file_to_read, "r");
		FILE *fw = fopen(output_file, "w");
		if(fp == NULL || fw == NULL)
		{
			printf("File could not be opened\n\n");
			return 1;
		}

		
		/* Reads a line every time it loops and stores it in the temp variable.
		   Then it writes it into the another file and sends a SIGUSR2 signal
		   to the DSMS to notify it that there is new data to read. It also
		   flushes when writing to make sure the data shows up right after
		   writing it. After the signal is sent, the program will pause for 
		   the user-defined time before resuming to another line. */
		   
		while (fgets(temp, 1000, fp)!=NULL)
		{
			fprintf(fw, "%s", temp);
			fflush(fw);

			if (kill(DSMS_process, SIGUSR2) != 0)
				printf("Error killing process\n");

			sleep(data_periodicity);
		}

		
		/* Closes both the writing and reading files opened before. */
		
		fclose(fp);
		fclose(fw);
	}
	return 1;
}
