#include <stdio.h>
void method(float y[],int l)
{
  int j, k, flag;
  int N = 100;
  float dr = 0.01;
  float  r = 0.01;
  for (j = 0; j < N; j++)
    {
      flag = 0;
      for (k = 0; k < l; k++)
	{
	  //	  printf("\n %d, %d,  %f, %f\n", j, k, r, y[k]);
	  printf("\n r = %f",r);
	  if (y[k] <= r)
	    {
	      printf("\n minG = %f",y[k]);
	      //	      goto loopend;
	      flag = -1;
	      break;
	    }
	}
      if(flag == -1)
	{
	  break;
	}
      
      r = r + dr;
      // loopend:
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 5

int main(void)
{
  int j, k;
  float x[N-1];
  float y[N-1];
  float g[N-1];
  float w = 1.0 / RAND_MAX;
  srand((unsigned) time(NULL));
    for (j = 0; j < N; j++)
      {
	y[j] =(float) rand() * w;
	printf(" G =  %f",y[j]);
      }
    method(y,N);
    
}
