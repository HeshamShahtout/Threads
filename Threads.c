#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct dimensions{
int row;
int column;
};

double** a;
double** b;
double result[100][100];
int temp1;
int temp2;
int arows,acolumns,brows,bcolumns;
double** readFile(char* file){
    int rows=0;
    int flag = 0;
    int columns=0;
    temp1 = 0;
    temp2 = 0;
    char ch;
    FILE *f = fopen(file,"r");
    if(f)
    {
        while((ch=fgetc(f))!=EOF)
        {

            if(ch==' '&&flag==0)
                columns++;
            if(ch=='\n')
            {
                rows++;
                flag = 1;
            }
        }
        columns++;
    }

    else
    {
        printf("file not found\n");
        rows=-1;
        columns=-1;
    }
    fseek(f,0,SEEK_SET);
    int i,j,k;
    temp1 = rows;
    temp2 = columns;
    double** arr = (double**) malloc(rows*sizeof(double*));
    for (i = 0; i < rows; ++i)
        arr[i] = (double*) malloc(columns*sizeof(double));

    /**read matrix and save it in arr**/
    for(i=0; i<rows; i++)
    {
        for(j=0; j<columns; j++)
        {
            fscanf(f,"%lf",&arr[i][j]);
        }
    }

    fclose(f);
    return arr;
}

void WriteMatrix(char *file)
{
    FILE* f;
    f=fopen(file,"w");
    int i,j;
    for(i=0;i<arows;i++)
    {
        for(j=0;j<bcolumns;j++)
        {
            fprintf(f,"%lf  ",result[i][j]);
        }
        fprintf(f,"\n");
    }
    fclose(f);
}


void PrintMatrix(double matrix[][100],int rows,int columns)
{
    int i,j;
    for(i=0; i<rows; i++)
       {
            for(j=0; j<columns; j++)
            {
                printf("%lf ",matrix[i][j]);
            }
            printf("\n");
       }
        printf("\n");
}

void* MatrixMulbyrow(void* arg)
{
        struct dimensions *d=arg;
	//int i= *(int*)arg;
	int j;
	int k;
	double sum=0;
	for(j=0; j<bcolumns; j++)
	{
		for(k=0; k<acolumns; k++)
		{
			sum=+(a[d->row][k]*b[k][d->column]);
		}
		result[d->row][d->column]=sum;	
		sum=0;
	}
	pthread_exit(0);
}

void* MatrixMulbyelement(void* arg)
{
	struct dimensions *d=arg;
	int k;
	double sum=0;
	for(k=0; k<acolumns; k++)
	{
		sum+=(a[d->row][k]*b[k][d->column]);
	}
	result[d->row][d->column]=sum;		
	pthread_exit(0);
	
}
void matrMul(){
    int i,j,k;
    for(i=0; i<arows; i++)
        {
            for(j=0; j<bcolumns; j++)
            result[i][j]=0;
        }
    

    
     if(acolumns==brows)
     {
        for(i=0; i<arows; i++)
        {
            for(j=0; j<bcolumns; j++)
            {
                for(k=0; k<acolumns; k++)
                    result[i][j]+=a[i][k]*b[k][j];
            }
    }
     } else
     printf("rows of 1st matrix is not equal to columns of matrix 2");
    

}
void threadedMatrixMulByElement(){
        int counter=0,i,j;
	pthread_t thread2[arows][bcolumns];
	struct dimensions* d;
	
	for(i=0; i<arows; i++)
	{
		for(j=0; j<bcolumns; j++)
		{
			d=malloc(sizeof(struct dimensions));
			d->row=i;
			d->column=j;
			pthread_create(&thread2[i][j], NULL, MatrixMulbyelement,d);
            counter++;
		}
	}
    
	for(i=0; i<arows; i++)
	{
		for(j=0; j<bcolumns; j++)
		{
			pthread_join(thread2[i][j],NULL);
		}
	}
        printf("Threads For Multiplication by Element = %d\n",counter);   
}
void threadedMatrixMulByRow()
{
        int counter=0;
	pthread_t thread[arows];
	int tid[arows],i;
	for(i=0; i<arows; i++)
	{
		tid[i]=i;
		pthread_create(&thread[i], NULL, MatrixMulbyrow,&tid[i]);
        counter++;
		
	}
	for(i=0; i<arows; i++)
	{
		pthread_join(thread[i],NULL);
	}
        printf("Threads For Multiplication by Row = %d\n",counter);
}
int main()
{
    char file[300];
    char file2[300];
    char* file3;
    file3="file3.txt";
    int i,j,k;
    printf("Enter The Name Of The First File: ");
    scanf("%s",file);
    a = readFile(file);
    arows = temp1;
    acolumns = temp2;
    printf("\nEnter The Name Of The Second File: ");
    scanf("%s",file2);
    b= readFile(file2);
    brows = temp1;
    bcolumns = temp2;
    
   /*Noraml Matrix Multiplication*/
   
    clock_t begin = clock();
    matrMul();
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time Spent For Normal Multiplication =%lf ms\n",time_spent*1000);
    WriteMatrix(file3);
    
    /*End*/

    /*Threaded Matrix Multiplication By Element*/
    
     clock_t begin2 = clock();
     threadedMatrixMulByElement();
     clock_t end2 = clock();
     double time_spent2 = (double)(end2 - begin2) / CLOCKS_PER_SEC;
     printf("Time Spent For Threaded Multiplication By Element =%lf ms\n",time_spent2*1000);
     
    /*End*/

    /*Threaded Matrix Multiplication By Row*/ 
    
    clock_t begin3 = clock();
    threadedMatrixMulByRow();
    clock_t end3 = clock();
    double time_spent3 = (double)(end3 - begin3) / CLOCKS_PER_SEC;
    printf("Time Spent For Threaded Multiplication By Row =%lf ms\n",time_spent3*1000);
    
    /*End*/

    return 0;

}
