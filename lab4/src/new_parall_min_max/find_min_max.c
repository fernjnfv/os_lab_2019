#include "find_min_max.h"
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

struct MinMax GetMinMax(int *array, unsigned int begin, unsigned int end) {
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  /* from begin to end-1
   * 1,5,2,3
   * the end num is 4 and the begin num is 0
   * so the size of arr is
   */  
  int arr_size = end-begin;

  printf("%d",arr_size);

  //copy array to be safe
  int* arr_copy = (int*) malloc(arr_size*sizeof(int));

  for (int i = 0; i < arr_size; i++){
    arr_copy[i] = array[i];
  }

  for (int i = begin; i < end; i++){
    if (min_max.min > arr_copy[i]){
      min_max.min = arr_copy[i];
    }
    if (min_max.max < arr_copy[i]){
      min_max.max = arr_copy[i];
    }
  }
  
  return min_max;
}
