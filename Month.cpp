#include <cstdlib>
#include <cstdio>
#include "Month.h"

Month::Month() : value(Invalid) {
}

Month::Month(std::string str) {
	Set(str);
}

void Month::Set(std::string str) {
	if(str=="Jan") {value = Jan; return;}
	if(str=="Feb") {value = Feb; return;}
	if(str=="Mar") {value = Mar; return;}

	
	if(str=="Apr") {value = Apr; return;}
	if(str=="May") {value = May; return;}
	if(str=="Jun") {value = Jun; return;}

	
	if(str=="Jul") {value = Jul; return;}
	if(str=="Aug") {value = Aug; return;}
	if(str=="Sep") {value = Sep; return;}

	
	if(str=="Oct") {value = Oct; return;}
	if(str=="Nov") {value = Nov; return;}
	if(str=="Dec") {value = Dec; return;}

	printf("Invalid month string: %s\n", str.c_str());
	exit(EXIT_FAILURE);
}
