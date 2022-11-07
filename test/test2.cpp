#include <stdio.h>

void call_func2(int i){
	printf("function2 called: %d\n", i);
	if(i < 10) call_func2(++i);
	printf("function2 ended\n");
}
