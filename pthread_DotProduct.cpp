#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <cmath>

#define N 1000000
#define N_THREADS 2 //Number of thread are defined here
float *A;
float *B;
float sum = 0; //global variable that can be accessed by all threads

using namespace std;
using namespace std::chrono;
pthread_mutex_t mtx;

float vectorSize(float *arr) //calculation of vector's size
{
   float sum = 0;
   for (int i = 0; i < N; i++)
   {
   sum += (arr[i] * arr[i]);
   }
   sum = sqrt(sum);
   return sum;
}

void normalisation(float* arr) //normlaisation process
{
   float vector_size = vectorSize(arr);
   for (int i = 0; i < N; i++)
   {
   arr[i] /= vector_size;
   }
}

void* similarity(void* arg) //dot product
{
   int thread_number = (uintptr_t) arg;
   float localsum = 0;
   for (int i = thread_number; i < N; i+= N_THREADS)
   {
      localsum += A[i] * B[i];
   }

   pthread_mutex_lock(&mtx);
   sum += localsum;
   pthread_mutex_unlock(&mtx);
}

int main(){

   A = new float[N];
   B = new float[N];

   for (int i = 0; i < N; i++)
   {
      A[i] = 1;
      B[i] = 2;
   }

   normalisation(A);
   normalisation(B);

   pthread_t threads[N_THREADS]; //threads initialisation
   auto start = high_resolution_clock::now(); //measure start time

   for (int i=0; i < N_THREADS; i++) {
      pthread_create(&threads[i], NULL, similarity, (void*) i); //thread creation
   }

   for (int i = 0; i < N_THREADS ; i++){
      pthread_join(threads[i], NULL); //waiting for threads to finish
   }

   auto stop = high_resolution_clock::now(); //measure stop time
   auto duration = duration_cast<microseconds>(stop - start); //determining execution time

   cout<<"\nExecution time with "<< N_THREADS <<" threads: " << duration.count()
   <<" microseconds"<<endl;

   
   delete(A); // deletion of memory allocation
   delete(B); // deletion of memory allocation
   pthread_exit(NULL); // deletion of memory allocation for threads
   return 0;
}
