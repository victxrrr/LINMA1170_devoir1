#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "lu.h"

#define TOL 1e-9


void test_lu_function(){

    //  2  5
    //  1  2

    Matrix * M = allocate_matrix(2, 2);
    M->a[0][0] = 2.0;
    M->a[0][1] = 5.0;
    M->a[1][0] = 1.0;
    M->a[1][1] = 2.0;

    assert(!lu(M));

    double true_lu0[2][2] = {{2, 5}, {0.5, -0.5}};
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            assert(fabs(M->a[i][j] - true_lu0[i][j]) < TOL);
        }
    }
    free_matrix(M);

    //  2 -1  0
    // -1  2 -1
    //  0 -1  2

    Matrix * A = allocate_matrix(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) A->a[i][j] = 2.0;
            else if (i + j == 2) A->a[i][j] = 0.0;
            else A->a[i][j] = -1.0;
        }
    }

    assert(!lu(A));

    double true_lu[3][3] = {{2.0, -1.0, 0.0}, {-0.5, 1.5, -1.0}, {0.0, -2.0/3.0, 4.0/3.0}};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            assert(fabs(A->a[i][j] - true_lu[i][j]) < TOL);
        }
    }

    Matrix *L = allocate_matrix(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < i; j++) {
            L->a[i][j] = A->a[i][j];
        }
        L->a[i][i] = 1.0;
    }

    Matrix *U = allocate_matrix(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = i; j < 3; j++) {
            U->a[i][j] = A->a[i][j];
        }
    }

    mult_matrix(L, U, A);
    double true_A[3][3] = {{2.0, -1.0, 0.0}, {-1.0, 2.0, -1.0}, {0.0, -1.0, 2.0}};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            assert(fabs(A->a[i][j] - true_A[i][j]) < TOL);
        }
    }
    free_matrix(L);
    free_matrix(U);
    free_matrix(A);


    // pas de rang plein

    Matrix * B = allocate_matrix(3, 3);
    B->a[0][0] = 1.0;
    B->a[0][1] = 2.0;
    B->a[0][2] = 1.0;
    B->a[1][0] = -2.0;
    B->a[1][1] = -3.0;
    B->a[1][2] = 1.0;
    B->a[2][0] = 3.0;
    B->a[2][1] = 5.0;
    B->a[2][2] = 0.0;

    assert(lu(B) == -1);
    free_matrix(B);


    //      2  3  4     
    // C =  5  6  7     (pivot nul en dernière position de la diagonale)
    //      8  9  10

    Matrix * C = allocate_matrix(3, 3);
    C->a[0][0] = 2.0;
    C->a[0][1] = 3.0;
    C->a[0][2] = 4.0;
    C->a[1][0] = 5.0;
    C->a[1][1] = 6.0;
    C->a[1][2] = 7.0;
    C->a[2][0] = 8.0;
    C->a[2][1] = 9.0;
    C->a[2][2] = 10.0;

    assert(lu(C) == -1);

    free_matrix(C);
    

    // matrices aléatoires, on vérifie que D = L@U

    for (int n = 8; n < 2048; n *= 2) {
        Matrix * D = allocate_matrix(n, n);
        Matrix * true_D = allocate_matrix(n, n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                double random;
                if (i == j) {
                    random = drand48();
                    D->a[i][j] = random + (double) n;
                    true_D->a[i][j] = random + (double) n;
                } else {
                    random = drand48();
                    D->a[i][j] = random;
                    true_D->a[i][j] = random;
                }
            }
        }
        assert(!lu(D));

        Matrix *DL = allocate_matrix(n, n);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                DL->a[i][j] = D->a[i][j];
            }
            DL->a[i][i] = 1.0;
        }

        Matrix *DU = allocate_matrix(n, n);
        for (int i = 0; i < n; i++) {
            for (int j = i; j < n; j++) {
                DU->a[i][j] = D->a[i][j];
            }
        }

        mult_matrix(DL, DU, D);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                assert(fabs(D->a[i][j] - true_D->a[i][j]) < TOL);
            }
        }

        free_matrix(DL);
        free_matrix(DU);
        free_matrix(D);
        free_matrix(true_D);
    }
}

void test_solve(){

    // A = 2  5       b = 21     Mx = b
    //     1  2           8

    Matrix * M = allocate_matrix(2, 2);
    M->a[0][0] = 2.0;
    M->a[0][1] = 5.0;
    M->a[1][0] = 1.0;
    M->a[1][1] = 2.0;

    lu(M);
    double *b = (double *) malloc(sizeof(double)*2);
    *b = 21.0;
    *(b + 1) = 8.0;

    assert(!solve(M, b));
    double true_sol0[2] = {-2.0, 5.0};

    for (int i = 0; i < 2; i++) {
        assert(fabs(b[i] - true_sol0[i]) < TOL);
    }

    free_matrix(M);
    free(b);

    //      2 -1  0        7     
    // A = -1  2 -1    y = 5     Ax = y
    //      0 -1  2        6

    Matrix * A = allocate_matrix(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) A->a[i][j] = 2.0;
            else if (i + j == 2) A->a[i][j] = 0.0;
            else A->a[i][j] = -1.0;
        }
    }

    lu(A);
    double *y = (double *) malloc(sizeof(double)*3);
    *y = 7.0;
    *(y + 1) = 5.0;
    *(y + 2) = 6.0;

    assert(!solve(A, y));
    double true_sol[3] = {37.0/4.0, 23.0/2.0, 35.0/4.0};

    for (int i = 0; i < 3; i++) {
        assert(fabs(y[i] - true_sol[i]) < TOL);
    }
    
    free_matrix(A);
    free(y);
}

void test_choleski(){

    //  2 -1  1
    // -1  2 -1
    //  1 -1  2

    double true_A[3][3] = {{2.0, -1.0, 1.0}, {-1.0, 2.0, -1.0}, {1.0, -1.0, 2.0}};

    Matrix * A = allocate_matrix(3, 3);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            A->a[i][j] = true_A[i][j];
        }
    }

    assert(!cholesky(A));

    double true_R[3][3] = {{sqrt(2), -sqrt(2)/2, sqrt(2)/2}, {0, sqrt(6)/2, -sqrt(6)/6}, {0, 0, 2*sqrt(3)/3}};
    for (int i = 0; i < 3; i++) {
        for (int j = i; j < 3; j++) {
            assert(fabs(A->a[i][j] - true_R[i][j]) < TOL);
        }
    }

    Matrix * R = allocate_matrix(3, 3);
    Matrix * RT = allocate_matrix(3, 3);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (j < i) {
                R->a[i][j] = 0.0;
                RT->a[j][i] = 0.0;
            } else {
                R->a[i][j] = A->a[i][j];
                RT->a[j][i] = A->a[i][j];
            }
        }
    }

    assert(!mult_matrix(RT, R, A));
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            assert(fabs(A->a[i][j] - true_A[i][j]) < TOL);
        }
    }
    free_matrix(R);
    free_matrix(RT);
    free_matrix(A);
}

int main(){
    
    test_lu_function();
    //test_solve();
    //test_choleski();

    return 0;
}