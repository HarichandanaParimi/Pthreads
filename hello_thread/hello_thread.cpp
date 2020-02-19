#include <iostream>
#include <cstdlib>
#include <pthread.h>

using namespace std;
pthread_mutex_t g_lock;
int global_nbthreads;
void *ThreadCount(void *threadid) {
   long t;
   t = (long)threadid;
pthread_mutex_lock(&g_lock);
   cout << " I am thread" << t <<"in"<<global_nbthreads<<endl;
pthread_mutex_unlock(&g_lock);
   pthread_exit(NULL);
}
int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
  }
   else{
    global_nbthreads=atoi(argv[1]);
   pthread_t threads[global_nbthreads];
   int rc;
   int i;
   for( i = 0; i < global_nbthreads; i++ ) {
     //create threads
      rc = pthread_create(&threads[i], NULL, ThreadCount, (void *)i);
      if (rc) {
         cout << "unable to create a thread," << rc << endl;
         exit(-1);
      }
   }
   for (int i=0; i < global_nbthreads; i++)
    pthread_join(threads[i+1], NULL);return 0;
   }
   return 0;
}
