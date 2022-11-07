/* timer */
long insert_timer_into_function();

long long* insert_pmu_into_function(char* func_name);

/* save record per function */
void insert_back_into_function(char* func_name, long start_time, long long* papi_start_value);

void init();

/* print record */
//void print_record();
