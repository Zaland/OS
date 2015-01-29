#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "sqlite3.h"

/* Global variables required by functions to run efficiently. These
   variables include the config file that holds all the information
   for the other 5 variables. */

char *config_file;
char db_filename[255];
char init_filename[255];
char usr1_filename[255];
char usr2_filename[255];
int clean;
int sig1 = 0;
int sig2 = 0;


/* Reads the config file and replaces all previously declared variables.
   Can be used again to reread the config file and replaces all the
   previously defined variables */

void readConfig()
{
	char temp[255];
	char c;

	FILE *fp = fopen(config_file, "r");
	if(fp == NULL)
		printf("File could not be opened\n\n");

	/* Loops through each individual lines and records each value into
	   own respective variables. If it discovers that init is empty,
	   then proceeds to leave that variable empty. Otherwise, it proceeds
	   to store the value in its own variable and continues storing the 
	   other file names in their respective variables. */
	 
	while ((c = fscanf(fp, "%s", temp)) != EOF)
	{
		if(strcmp(temp, "db") == 0)
			c = fscanf(fp, "%s", db_filename);
		else if(strcmp(temp, "init") == 0)
		{
			c = fscanf(fp, "%s", init_filename);
			if(strcmp(init_filename, "usr1") == 0)
			{
				c = fscanf(fp, "%s", usr1_filename);
				init_filename[0] = '\0';
			}
			else
			{
				c = fscanf(fp, "%s", usr1_filename);
				c = fscanf(fp, "%s", usr1_filename);
			}
		}
		else if(strcmp(temp, "usr2") == 0)
			c = fscanf(fp, "%s", usr2_filename);
		else if(strcmp(temp, "clean") == 0)
		{
			c = fscanf(fp, "%s", temp);
			clean = atoi(temp);
		}
    /* testing purposes
    printf(" %d -> %s | %s | %s | %s | %s | %d\n", a, temp, db_filename, init_filename, usr1_filename, usr2_filename, clean);
    sleep(1);*/
	}
	fclose(fp);
}


/* Handles SIGUSR1 and SIGUSR2 and notifies this program that there is data to be read
   from a file located within the config file. It updates the value of 
   and integer that keeps track of how many lines to read. In case the 
   scanner keeps running while signals keep comming, the number will continue
   to add and then a function will read all the lines that need to be read.

   Handles SIGHUP and notifies the program that the config file should be
   reread. It activates the function to read the file and will replace
   all the variables that already exist. */
   
void sigHandler(int the_signal)
{
	if(the_signal == SIGUSR1)
		sig1++;
	if(the_signal == SIGUSR2)	
		sig2++;
	if(the_signal == SIGHUP)
		readConfig();
}


/* Displays the data from the database in a certain format */

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i=0; i<argc; i++)
	{
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}


/* Function timer where a timer is running. After every user-defined amount
   of seconds, the timer will purge the sql database of all data. Then it 
   will reset the timer and continue in this manner, until the program ends.
   This program is constantly invoked by SIGALRM. */
   
void timer(int the_signal)
{	
	static int clock = 1;
	clock++;
	
	char query1[255] = "delete from price";
	char query2[255] = "delete from news";
	char *zErrMsg;
	if(clock == clean)
	{
		/* Sets up connection with database and confirms that there are
		   no errors. */
		   
		sqlite3 *database;
		int rc;
		rc = sqlite3_open(db_filename, &database);
		if(rc)
		{
			printf("Could not open database.");
			sqlite3_close(database);
		}
		
		
		/* Runs both queries to delete both price and news with all 
		   their data in them. These queries will be run every time
		   the timer reaches the desired time to delete. */
		   
		rc = sqlite3_exec(database, query1, callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sleep(1);
		
		rc = sqlite3_exec(database, query2, callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sleep(1);
		
		
		/* Closes the database and resets the timer to let the timer run
		   again until the program stops. */
		   
		sqlite3_close(database);
		clock -= clean;
	}
	
	alarm( 1 );
}


/* Reads new data to be read from files specified in the config file. It will
   record this data within its database. This function has three parameters; 
   the file pointers are parameters for reading the file that has the data. 
   Conveniently these files are already open and only need to be read from.
   The third parameter is the database connection allowing for addition to 
   database to be done with ease. This function from the two user files 
   and insert these data accordingly into the database. */
   
void readData(FILE *file1, FILE *file2, sqlite3 *database)
{
	char temp1[255], temp2[255];
	int rc;
	double temp;
	char *zErrMsg;
	
	
	/* Reads data on the request of SIGUSR1. */
	
	while(sig1 != 0)
	{
		/* Scans the words and numbers from the file when they are ready and
		   prepares a query with these values. */
		   
		char query1[500];
		fscanf(file1, "%s", temp1);
		fscanf(file1, "%lf", &temp);
		snprintf(query1, sizeof query1, "insert into price values (NULL, \"%s\", %lf)", temp1, temp);
		
		
		/* Runs the query and confirms whether it works. Minimize the value
		   of sig1 and keep repeating until sig1 = 0. */
		
		rc = sqlite3_exec(database, query1, callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sig1--;
	}
	
	
	/* Reads data on the request of SIGUSR2. */
	
	while(sig2 != 0)
	{
		/* Scans the words and numbers from the file when they are ready and
		   prepares a query with these values. */
		   
		char query2[500];
		fscanf(file2, "%s", temp1);
		fscanf(file2, "%s", temp2);
		snprintf(query2, sizeof query2, "insert into news values (NULL, \"%s\", \"%s\")", temp1, temp2);
		
		
		/* Runs the query and confirms whether it works. Minimize the value
		   of sig2 and keep repeating until sig2 = 0. */
		
		rc = sqlite3_exec(database, query2, callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sig2--;
	}
}

int main(int argc, char *argv[])
{
	/* Grabs the process ID of this process and prints it to the screen
	   for the user to use for the stream programs. */
	
	int process_id;
	process_id = (int)getpid();
	printf("Process ID: %d\n\n", process_id);

	
	/* Checks to see if there are correct number of arguments provided
	   when initiating the program. Otherwise the program will display
	   a reference to what arguments are required for the program to run
	   and then closes the program. */
	   
	if(argc != 2)
	{
		printf("Incorrect amount of arguments. Please try again.\n");
		printf("'DSMS config_file'\n\n");
		return 1;
	}

	
	/* Stores the file name of the config file obtained from the argument
	   provided by the user into a global variable. After the file name is
	   stored, read the file and store the appropriate information into their
	   respective variables to be used in the future. */
	   
	config_file = argv[1];
	readConfig();
	
	
	/* Opens both files and awaits signals to start reading them both. */
	
	FILE *fp1 = fopen(usr1_filename, "r");
	FILE *fp2 = fopen(usr2_filename, "r");
	if(fp1 == NULL || fp2 == NULL)
	{
		printf("File could not be opened\n\n");
		return 1;
	}
	
	
	/* Prepares the database for use by the init file and the user commands
       to follow. Also confirms if the database has been created, otherwise
	   notify the user that the database has failed in opening and end the
	   program. */
	   
	sqlite3 *db;
	int rc;
	rc = sqlite3_open(db_filename, &db);
	if(rc)
	{
		printf("Could not open database.");
		sqlite3_close(db);
	}
	
	
	/* If there is an init file provided in the config file, then proceed to
	   run commands from that file, then proceed to accept user input. If no 
	   file is provided, then proceed to accepting user input right away. */ 
	
	char temp[255];
	char *zErrMsg = 0;
	if(init_filename[0] != '\0')
	{
		FILE *fp = fopen(init_filename, "r");
		if(fp == NULL)
		{
			printf("Init file could not be opened\n\n");
			return 1;
		}
		while (fgets(temp, 1000, fp)!=NULL)
		{
			rc = sqlite3_exec(db, temp, callback, 0, &zErrMsg);
			if(rc != SQLITE_OK)
			{
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			sleep(1);
		}
		fclose(fp);
	}
	   

	/* Installs SIGUSR1, SIGUSR2, and SIGHUP handlers and to respond to their
	   respective functions when invoked. It also installs the SIGALARM handler
	   for the timer part of the program. If any of them fail, the user will
	   be notified of the specific failed handler. */
	   
	if(signal(SIGUSR1, sigHandler) == SIG_ERR)
		printf("Unable to install SIGUSR1 handler\n");
		
	if(signal(SIGUSR2, sigHandler) == SIG_ERR)
		printf("Unable to install SIGUSR2 handler\n");
		
	if(signal(SIGHUP, sigHandler) == SIG_ERR)
		printf("Unable to install SIGHUP handler\n");

	if(signal(SIGALRM, timer) == SIG_ERR)
		printf("Unable to install SIGALRM handler\n");
	
	
	/* Runs an infinite loop while waiting for signals and user input 
	   before resuming its functions. Whenever loop restarts, the loop
	   checks if their is new data to be read, and reads it before 
	   continuing to accept user input. After, the infinite loop will end 
	   and the two files that were opened earlier are closed. Also closes
	   the database as it is not required any more. Also initiates the 
	   start of the timer. */
	   
	alarm(1);
	while(1)
	{
		if(sig1 != 0 || sig2 != 0)
			readData(fp1, fp2, db);
		scanf("%s", &temp);
		fgets(temp, sizeof temp, stdin);
		rc = sqlite3_exec(db, temp, callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sleep(1);
	}
	
	fclose(fp1);
	fclose(fp2);
	sqlite3_close(db);
}
