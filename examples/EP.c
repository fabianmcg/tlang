#include "math.h"

extern void timer_start(int n);
extern void timer_stop(int n);
extern double randlc(double *x, double a);
extern void vranlc(int n, double *x, double a, double y[]);
extern int f2i(double);


void EP(double *x, double *q, double A, double S, double *sxC, double *syC, double *gcC, int np, int NK, int MK, int NQ,
        int timers_enabled) {
  double t1, t2, t3, t4, x1, x2;
  double sx, sy, an, tt, gc;
  int i, ik, kk, l, k, nit;
  int k_offset;

  t1 = A;
  vranlc(0, &t1, A, x);
  
  t1 = A;
  
  for (i = 0; i < MK + 1; i++) {
    t2 = randlc(&t1, t1);
  }

  an = t1;
  tt = S;
  gc = 0.0;
  sx = 0.0;
  sy = 0.0;

  for (i = 0; i < NQ; i++) {
    q[i] = 0.0;
  }

  k_offset = -1;

  for (k = 1; k <= np; k++) {
    kk = k_offset + k;
    t1 = S;
    t2 = an;
    
    for (i = 1; i <= 100; i++) {
      ik = kk / 2;
      if ((2 * ik) != kk)
        t3 = randlc(&t1, t2);
      if (ik == 0)
        break;
      t3 = randlc(&t2, t2);
      kk = ik;
    }

    if (timers_enabled)
      timer_start(2);
    vranlc(2 * NK, &t1, A, x);
    if (timers_enabled)
      timer_stop(2);

    if (timers_enabled)
      timer_start(1);

    for (i = 0; i < NK; i++) {
      x1 = 2.0 * x[2 * i] - 1.0;
      x2 = 2.0 * x[2 * i + 1] - 1.0;
      t1 = x1 * x1 + x2 * x2;
      if (t1 <= 1.0) {
        t2 = sqrt(-2.0 * log(t1) / t1);
        t3 = (x1 * t2);
        t4 = (x2 * t2);
        l = fmax(fabs(t3), fabs(t4));
        q[l] = q[l] + 1.0;
        sx = sx + t3;
        sy = sy + t4;
      }
    }

    if (timers_enabled)
      timer_stop(1);
  }

  for (i = 0; i < NQ; i++) {
    gc = gc + q[i];
  }
  *sxC = sx;
  *syC = sy;
  *gcC = gc;
}
