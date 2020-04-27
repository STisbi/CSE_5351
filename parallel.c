#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>


/**
 * This program is a SPMD program. It proceeds as follows.
 * Each process finds the prime number from 0 to the square
 * root of the input range, n. Then remaining primes are
 * split up so that each process gets an even amount. At
 * the very end, process 0 receives all the prime numbers
 * from the other processes and prints them. 
 */
int main(int argc, char** argv)
{
  int rank;
  int num_proc;

  // Required to start MPI
  MPI_Init(&argc, &argv);

  // 'comm_world' is a 'communicator'
  // Rank is ID of a proc
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

  // argv[0] is the program name
  // argv[1] is p, the number of processes
  // argv[2] is n, the limit within which to find multiples
  if (argc < 3)
  {

    // If rank 0 is not specified then every process would print this 
    if (rank == 0)
    {
      printf("Not enough arguments provided.\nExiting.\n");
    }

    exit(1);
  }
 
  // As above, so below
  if (rank == 0)
  {
    printf("Finding all primes in the range from 0 to %s with %s processes.\n", argv[2], argv[1]);
  }

  int* equal_list;
  int* split_list;

  int split = 0;
  int rmndr = 0;
  int low   = 0;
  int high  = 0;

  // Cast to an int
  int n = atoi(argv[2]);


  // Determine the split and remainder if n is odd
  // Along with the floor and ceiling of the splits
  split = (n - (sqrt(n) + 1)) / num_proc;

  // We have to cast to an int because we can't use
  // the modulus operator with a double (sqrt)
  rmndr = ((int) (n - (sqrt(n) + 1))) % num_proc;

  // Calculate the split based on which process this is
  // rank gives the process number starting from 0
  low = sqrt(n) + rank * split + 1;
  high = low + split - 1;

  // Give the last process the remainder
  if (rank == num_proc - 1)
  {
    high += rmndr;
  }

  // Calloc gives zero'd out memory
  equal_list = (int*) calloc(1, sqrt(n + 1) * sizeof(int));
  split_list = (int*) calloc(1, (high - low + 1) * sizeof(int));

  int index_1;

  // Run through the same list on all processes (equal_list)
  for (index_1 = 2; index_1 <= sqrt(n); index_1++)
  {
    // If unmarked
    if (equal_list[index_1] == 0)
    {
      // Go through number bigger than it in the equal_list
      int index_2;

      for (index_2 = index_1 + 1; index_2 <= sqrt(n); index_2++)
      {
        // Is this a multiple...
        if (index_2 % index_1 == 0)
        {
          // ...then mark it
          equal_list[index_2] = 1;
        }
      }

      // Run through number bigger than index_1 in the split_list
      for (index_2 = low; index_2 <= high; index_2++)
      {
        // Is this a multiple...
        if (index_2 % index_1 == 0)
        {
          // ...then mark it
          split_list[index_2 - low] = 1;
        }
      }
    }
  }

  // Are we in process 0?
  if (rank == 0)
  {
    printf("\nThe primes numbers are: ");

    // Iterate through list 1
    for (index_1 = 2; index_1 <= sqrt(n); index_1++)
    {
      // Was this a prime number?
      if (equal_list[index_1] == 0)
      {
        printf("%d ", index_1);
      }
    }

    // Process's 0 own list
    for (index_1 = low; index_1 <= high; index_1++)
    {
      // Was this a prime number?
      if (split_list[index_1 - low] == 0)
      {
        printf("%d ", index_1);
      }
    }

    // A buffer to put the other process's lists into
    int* buffer = (int*) calloc(1, n * sizeof(int));

    // Go through the other processes
    for (rank = 1; rank <= num_proc - 1; rank++)
    {
      // Calculate low and high for this rank
      low = sqrt(n) + rank * split + 1;
      high = low + split - 1;

      if (rank == num_proc - 1)
      {
        high += rmndr;
      }

      // Get the list of multiples
      MPI_Recv(buffer, high - low + 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    


      // Go through what we got from the other processes
      for (index_1 = low; index_1 <= high; index_1++)
      {
        // Was this a prime number?
        if (buffer[index_1 - low] == 0)
        {
          printf("%d ", index_1);
        }
      }
    }
    printf("\n\nFinished. Exiting\n\n");
  }
  else
  {
    /*
    for (index_1 = low; index_1 <= high; index_1++)
    {
      // Was this a prime number?
      if (split_list[index_1 - low] == 0)
      {
        printf("Rank %d found prime %d\n", rank, index_1);
      }
    }
    */
 
    // We're not process 0 so send stuff to it
    MPI_Send(split_list, high - low + 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  // Required by MPI
  MPI_Finalize();

  return 0;
}
