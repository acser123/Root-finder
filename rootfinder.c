/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
/* Finds roots of expression in  the first argument expression:
Example
$ rootfinder "(2*asin((x*x-2)/2)-x*x*(2*asin(x/2)-3.1415)-x*sqrt(4-x*x))/4" -1 2 100
result: Bisection: -0.53168220620747908001
result: Bisection: 1.15875664258346633417
Time spent: 0.00772799999999999963

*/   

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "tinyexpr.h"

#define EPSILON 0.000000000001 // tolerance, do not decreaase, this is all 'double' in C can do
#define DX      0.000000000001      // dx, delta x: for numerical differentiation, do not decrease, this is all 'float in C can do

char *function_arg;

// Function to find the roots for
double f(double x_h) {
 // Initialize the expression evaluator
 double x;
 te_variable vars[] = {{"x", &x}};
 /* This will compile the expression and check for errors. */
 int err;
 te_expr *n = te_compile(function_arg, vars, 1, &err);
 x=x_h;
 double r;
 if (n) {
  r = te_eval(n);
  te_free(n);
 }  else {

   //printf("*debug* Error x=%f; r=%f\n", x, r);
 }
 //printf("*debug* x=%f; r=%f\n", x, r);
 return r;

// return (2*asin((x*x-2)/2)-x*x*(2*asin(x/2)-M_PI)-x*pow(4-x*x, 0.5))/4;
 // return ((x+1)*(x+1)*(x-1)*x*(x+5))*(2*asin((x*x-2)/2)-x*x*(2*asin(x/2)-M_PI)-x*pow(4-x*x, 0.5))/4;
 // return ((x+1)*(x+1)*(x-1)*x*(x+5));
 //return ((x-3)*(x-3)*(x+5));
// return(-x*x*x-3*x*x+2*x-5);
// return (sin(x)/(x*x-x+5));
} 

// numerical differentiation of the function
double df(double x) {
 return ((f(x+DX)-f(x))/DX);
}

// Simple bisection algorithm using the f function
double bisectf(double from, double to) {

 double a = from;
 double b = to;
 double c; 
 double fc; 

 
// Ensuring convergence
 if (f(a)>f(b)) { 
  c=a; a=b; b=c;
 }

 int i = 0;

 // Bisection algorithm
 do {
  c = (a+b)/2;
  // call the function
  fc = f(c);
  
  if (fc<=0) { 
   a=c; } else {
   b=c;
  }
  i++;
  //printf("*debug* i=%d;  a=%.20f;  b=%.20f;  c=%.20f;  fc=%.20f\n", i, a, b, c, fc);  
 } while (fabs(a-b)>EPSILON);
 return c;
}


// Simple bisection algorithm using the f' (df/dx) function
double bisectdf(double from, double to) {

 double a = from;
 double b = to;
 double c; 
 double fc; 

 
// Ensuring convergence
 if (df(a)>df(b)) { 
  c=a; a=b; b=c;
 }

 int i = 0;

 // Bisection algorithm
 do {
  c = (a+b)/2;
  // call the function
  fc = df(c);
  
  if (fc<=0) { 
   a=c; } else {
   b=c;
  }
  i++;
  //printf("i=%d;  a=%.20f;  b=%.20f;  c=%.20f;  fc=%.20f\n", i, a, b, c, fc);  
 } while (fabs(a-b)>EPSILON);
 return c;
}

// main
int main(int argc, char* argv[]) {

 // Convert first two arguments to double a=lower bound, b=upper bound
 clock_t begin = clock();

 // Get the expression from the command line
 function_arg = argv[1];


 double from = strtod(argv[2], NULL); // beginning of the range to look for roots
 double to   = strtod(argv[3], NULL); // end of the range to look for roots
 double num_seg   = strtod(argv[4], NULL); // number of intervals between from and to
 double result;
 int i=0; // counter
 double h = (to - from)/num_seg; // this is the length of an interval segment
 
double ih, iph, fih, fiph; // variables to store precalculated values to avoid double+ computation
int flag;

 do {
  flag = 1;
// beginning of the interval
  ih = from + i*h;
// end of the interval
  iph = from + (i+1)*h;
// function value at the beginning of the interval
  fih = f(ih);
// function value at the end of the interval
  fiph= f(iph);
// debug
//  printf("*debug* i=%i; flag=%d; ih=%.10f; iph=%.10f; fih=%.10f; fiph=%0.10f;\n", i, flag, ih, iph, fih, fiph); 
// function crosses the x axis and has a root/zero in the interval
  if ((flag==1) && fabs(fih-0)<EPSILON) {  
   result = ih;
   printf("result: Zero at fih: %.20f\n", result);  
   flag = 0;
  } 
// Bisection
  if ((flag==1) && (fih*fiph<0)) {  
   result = bisectf (ih, iph);
   printf("result: Bisection: %.20f\n", result);  
   flag = 0;
  } 
// Otherwise we are looking for a min/max or inflexion point where the first derivative is 0
  if ((flag==1) && (df(ih)*df(iph)<0)) {
   // Looking for the zero value of the first derivative of the function
   result = bisectdf (ih, iph);
   // but it's only a root if the function is zero here, otherwise it's only an inflexion point
   if (fabs(f(result)-0)<EPSILON){
    printf("result: df/dx=0: %.20f\n", result);  
   }
   flag = 0;
  }

  i++;
 } while (iph <= to);

 // End timer, print timing information
 clock_t end = clock();
 double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
 //printf("Time spent: %.20f\n",  time_spent);
 return 0;
}
