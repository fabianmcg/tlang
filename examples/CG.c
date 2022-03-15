#include "math.h"

extern void timer_start(int n);
extern void timer_stop(int n);
extern double randlc(double *x, double a);
extern void vranlc(int n, double *x, double a, double y[]);
extern double fmax(double, double);

void conj_grad(int *colidx, int *rowstr, double *x, double *z, double *a,
               double *p, double *q, double *r, double *rnorm, int firstrow,
               int lastrow, int firstcol, int lastcol, int naa) {
  int j, k;
  int cgit, cgitmax = 25;
  double d, sum, rho, rho0, alpha, beta;

  rho = 0.0;

  for (j = 0; j < naa + 1; j++) {
    q[j] = 0.0;
    z[j] = 0.0;
    r[j] = x[j];
    p[j] = r[j];
  }

  for (j = 0; j < lastcol - firstcol + 1; j++) {
    rho = rho + r[j] * r[j];
  }

  for (cgit = 1; cgit <= cgitmax; cgit++) {

    for (j = 0; j < lastrow - firstrow + 1; j++) {
      sum = 0.0;
      for (k = rowstr[j]; k < rowstr[j + 1]; k++) {
        sum = sum + a[k] * p[colidx[k]];
      }
      q[j] = sum;
    }

    d = 0.0;
    for (j = 0; j < lastcol - firstcol + 1; j++) {
      d = d + p[j] * q[j];
    }

    alpha = rho / d;

    rho0 = rho;

    rho = 0.0;
    for (j = 0; j < lastcol - firstcol + 1; j++) {
      z[j] = z[j] + alpha * p[j];
      r[j] = r[j] - alpha * q[j];
    }

    for (j = 0; j < lastcol - firstcol + 1; j++) {
      rho = rho + r[j] * r[j];
    }

    beta = rho / rho0;

    for (j = 0; j < lastcol - firstcol + 1; j++) {
      p[j] = r[j] + beta * p[j];
    }
  } // end of do cgit=1,cgitmax
  sum = 0.0;
  for (j = 0; j < lastrow - firstrow + 1; j++) {
    d = 0.0;
    for (k = rowstr[j]; k < rowstr[j + 1]; k++) {
      d = d + a[k] * z[colidx[k]];
    }
    r[j] = d;
  }
  for (j = 0; j < lastcol - firstcol + 1; j++) {
    d = x[j] - r[j];
    sum = sum + d * d;
  }

  *rnorm = sqrt(sum);
}

void CG(double *Z, int *colidx, int *rowstr, double *x, double *z, double *a, double *p,
        double *q, double *r, double amult, double tran, int NA, int NITER,
        int naa, int nzz, int firstrow, int lastrow, int firstcol, int lastcol,
        int timeron, double SHIFT) {
  int i, j, k, it;

  double zeta;
  double rnorm;
  double norm_temp1, norm_temp2;


  for (j = 0; j < lastrow - firstrow + 1; j++) {
    for (k = rowstr[j]; k < rowstr[j + 1]; k++) {
      colidx[k] = colidx[k] - firstcol;
    }
  }

  for (i = 0; i < NA + 1; i++) {
    x[i] = 1.0;
  }
  for (j = 0; j < lastcol - firstcol + 1; j++) {
    q[j] = 0.0;
    z[j] = 0.0;
    r[j] = 0.0;
    p[j] = 0.0;
  }

  zeta = 0.0;

  for (it = 1; it <= 1; it++) {
    conj_grad(colidx, rowstr, x, z, a, p, q, r, &rnorm, firstrow, lastrow,
              firstcol, lastcol, naa);

    norm_temp1 = 0.0;
    norm_temp2 = 0.0;
    for (j = 0; j < lastcol - firstcol + 1; j++) {
      norm_temp1 = norm_temp1 + x[j] * z[j];
      norm_temp2 = norm_temp2 + z[j] * z[j];
    }

    norm_temp2 = 1.0 / sqrt(norm_temp2);
    for (j = 0; j < lastcol - firstcol + 1; j++) {
      x[j] = norm_temp2 * z[j];
    }
  } // end of do one iteration untimed

  for (i = 0; i < NA + 1; i++) {
    x[i] = 1.0;
  }

  zeta = 0.0;

  timer_stop(0);

  //   printf(" Initialization time = %15.3f seconds\n", timer_read(0));

  timer_start(1);

  for (it = 1; it <= NITER; it++) {
    if (timeron)
      timer_start(2);
    conj_grad(colidx, rowstr, x, z, a, p, q, r, &rnorm, firstrow, lastrow,
              firstcol, lastcol, naa);
    if (timeron)
      timer_stop(2);

    norm_temp1 = 0.0;
    norm_temp2 = 0.0;
    for (j = 0; j < lastcol - firstcol + 1; j++) {
      norm_temp1 = norm_temp1 + x[j] * z[j];
      norm_temp2 = norm_temp2 + z[j] * z[j];
    }

    norm_temp2 = 1.0 / sqrt(norm_temp2);

    zeta = SHIFT + 1.0 / norm_temp1;
    //     if (it == 1)
    //       printf("\n   iteration           ||r||                 zeta\n");
    //     printf("    %5d       %20.14E%20.13f\n", it, rnorm, zeta);

    for (j = 0; j < lastcol - firstcol + 1; j++) {
      x[j] = norm_temp2 * z[j];
    }
  } // end of main iter inv pow meth
  *Z = zeta;
  timer_stop(1);
}
