#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lock1, lock2;

void *function1()
{
  printf("Lock function1 by lock1\n");
  pthread_mutex_lock(&lock1); 
  sleep(1);
  printf("Lock function1 by lock2(wait for it unlock)\n");
  pthread_mutex_lock(&lock2); 
  printf("wery inportant code function1\n");
  pthread_mutex_unlock(&lock2);
  pthread_mutex_unlock(&lock1);
} 

void *function2()
{
  printf("Lock function2 by lock2\n");
  pthread_mutex_lock(&lock2);
  sleep(2);
  printf("Lock function2 by lock1(wait for it unlock)\n");
  pthread_mutex_lock(&lock1); 
  printf("wery inportant code function2\n");
  pthread_mutex_unlock(&lock1);
  pthread_mutex_unlock(&lock2);
} 

int main() {
  pthread_t thread1, thread2;

  pthread_create(&thread1, NULL, function1, NULL);
  pthread_create(&thread2, NULL, function2, NULL);

  pthread_join(thread1, 0);
  pthread_join(thread2, 0);

  return 0;
}
