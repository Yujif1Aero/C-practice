#include <stdio.h>
void sort(int n, int a[])
{
  int j, k, flag;
  int work;
  
  flag = n - 1;
  while (flag >= 0) {
    k = -1;  //交換する数字がない場合はwhile文から出る
    for (j = 1; j <= flag; j++)
      if (a[j - 1] > a[j]) {
	k = j - 1;
	work = a[k];
	a[k] = a[j];
	a[j] = work;
      }
    flag = k;
  }
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100

int main(void)
{
  int min = 0, range = N;
  int data[N] = { 0 };
  int j, k;
  k = -1;
  srand((unsigned)time(NULL));
  printf("整列前 \n");
  for (j = 0; j < N; j++)
    {
      do
        {
	  data[j] = rand() % range + min + 1;
	  for (k = 0; k < j ; k++)
            {
	      if (data[j] == data[k]) break;
            }
        } while (j != k);
      printf(" %2d",data[j]);
    }
  sort(N, data);
  printf("\n 整列後\n");
  
  for(j = 0; j < N; j++)
    printf("% 2d",data[j]); 
}
