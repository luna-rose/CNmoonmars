#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "Month.h"

Month::Month() : value(Invalid) {
}

Month::Month(char* str) {
	Set(str);
}

void Month::Set(char* str) {
	if(strcmp(str, "Jan")==0) {value = Jan; return;}
	if(strcmp(str, "Feb")==0) {value = Feb; return;}
	if(strcmp(str, "Mar")==0) {value = Mar; return;}

	
	if(strcmp(str, "Apr")==0) {value = Apr; return;}
	if(strcmp(str, "May")==0) {value = May; return;}
	if(strcmp(str, "Jun")==0) {value = Jun; return;}

	
	if(strcmp(str, "Jul")==0) {value = Jul; return;}
	if(strcmp(str, "Aug")==0) {value = Aug; return;}
	if(strcmp(str, "Sep")==0) {value = Sep; return;}

	
	if(strcmp(str, "Oct")==0) {value = Oct; return;}
	if(strcmp(str, "Nov")==0) {value = Nov; return;}
	if(strcmp(str, "Dec")==0) {value = Dec; return;}

	printf("Invalid month string: %s\n", str);
	exit(EXIT_FAILURE);
}