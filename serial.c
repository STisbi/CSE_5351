#include <stdio.h>
#include <math.h>
#include <stdlib.h>


/**
  * This program is similar to the parallel version.
  * However it does not separate the range of numbers
  * from 0 to n into chunks by the number of processes.
*/
int main(int argc, char** argv)
{
  // argv[0] is the program name
  // argv[1] is n, the limit within which to find multiples
  if (argc < 2)
  {
    printf("Not enough arguments provided.\nExiting.\n");

    exit(1);
  }

  printf("Finding all primes in the range from 0 to %s.\n", argv[1]);

  int n = atoi(argv[1]);
  int index_1 = 2;
  int index_2 = 3;

  int* total_list;

  // Gives a block of zero'd out memory
  total_list = (int*) calloc(1, n * sizeof(int));

  // Iterate through the range of numbers from 2 to
  // through the square root of n
  for (index_1 = 2; index_1 <= sqrt(n); index_1++)
  {
    // If this number is unmarked
    if (total_list[index_1] == 0)
    {
      // Iterate through the list of numbers from the above index
      // to the end of the range.
      for (index_2 = index_1 + 1; index_2 <= n - 1; index_2++)
      {
        // Is this a multiple?
        if (index_2 % index_1 == 0)
        {
          // Then mark it 
          total_list[index_2] = 1;
        }
      }
    }
  } 

  printf("\nThe primes numbers are: ");

  // Print the list of prime numbers
  for (index_1 = 2; index_1 <= n - 1; index_1++)
  {
    if (total_list[index_1] == 0)
    {
      printf("%d ", index_1);
    }
  }

  printf("\n\nFinished. Exiting.\n\n");

  return 0;
}
