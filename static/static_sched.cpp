#include <iostream>
	#include<stdio.h>
	#include<pthread.h>
	#include<chrono>
	#include<math.h>
	#include<stdlib.h>
	#include<string.h>

	#ifdef __cplusplus
	extern "C" {
	#endif

  float f1(float x, int intensity);
	float f2(float x, int intensity);
	float f3(float x, int intensity);
	float f4(float x, int intensity);

	#ifdef __cplusplus
	}
	#endif

	float result = 0,y  ;
	pthread_mutex_t result_lock;

	struct data
	{
	    float a, b;
	    unsigned long n, start, end;
	    float result, y;
	    int intensity, functionid;
	    std::string sync;
	};
//interation level integration
	void* iterationwise(void * inputument)
	{
	    struct data* input = (struct data* )inputument;
		int start=input->start;
		int end=input->end;
		float a=input->a;
		float b=input->b;
		int n=input->n;
		int functionid=input->functionid;

	    for(int i = start; i < end; i++)
	    {
	    	pthread_mutex_lock(&result_lock);
			y = (a + (i + 0.5) * ((b - a) / (float)n));
			switch(functionid)
	    	{

	    		case 1: result+=  f1(y, input->intensity) * ((b - a)/n);

				break;
	          	case 2: result+= f2(y, input->intensity) * ((b - a)/n);
				break;
	          	case 3: result+= f3(y, input->intensity) * ((b - a)/n);
				break;
	      	  	case 4: result+= f4(y, input->intensity) * ((b - a)/n);
				break;
	        	default: std::cout<<"\nWrong functionidtion id"<<std::endl;
	      	}
	      	pthread_mutex_unlock(&result_lock);
	      }
	      pthread_exit(NULL);
	}
//thread level integration
	void* threadwise(void * inputument)
	{
	    struct data* input = (struct data* )inputument;
		int start=input->start;
		int end=input->end;
		float a=input->a;
		float b=input->b;
		int n=input->n;
		int functionid=input->functionid;

	    for(int i = start; i < end; i++)
	    {

	    	y = (a + (i + 0.5) * ((b - a) / (float)n));
			switch(functionid)
	    	{
	      		case 1: input->result+= f1(y, input->intensity) * ((b - a)/n);
		      	break;
	          	case 2: input->result+= f2(y, input->intensity) * ((b - a)/n);
		      	break;
	          	case 3: input->result+= f3(y, input->intensity) * ((b - a)/n);
		      	break;
	      	  	case 4: input->result+= f4(y, input->intensity) * ((b - a)/n);
		      	break;
	            default: std::cout<<"\nWrong functionidtion id"<<std::endl;
	      	 }
	    }
	    pthread_exit(NULL);
	}
	int main (int inputc, char* inputv[]) {
	  if (inputc < 8) {
    std::cerr<<"usage: "<<inputv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }
    else
    {
clock_t start_time,end_time;
    start_time=clock();

	    float final_result = 0, y;
	    float a, b;
	    unsigned long n;
	    double cpu_time;
	    int functionid, intensity, nbthreads;
	    char* sync;
	    functionid = atoi(inputv[1]);
	    a = atof(inputv[2]);
	    b = atof(inputv[3]);
	    n = atof(inputv[4]);
	    intensity = atoi(inputv[5]);
	    nbthreads = atoi(inputv[6]);
	    sync = inputv[7];

	    pthread_t* threads;

	    struct data* input;

	    threads = (pthread_t *)malloc(nbthreads * sizeof(pthread_t));
	    input = (struct data*)malloc(nbthreads * sizeof(struct data));

	    pthread_mutex_init(&result_lock, NULL);

	    auto clock_start = std::chrono::system_clock::now();

	    if ( strcmp(sync, "thread") == 0)
	    {
	    	for ( int j = 0; j < nbthreads; j++)
	    	{
	    	    input[j].a = a;
	    	    input[j].b = b;
	    	    input[j].start = j * (n/nbthreads);
	    	    input[j].end = input[j].start + (n/nbthreads);
	    	    input[j].intensity =intensity;
	    	    input[j].functionid = functionid;
	    	    input[j].n = n;
	    	    pthread_create(&threads[j], NULL, threadwise, (void *)&(input[j]));
	        }
		for ( int i = 0; i < nbthreads; i++)
	        {
	    	    pthread_join(threads[i], NULL);
	    	}
	    	for( int k = 0; k < nbthreads; k++)
	    	{
	    	    result += input[k].result;
	    	}
	    }
	    else
	    {
	    	for ( int j = 0; j < nbthreads; j++)
	    	{
	    	    input[j].a = a;
	    	    input[j].b = b;
	    	    input[j].start = j * (n/nbthreads);
	    	    input[j].end = input[j].start + (n/nbthreads);
	    	    input[j].intensity =intensity;
	    	    input[j].functionid = functionid;
	    	    input[j].n = n;
	    	    pthread_create(&threads[j], NULL, iterationwise, (void *)&(input[j]));
	        }

	        for(int j = 0; j < nbthreads; j++)
	        {
	            pthread_join(threads[j], NULL);
	        }
	    }
		end_time=clock();
float cpu_time_used=((double)(end_time-start_time))/CLOCKS_PER_SEC;
std::cout<<result;
std::cout<<"\n";
fprintf(stderr,"%f",cpu_time_used);

	  return 0;
	  }

}
