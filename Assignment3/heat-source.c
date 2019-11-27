#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>

#define COLS 1000
#define ROWS 1000

#define WHITE    "15 15 15 "
#define RED      "15 00 00 "
#define ORANGE   "15 05 00 "
#define YELLOW   "15 10 00 "
#define LTGREEN  "00 13 00 "
#define GREEN    "05 10 00 "
#define LTBLUE   "00 05 10 "
#define BLUE     "00 00 10 "
#define DARKTEAL "00 05 05 "
#define BROWN    "03 03 00 "
#define BLACK    "00 00 00 "


void copyNewToOld(float grid_a[ROWS][COLS], float grid_b[ROWS][COLS], int x_lower, int x_upper) {
  int x, y;
  for (x = x_lower; x < x_upper; ++x) {
    for (y = 0; y < COLS; ++y) {
      grid_b[x][y] = grid_a[x][y];
    }
  }
}


void calculateNew(
  float grid_a[ROWS][COLS], 
  float grid_b[ROWS][COLS],
  int x_lower, 
  int x_upper
) {
  // Adjust bounds
  if (x_lower == 0) x_lower = 1;
  if (x_upper == ROWS) x_upper = ROWS - 1;
  int x, y;
  for (x = x_lower; x < x_upper - 1; ++x) {
    for (y = 1; y < COLS - 1; ++y) {
      grid_a[x][y] = 0.25 * (grid_b[x-1][y] + grid_b[x+1][y] + grid_b[x][y-1] + grid_b[x][y+1]);
    }
  }
}


void printGridtoFile(FILE* fp, float grid[ROWS][COLS], int x_lower, int x_upper) {
  int x, y;
  for (x = x_lower; x < x_upper; ++x) {
    for (y = 0; y < COLS; ++y) {
      if (grid[x][y] > 250) {
        fprintf(fp, "%s ", RED );
      } else if (grid[x][y] > 180) {
        fprintf(fp, "%s ", ORANGE );
      } else if (grid[x][y] > 120) {
        fprintf(fp, "%s ", YELLOW );
      } else if (grid[x][y] > 80) {
        fprintf(fp, "%s ", LTGREEN );
      } else if (grid[x][y] > 60) {
        fprintf(fp, "%s ", GREEN );
      } else if (grid[x][y] > 50) {
        fprintf(fp, "%s ", LTBLUE );
      } else if (grid[x][y] > 40) {
        fprintf(fp, "%s ", BLUE );
      } else if (grid[x][y] > 30) {
        fprintf(fp, "%s ", DARKTEAL );
      } else if (grid[x][y] > 20) {
        fprintf(fp, "%s ", BROWN );
      } else {
        fprintf(fp, "%s ", BLACK );
      }
    }
    fprintf(fp, "\n");
  }
}



int main(int argc, char **argv) {
  int h, w, cycles, heat;
  float grid_a[ROWS][COLS];
  float grid_b[ROWS][COLS];

  if (argc != 2) {
    printf("Usage: ./program <number of timestamps>\n");
    exit(0);
  }
  cycles = atoi(argv[1]);

 
  MPI_Init(NULL, NULL);

  int mpi_size;
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  int mpi_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  int x_lower =  mpi_rank    * ROWS / mpi_size;
  int x_upper = (mpi_rank+1) * ROWS / mpi_size;
  

  
  /*fprintf(stderr, "CPU=%d/%d x_lower=%d x_upper=%d\n", mpi_rank, mpi_size, x_lower, x_upper);*/

 
  for (h = x_lower; h < x_upper; ++h) {
    for (w = 0; w < COLS; ++w) {
      grid_a[w][h] = 20;
    }
  }

  for (heat = 299; heat < 700; ++heat) {
    grid_a[0][heat] = 300;
  }

  for (cycles; cycles > 0; --cycles) {
    /*fprintf(stderr, "[%d] Cycle=%d\n", mpi_rank, cycles);*/
    copyNewToOld(grid_a, grid_b, x_lower, x_upper);
    if (mpi_rank != 0) MPI_Sendrecv(
      grid_a[x_lower-1],  // sendbuf
      COLS,               // sendcount
      MPI_FLOAT,          // sendtype
      mpi_rank-1,         // dest
      0,                  // sendtag

      grid_b[x_lower-1],  // recvbuf
      COLS,               // recvcount
      MPI_FLOAT,          // recvtype
      mpi_rank-1,         // source
      0,                  // recvtag

      MPI_COMM_WORLD,     // communicator
      MPI_STATUS_IGNORE   // status
    );

    
    if (mpi_rank+1 != mpi_size) MPI_Sendrecv(
      grid_a[x_upper+1],  // sendbuf
      COLS,               // sendcount
      MPI_FLOAT,          // sendtype
      mpi_rank+1,         // dest
      0,                  // sendtag

      grid_b[x_upper+1],  // recvbuf
      COLS,               // recvcount
      MPI_FLOAT,          // recvtype
      mpi_rank+1,         // source
      0,                  // recvtag

      MPI_COMM_WORLD,     // communicator
      MPI_STATUS_IGNORE   // status
    );

    calculateNew(grid_a, grid_b, x_lower, x_upper);
  }

  FILE* fp;
  if (mpi_rank == 0) {
    fp = fopen("c.pnm", "w");
    fprintf(fp, "P3\n%d %d\n15\n", COLS, ROWS);
    fclose(fp);
  } 

  char dummy = 1;
  if (mpi_rank != 0) MPI_Recv(
    &dummy,             // buf
    1,                  // count
    MPI_BYTE,           // type
    mpi_rank-1,         // source
    0,                  // tag
    MPI_COMM_WORLD,     // communicator
    MPI_STATUS_IGNORE   // status
  );

  /*fprintf(stderr, "[%d] Open file\n", mpi_rank);*/
  fp = fopen("c.pnm", "a");
  if (!fp) *NULL;
  /*fprintf(stderr, "[%d] Start printing\n", mpi_rank);*/
  printGridtoFile(fp, grid_a, x_lower, x_upper);
 
  /*fprintf(stderr, "[%d] Done printing\n", mpi_rank);*/
  fclose(fp);

  if (1) MPI_Send(
    &dummy,                   // buf
    1,                        // count
    MPI_BYTE,                 // type
    (mpi_rank+1) % mpi_size,  // source
    0,                        // tag
    MPI_COMM_WORLD            // communicator
  );

  if (mpi_rank == 0) {
    MPI_Recv(
      &fp,                // buf
      1,                  // count
      MPI_UNSIGNED_LONG,  // type
      mpi_size-1,         // source
      0,                  // tag
      MPI_COMM_WORLD,     // communicator
      MPI_STATUS_IGNORE   // status
    );
    system("convert c.pnm c.png");
  }


  MPI_Finalize();

  return 0;
}
