//
//  Library file for polygon functions
//  
//
//  Created by chris burnham on 10/1/14.
//  Authors: Chris Burnham, Victoria Tisdale
//

#include <stdlib.h>
#include <stdio.h>
#include "ppmIO.h"
#include "image.h"
#include "objects.h"
#include "polygon.h"


// returns an allocated Polygon pointer initialized so that numVertex is 0 and
// vertex is NULL.
Polygon *polygon_create(){
    Polygon *p;
    
    p = malloc(sizeof(Polygon));
    
    p->zBuffer = 1;
    p->nVertex = 0;
    p->vertex = NULL;
    
    return p;
}

// returns an allocated Polygon pointer with the vertex list initialized to a
// copy of the points in vlist.
Polygon *polygon_createp(int numV, Point *vlist){
    Polygon *p;
    int i;
    
	p = malloc(sizeof(Polygon));
    
	p->zBuffer = 1;
	p->nVertex = numV;
	p->vertex = malloc(sizeof(Point) * numV);
	for(i=0; i<numV; i++){
        p->vertex[i] = vlist[i];
    }
    
    return p;
}

// frees the internal data for a Polygon and the Polygon pointer.
void polygon_free(Polygon *p){
    if(p){
		if(p->vertex){
			free(p->vertex);
		}
		free(p);
	}
}

// initializes the existing Polygon to an empty Polygon.
void polygon_init(Polygon *p){
  if(p){
		p->zBuffer = 1;
		p->nVertex = 0;
    if(p->vertex){
			free(p->vertex);
		}
		p->vertex = NULL;
	}
}

// initializes the vertex array to the points in vlist.
void polygon_set(Polygon *p, int numV, Point *vlist){
    int i;
    
	if(p){
		if(p->vertex){
			free(p->vertex);
		}
		p->zBuffer = 1;
		p->nVertex = numV;
		p->vertex = malloc(sizeof(Point) * numV);
        for(i=0; i<numV; i++){
            p->vertex[i] = vlist[i];
        }
	}
}

// frees the internal data and resets the fields.
void polygon_clear(Polygon *p){
    if(p){
		if(p->vertex){
			free(p->vertex);
		}
		p->zBuffer = 1;
		p->nVertex = 0;
		p->vertex = NULL;
	}
}

// sets the z-buffer flag to the given value.
void polygon_zBuffer(Polygon *p, int flag){
    if(p){
		p->zBuffer = flag;
	}
}

// De-allocates/allocates space and copies the vertex and color data from one
// polygon to the other.
void polygon_copy(Polygon *to, Polygon *from){
    if(from){
		polygon_set(to, from->nVertex, from->vertex);
		polygon_zBuffer(to, from->zBuffer);
	}
}

// prints polygon data to the stream designated by the FILE pointer.
void polygon_print(Polygon *p, FILE *fp){
    int i;
	
	if(p){
		fprintf(fp, "Polyline:\n");
		if(p->vertex){
			for(i = 0; i < p->nVertex; i++){
				fprintf(fp, "(%f,%f,%f) , ", p->vertex[i].val[0], p->vertex[i].val[1], p->vertex[i].val[2]);
			}
			fprintf(fp, "\n");
		}
	}
}

// draw the outline of the polygon using color c.
void polygon_draw(Polygon *p, Image *src, Color c){
    int i;
	Line edge;
    
	if(p){
		if(p->vertex){
			for(i = 1; i < p->nVertex; i++){
				line_set(&edge, p->vertex[i-1],  p->vertex[i]);
				line_draw(&edge, src, c);
			}
            line_set(&edge, p->vertex[0],  p->vertex[p->nVertex - 1]);
            line_draw(&edge, src, c);
		}
	}
}

// draw the filled polygon using color c with the scanline rendering algorithm.
void polygon_drawFill(Polygon *p, Image *src, Color c){
    
    printf("0");
    
}

// draw the filled polygon using color c with the Barycentric coordinates
// algorithm. Will draw for the zero case.
void polygon_drawFillB(Polygon *p, Image *src, Color c){
    float a, b, g, dXab, dYab, dXac, dYac, xyabDiff, xyacDiff, abDenom, acDenom,
		epsilon, Xa, Xb, Xc, Ya, Yb, Yc;
    int maxX, minY, maxY,  i, j;
    
    if(p){
        if(p->nVertex == 3){
						Xa = p->vertex[0].val[0];
						Ya = p->vertex[0].val[1];
						Xb = p->vertex[1].val[0];
						Yb = p->vertex[1].val[1];
						Xc = p->vertex[2].val[0];
						Yc = p->vertex[2].val[1];
						
						maxX = (int)(Xa) + 1;
						if( ((int)(Xb) + 1) > maxX ){
							maxX = (int)(Xb) + 1;
            }
						if( ((int)(Xc) + 1) > maxX ){
							maxX = (int)(Xc) + 1;
            }

						i = (int)(Xa);
            if( (int)(Xb) < i ){
              i = (int)(Xb);
            }
						if( (int)(Xc) < i ){
              i = (int)(Xc);
            }

            maxY = (int)(Ya) + 1;
						if( ((int)(Yb) + 1) > maxY ){
							maxY = (int)(Yb) + 1;
            }
						if( ((int)(Yc) + 1) > maxY ){
							maxY = (int)(Yc) + 1;
            }

						minY = (int)(Ya);
            if( (int)(Yb) < minY ){
              minY = (int)(Yb);
            }
						if( (int)(Yc) < minY ){
              minY = (int)(Yc);
            }

            if( maxX > i && maxY > minY ){
            // if this is false something about our bounding box is wrong

                epsilon = -.00001; // closer than this we consider zero
                
                //calculating peices for beta and gama that are not point dependent
                dXab = Xb - Xa;
                dYab = Ya - Yb;
                dXac = Xa - Xc;
                dYac = Yc - Ya;
                
                xyabDiff = (Xa * Yb) - (Xb * Ya);
                
                xyacDiff = (Xc * Ya) - (Xa * Yc);
                
                abDenom = (dYab * Xc) + (dXab * Yc) + xyabDiff;
                
                acDenom = (dYac * Xb) + (dXac * Yb) + xyacDiff;

                for(; i<maxX; i++){ // loop through bounding box
                    for(j = minY; j<maxY; j++){
                        b = ( (dYac * (i + 0.5)) + (dXac * (j + 0.5)) + xyacDiff)
                        / acDenom;
                        if( b > epsilon ){
                            g = ( (dYab * (i + 0.5)) + (dXab * (j + 0.5))
                            + xyabDiff) / abDenom;
                            if( g > epsilon ){
                                a = 1.0 - b - g;
                                if( a > epsilon ){
																		src->data[j][i].rgb[0] = c.c[0];
                                    src->data[j][i].rgb[1] = c.c[1];
                                    src->data[j][i].rgb[2] = c.c[2];
                                }
                            }
                        }
                    }
                }
            }
						else{
							printf("bonding box error:\nminX:%i, maxX:%i, minY:%i, maxY:%i\n", i, maxX, minY, maxY);
						}
        }
        else{
            printf("Not given a triangle. Given polygon has %i verticies", p->nVertex);
        }
    }
}




