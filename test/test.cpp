#include <stdio.h>
#include <stdlib.h>
#include "test2.h"

void call_func(int i){
	printf("function called: %d\n", i);
	if(i < 10) call_func(++i);
	printf("function ended\n");
}

int main(int argc, char** argv){
	printf("main start\n");
	call_func2(0);
	printf("main end\n");
}
