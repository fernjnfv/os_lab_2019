#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock1, lock2;

int let_it_go = 0;

void *function1()
{
  printf("Lock function1 by lock1\n");
  pthread_mutex_lock(&lock1); //step 1

  printf("Execute some code in function1 locked by lock1\n");
  sleep(1);
  printf("End execute some code in function1 locked by lock1\n");

  printf("Lock function1 by lock2\n");
  pthread_mutex_lock(&lock2); //step 4 - f1 wait for f2 to take off lock2

  printf("Execute some code in function1 locked by lock2 and lock1\n");
  let_it_go = 1;

  pthread_mutex_unlock(&lock2);

  pthread_mutex_unlock(&lock1);
} 

void *function2()
{
  printf("Lock function2 by lock2\n");
  pthread_mutex_lock(&lock2); //step 2

  printf("Execute some code in function2 locked by lock2\n");
  sleep(2);
  printf("End execute some code in function2 locked by lock2\n");

  printf("Lock function2 by lock1\n");
  pthread_mutex_lock(&lock1); //step 3 - f2 wait for f1 to take off lock1
  
  printf("Execute some code in function2 locked by lock1 and lock2\n");

  pthread_mutex_unlock(&lock1);

  pthread_mutex_unlock(&lock2);
} 

int main() {
  pthread_t thread1, thread2;

  pthread_create(&thread1, NULL, function1, NULL);
  pthread_create(&thread2, NULL, function2, NULL);

  pthread_join(thread1, 0);
  pthread_join(thread2, 0);

  //while (!let_it_go);
  
  return 0;
}
