/** @file testlog.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

int main()
{
	log_t mLog;
	log_init(&mLog);
	
	
	log_append(&mLog, "item1");
	log_append(&mLog, "item2");
	log_append(&mLog, "item3");
	
	char *tempy = (char *)malloc(10);
	strcpy(tempy, "hello");
	char tempy1[strlen(tempy)];
	strcpy(tempy1, tempy);
	log_append(&mLog, tempy1);
	
	
	strcpy(tempy, "world");
	char tempy2[strlen(tempy)];
	strcpy(tempy2, tempy);
	log_append(&mLog, tempy2);
	
	/*
	 char *search_ret = log_search(&mLog, "item2");
	 printf("search_ret: %s\n", search_ret);
	 
	 char *at_ret = log_at(&mLog, 4);
	 printf("at_ret: %s\n", at_ret);
	 char *at_ret2 = log_at(&mLog, 2);
	 printf("at_ret2: %s\n", at_ret2);
	 
	 char *pop_ret = log_pop(&mLog);
	 printf("pop_ret: %s\n", pop_ret);
	 */
	
	int i;
	for(i = 0; i<(int)log_size(&mLog); i++)
		printf("%s\n", log_at(&mLog, i));
	
	log_destroy(&mLog);
	free(tempy);
	
    return 0;
}
