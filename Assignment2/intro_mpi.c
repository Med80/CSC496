#include <stdio.h>

#include <stdlib.h>

#include <time.h>

#include <mpi.h>


#define RMAX 10

void printArr(int *arr, int count)
 {
  int i;
  printf("{");
  
for (i = 0; i < count; i++) 
{
   
 if (i != 0)
      
 printf(",");
 
   printf("%d", arr[i]);
 
 }

  printf("}");

}


int main(int argc, char *argv[])
{


  srand(time(NULL));

 
 int rank, size;
 
 int tmp;
  int tag;
 
 int i;

  
int arrayZero[16];
  
int arrayOne[16];
  
int fromArray[2];
  
int fromOne;
  int sum;

 
 MPI_Status status;

  
MPI_Init(&argc,&argv);

  
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
 MPI_Comm_size(MPI_COMM_WORLD, &size);

  
if (rank == 0)
    
 printf("\nPart 1.1\n\n");

  
if (rank == 0) 
{
    
int i;
   
 for (i = 0; i < 16; i++)
 {
      arrayZero[i] = rand() % RMAX + 1;
    }
   

 MPI_Send(&arrayZero[1], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
   
 printf("[%d/%d] arrayZero[1] = %d\n", rank, size, arrayZero[1]);
 
 } 
else if (rank == 1) 
{
    
MPI_Recv(&fromOne, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
printf("[%d/%d] fromOne = %d\n", rank, size, fromOne);
 
 }


  MPI_Barrier(MPI_COMM_WORLD);

  
if (rank == 0)
    
printf("\nPart 1.2\n\n");

 
 MPI_Bcast(&fromOne, 1, MPI_INT, 1, MPI_COMM_WORLD);
 
 printf("[%d/%d] fromOne = %d\n", rank, size, fromOne);

 
 MPI_Barrier(MPI_COMM_WORLD);

 
 if (rank == 0)
   
 printf("\nPart 1.3\n\n");

  
MPI_Reduce(&fromOne, &sum, 1, MPI_INT, MPI_SUM, 2, MPI_COMM_WORLD);
  
if (rank == 2)
   
 printf("[%d/%d] sum = %d\n", rank, size, sum);

  
MPI_Barrier(MPI_COMM_WORLD);

 
 if (rank == 0)
   
 printf("\nPart 1.4\n\n");

  
MPI_Scatter(&arrayZero, 2, MPI_INT, &fromArray, 2, MPI_INT, 0, MPI_COMM_WORLD);
 
 printf("[%d/%d] fromArray = ", rank, size);
  
printArr(fromArray, 2);
 
 fflush(stdout);
 
 printf("\n");

 
 MPI_Barrier(MPI_COMM_WORLD);

 
 if (rank == 0)
  
  printf("\nPart 1.5\n\n");

  
MPI_Gather(&fromArray, 2, MPI_INT, &arrayOne, 2, MPI_INT, 0, MPI_COMM_WORLD);
 
 if (rank == 0)
 {
   
 printf("[%d/%d] ", rank, size);
  
  printArr(arrayZero, 16);
  
  fflush(stdout);
   
 printf("\n[%d/%d] ", rank, size);
  
  printArr(arrayOne, 16);
   
 fflush(stdout);
    

printf("\n");
  }

 

 MPI_Finalize();
  
return 0;

}
