#include<iostream>
	#include<string.h>
	#include<pthread.h>
	#include<stdlib.h>
	#include<math.h>
	#include<chrono>
	#include<stdio.h>


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

	float global_result = 0, global_x_int;
	float a, b;
	int n;
	double cpu_time;
	int functionid, intensity, granularity, work_done = 0, nbthreads;
	char* sync;
	unsigned long startloop=0, endloop;
	float result = 0;

	pthread_mutex_t resultlocks;

	struct data
	{
	    float a, b;
	    long n, start, end;
	    float result, y;
	    int intensity, functionid;
	    std::string sync;
	};

	int get_end(int start)
	{
		int loopend = (start + granularity);
		if( loopend >= n)
			return n;

		return loopend;
	}

	int get_start()
	{
		int temp;
		pthread_mutex_lock(&resultlocks);
		temp = startloop;
		startloop = startloop + granularity;
		if (startloop + granularity > n)
			work_done = 1;
		pthread_mutex_unlock(&resultlocks);
		return temp;
	}
// iteration level integration
	void* iterationwise(void * inputument)
	{
	    struct data* input = (struct data* )inputument;
		int start=input->start;
		int end=input->end;
		int a=input->a;
		int b=input->b;
		int n=input->n;
		int intensity=input->intensity;
		int functionid=input->functionid;

	    while (work_done!= 1)
	    {
	  		start = get_start();
	    	end = get_end(start);
	    	for(int i = start; i < end ; i++)
	    	{
	    		pthread_mutex_lock(&resultlocks);
				global_x_int = (a + (i + 0.5) * ((b - a) / (float)n));
				//global_x_int = global_x_int + global_x_int;
				switch(functionid)
	    		{

	    			case 1: global_result+=  f1(global_x_int, intensity) * ((b - a)/(float)n);
					break;
	    	      	case 2: global_result+= f2(global_x_int, intensity) * ((b - a)/(float)n);
					break;
	    	      	case 3: global_result+= f3(global_x_int, intensity) * ((b - a)/(float)n);
					break;
	    	  	  	case 4: global_result+= f4(global_x_int, intensity) * ((b - a)/(float)n);
					break;
	    	    	default: std::cout<<"\nWrong functionidtion id"<<std::endl;
	    	  	}
	    	  	if (i == end-1 && endloop>=n-1)
	    	  		work_done = 1;
	    	  	pthread_mutex_unlock(&resultlocks);

	    	  }
	    }
	    	  pthread_exit(NULL);
	}

// chunk level integration
	void* chunk_level(void *unused)
	{
		float chunk_result = 0, chunk_int, chunk_val=0;
		int loop_end, loop_start;
		while(work_done != 1)
		{chunk_result=0;
			loop_start = get_start();
			loop_end = get_end(loop_start);
			for(int i = loop_start; i < loop_end; i++)
	    	{
				chunk_int = (a + (i + 0.5) * ((b - a) / (float)n));
				switch(functionid)
	        	{
	      			case 1: chunk_result+= f1(chunk_int, intensity) * ((b - a)/n);
							break;
	        		case 2: chunk_result+= f2(chunk_int, intensity) * ((b - a)/n);
							break;
	        	  	case 3: chunk_result+= f3(chunk_int, intensity) * ((b - a)/n);
							break;
	      		  	case 4: chunk_result+= f4(chunk_int, intensity) * ((b - a)/n);
							break;
	        	  	default: std::cout<<"\nWrong functionidtion id"<<std::endl;
	      		}

			}
		//	pthread_mutex_lock(&resultlocks);
			if ( endloop>=n-1)
	    	  		work_done = 1;
pthread_mutex_lock(&resultlocks);
global_result = global_result + chunk_result;
	    pthread_mutex_unlock(&resultlocks);
	    //	pthread_mutex_unlock(&resultlocks);
		}
		
	}
// if the sync is thread
	void* threadwise(void *unused)
	{
		float thread_result = 0, thread_int ;
		int loop_end, loop_start;
		while(work_done != 1)
		{
			loop_start = get_start();
			loop_end = get_end(loop_start);
			for(int i = loop_start; i < loop_end; i++)
	    	{
				thread_int = (a + (i + 0.5) * ((b - a) / (float)n));
				switch(functionid)
	        	{
	      			case 1: thread_result+= f1(thread_int, intensity) * ((b - a)/n);
							break;
	        		case 2: thread_result+= f2(thread_int, intensity) * ((b - a)/n);
							break;
	        	  	case 3:thread_result+= f3(thread_int, intensity) * ((b - a)/n);
							break;
	      		  	case 4: thread_result+= f4(thread_int, intensity) * ((b - a)/n);
							break;
	        	  	default: std::cout<<"\nWrong functionidtion id"<<std::endl;
	      		}

			}
		//	pthread_mutex_lock(&resultlocks);
			if ( endloop>=n-1)
	    	  		work_done = 1;
	    //	pthread_mutex_unlock(&resultlocks);
		}
		pthread_mutex_lock(&resultlocks);
global_result = global_result + thread_result;
	    pthread_mutex_unlock(&resultlocks);
	}

	int main (int argc, char* argv[])
	{

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

	    if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
     }

     else
     {
		functionid = atoi(argv[1]);
	    a = atof(argv[2]);
	    b = atof(argv[3]);
	    n = atof(argv[4]);
	    intensity = atoi(argv[5]);
	    nbthreads = atoi(argv[6]);
	    sync = argv[7];
		granularity = atoi(argv[8]);
	    pthread_t *threads;
		struct data* input;

	    threads = (pthread_t *)malloc(nbthreads * sizeof(pthread_t));
	    input = (struct data*)malloc(nbthreads * sizeof(data));
//start the clock
	  clock_t start_time,end_time;
    start_time=clock();

// if the type of sync is thread
	    if ( strcmp(sync, "thread") == 0)
	    {
	    	for ( int j = 0; j < nbthreads; j++)
	    	{
	    	    input[j].a = a;
	    	    input[j].b = b;
	    	    input[j].intensity =intensity;
	    	    input[j].functionid = functionid;
	    	    input[j].n = n;
            //create nbthreads
	    	    pthread_create(&threads[j], NULL, threadwise, (void *)&(input[j]));
	        }
		for ( int i = 0; i < nbthreads; i++)
	        {
	    	    pthread_join(threads[i], NULL);
	    	}
	    	for( int k = 0; k < nbthreads; k++)
	    	{
		    pthread_mutex_lock(&resultlocks);
	    	    global_result += input[k].result;
		    pthread_mutex_unlock(&resultlocks);
	    	}
	    }
	    else if(strcmp(sync, "iteration") == 0)
	    {
	    	for ( int j = 0; j < nbthreads; j++)
	    	{
	    	    input[j].a = a;
	    	    input[j].b = b;
	    	    input[j].intensity =intensity;
	    	    input[j].functionid = functionid;
	    	    input[j].n = n;
            //create nbthreads
	    	    pthread_create(&threads[j], NULL, iterationwise, (void *)&(input[j]));
	        }

	        for(int j = 0; j < nbthreads; j++)
	        {
	            pthread_join(threads[j], NULL);
	        }
	    }
	 	else if( strcmp(sync, "chunk") == 0)
	 	{
	 		for(int i = 0; i < nbthreads; i++)
	 		{
        //create nbthreads
	 			pthread_create(&threads[i], NULL, chunk_level, NULL);
	 		}

	 	  	for(int i = 0;i < nbthreads; i++)
	 		{
	 			pthread_join(threads[i], NULL);
	 		}
	 	}

//close the clock
		end_time=clock();
		std::cout<<global_result;

float cpu_time_used=((double)(end_time-start_time))/CLOCKS_PER_SEC;
std::cout<<"\n";
 std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
 std::cerr<<elapsed_seconds.count()<<std::endl;

//fprintf(stderr,"%f",cpu_time_used);
//std::cerr<<cpu_time_used;
	  return 0;
     }
	}

