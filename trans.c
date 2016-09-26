/* Name:  Aayush Karki 
 andnrew: aayushka*/

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    ENSURES(is_transpose(M, N, A, B));

    int i,j,k,r,c,diag_c, diagElem, row, col , a,b,c0,d;

    switch (M){
    case 32:
    /*Parition into in 16 equal square blocks
      Each sublock is 8x8 */
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            for ( r = 8*i; r < 8*i+8 ; ++r) // rows
            {
                for ( c = 8*j; c < 8*j+8; ++c) //cols
                {
                    if (r!=c) B[c][r] = A[r][c];
                    else{
                        diag_c = c; // store the diagonal value
                        diagElem = A[diag_c][diag_c];
                    }  
                } // for c
            
            if (i==j) B[diag_c][diag_c] = diagElem;
            } // for r
            
        }  // for j
    } // for i
    break;

    case 61:
        /*iterate through 20 blocks. 
         Assume each block is 8x8. 
         So 5 blocks along the column and 4 along the row*/

            for ( k = 0; k <  20; k++)
            { 
                c = 16 *(k % 5);

                for( col = c; col < c+16;col++){ //iterate columns
            

                     r = 16 *(k/5) ;
                    for ( row = r; row <r+16; row++) //iterate rows
                    {                                                          
                         //check for bounds
                        if (!(col<67 && row<61)) break;

                        if (row != col){//check if diagonal
                            B[row][col] = A[col][row];
                        }
                        else
                        {
                            diag_c = col; //store the diagonal
                            diagElem = A[col][row];
                        }   
                  
                    } // for row
                    // check if the block contains the diagonal of A
                    if (k%6==0) B[diag_c][diag_c] = diagElem;
                       
                } // for col
            } // for k
            break;
            
        case 64:

            /*Iterate each 8x8 block in 4x4 sub-blocks
            Assume each 8x8 is [[a,b],[c,d]] where a,b,c,d
            each are sub-blocks.
            The transpose of this is [[a',c'],[b',d']].
            But we proceed by transposing [a,b] into
            [a',b'] in matrix B. When changing [c,d] to [b',d'], we 
            read b' into place of c from [a',b'] from B instead of 
            directly from matrix A. Read d' directly from A.
            */
            for ( r = 0; r < 64; r+=8) //iterate (8x8) rows
            {
                for (c = 0; c < 64; c+=8) //iterate (8x8) cols
                {
                    
                    for (row = r; row < r+4; row+=1)//iterate rows of subblocks
                    {
                     /*Read a from A and put a' into correct pos in B*/
                        for (col= c; col < c+4; col+=1)
                        {
                            B[col][row] = A[row][col];
                        }
                        /*Read entire row of b from A*/
                            a = A[row][c+4];
                            b = A[row][c+5];
                            c0 = A[row][c+6];
                            d = A[row][c+7]; 

                          /*Get a row of b into corresponding 
                            col in b' in [a',b'] in B*/
                            B[c+0][r+4+row%4] = a ;
                            B[c+1][r+4+row%4] = b ;
                            B[c+2][r+4+row%4] = c0 ;
                            B[c+3][r+4+row%4] = d   ;                  
                    }
                   
                    for (row = c; row < c+4; row+=1)
                    {
                         /* Get a row of b' from [a',b'] from B*/
                            a = B[row][r+4];
                            b = B[row][r+5];
                            c0 = B[row][r+6];
                            d = B[row][r+7];
                            
                        /* Get a row of c from A to put in
                           [a', b'] to change it into [a',c'] 
                           in B*/
                            B[row][r+4] = A[r+4][row];
                            B[row][r+5] = A[r+5][row];
                            B[row][r+6] = A[r+6][row];
                            B[row][r+7] = A[r+7][row];   

                         /*Put the row of b' in c'*/
                            B[4+row][r] = a;
                            B[4+row][r+1] = b;
                            B[4+row][r+2] = c0;
                            B[4+row][r+3] = d;            

                        /*Read a row of d from A into d' in B*/
                            B[4+row][r+4] = A[r+4][4+row];
                            B[4+row][r+5] = A[r+5][4+row];
                            B[4+row][r+6] = A[r+6][4+row];
                            B[4+row][r+7] = A[r+7][4+row];
                    }

                } //for loop -c
            } // for loop -r
        
            break;
 
}// switch

}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

