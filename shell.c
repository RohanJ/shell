/* MP3: Shell */
//|========================================
//|Program:				shell
//|File Name:			shell.c
//|Author:				Rohan Jyoti
//|Purpose:				CS241 Shell
//|________________________________________

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include "log.h"
#include <signal.h>

#ifndef shell_h
#define shell_h
//fucntions
void mDestroy();
int mTokenize(char *cmd, char **tokenList);
void print_mTokens(int numTokens);
void mDestroy_mTokens();
int decipher_mTokens(char **tokenList);
int exec_built_in();
int exec_external();
int parse_exec(char *inCMD);
//variables
log_t mLog;
char *cwd;
char *command;
char *mCMD;
char **mTokens;
int numTokens;
size_t command_buffer_size = 512;

int internal_cmd_type;
int origin = -1;


#define MAX_NUM_STRINGS 1024
#define MAX_STRING_LENGTH 2048

#define BUILT_IN_COMMAND 1
#define CMD_TYPE_CD 2
#define CMD_TYPE_EXIT 3
#define CMD_TYPE_LOG 4
#define CMD_TYPE_QUERY 5
#define EXTERNAL_COMMAND 6

#define CALL_FROM_EXEC_BUILT_IN 1
#endif


/*
 STATUS: Finished programming; Valgrind verified (NO MEMORY LEAKS)
 
 confirm whether or not in external command, do we add to log
 on a "Not valid" command
 
 Confirm if order of !# printing is correct
 
 */
int main()
{
	log_init(&mLog);
	size_t cwd_size = pathconf(".", _PC_PATH_MAX);
	char *cwd_buffer = (char *)malloc(cwd_size * sizeof(char));
	
	//shell prompt format: ￼￼(pid=x)/path/to/cwd$
	int running = 0;
	while(running == 0)
	{
		cwd = getcwd(cwd_buffer, cwd_size);
		printf("(pid=%d)%s$ ",getpid() , cwd);
		
		fflush(stdin);
		fflush(stdout);
		getline(&command, &command_buffer_size, stdin);
		command[strlen(command) -1] = '\0'; //strip \n from end of line
		
		
		mCMD = (char *)malloc((strlen(command) + 1) * sizeof(char));
		strcpy(mCMD, command);
		running = parse_exec(command);
	}
	mDestroy();
    return 0;
}

/*******************************************************************************
 * @author	: Rohan Jyoti
 * @name	: parse_exec
 * @param	: char *
 * @return	: int
 * @purpose	: main shell wrapper
 ******************************************************************************/
int parse_exec(char *inCMD)
{
	numTokens = mTokenize(inCMD, mTokens);
	//print_mTokens(numTokens);
	int cmd_type = decipher_mTokens(mTokens);
	
	if(internal_cmd_type != CMD_TYPE_LOG && internal_cmd_type != CMD_TYPE_QUERY)
		log_append(&mLog, mCMD); //log interaction
	
	if(origin == CALL_FROM_EXEC_BUILT_IN)
	{
		free(inCMD);
		origin = -1;
	}
	
	if(cmd_type == BUILT_IN_COMMAND) return exec_built_in();
	else if(cmd_type == EXTERNAL_COMMAND) return exec_external();
	else printf("There was a internal program error\n");
	return 0;
}

/*******************************************************************************
 * @author	: Rohan Jyoti
 * @name	: mTokenize
 * @param	: char *, char **
 * @return	: int
 * @purpose	: Tokenize input command and store into mTokens
 ******************************************************************************/
int mTokenize(char *cmd, char **tokenList)
{
	if(mTokens != tokenList)
	{
		printf("internal mTokenize error\n");
		return -1;
	}
	if(mTokens)
		free(mTokens);
	
	const char *delimiters = " ,"; //" ,-"
	char *temp = strtok(cmd, delimiters);
	mTokens = (char **)malloc(MAX_NUM_STRINGS*sizeof(MAX_STRING_LENGTH));
	//mTokens = (char **)malloc(strlen(cmd) * sizeof(strlen(cmd) +1));
	
	numTokens=0;
	while(temp != NULL)
	{
		mTokens[numTokens] = temp;
		numTokens++;
		temp = strtok(NULL, delimiters);
	}
	return numTokens;
}

/*******************************************************************************
 * @author	: Rohan Jyoti
 * @name	: decipher_mTokens
 * @param	: char **
 * @return	: int
 * @purpose	: Look at tokenList and determine type of call (internal/external)
 ******************************************************************************/
int decipher_mTokens(char **tokenList)
{
	char head[strlen(tokenList[0])]; 
	strcpy(head, tokenList[0]);
	int i;
	for(i=0; i<(int)strlen(head); i++)
		head[i] = tolower(head[i]);
	
	if(strcmp(head, "cd") == 0) internal_cmd_type = CMD_TYPE_CD;
	else if(strcmp(head, "exit") == 0) internal_cmd_type = CMD_TYPE_EXIT;
	else if(head[0] == '!')
	{
		if(strcmp(head, "!#") == 0) internal_cmd_type = CMD_TYPE_LOG;
		else internal_cmd_type = CMD_TYPE_QUERY;
	}
	else
	{
		internal_cmd_type = EXTERNAL_COMMAND;
		return EXTERNAL_COMMAND;
	}
	return BUILT_IN_COMMAND;
}

/*******************************************************************************
 * @author	: Rohan Jyoti
 * @name	: exec_built_in
 * @param	: none
 * @return	: int
 * @purpose	: executes the defined built in commands
 ******************************************************************************/
int exec_built_in()
{
	if(internal_cmd_type == CMD_TYPE_CD)
	{
		if(numTokens >= 2)
		{
			int ret = chdir(mTokens[1]);
			if(ret != 0)
				printf("%s: No such file or directory\n", mTokens[1]);
		}
		else printf("invalid use of cd\n");
	}
	else if(internal_cmd_type == CMD_TYPE_EXIT) return -1;
	else if(internal_cmd_type == CMD_TYPE_LOG) 
	{
		int i;
		for(i=0 ; i<(int)log_size(&mLog); i++) printf("%s\n", log_at(&mLog, i));
		free(mCMD);
	}
	else if(internal_cmd_type == CMD_TYPE_QUERY)
	{
		int i,j, match;
		char *log_entry;
		for(i=log_size(&mLog) -1; i>=0; i--)
		{
			log_entry = log_at(&mLog, i);
			match = 1;
			for(j=1; j<(int)strlen(mCMD); j++)
			{
				if(mCMD[j] != log_entry[j-1])
				{
					match=-1;
					break;
				}
			}
			if(match == 1) break;
		}
		if(match==1)
		{
			printf("%s matches %s\n", &mCMD[1], log_entry);
			free(mCMD);
			mCMD = (char *)malloc(strlen(log_entry) +1);
			char *forTokenizer = (char *)malloc(strlen(log_entry) +1);
			strcpy(forTokenizer, log_entry);
			strcpy(mCMD, log_entry);
			origin = CALL_FROM_EXEC_BUILT_IN;
			parse_exec(forTokenizer);
		}
		else
		{
			free(mCMD);
			printf("No Match\n");
		}
	}
	else printf("Incorrectly interpreted as internal\n");
	return 0;
}

/*******************************************************************************
 * @author	: Rohan Jyoti
 * @name	: exec_external
 * @param	: none
 * @return	: int
 * @purpose	: executes external commands
 ******************************************************************************/
int exec_external()
{
	system(mCMD);
	return 0;
}

void print_mTokens(int numTokens)
{
	int i;
	for(i=0; i<numTokens; i++)
		printf("mTokens[%d]: %s\n", i, mTokens[i]);
}

void mDestroy()
{
	if(cwd) 
	{ 
		free(cwd);
		cwd = NULL;
	}
	if(command) 
	{
		free(command);
		command = NULL;
	}
	// DO NOT FREE mCMD here cuz log_destroy should take care of that 
	mDestroy_mTokens();
	log_destroy(&mLog);
}

void mDestroy_mTokens()
{
	if(mTokens)
	{
		free(mTokens);
		mTokens = NULL;
	}
	numTokens = 0;
}


