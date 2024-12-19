#include <stdio.h>
#define NUMBER_OF_PAGES     2
#define NUMBER_OF_ROWS      3
#define NUMBER_OF_COLUMNS   4


int pointer_arry1();
int pointer_arry2();
int pointer_arry3();
int pointer_arry4();

int main(){
  pointer_arry1();
  pointer_arry2();
  pointer_arry3();
  pointer_arry4();
  
}

int pointer_arry1(){
  int square_array[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS]
  = {
      {  1,  2,  3,  4},
      { 11, 12, 13, 14},
      {  101,  102,  103,  104},
    };
 
  int *top_of_columns;
 
  top_of_columns = square_array[0];
 
  printf("square_array[0][1] = %d\n", top_of_columns[2]); /*  2 */
 
  top_of_columns = square_array[1];
 
  printf("square_array[1][1] = %d\n", top_of_columns[1]); /* 12 */
 
  return 0;
}


int pointer_arry2(){
   int square_array_x[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS]
  = {
      {  1,  2,  3,  4},
      { 11, 12, 13, 14},
      {  101,  102,  103,  104},
    };
 
  int square_array_y[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS]
  = {
      { 21, 22, 23, 24},
      { 31, 32, 33, 34},
      {  41,  42,  43,  44},
    };
 
  int (*top_of_rows)[NUMBER_OF_COLUMNS]; // top_of_rows is pointer to arry 4 of int
  int *top_of_columns;
 
  top_of_rows = square_array_x; //top_of_rows indicates the head pointer of square_array_x
  top_of_columns = top_of_rows[1]; //top_of_rows[1] means the indicated pointer ahead of size_of(int NUMBER_OF_COLUMNS)
 
  printf("square_array_x[1][1] = %d\n", top_of_columns[0]); /* 11 */
 
  top_of_rows = square_array_y;
  top_of_columns = top_of_rows[0];
 
  printf("square_array_y[1][1] = %d\n", top_of_columns[3]); /* 24 */
 
  return 0;
}

int pointer_arry3(){
   int square_array[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS]
  = {
      {  1,  2,  3,  4},
      { 11, 12, 13, 14},
      { 101, 102, 103, 104},
    };
   int a = 0, b = 0;
   int *top_of_columns[NUMBER_OF_ROWS];
 
  top_of_columns[0] = square_array[0];
  top_of_columns[1] = square_array[1]; // square_arrary[1] indicates the pointer of square_array[1][0]
  a = top_of_columns[0][3];// top_of_colums[0][3] means that (top_of_pointer[0] + sizeof(3*int))
  b =  *(top_of_columns[0] + 3);
  printf("a=square_array[0][1] = %d\n", a); /*  2 */
  printf("b=square_array[0][1] = %d\n", b); /*  2 */
 
  printf("square_array[1][1] = %d\n", top_of_columns[1][1]); /* 12 */

   printf("*square_array[1] = %d\n", *top_of_columns[1]);   
  return 0;
}

int pointer_arry4(){
  int cubic_array[NUMBER_OF_PAGES][NUMBER_OF_ROWS][NUMBER_OF_COLUMNS]
  = {
      {
        {  1,  2,  3,  4},
        { 11, 12, 13, 14},
        { 21, 22, 23, 24},
      },
      {
        {101,102,103,104},
        {111,112,113,114},
        {121,122,123,124},
      },
    };
 
 
  int (*top_of_rows)[NUMBER_OF_COLUMNS]; //insterd of an expression top_of_rows[1], *(top_of_rows + 1) 
  int *top_of_columns;
 
  top_of_rows = cubic_array[0];
  // top_of_columns = top_of_rows[1];
  top_of_columns = *(top_of_rows + 1);
 
  printf("square_array[0][1][1] = %d\n", top_of_columns[1]); /* 12 */
 
  top_of_rows = cubic_array[1];
  top_of_columns = top_of_rows[2];
 
  printf("square_array[1][2][1] = %d\n", top_of_columns[1]); /*122 */
 
  return 0;
}
