#include <stdio.h>
#include <stdlib.h>
#include "test1.h"

void call_func(int i){
	printf("%d-th call_func() called\n", i);
	if(i < 5) call_func(++i);
}

int main(int argc, char** argv){
	printf("main() started.\n");
	call_func(0);
	call_func1(0);
	printf("main() ended. Try typing anything. If you type 'escape', the process ends\n");
}
