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
  /* int work, workmax; */
  /* int N, i; */
  /* N = 5; */
  /* int a[N]; */
  /* srand((unsigned)time(NULL)); */
  /* work = rand(); */
  /* workmax = RAND_MAX; */
  
  /* printf("%d, %d", work, workmax); */
  /* for (i = 1; i <= N; i++) { */
  /*       a[i] = rand() % N ; */
  /*       printf(" %2d", a[i]); */
  /*   } */
  // 最小値、レンジ
  int min = 0, range = N;
  // データ
  int data[N] = { 0 };
  // ループ変数
  int j, k;
  k = -1;
  // 乱数シード初期化
  srand((unsigned)time(NULL));
  // データ生成
  printf("整列前 \n");
    for (j = 0; j < N; j++)
    {
        // 重複チェック
        do
        {
            // 乱数生成
            data[j] = rand() % range + min + 1;
            // 生成済みデータと比較
            for (k = 0; k < j ; k++)
            {
                // 生成済みデータに含まれる場合再度乱数生成
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
