#include <iostream>
#include <chrono>
#include <cstdlib>
#include <pthread.h>
#include <cmath>
#include <omp.h>

#define N 10000000
#define N_THREADS 8

float *A;
float *B;

using namespace std;
using namespace std::chrono;

//-----------------------------------------FUNCTIONs FOR SERIAL EXECUTION---------------------------------------------------
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

double similarity(float* arr, float* arr2) //dot product
{
   float sum = 0;

   for (int i = 0; i < N; i++)
   {
      sum += arr[i] * arr2[i];
   }

   return sum;
}

//----------------------------------------FUNCTIONs FOR PARALLEL EXECUTION--------------------------------------------

float vectorSize_parallel(float *arr) //calculation of vector's size
{
   float sum = 0;
   
      #pragma omp parallel for shared(arr) reduction (+:sum) //for loop is diveded between thread with shared arr and
         for (int i = 0; i < N; i++)                         //added at the end to sum variable
         {
            sum += (arr[i] * arr[i]);
         }

   return sqrt(sum);
}

void normalisation_parallel(float* arr) //normalisation process
{
   float vector_size = vectorSize(arr);

   #pragma omp parallel for shared(arr, vector_size) //for loop is divided between threads sharing arr 
   for (int i = 0; i < N; i++)
   {
   arr[i] /= vector_size;
   }
}

double similarity_parallel(float* arr, float* arr2) //dot product
{
   float sum = 0;

   #pragma omp parallel for shared(arr, arr2) reduction (+:sum) //for loop is diveded between thread with shared arr and
   for (int i = 0; i < N; i++)                                  //added at the end to sum variable
   {
      sum += arr[i] * arr2[i];
   }
   return sum;
}

int main() {

   A = new float[N];
   B = new float[N];

   //----------------------------------------------- SERIAL EXECUTION-----------------------------------------------
   auto start = high_resolution_clock::now(); //measure start time

   for(int i=0; i < N;  i++){
      A[i] = -200 + (rand() % 400 + 1);
      B[i] = -200 + (rand() % 400 + 1);
   }
   
   normalisation(A);
   normalisation(B);

   double sim = similarity(A, B);

   auto stop = high_resolution_clock::now(); //measure stop time
   auto duration = duration_cast<microseconds>(stop - start); //determining execution time

   cout<<"Serial Execution"<<endl;
   cout<<"Similarity of vector A nad B equals: "<<sim<<endl;
   cout<<"Execution time "<< duration.count()<<" microseconds"<<endl;

   //----------------------------------------PARALLEL IMPLEMENTATION WITH OPEN MP-----------------------------------------

   auto start2 = high_resolution_clock::now(); 

   omp_set_num_threads(N_THREADS);

   #pragma omp for
   for(int i=0; i < N;  i++){ // reassignement of values to the vectors
      A[i] = -200 + (rand() % 400 + 1);
      B[i] = -200 + (rand() % 400 + 1);
   }

   normalisation_parallel(A);
   normalisation_parallel(B);

   double sim_parallel = similarity_parallel(A, B);

   auto stop2 = high_resolution_clock::now(); 
   duration = duration_cast<microseconds>(stop2 - start2); //determining execution time

   cout<<"\nParallel Execution for "<<N_THREADS<<" threads"<<endl;
   cout<<"Similarity of vector A nad B equals: "<<sim<<endl;
   cout<<"Execution time "<< duration.count()<<" microseconds"<<endl;
 
   return 0;
}