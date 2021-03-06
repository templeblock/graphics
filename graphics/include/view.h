/* view function and type defs
 Chris Burnham and Victoria Tisdale 10/16
 */

#ifndef VIEW_H
#define VIEW_H

#include "matrix.h"

typedef struct{
	Point vrp;
	double dx;
	Vector x;
	double screenx;
	double screeny;
} View2D;

typedef struct {
	Point vrp;
	Vector vpn;
	Vector vup;
	double d;
	double du;
	double dv;
	double f;
	double b;
	int screenx;
	int screeny;
} View3D;

//sets up the view
void view2D_set(View2D *view, Point *vrp, double dx, Vector *x, double screenx, double screeny);

//sets the vtm to be the view transformation defined by the 2D View structure
void matrix_setView2D(Matrix *vtm, View2D *view);

//3D viewing pipeline. VTM is complete view matrix. none of the values of the View structure should be edited.
void matrix_setView3D(Matrix *vtm, View3D *view);


#endif
