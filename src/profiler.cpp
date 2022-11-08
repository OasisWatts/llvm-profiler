#include <stdio.h>
#include <time.h>
#include <map>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "papi.h"
#include "profiler.h"

//#define PROFILER_DIR /home/oasis/profiler
#define STR(s) #s
#define STR2(s) STR(s)
//#define DEBUG

#ifdef DEBUG
#define d_printf(...) printf(__VA_ARGS__)
#else
#define d_printf(...)
#endif

#define MAXSTR 256

using namespace std;

int papi_event_num = 2;
int call_cnt = 0;
int papi_event = PAPI_NULL;
bool inited = false;
time_t begin_time;
char folder_name[100];
int record_num = 0;

/** function execution count */
map<char*, int> CNT = {};
/** total taken time during execution */
map<char*, long> TOTAL_TIME = {};
/** average taken time during execution */
map<char*, float> AVG_TIME = {};
/** tlb data misses (PAPI_TLB_DM) */
map<char*, long long> TOTAL_TLB_DM = {};
/** tlb instruction misses (PAPI_TLB_IM) */
map<char*, long long> TOTAL_TLB_IM = {};
/** Level 1 data cache access (PAPI_L1_DCA)*/
//map<char*, long long> TOTAL_L1_DATA_A = {};
//map<char*, long long> AVG_L1_DATA_A = {};
/** Level 1 data cache missesss (PAPI_L1_DCM)*/
//map<char*, long long> TOTAL_L1_DATA_M = {};
//map<char*, long long> AVG_L1_DATA_M = {};
/** Level 2 data cache access (PAPI_L2_DCA)*/
//map<char*, long long> TOTAL_L2_DATA_A = {};
//map<char*, long long> AVG_L2_DATA_A = {};
/** Level 2 data cache missesss (PAPI_L2_DCM)*/
//map<char*, long long> TOTAL_L2_DATA_M = {};
//map<char*, long long> AVG_L2_DATA_M = {};
/** Level 3 data cache access (PAPI_L3_DCA)*/
//map<char*, long long> TOTAL_L3_DATA_A = {};
//map<char*, long long> AVG_L3_DATA_A = {};
/** Level 3 data cache missesss (PAPI_L3_DCM)*/
//map<char*, long long> TOTAL_L3_DATA_M = {};
//map<char*, long long> AVG_L3_DATA_M = {};

// map utils
template <typename T> static const char* out_format = "";
template <> const char* out_format<int> = "%d";
template <> const char* out_format<long> = "%ld";
template <> const char* out_format<long long> = "%lld";
template <> const char* out_format<float> = "%f";

template <typename T>
void print_map(map<char*, T> m){
	for (typename map<char*, T>::iterator itr = m.begin(); itr != m.end(); ++itr){
		printf("%s : ", itr->first);
		printf(out_format<T>, itr->second);
		printf("\n");
	}
}

template <typename T>
void push_map(map<char*, T> &m, char* name, T add){
	//d_printf("push_map %s : ", name);
	//d_printf(out_format<T>, m[name]);
	//d_printf(" cnt %d\n", m.count(name));
	if(m.count(name) < 1) m[name] = add;
	else m[name]+= add;
}

void handle_error(char* msg, int err = -1){
	fprintf(stderr, "%s, err no: %d\n", msg, err);
	exit(1);
}

/* timer */
long insert_timer_into_function(){
	struct timespec fun_start;
	clock_gettime(CLOCK_MONOTONIC, &fun_start);
	//d_printf("timer func\n");
	return fun_start.tv_sec*1000000000 + fun_start.tv_nsec;
}

long long* insert_pmu_into_function(char* func_name){
	if(!inited) return NULL;
	long long* papi_start_value = (long long*)malloc(sizeof(long long)*papi_event_num);
	if (PAPI_read(papi_event, papi_start_value) != PAPI_OK)
		fprintf(stderr, "papi read err %s\n", func_name);
	//d_printf("papi start value %lld %lld\n", papi_start_value[0], papi_start_value[1]);
	return papi_start_value;
}

/* save record per function */
void insert_back_into_function(char* func_name, long start_time, long long* papi_start_value){
	if(!inited) return;
	long long* papi_end_value = (long long*)malloc(sizeof(long long)*papi_event_num);
	if (PAPI_read(papi_event, papi_end_value) != PAPI_OK)
		fprintf(stderr, "papi read err %s\n", func_name);
	struct timespec fun_end;
	clock_gettime(CLOCK_MONOTONIC, &fun_end);
	long time = fun_end.tv_sec*1000000000 + fun_end.tv_nsec - start_time;
	push_map(TOTAL_TIME, func_name, time);
	push_map(CNT, func_name, 1);
	d_printf("cnt %s %d \n", func_name, CNT[func_name]);
	push_map(TOTAL_TLB_DM, func_name, papi_end_value[0] - papi_start_value[0]);
	push_map(TOTAL_TLB_IM, func_name, papi_end_value[1] - papi_start_value[1]);
	call_cnt++;

	//d_printf("record func %s: %ld\n", func_name, time);

	free(papi_start_value);
	free(papi_end_value);
}

void init(){
	int retval = PAPI_library_init(PAPI_VER_CURRENT);

	if(retval != PAPI_VER_CURRENT && retval > 0)
		handle_error((char*)"papi library init err");

	if (retval < 0)
	 	handle_error((char*)"papi initialize err");

	if (PAPI_create_eventset(&papi_event) < 0)
	 	handle_error((char*)"papi create eventset err");
	
	int papi_event_code[2] = {PAPI_TLB_DM, PAPI_TLB_IM};
	if ((retval = PAPI_add_events(papi_event, papi_event_code, 2)) < 0)
	 	handle_error((char*)"papi add event err", retval);
	
/*	if ((retval = PAPI_add_event(papi_event, PAPI_L1_DCH)) < 0) // LDM -ㅇ
	 	handle_error((char*)"papi add event l1_dca err", retval);
	if (PAPI_add_event(papi_event, PAPI_L1_DCA) != PAPI_OK)
	 	handle_error((char*)"papi add event l1_dca err");
	if (PAPI_add_event(papi_event, PAPI_L1_DCM) != PAPI_OK)
	 	handle_error((char*)"papi add event l1_dcm err");
	if (PAPI_add_event(papi_event, PAPI_L2_DCA) != PAPI_OK)
	 	handle_error((char*)"papi add event l2_dca err");
	if (PAPI_add_event(papi_event, PAPI_L2_DCM) != PAPI_OK)
	 	handle_error((char*)"papi add event l2_dcm err");
	if (PAPI_add_event(papi_event, PAPI_L3_DCA) != PAPI_OK)
	 	handle_error((char*)"papi add event l3_dca err");
	if (PAPI_add_event(papi_event, PAPI_L3_DCM) != PAPI_OK)
	 	handle_error((char*)"papi add event l3_dcm err");
*/
	if (PAPI_start(papi_event) < 0)
	 	handle_error((char*)"papi start event err\n");
	
	inited = true;
}

/* print record */
void print_record(){
	record_num++;
	time_t end_time;
	time(&end_time);
	double diff_time = difftime(end_time, begin_time);
	char info_file_name[MAXSTR];
	char total_time_file_name[MAXSTR];
	char cnt_file_name[MAXSTR];
	char total_tlb_dm_file_name[MAXSTR];
	char total_tlb_im_file_name[MAXSTR];
	char avg_time_file_name[MAXSTR];
	char avg_tlb_dm_file_name[MAXSTR];
	char avg_tlb_im_file_name[MAXSTR];
	sprintf(info_file_name, "%s/info.json", folder_name);
	sprintf(total_time_file_name, "%s/total_time", folder_name);
	sprintf(cnt_file_name, "%s/cnt", folder_name);
	sprintf(total_tlb_dm_file_name, "%s/total_tlb_dm", folder_name);
	sprintf(total_tlb_im_file_name, "%s/total_tlb_im", folder_name);
	sprintf(avg_time_file_name, "%s/avg_time", folder_name);
	sprintf(avg_tlb_dm_file_name, "%s/avg_tlb_dm", folder_name);
	sprintf(avg_tlb_im_file_name, "%s/avg_tlb_im", folder_name);
	FILE* info_FILE = fopen(info_file_name, "w");//don't use ~ for home dir
	FILE* total_time_FILE = fopen(total_time_file_name, "a");
	FILE* cnt_FILE = fopen(cnt_file_name, "a");
	FILE* total_tlb_dm_FILE = fopen(total_tlb_dm_file_name, "a");
	FILE* total_tlb_im_FILE = fopen(total_tlb_im_file_name, "a");
	FILE* avg_time_FILE = fopen(avg_time_file_name, "a");
	FILE* avg_tlb_dm_FILE = fopen(avg_tlb_dm_file_name, "a");
	FILE* avg_tlb_im_FILE = fopen(avg_tlb_im_file_name, "a");
	char diff_time_str[13];
	char info_str[MAXSTR];
	sprintf(diff_time_str, "# %d\n", int(diff_time));
	sprintf(info_str, "{\"record_num\": %d, \"call_cnt\": %d, \"unique_cnt\": %zu}", record_num, call_cnt, CNT.size());
	fputs(info_str, info_FILE);
	fputs(diff_time_str, total_time_FILE);
	fputs(diff_time_str, cnt_FILE);
	fputs(diff_time_str, total_tlb_dm_FILE);
	fputs(diff_time_str, total_tlb_im_FILE);
	fputs(diff_time_str, avg_time_FILE);
	fputs(diff_time_str, avg_tlb_dm_FILE);
	fputs(diff_time_str, avg_tlb_im_FILE);
	for (map<char*, long>::iterator itr = TOTAL_TIME.begin(); itr != TOTAL_TIME.end(); ++itr){
		char* str = (char*)malloc(MAXSTR*sizeof(char));
		sprintf(str, "%s %ld\n", itr->first, itr->second);
		fputs(str, total_time_FILE);
		free(str);
	}
	for (map<char*, int>::iterator itr = CNT.begin(); itr != CNT.end(); ++itr){
		char* str = (char*)malloc(MAXSTR*sizeof(char));
		sprintf(str, "%s %d\n", itr->first, itr->second);
		fputs(str, cnt_FILE);
		free(str);
	}
	for (map<char*, long long>::iterator itr = TOTAL_TLB_DM.begin(); itr != TOTAL_TLB_DM.end(); ++itr){
		char* str = (char*)malloc(MAXSTR*sizeof(char));
		sprintf(str, "%s %lld\n", itr->first, itr->second);
		fputs(str, total_tlb_dm_FILE);
		free(str);
	}
	for (map<char*, long long>::iterator itr = TOTAL_TLB_IM.begin(); itr != TOTAL_TLB_IM.end(); ++itr){
		char* str = (char*)malloc(MAXSTR*sizeof(char));
		sprintf(str, "%s %lld\n", itr->first, itr->second);
		fputs(str, total_tlb_im_FILE);
		free(str);
	}
	for (map<char*, long>::iterator itr = TOTAL_TIME.begin(); itr != TOTAL_TIME.end(); ++itr){
		char* str = (char*)malloc(MAXSTR*sizeof(char));
		sprintf(str, "%s %d\n", itr->first, int((float)itr->second/(float)CNT[itr->first]));
		if(CNT[itr->first]){
			fputs(str, avg_time_FILE);
		}
		free(str);
	}
	for (map<char*, long long>::iterator itr = TOTAL_TLB_DM.begin(); itr != TOTAL_TLB_DM.end(); ++itr){
		char* str = (char*)malloc(MAXSTR*sizeof(char));
		sprintf(str, "%s %d\n", itr->first, int((float)itr->second/(float)CNT[itr->first]));
		if(CNT[itr->first]){
			fputs(str, avg_tlb_dm_FILE);
		}
		free(str);
	}
	for (map<char*, long long>::iterator itr = TOTAL_TLB_IM.begin(); itr != TOTAL_TLB_IM.end(); ++itr){
		char* str = (char*)malloc(MAXSTR*sizeof(char));
		sprintf(str, "%s %d\n", itr->first, int((float)itr->second/(float)CNT[itr->first]));
		if(CNT[itr->first]){
			fputs(str, avg_tlb_im_FILE);
		}
		free(str);
	}
	fclose(info_FILE);
	fclose(total_time_FILE);
	fclose(cnt_FILE);
	fclose(total_tlb_im_FILE);
	fclose(total_tlb_dm_FILE);
	fclose(avg_time_FILE);
	fclose(avg_tlb_dm_FILE);
	fclose(avg_tlb_im_FILE);
}
struct thread_arg {
	int c;
	char** v;
};
void* target_thread(void* t_arg){
	struct thread_arg *arg = (struct thread_arg*)t_arg;
	struct stat dir_stat = {0};
	time(&begin_time);
#ifdef PROFILER_DIR
	sprintf(folder_name, "%s/record/%ld", STR2(PROFILER_DIR), begin_time);
#else
	sprintf(folder_name, "record/%ld", begin_time);
#endif
	printf("folder name %s\n", folder_name);
	printf("The data will going to save in folder, '%s'.\n", folder_name);
	if(stat(folder_name, &dir_stat) == -1){
		mkdir(folder_name, 0777);
	}
	targetMain(arg->c, arg->v);
	return NULL;
}

int main(int argc, char** argv){
	pthread_t pthread;
	int status;
	char* c = (char*)malloc(MAXSTR);
	struct thread_arg *t_arg = (struct thread_arg*)malloc(sizeof(struct thread_arg));
	t_arg->c = argc;
	t_arg->v = argv;
	int thr_id = pthread_create(&pthread, NULL, &target_thread, t_arg);
	if(thr_id < 0){
		handle_error((char*)"thread create error\n");	
		exit(EXIT_FAILURE);
	}
	printf("Type anything and you can get record. To type without getting record, type escape.\n");
	while(fgets(c, 7, stdin)){
		if(strcmp(c,"escape") == 0) break;
		print_record();
		printf("Recorded, record number: %d.\n", record_num);
	}
	if(pthread_join(pthread, (void**)&status) != 0){
		handle_error((char*)"thread join error\n");
	}
	print_record();
	free(t_arg);
	return 0;
}
