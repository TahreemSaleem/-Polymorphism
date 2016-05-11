#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <time.h>
#include <stdio.h>      /* printf */
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <math.h> 
#define SIZE_VECTOR 10
#define ROW_MATRIX1 10
#define ROW_MATRIX2 10
#define COLUMN_MATRIX1 10
#define COLUMN_MATRIX2 10
#define num_thread 4
typedef struct vectorClass vectorClass;
typedef struct matrixClass matrixClass;
struct vectorClass
{
	int column;
	int *__restrict vector;
	void (* multiply)(vectorClass *this, vectorClass *);
};
struct matrixClass
{
	int column;
	int row;
	int **__restrict matrix;
	void (*multiply)(matrixClass *this, matrixClass *);
	void(*add)( matrixClass *this, matrixClass * );
};
struct matrixClass result;


void vectorClass_allocate(struct vectorClass, int, int);
void vectorClass_show(struct vectorClass);
void vectorClass_init(struct vectorClass);

void matrixClass_allocate(struct matrixClass, int, int, int);
void matrixClass_show(struct matrixClass);
void matrixClass_init(struct matrixClass);



void vectorClass_allocate(struct vectorClass v, int position, int value)
{
	v.vector[position] = value;
}
void vectorClass_show(struct vectorClass v)
{
	for (int i = 0; i < v.column; i++)
		printf("\t\t|\t%d\t|\n", v.vector[i]);
}
void vectorClass_init( vectorClass v)
{
	for (int position = 0; position < v.column; position++)
	{
		v.vector[position] = 0;
	}

}
void matrixClass_allocate( matrixClass m, int row, int column, int value)
{
	//row major ordered
	m.matrix[row ][column] = value;
}
void matrixClass_show(struct matrixClass m)
{
	for (int i = 0; i < m.row; i++)
	{
		printf("\t\t|\t");
		for (int j = 0; j < m.column; j++)
		{
			printf("%d\t", m.matrix[i][j]);
		}
		printf("|\n");
	}
}
void matrixClass_init(matrixClass m)
{

	for (int i = 0; i < m.row; i++)
	{
		for (int j = 0; j < m.column; j++)
		{
			m.matrix[i][j] = 0;
		}
	}


}
void * mult_matrix(matrixClass *this, matrixClass *m2)
{
	
	//printf("%d\n",pthread_self());
	
	int i, j, k;

	//printf("computing slice %d (from row %d to %d)\n", arg->slice, from, to - 1);
	for (int i = 0; i < this->row; i++)
	{
		for (int j = 0; j < m2->column; j++)
		{
			for (int k = 0; k < this->column; k++)
			{
				//c[i][j] = c[i][j] + (a[i][k] * b[k][j]);
				
				result.matrix[i][j] += this->matrix[i][k] * m2->matrix[k][j];
			}

		}
	}
	//printf("finished slice %d\n", arg->slice);
	return 0;
}
void * add_matrix(matrixClass *this, matrixClass *m2)
{
	
	//printf("%d\n",pthread_self());
	
	int i, j, k;

	//printf("computing slice %d (from row %d to %d)\n", arg->slice, from, to - 1);
	for (int i = 0; i < this->row; i++)
	{
		for (int j = 0; j < m2->column; j++)
		{
			
			result.matrix[i][j] = this->matrix[i][j] + m2->matrix[i][j];
		}
	}
	//printf("finished slice %d\n", arg->slice);
	return 0;
}

void * mult_vector(vectorClass *this, matrixClass *m)
{
	struct vectorClass result;
	result.column = (m->row);
	result.vector = malloc(sizeof(int) *result.column);
	for (int i = 0; i<this->column; i++)
	{
		result.vector[i] = 0;
	}
	for (int i = 0; i < (m->row); i++)
	{
		for (int j = 0; j < this->column; j++)
		{//A[j] += x[j][i] * y[i];
			result.vector[j] += (m->matrix[j][i] * this->vector[j]);
		}
	}

}

int main()

{

	clock_t t;
	int f;
	t = clock();


	srand(time(NULL));
	//struct vectorClass v1;
	//v1.column = SIZE_VECTOR;
	//v1.vector = malloc(sizeof(int) *v1.column);
	//v1.multiply = mult_vector;
	//vectorClass_init(v1);
	//for (int i = 0; i < v1.column; i++)
	//{

	//vectorClass_allocate(v1, i, rand());
	//}
	//printf("Your vector: \n");
	//vectorClass_show(v1);

	struct matrixClass m1;
	m1.column = COLUMN_MATRIX1;
	m1.row = ROW_MATRIX1;
	m1.matrix = (int**)malloc(sizeof(int) * m1.row);
	for (int i = 0; i < m1.row; i++)
		m1.matrix[i] = (int*)malloc(m1.column*sizeof(int));
	m1.multiply = mult_matrix;
	m1.add = add_matrix;
	matrixClass_init(m1);
	int val = 0;
	for (int i = 0; i < m1.row; i++)
	{
		for (int j = 0; j < m1.column; j++)
		{
			matrixClass_allocate(m1, i, j, val++);
		}
	}
	printf("Your matrix: \n");
	matrixClass_show(m1);

	struct matrixClass m2;
	m2.column = COLUMN_MATRIX2;
	m2.row = ROW_MATRIX2;
	m2.matrix = (int**)malloc(sizeof(int) * m2.row);
	for (int i = 0; i < m2.row; i++)
		m2.matrix[i] = (int*)malloc(m2.column*sizeof(int));
	m2.multiply = mult_matrix;
	m2.add = add_matrix;
	matrixClass_init(m2);
	val = 0;
	for (int i = 0; i < m2.row; i++)
	{
		for (int j = 0; j < m2.column; j++)
		{
			matrixClass_allocate(m2, i, j, val++);

		}
	}
	printf("Your matrix: \n");
	matrixClass_show(m2);

	result.row = ROW_MATRIX1;
	result.column = COLUMN_MATRIX2;
	result.add = add_matrix;
	result.multiply = mult_matrix;
	result.matrix = (int**)malloc(sizeof(int) * result.row);
	for (int i = 0; i < result.row; i++)
		result.matrix[i] = (int*)malloc(result.column*sizeof(int));
	matrixClass_init(result);
	
	m1.add(&m1, &m2);
	
	printf("done\n\n");
	matrixClass_show(result);
	
	t = clock() - t;
	printf("It took me %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);
	return 0;
}
