#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "sqlite3.h"

/* Testing lines
every 10 format key-pairs to test.txt report select * from stock
every 10 format csv to test1.txt report select * from stock
every 10 format fixed-width to test2.txt report select * from stock */
	

/* Length of strings. */

#define STR_LEN (512)


/* Global signal values to notify the DSMS that there is data to be read. 
   There is also a clock for the threads to use to complete their tasks. 
   A global declaration of the database is also located here for use by
   all the functions. */

int sig1 = 0;
int sig2 = 0;
int id_counter = 1;
int global_clock = 1;
int delete_id = 0;
sqlite3 *db;


/* Structure for holding configuration file information. Also declares
   a structure for use globally. */

typedef struct
{
	char *config_file;
	char db_filename[STR_LEN];
	char init_filename[STR_LEN];
	char usr1_filename[STR_LEN];
	char usr2_filename[STR_LEN];
	int clean;
} config_args;
config_args config;


/* Structure for holding the data for the continuous query. */

typedef struct
{
	int id;
	int time;
	char format[STR_LEN];
	char output[STR_LEN];
	char query[STR_LEN];
	FILE *cw;
} con_args;


/* Reads the config file and replaces all previously declared variables.
   Can be used again to reread the config file and replaces all the
   previously defined variables */

void readConfig()
{
	char temp[STR_LEN];
	char c;

	FILE *fp = fopen(config.config_file, "r");
	if(fp == NULL)
		printf("File could not be opened\n\n");

		
	/* Loops through each individual lines and records each value into
	   its own respective variables. If it discovers that init is empty,
	   then proceeds to leave that variable empty. Otherwise, it proceeds
	   to store the value in its own variable and continues storing the 
	   other file names in their respective variables. */
	 
	while ((c = fscanf(fp, "%s", temp)) != EOF)
	{
		if(strcmp(temp, "db") == 0)
			c = fscanf(fp, "%s", config.db_filename);
		else if(strcmp(temp, "init") == 0)
		{
			c = fscanf(fp, "%s", config.init_filename);
			if(strcmp(config.init_filename, "usr1") == 0)
			{
				c = fscanf(fp, "%s", config.usr1_filename);
				config.init_filename[0] = '\0';
			}
			else
			{
				c = fscanf(fp, "%s", config.usr1_filename);
				c = fscanf(fp, "%s", config.usr1_filename);
			}
		}
		else if(strcmp(temp, "usr2") == 0)
			c = fscanf(fp, "%s", config.usr2_filename);
		else if(strcmp(temp, "clean") == 0)
		{
			c = fscanf(fp, "%s", temp);
			config.clean = atoi(temp);
		}
    /* testing purposes
    printf(" %d -> %s | %s | %s | %s | %s | %d\n", a, temp, db_fiSTR_LEName, init_fiSTR_LEName, usr1_fiSTR_LEName, usr2_fiSTR_LEName, clean);
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


/* Displays the data from the database in a certain format. With the use of
   continuous query, the data here is formatted in a specific format specified
   by the user. */

static int callback(void *a, int argc, char **argv, char **azColName)
{
	/* If the user simply runs a statement, the execution statement passes along
	   a 0 as the data argument. Therefore confirm if the user is simply asking
	   for data to be printed. Otherwise continue on to printing data into a file. */
	   
	if((int*)a == 0)
	{
		int i;
		for(i=0; i<argc; i++)
		{
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}
		printf("\n");
		return 0;
	}
	

	/* This part of the program is invoked by the formatter thread to format the 
	   data into a specific format and print it into a file. */
	
	con_args *data = (con_args *)a;
	int i;
	
	
	/* Prints the SQL data in csv format. Print each field of one record on the 
	   same output line and seperate field values with commas ("comma separated
	   values" format). Enclose strings in quotation marks. */
	   
	if(strcmp(data->format, "csv") == 0)
	{
		for(i = 0; i < argc; i++)
		{
			if(i != 0)
				fprintf(data->cw, ", ");
			char temp[STR_LEN];
			strcpy(temp, argv[i] ? argv[i] : "NULL");
			if(atoi(temp) == 0)
				fprintf(data->cw, "\"%s\"", temp);
			else
				fprintf(data->cw, "%s", temp);
			fflush(data->cw);
		}
		fprintf(data->cw, "\n");
		return 0;
	}
	
	
	/* Prints the SQL data in fixed-width format. Print each field of one record on
	   the same output line. Each field should use exactly 15 columns of output. Pad
	   entries that are too short with spaces on the right. Truncate entries that
	   are longer than 15 columns. Seperate each column with a space. */
	   
	else if(strcmp(data->format, "fixed-width") == 0)
	{
		for(i = 0; i < argc; i++)
		{
			char temp[STR_LEN];
			strncpy(temp, argv[i] ? argv[i] : "NULL", 15);
			fprintf(data->cw, "%-15s ", temp);
			fflush(data->cw);
		}
		fprintf(data->cw, "\n");
		return 0;
	}
	
	
	/* Prints the SQL data in key-pairs format. Print each field of one record on
       its own line in the format "column_name = value". */
	   
	else if(strcmp(data->format, "key-pairs") == 0)
	{
		for(i = 0; i < argc; i++)
		{
			fprintf(data->cw, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
			fflush(data->cw);
		}
		fprintf(data->cw, "\n");
		return 0;
	}
}


/* Function timer where a timer is running. After every user-defined amount
   of seconds, the timer will purge the sql database of all data. Then it 
   will reset the timer and continue in this manner, until the program ends.
   This program is constantly invoked by SIGALRM. */
   
void timer(int the_signal)
{	
	static int clock = 1;
	clock++;
	global_clock++;
	
	char query1[STR_LEN] = "delete from price";
	char query2[STR_LEN] = "delete from news";
	char *zErrMsg;
	int rc;
	
	if(clock == config.clean)
	{
		/* Runs both queries to delete both price and news with all 
		   their data in them. These queries will be run every time
		   the timer reaches the desired time to delete. */
		   
		rc = sqlite3_exec(db, query1, callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sleep(1);
		
		rc = sqlite3_exec(db, query2, callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sleep(1);
		
		
		/* Resets the timer to let the timer run again until the program
           stops. */
		   
		clock -= config.clean;
		
		
		/* If global clock reaches 30000, reset it to zero to prevent any
		   other errors. */
		  
		if(global_clock == 30000)
			global_clock = 0;
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
	char temp1[STR_LEN], temp2[STR_LEN];
	int rc;
	double temp;
	char *zErrMsg;
	
	
	/* Reads data on the request of SIGUSR1. */
	
	while(sig1 != 0)
	{
		/* Scans the words and numbers from the file when they are ready and
		   prepares a query with these values. */
		   
		char query1[STR_LEN];
		fscanf(file1, "%s", temp1);
		fscanf(file1, "%lf", &temp);
		snprintf(query1, sizeof query1, "insert into price values (NULL, \"%s\", %lf)", temp1, temp);
		
		
		/* Runs the query and confirms whether it works. subtract the value
		   of sig1 by 1 and keep repeating until sig1 = 0. */
		
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
		   
		char query2[STR_LEN];
		fscanf(file2, "%s", temp1);
		fscanf(file2, "%s", temp2);
		snprintf(query2, sizeof query2, "insert into news values (NULL, \"%s\", \"%s\")", temp1, temp2);
		
		
		/* Runs the query and confirms whether it works. subtract the value
		   of sig2 by 1 and keep repeating until sig2 = 0. */
		
		rc = sqlite3_exec(database, query2, callback, 0, &zErrMsg);
		if(rc != SQLITE_OK)
		{
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sig2--;
	}
}


/* Another thread invoked by the continuous query when its time to write
   data. This thread only runs once and then quits. This function executes
   the SQL query and from there the callback function prints the data in the
   appropriate formate. */

void *formatQuery(void *a)
{
	con_args *data = (con_args *)a;
	sqlite3 *database;
	char *zErrMsg;
	int rc;
	
	
	/* Executes the query and passes along the file pointer to write data
	   to. After the query is complete, add 20 stars to the bottom of the
	   file to signal the end of a query, then close the file. */
	   
	data->cw = fopen(data->output, "a");
	rc = sqlite3_exec(db, data->query, callback, (void*)data, &zErrMsg);
	if(rc != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	
	
	/* Prints the end of the query in a specific format for each individual
	   format. */
	   
	if(strcmp(data->format, "csv") == 0)
		fprintf(data->cw, "\n********************\n\n");
	else if(strcmp(data->format, "fixed-width") == 0)
		fprintf(data->cw, "\n********************\n\n");
	else if(strcmp(data->format, "key-pairs") == 0)
		fprintf(data->cw, "********************\n\n");
		
	fflush(data->cw);
	fclose(data->cw);
	return NULL;
}


/* Is a thread invoked by the user that runs in an infinite loop until 
   told to stop by the user. This thread is responsible for keeping
   track of when to invoke the formatter thread. After invoking the 
   formatter thread, this thread will wait till data has been written
   before continuing. */

void *runConQuery(void *a)
{
	/* Sets up the structure and declares the variables. */
		   
	con_args *data = (con_args *)a;
	int pause = 0, local_clock = 0, temp = global_clock, time = data->time;


	/* Continuous loop until the user notifies to end this continuous
       query, hence the infinite loop. It also keeps track of when to
	   run the formatter function. It runs the formatter function once
	   every time the timer reaches the user defined value. */
	
	printf("Thread %d created.\n\n", data->id);
	while(1)
	{
		/* Checks the value of delete_id and confirms if it matches this
		   thread's id. If it matches, then this thread will terminate 
		   and reset the value of delete_id to 0. */
		   
		if(delete_id == data->id)
		{
			delete_id = 0;
			break;
		}
		   
		/* If the value of the global_clock changes, then update the value
		   of the local_clock as well. Also unpauses the program and allows
		   it to continue. */
		   
		if(temp != global_clock)
		{
			local_clock++;
			temp = global_clock;
			pause = 0;
		}
		
		
		/* If the local_clock has reached the desired time and the program is
		   not on pause. Then notify the formatter to write the data into the
		   file. Afterwards, reset the value of local_clock. */
		   
		if(local_clock == time && pause == 0)
		{
			pause = 1;
			local_clock = 0;
			
			
			/* Creates a thread to run the formatter function and passing the
			   continuous query structure to the thread. It will then wait 
			   till the formatter function is completed before resuming back 
			   to its old state. */
			   
			pthread_t pth;
			pthread_attr_t attrib;  
			pthread_attr_init(&attrib);                                           
			pthread_attr_setscope(&attrib, PTHREAD_SCOPE_SYSTEM);
			pthread_create(&pth, &attrib, formatQuery, data);
			pthread_join(pth, NULL);
		}
		
	}
	printf("Thread %d stopped\n\n", data->id);
	return NULL;
} 


/* This function is invoked by the main function when a continuous query
   has been detected. This function is responsible for reading the commands
   the user has provided and runs the query by invoking another function. */

int readConQuery()
{
	/* Declares the structure and gives it some memory so that the other 
       threads can use this structure. Otherwise there are some scope errors. */
	
	con_args *a = malloc(sizeof(con_args));
	
	
	/* Starts reading the time to wait between executing SQL queries. If the command
	   structure is inaccurate, then return a 0; incorrect command structure. */
	   
	char temp[STR_LEN];
	scanf("%s", &temp);
	a->time = atoi(temp);
	
	
	/* We need to confirm that the format choice is valid and is one of the three
	   formates: csv, fixed-width, key-pairs. If it's an incorrect format choice, 
	   return a 1; incorrect format type. */
	   
	scanf("%s", &temp);
	if(strcmp(temp, "format") != 0)
		return 0;
	   
	scanf("%s", &a->format);
	if(strcmp(a->format, "csv") != 0 
	&& strcmp(a->format, "fixed-width") != 0
	&& strcmp(a->format, "key-pairs") != 0)
		return 1;
	
	scanf("%s", &temp);
	if(strcmp(temp, "to") != 0)
		return 0;
	scanf("%s", &a->output);
	
	scanf("%s", &temp);
	if(strcmp(temp, "report") != 0)
		return 0;
	scanf(" ", &temp); // removes the extra space before the query
	fgets(a->query, sizeof a->query, stdin);
	
	
	/* Updates the value of the id of the thread to keep track of its id.
	   Creates the thread which invokes the function that runs the continuous
	   query. And this thread passes along the continuous query structure for
	   the thread to use. */
	  
	a->id = id_counter;
	id_counter++;
	
	pthread_t pth;
	pthread_attr_t attrib;  
	pthread_attr_init(&attrib);                                           
	pthread_attr_setscope(&attrib, PTHREAD_SCOPE_SYSTEM);
	pthread_create(&pth, &attrib, runConQuery, a);
	return 2;
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
	   
	config.config_file = argv[1];
	readConfig();
	
	
	/* Opens both files and awaits signals to start reading them both. */
	
	FILE *fp1 = fopen(config.usr1_filename, "r");
	FILE *fp2 = fopen(config.usr2_filename, "r");
	if(fp1 == NULL || fp2 == NULL)
	{
		printf("File could not be opened\n\n");
		return 1;
	}
	
	
	/* Prepares the database for use by the init file and the user commands
       to follow. Also confirms if the database has been created, otherwise
	   notify the user that the database has failed in opening and end the
	   program. */
	   
	int rc;
	rc = sqlite3_open(config.db_filename, &db);
	if(rc)
	{
		printf("Could not open database.");
		sqlite3_close(db);
	}
	
	
	/* If there is an init file provided in the config file, then proceed to
	   run commands from that file, then proceed to accept user input. If no 
	   file is provided, then proceed to accepting user input right away. */ 
	
	char temp[STR_LEN];
	char *zErrMsg = 0;
	if(config.init_filename[0] != '\0')
	{
		FILE *fp = fopen(config.init_filename, "r");
		if(fp == NULL)
		{
			printf("Init file could not be opened\n\n");
			return 1;
		}
		while (fgets(temp, sizeof temp, fp)!=NULL)
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
	   continuing to accept user input. After, the infinite loop ends 
	   and the two files that were opened earlier are closed. Also closes
	   the database as it is not required any more and initiates the 
	   start of the timer. */
	   
	alarm(1);
	while(1)
	{
		/* If there is data to be read, read the data and then continue. */
		
		scanf("%s", &temp);
		if(sig1 != 0 || sig2 != 0)
			readData(fp1, fp2, db);
		
		
		/* If the user inputs exit, then exist the program by breaking the
		   loop and proceeding with closing the files and SQL database. */
		   
		if(strcmp(temp, "exit") == 0)
			break;
		
		
		/* If the user inputs starts with every, proceed to run the conQuery
		   function that deals with reading the rest of the command and runs
		   the continuous query. If a 0 is returned by the function, then there
		   was an error, and inform the user that there was an error. */
		   
		else if(strcmp(temp, "every") == 0)
		{
			int pr = readConQuery();
			if(pr == 0)
			{
				printf("Incorrect command. Please try again.\n\n");
				fgets(temp, sizeof temp, stdin);
			}
			else if(pr == 1)
			{
				printf("Incorrect format type. Please try again.\n\n");
				fgets(temp, sizeof temp, stdin);
			}
		}			
		
		
		/* If the user inputs a SQL query, then run that query. If it's an
		   incorrect query, then notify the user. */
		
		else if(strcmp(temp, "exec") == 0)
		{
			fgets(temp, sizeof temp, stdin);
			rc = sqlite3_exec(db, temp, callback, 0, &zErrMsg);
			if(rc != SQLITE_OK)
			{
				fprintf(stderr, "SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			sleep(1);
		}
		
		
		/* Updates the value of delete_id, which the threads constantly check.
		   If the thread matches the id number specified to stop. Then the
		   thread will terminate itself. */
		   
		else if(strcmp(temp, "stop") == 0)
		{
			scanf("%d", &delete_id);
		}
		
		
		/* Unrecognised command, notifies the user that its unrecognised. */
		
		else
			printf("Unidentified command. Please try again.\n\n");
	}
	
	fclose(fp1);
	fclose(fp2);
	sqlite3_close(db);
}
