#include <stdio.h>

void call_func1(int i){
	printf("%d-th call_func1() called\n", i);
	if(i < 10) call_func1(++i);
}
