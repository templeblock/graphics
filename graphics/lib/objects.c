/* methods for objects (Point, Line, Circle, Ellipse, Polyline) 
Chris Burnham
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image.h"
#include "ppmIO.h"
#include "objects.h"

/* Point functions */
//set the first two values of the vector to x and y. Set the third value to 0.0 and the fourth value to 1.0
void point_set2D(Point *p, double x, double y){
	p->val[0] = x;
	p->val[1] = y;
	p->val[2] = 0.0;
	p->val[3] = 1.0;
}

//set the point’s values to x and y and z. Set the homogeneous coordinate to 1.0
void point_set3D(Point *p, double x, double y, double z){
	p->val[0] = x;
	p->val[1] = y;
	p->val[2] = z;
	p->val[3] = 1.0;
}

//set the four values of the vector to x, y, z, and h, respectively
void point_set(Point *p, double x, double y, double z, double h){
	p->val[0] = x;
	p->val[1] = y;
	p->val[2] = z;
	p->val[3] = h;
}

//copy the point data structure
void point_copy(Point *to, Point *from){
	*to = *from;
}

//draw the point into src using color c
void point_draw(Point *p, Image *src, Color c){
	image_setColor(src, p->val[0], p->val[1], c);
}

//draw the p into src using FPixel c
void point_drawf(Point *p, Image *src, FPixel c){
	image_setf(src, p->val[0], p->val[1], c);
}

/* Line functions */
//initialize a 2D line
void line_set2D(Line *l, double x0, double y0, double x1, double y1){
	l->zBuffer = 1;
	point_set2D(&l->a, x0, y0);
	point_set2D(&l->b, x1, y1);
}

//initialize a line to ta and tb
void line_set(Line *l, Point ta, Point tb){
	l->zBuffer = 1;
	l->a = ta;
	l->b = tb;
}

//set the z-buffer flag to the given value
void line_zBuffer(Line *l, int flag){
	l->zBuffer = flag;
}

//copy the line data structure
void line_copy(Line *to, Line *from){
	*to = *from;
}

//draw the line into src using color c
void line_draw(Line *l, Image *src, Color c){
	int i, x, y, dx, dy, e;
	double a[2];
	double b[2];

	if( (int)(l->a.val[0]) == (int)(l->b.val[0]) ){
		if( (int)(l->a.val[1]) != (int)(l->b.val[1]) ){
			x = (int)(l->a.val[0]);
			if(l->a.val[1] < l->b.val[1]){
				y = (int)l->a.val[1];
				dy = (int)(l->b.val[1] - l->a.val[1]);
			}
			else{
				y = (int)l->b.val[1];
				dy = (int)(l->a.val[1] - l->b.val[1]);
			}
			for(i = 0; i < dy; i++){
				image_setColor(src, y + i, x, c);
			}
		}
		return;
	}
	else if( (int)(l->a.val[1]) == (int)(l->b.val[1]) ){
		if( (int)(l->a.val[0]) != (int)(l->b.val[0]) ){
			y = (int)(l->a.val[1]);
			if(l->a.val[0] < l->b.val[0]){
				x = (int)l->a.val[0];
				dx = (int)(l->b.val[0] - l->a.val[0]);
			}
			else{
				x = (int)l->b.val[0];
				dx = (int)(l->a.val[0] - l->b.val[0]);
			}
			for(i = 0; i < dx; i++){
				image_setColor(src, y, x + i, c);
			}
		}
		return;
	}
	else if(l->a.val[1] < l->b.val[1]){
		a[0] = l->a.val[0];
		a[1] = l->a.val[1];
		b[0] = l->b.val[0];
		b[1] = l->b.val[1];
	}
	else{
		b[0] = l->a.val[0];
		b[1] = l->a.val[1];
		a[0] = l->b.val[0];
		a[1] = l->b.val[1];
	}
	
	if(a[0] < b[0]){
		if((b[1] - a[1]) < (b[0] - a[0])){
			x = (int)a[0];
			y = (int)a[1];
			dx = (int)(b[0] - a[0]);
			dy = (int)(b[1] - a[1]);
			e = 2*dy - dx;

			for(i=0; i<dx; i++){
				image_setColor(src, y, x, c);
				while(e > 0){
					y = y + 1;
					e = e - 2*dx;
				}			
				x = x + 1;
				e = e + 2*dy;
			}
		}
		else{
			x = (int)a[0];
			y = (int)a[1];
			dx = (int)(b[0] - a[0]);
			dy = (int)(b[1] - a[1]);
			e = 2*dx - dy;

			for(i=0; i<dy; i++){
				image_setColor(src, y, x, c);
				while(e > 0){
					x = x + 1;
					e = e - 2*dy;
				}			
				y = y + 1;
				e = e + 2*dx;
			}
		}
	}
	else{
		if((b[1] - a[1]) < (a[0] - b[0])){
			x = (int)a[0];
			y = (int)a[1];
			dx = (int)(a[0] - b[0]);
			dy = (int)(b[1] - a[1]);
			e = 2*dy - dx;

			for(i=0; i<dx; i++){
				image_setColor(src, y, x, c);
				while(e > 0){
					y = y + 1;
					e = e - 2*dx;
				}			
				x = x - 1;
				e = e + 2*dy;
			}
		}
		else{
			x = (int)a[0];
			y = (int)a[1];
			dx = (int)(a[0] - b[0]);
			dy = (int)(b[1] - a[1]);
			e = 2*dx - dy;

			for(i=0; i<dy; i++){
				image_setColor(src, y, x, c);
				while(e > 0){
					x = x - 1;
					e = e - 2*dy;
				}			
				y = y + 1;
				e = e + 2*dx;
			}
		}
	}
}

/* Circle functions */
//initialize to center tc and radius tr
void circle_set(Circle *c, Point tc, double tr){
	c->c = tc;
	c->r = tr;
}

//draw the circle into src using color p
void circle_draw(Circle *c, Image *src, Color p){
	int x, y, p2, cx, cy;	

	x = -1;
	y = -(int)(c->r);
	p2 = 1 + y;
	cx = c->c.val[0];
	cy = c->c.val[1];
	
	image_setColor(src, x + cy, y + cx, p);
	image_setColor(src, -x - 1 + cy, y + cx, p);
	image_setColor(src, -y - 1 + cy, x + cx, p);
	image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
	image_setColor(src, -x - 1 + cy, -y - 1 + cx, p);
	image_setColor(src, x + cy, -y - 1 + cx, p);
	image_setColor(src, y + cy, -x - 1 + cx, p);
	image_setColor(src, y + cy, x + cx, p);

	while( x > y ){
		x--;
		if( p2 < 0){
			p2 = p2 + 1 - 2*x;
		}
		else{
			y++;
			p2 = p2 + 1 - 2*(x-y);
		}
		image_setColor(src, x + cy, y + cx, p);
		image_setColor(src, -x - 1 + cy, y + cx, p);
		image_setColor(src, -y - 1 + cy, x + cx, p);
		image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
		image_setColor(src, -x - 1 + cy, -y - 1 + cx, p);
		image_setColor(src, x + cy, -y - 1 + cx, p);
		image_setColor(src, y + cy, -x - 1 + cx, p);
		image_setColor(src, y + cy, x + cx, p);
	}
}

//draw a filled circle into src using color p
void circle_drawFill(Circle *c, Image *src, Color p){
	int x, y, p2, cx, cy, i;	

	x = -1;
	y = -(int)(c->r);
	p2 = 1 + y;
	cx = c->c.val[0];
	cy = c->c.val[1];
	for(i = 0; i > 2*y + 1; i--){
		image_setColor(src, x + cy, -y - 1 + i + cx, p);
		image_setColor(src, -x - 1 + cy, -y - 1 + i + cx, p);
	}

	for(i = 0; i > 2*x + 1; i--){
		image_setColor(src, y + cy, -x - 1 + i + cx, p);
		image_setColor(src, -y - 1 + cy, -x - 1 + i + cx, p);
	}
	
	while( x > y ){
		x--;
		if( p2 < 0){
			p2 = p2 + 1 - 2*x;
		}
		else{
			y++;
			p2 = p2 + 1 - 2*(x-y);
		}
		for(i = 0; i > 2*y + 1; i--){
			image_setColor(src, x + cy, -y - 1 + i + cx, p);
			image_setColor(src, -x - 1 + cy, -y - 1 + i + cx, p);
		}

		for(i = 0; i > 2*x + 1; i--){
			image_setColor(src, y + cy, -x - 1 + i + cx, p);
			image_setColor(src, -y - 1 + cy, -x - 1 + i + cx, p);
		}
	}
}

/* Ellipse function */
//initialize an ellipse to location tc and radii ta and tb
void ellipse_set(Ellipse *e, Point tc, double ta, double tb){
	e->ra = ta;
	e->rb = tb;
	e->c = tc;
}

//draw into src using color p
void ellipse_draw(Ellipse *e, Image *src, Color p){
	int x, y, cx, cy, rx, ry, px, py, p2;

	cx = (int)(e->c.val[0]);
	cy = (int)(e->c.val[1]);
	rx = (int)(e->ra);
	ry = (int)(e->rb);
	
	x = -1;
	y = -ry;
	px = 2 * ry * ry;
	py = 2 * rx * rx * -y;

	image_setColor(src, y + cy, x + cx, p);
	image_setColor(src, y + cy, -x - 1 + cx, p);
	image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
	image_setColor(src, -y - 1 + cy, x + cx, p);

	p2 = ry*ry - rx*rx * ry + rx*rx/4 + ry*ry + px;

	while( px < py ){
		x--;
		px = px + 2*ry*ry;
		if(p2 < 0){
			p2 = p2 + ry*ry + px;
		}
		else{
			y++;
			py = py - 2*rx*rx;
			p2 = p2 + ry*ry + px - py;
		}
		image_setColor(src, y + cy, x + cx, p);
		image_setColor(src, y + cy, -x - 1 + cx, p);
		image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
		image_setColor(src, -y - 1 + cy, x + cx, p);
	}

	p2 = ry*ry * (x*x + x) + rx*rx * (y*y - 2*y + 1) - rx*rx * ry*ry + rx*rx - py;
	
	while( y < 0 ){
		y++;
		py = py -2*rx*rx;
		if(p2 > 0){
			p2 = p2 + rx*rx - py;
		}
		else{
			x--;
			px = px + 2*ry*ry;
			p2 = p2 + rx*rx - py +px;
		}
		image_setColor(src, y + cy, x + cx, p);
		image_setColor(src, y + cy, -x - 1 + cx, p);
		image_setColor(src, -y - 1 + cy, -x - 1 + cx, p);
		image_setColor(src, -y - 1 + cy, x + cx, p);
	}
}

//draw a filled ellipse into src using color p
void ellipse_drawFill(Ellipse *e, Image *src, Color p){
int x, y, cx, cy, rx, ry, px, py, p2, i;

	cx = (int)(e->c.val[0]);
	cy = (int)(e->c.val[1]);
	rx = (int)(e->ra);
	ry = (int)(e->rb);
	
	x = -1;
	y = -ry;
	px = 2 * ry * ry;
	py = 2 * rx * rx * -y;

	for(i = 0; i > 2*x + 1; i--){
		image_setColor(src, y + cy, x + cx - i, p);
		image_setColor(src, -y - 1 + cy, x + cx - i, p);
	}

	p2 = ry*ry - rx*rx * ry + rx*rx/4 + ry*ry + px;

	while( px < py ){
		x--;
		px = px + 2*ry*ry;
		if(p2 < 0){
			p2 = p2 + ry*ry + px;
		}
		else{
			y++;
			py = py - 2*rx*rx;
			p2 = p2 + ry*ry + px - py;
		}
		for(i = 0; i > 2*x + 1; i--){
			image_setColor(src, y + cy, x + cx - i, p);
			image_setColor(src, -y - 1 + cy, x + cx - i, p);
		}
	}

	p2 = ry*ry * (x*x + x) + rx*rx * (y*y - 2*y + 1) - rx*rx * ry*ry + rx*rx - py;
	
	while( y < 0 ){
		y++;
		py = py -2*rx*rx;
		if(p2 > 0){
			p2 = p2 + rx*rx - py;
		}
		else{
			x--;
			px = px + 2*ry*ry;
			p2 = p2 + rx*rx - py +px;
		}
		for(i = 0; i > 2*x + 1; i--){
			image_setColor(src, y + cy, x + cx - i, p);
			image_setColor(src, -y - 1 + cy, x + cx - i, p);
		}
	}
}

/* Polyline functions */
//returns an allocated Polyline pointer initialized so that numVertex is 0 and vertex is NULL
Polyline *polyline_create(){
	Polyline *p;
	
	p = malloc(sizeof(Polyline));

	p->zBuffer = 1;
	p->numVertex = 0;
	p->vertex = NULL;

	return p;
}

//returns an allocated Polyline pointer with the vertex list initialized to the points in vlist
Polyline *polyline_createp(int numV, Point *vlist){
	Polyline *p;

	p = malloc(sizeof(Polyline));

	p->zBuffer = 1;
	p->numVertex = numV;
	p->vertex = malloc(sizeof(Point) * numV);
	for(i=0; i<numV; i++){
        p->vertex[i] = vlist[i];
    }

	return p;
}

//frees the internal data and the Polyline pointer
void polyline_free(Polyline *p){
	if(p){
		if(p->vertex){
			free(p->vertex);
		}
		free(p);
	}
}

//initializes the pre-existing Polyline to an empty Polyline
void polyline_init(Polyline *p){
	if(p){
		p->zBuffer = 1;
		p->numVertex = 0;
		p->vertex = NULL;
	}
}

//initializes the vertex list to the points in vlist. De-allocates/allocates the vertex list for p, as necessary
void polyline_set(Polyline *p, int numV, Point *vlist){
    int i;
    
	if(p){
		if(p->vertex){
			free(p->vertex);
		}
		p->zBuffer = 1;
		p->numVertex = numV;
		p->vertex = malloc(sizeof(Point) * numV);
        for(i=0; i<numV; i++){
            p->vertex[i] = vlist[i];
        }
	}
}

//frees the internal data for a Polyline, if necessary, and sets numVertex to 0 and vertex to NULL
void polyline_clear(Polyline *p){
	if(p){
		if(p->vertex){
			//free(p->vertex);
		}
		polyline_init(p);
	}
}

//sets the z-buffer flag to the given value
void polyline_zBuffer(Polyline *p, int flag){
	if(p){
		p->zBuffer = flag;
	}
}

//De-allocates/allocates space as necessary in the destination Polyline data structure and copies the vertex data from the source polyline (from) to the destination (to)
void polyline_copy(Polyline *to, Polyline *from){
	if(from){
		polyline_set(to, from->numVertex, from->vertex);
		polyline_zBuffer(to, from->zBuffer);
	}
}

//prints Polyline data to the stream designated by the FILE pointer
void polyline_print(Polyline *p, FILE *fp){
	int i;
	
	if(p){
		fprintf(fp, "Polyline:\n");
		if(p->vertex){
			for(i = 0; i < p->numVertex; i++){
				fprintf(fp, "(%f,%f,%f) , ", p->vertex[i].val[0], p->vertex[i].val[1], p->vertex[i].val[2]);
			}
			fprintf(fp, "\n");
		}
	}
}

//draw the polyline using color c
void polyline_draw(Polyline *p, Image *src, Color c){
	int i;
	Line *edge;

	if(p){
		if(p->vertex){
			edge = malloc(sizeof(Line));
			for(i = 1; i < p->numVertex; i++){
				line_set(edge, p->vertex[i-1],  p->vertex[i]);
				line_draw(edge, src, c);
			}
			free(edge);
		}
	}
}





