/*
  Bruce A. Maxwell
  Fall 2014

	Skeleton scanline fill algorithm

	* edits by Victoria Tisdale *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "scanline.h"
#include "list.h"
#include "objects.h"
// #include "matrix.h"

// define the struct here, because it is local to only this file
typedef struct tEdge {
  float x0, y0;                   /* start point for the edge */
  float x1, y1;                   /* end point for the edge */
  int yStart, yEnd;               /* start row and end row */
  float xIntersect, dxPerScan;    /* where the edge intersects the current scanline and how it changes */
  float zIntersect, dzPerScan;      /* z-buffer things */
  Color cIntersect, dcPerScan;    /* gouraud shading */
  float tIntersect, dtPerScan;
  float sIntersect, dsPerScan;
  // need to add s and t stuff
  /* we'll add more here later */
  struct tEdge *next;
} Edge;

/********************
Scanline Fill Algorithm
********************/


/*
    This is a comparison function that returns a value < 0 if a < b, a
    value > 0 if a > b, and 0 if a = b.  It uses the yStart field of the
    Edge structure.  It is used to sort the overall edge list.
 */
static int compYStart( const void *a, const void *b ) {
    Edge *ea = (Edge *)a;
    Edge *eb = (Edge *)b;

    return(ea->yStart - eb->yStart);
}


/*
    This is a comparison function that returns a value < 0 if a < b, a
    value > 0 if a > b, and 0 if a = b.  It uses the xIntersect field of the
    Edge structure.  It is used to sort the active edge list.
 */
static int compXIntersect( const void *a, const void *b ) {
    Edge *ea = (Edge *)a;
    Edge *eb = (Edge *)b;

    if( ea->xIntersect < eb->xIntersect )
        return(-1);
    else if(ea->xIntersect > eb->xIntersect )
        return(1);

    return(0);
}

/*
    Allocates, creates, fills out, and returns an Edge structure given
    the inputs.

    Current inputs are just the start and end location in image space.
    Eventually, the points will be 3D and we'll add color and texture
    coordinates.
 */
static Edge *makeEdgeRec( Point start, Point end, Image *src, int zFlag, 
    int dsFlag, Color c1, Color c2, float* s)
{
  Edge *edge;
  float dscan = end.val[1] - start.val[1];

  if (start.val[1]>src->rows || end.val[1]<0){
    return ((Edge *)NULL);
    // print("returning NULL\n");
  }

  edge = malloc(sizeof(Edge));
  edge->x0 = start.val[0];
  edge->x1 = end.val[0];
  edge->y0 = start.val[1];
  edge->y1 = end.val[1];

  edge->yStart = (int)(edge->y0+0.5);
  edge->yEnd = (int)(edge->y1+0.5)-1;

  if (zFlag != 0){
    edge->zIntersect = 1/start.val[2];
    edge->dzPerScan = ((1/end.val[2])-(1/start.val[2]))/dscan;
  }

  if (edge->yEnd >= src->rows){
    edge->yEnd = src->rows-1;
  }

  edge->dxPerScan = ( edge->x1 - edge->x0 )/(dscan);
  edge->xIntersect = start.val[0];

  if (edge->y0 - floor(edge->y0) <= 0.5){
    edge->xIntersect += ((0.5-(floor(edge->y0)-edge->y0))*edge->dxPerScan);
  }

  else {
    edge->xIntersect += (((1.0-(floor(edge->y0)-edge->y0))+0.5)*edge->dxPerScan);
  }

  if (edge->y0 < 0){
    printf("*** yStart less than zero ***\n");
    edge->yStart = 0;
    edge->xIntersect += fabs(edge->y0)*edge->dxPerScan;
  }

  if (edge->dxPerScan > 0 && edge->xIntersect > edge->x1){
    edge->xIntersect = edge->x1;
  }
  else if (edge->dxPerScan < 0 && edge->xIntersect < edge->x1){
    edge->xIntersect = edge->x1;
  }

  if (dsFlag == 2){ 
    edge->dcPerScan.c[0] = ((c2.c[0]/end.val[2])-(c1.c[0]/start.val[2]))/dscan;
    edge->dcPerScan.c[1] = ((c2.c[1]/end.val[2])-(c1.c[1]/start.val[2]))/dscan;
    edge->dcPerScan.c[2] = ((c2.c[2]/end.val[2])-(c1.c[2]/start.val[2]))/dscan;
    edge->cIntersect.c[0] = c1.c[0]/start.val[2];
    edge->cIntersect.c[1] = c1.c[1]/start.val[2];
    edge->cIntersect.c[2] = c1.c[2]/start.val[2];
  }

  // texture mapping
  if (dsFlag == 3){
      
    float s0 = s[0];
    float t0 = s[1];
    float s1 = s[2];
    float t1 = s[3];
    // printf("t0 calc %f, %f\n", start.val[1] / midt, (asin(start.val[1] / midt)));
    // printf("t1 calc %f, %f\n", end.val[1] / midt, (asin(end.val[1] / midt)));
    edge->dsPerScan = ((s1/end.val[2])-(s0/start.val[2]))/dscan;
    edge->dtPerScan = ((t1/end.val[2])-(t0/start.val[2]))/dscan;
    edge->sIntersect = s0;
    edge->tIntersect = t0;
    printf("s0: %f,  t0: %f\n", s0, t0);
    printf("ds: %f,  dt: %f\n", edge->dsPerScan, edge->dtPerScan);
  }

  return( edge );
}


/*
    Returns a list of all the edges in the polygon in sorted order by
    smallest row.
*/
static LinkedList *setupEdgeList( Polygon *p, Image *src, int dsFlag) {
  LinkedList *edges = NULL;
  Point v1, v2;
  Color c1, c2;
  int i;
  float s[4];
  
  edges = ll_new();

  v1 = p->vertex[p->nVertex-1];
  if (dsFlag == 2){
    color_copy(&c1, &p->color[p->nVertex-1]);
  }
  else if (dsFlag == 3){
    s[0] = p->s[p->nVertex-1];
    s[1] = p->t[p->nVertex-1];
  }
  

  for(i=0;i<p->nVertex;i++) {
        
    v2 = p->vertex[i];
    if (dsFlag == 2){
      color_copy(&c2, &p->color[i]);
    }
    else if (dsFlag == 3){
      s[2] = p->s[i];
      s[3] = p->t[i];
    }
    
    if( (int)(v1.val[1]+0.5) != (int)(v2.val[1]+0.5) ) {
      Edge *edge;

      if( v1.val[1] < v2.val[1] ){
        if (dsFlag == 3){
          edge = makeEdgeRec( v1, v2, src, p->zBuffer, dsFlag, c1, c2, s);
        }
        else {
          edge = makeEdgeRec( v1, v2, src, p->zBuffer, dsFlag, c1, c2, NULL);
        }
        

      }
      else {
        if (dsFlag == 3){
          edge = makeEdgeRec( v2, v1, src, p->zBuffer, dsFlag, c2, c1, s);
        }
        else {
          edge = makeEdgeRec( v2, v1, src, p->zBuffer, dsFlag, c2, c1, NULL); 
        }
      }

      if( edge ){
        ll_insert( edges, edge, compYStart );
      }         
    }
    v1 = v2;
    if (dsFlag == 2){
      color_copy(&c2, &c1);
    }
    else if (dsFlag == 3){
      s[0] = s[2];
      s[1] = s[3];
    }
  }

  // check for empty edges (like nothing in the viewport)
  if( ll_empty( edges ) ) {
    ll_delete( edges, NULL );
    edges = NULL;
  }
  return(edges);
}

/*
    Draw one scanline of a polygon given the scanline, the active edges,
    a DrawState, the image, and some Lights (for Phong shading only).
 */
static void fillScan( int scan, LinkedList *active, Image *src, Color c, 
  int zFlag, int dsFlag, Mipmap *mipmap ) {
  Edge *p1, *p2;
  int i, start, finish;
  float zBuffer = 1.0, dzPerCol = 0.0;
  Color tc;
  color_copy(&tc, &c);
  Color dcPerCol; // c2, 
  float dsPerCol, dtPerCol;
  float s, t;
  float dim, lev, tmp;
  int upper, lower;

  p1 = ll_head( active );

  while(p1) {
    p2 = ll_next( active );
    
    if( !p2 ) {
        printf("*** bad bad bad (your edges are not coming in pairs) ***\n");
        break;
    }

    if( p2->xIntersect == p1->xIntersect ) {
      p1 = ll_next( active );
      continue;
    }

    start = (int)(p1->xIntersect + 0.5);
    finish = (int)(p2->xIntersect + 0.5);

    if (zFlag != 0){
      zBuffer = p1->zIntersect;
      dzPerCol = (p2->zIntersect - p1->zIntersect)/(finish-start);
    }

    if (dsFlag == 2){
      // c2 = p2->cIntersect;
      dcPerCol.c[0] = (p2->cIntersect.c[0]-p1->cIntersect.c[0])/(finish-start);
      dcPerCol.c[1] = (p2->cIntersect.c[1]-p1->cIntersect.c[1])/(finish-start);
      dcPerCol.c[2] = (p2->cIntersect.c[0]-p1->cIntersect.c[2])/(finish-start);
      color_copy(&tc, &p1->cIntersect);
    }
    else if (dsFlag == 3){
      s = p1->sIntersect;
      t = p1->tIntersect;
      dsPerCol = (p2->sIntersect-p1->sIntersect)/(finish-start);
      dtPerCol = (p2->tIntersect-p1->tIntersect)/(finish-start);
    }

    if (start < 0){
      if (zFlag != 0){
        zBuffer += (-start)*dzPerCol;
      }
      if (dsFlag == 2){
        tc.c[0] += (-start)*dcPerCol.c[0];
        tc.c[1] += (-start)*dcPerCol.c[1];
        tc.c[2] += (-start)*dcPerCol.c[2]; 
      }
      else if (dsFlag == 3){
        // ??
      }
      start = 0;
    }
    finish = finish>=src->cols ? src->cols : finish;

    for (i=start; i<finish; i++){
      if ( zFlag != 0 && zBuffer < src->data[scan][i].z){
        zBuffer += dzPerCol;
        continue;
      }
      if (dsFlag == 1){
        tc.c[0] = c.c[0]*(1-(1/zBuffer));
        tc.c[1] = c.c[1]*(1-(1/zBuffer));
        tc.c[2] = c.c[2]*(1-(1/zBuffer));
      }
      else if (dsFlag == 2){
        tc.c[0] += dcPerCol.c[0];
        tc.c[1] += dcPerCol.c[1];
        tc.c[2] += dcPerCol.c[2];
      }
      else if (dsFlag == 3){
        dim = fmaxf(fabs(dsPerCol), fabs(dtPerCol));
        // printf("max. %f %f\n", dsdy+dsPerCol, dtdy+dtPerCol);
        if (dim == 0){
          lev = 0.0;
        }
        else {
          lev = log2f(256*dim);
        }
        tmp = lev - (int)lev;
        int base = exp2(floor(lev-1));
        lower = exp2(floor(lev));
        upper = exp2(ceil(lev));
        tc.c[0] = ((1.0-tmp)*mipmap->data[(int)((512-lower)+(lower*t))][(int)((512-lower)+(lower*s))]) + 
            ((1.0-(1.0-tmp))*mipmap->data[(int)((512-upper)+(upper*t))][(int)((512-upper)+(upper*s))]);
        tc.c[1] = ((1.0-tmp)*mipmap->data[(int)((512-lower)+(lower*t))][(int)((512-lower)+base+(lower*s))]) + 
            ((1.0-(1.0-tmp))*mipmap->data[(int)((512-upper)+(upper*t))][(int)((512-upper)+lower+(upper*s))]);
        tc.c[2] = ((1.0-tmp)*mipmap->data[(int)((512-lower)+base+(lower*t))][(int)(512-lower+(lower*s))]) +
            ((1.0-(1.0-tmp))*mipmap->data[(int)((512-upper)+lower+(upper*t))][(int)(512-upper+(upper*s))]);
      }
      src->data[scan][i].z = zBuffer;
      image_setColor(src, scan, i, tc);

      if (zFlag != 0){
        zBuffer += dzPerCol;
      }
      if (dsFlag == 3){
        s += dsPerCol;
        t += dtPerCol;
      }
    }

    p1 = ll_next( active );

  }

  return;
}

/* 
   Process the edge list, assumes the edges list has at least one entry
*/
static int processEdgeList( LinkedList *edges, Image *src, Color c, int zFlag, int dsFlag, Mipmap *mipmap ) {
  LinkedList *active = NULL;
  LinkedList *tmplist = NULL;
  LinkedList *transfer = NULL;
  Edge *current;
  Edge *tedge;
  int scan = 0;

  active = ll_new( );
  tmplist = ll_new( );

  // get a pointer to the first item on the list and reset the current pointer
  current = ll_head( edges );

  // start at the first scanline and go until the active list is empty
  for(scan = current->yStart;scan < src->rows;scan++ ) {

    // grab all edges starting on this row
    while( current != NULL && current->yStart == scan ) {
      ll_insert( active, current, compXIntersect );
      current = ll_next( edges );
    }
    // current is either null, or the first edge to be handled on some future scanline

    if( ll_empty(active) ) {
      break;
    }

    // if there are active edges
    // fill out the scanline
    fillScan( scan, active, src, c, zFlag, dsFlag, mipmap);

    // remove any ending edges and update the rest
    for( tedge = ll_pop( active ); tedge != NULL; tedge = ll_pop( active ) ) {

      // keep anything that's not ending
      if( tedge->yEnd > scan ) {
        float a = 1.0;

        // update the edge information with the dPerScan values
        tedge->xIntersect += tedge->dxPerScan;
        if (zFlag != 0){
          tedge->zIntersect += tedge->dzPerScan;
        }

        if (dsFlag == 2){
          tedge->cIntersect.c[0] += tedge->dcPerScan.c[0];
          tedge->cIntersect.c[1] += tedge->dcPerScan.c[1];
          tedge->cIntersect.c[2] += tedge->dcPerScan.c[2];
        }
        else if (dsFlag == 3){
          tedge->sIntersect += tedge->dsPerScan;
          tedge->tIntersect += tedge->dtPerScan;
          // printf("s: %f,  t:  %f\n", tedge->sIntersect, tedge->tIntersect);
        }
        
        // adjust in the case of partial overlap
        if( tedge->dxPerScan < 0.0 && tedge->xIntersect < tedge->x1 ) {
          a = (tedge->xIntersect - tedge->x1) / tedge->dxPerScan;
          tedge->xIntersect = tedge->x1;
        }

        else if( tedge->dxPerScan > 0.0 && tedge->xIntersect > tedge->x1 ) {
          a = (tedge->xIntersect - tedge->x1) / tedge->dxPerScan;
          tedge->xIntersect = tedge->x1;
        }

        ll_insert( tmplist, tedge, compXIntersect );
      }
    }
    transfer = active;
    active = tmplist;
    tmplist = transfer;
  }

  // get rid of the lists, but not the edge records
  ll_delete(active, NULL);
  ll_delete(tmplist, NULL);

  return(0);
}

void scanline_drawFill(Polygon *p, Image *src, Color c, int dsFlag, Mipmap *mipmap){
    LinkedList *edges = NULL;

    edges = setupEdgeList( p, src, dsFlag );

    if( !edges ) {
      return;
    } 

    processEdgeList( edges, src, c, p->zBuffer, dsFlag, mipmap);

    ll_delete( edges, (void (*)(const void *))free );

    return;
}






































// _____________________________ pattern _______________________________________


/*
    Draw one scanline of a polygon given the scanline, the active edges,
    a DrawState, the image, and some Lights (for Phong shading only).
 */
static void fillScan2( int scan, LinkedList *active, Image *src, Color *c, double x, double y ) {
  Edge *p1, *p2;
  int i, f;

    // loop over the list
  p1 = ll_head( active );
  f = 1;

  while(p1) {
    // the edges have to come in pairs, draw from one to the next
    p2 = ll_next( active );
    
    if( !p2 ) {
        printf("bad bad bad (your edges are not coming in pairs)\n");
        //printf("found %d edges.\n", f);
        break;
    }

    //printf("Scan from %f to %f.\n", p1->xIntersect, p2->xIntersect );
    //printf("(x0, y0): (%f, %f)\n", p2->x0, p2->y0);
        // if the xIntersect values are the same, don't draw anything.
        // Just go to the next pair.
    if( p2->xIntersect == p1->xIntersect ) {
      p1 = ll_next( active );
      continue;
    }
    else {
      for (i=p1->xIntersect; i<p2->xIntersect; i++){
        image_setBitmap(src, scan, i, x, y, c);
      }
    }

        // move ahead to the next pair of edges
    p1 = ll_next( active );
  }

  return;
}

 /* 
     Process the edge list, assumes the edges list has at least one entry
  */
static int processEdgeList2( LinkedList *edges, Image *src, Color *c, double x, double y ) {
  LinkedList *active = NULL;
  LinkedList *tmplist = NULL;
  LinkedList *transfer = NULL;
  Edge *current;
  Edge *tedge;
  int scan = 0;
  int count = 0;

  active = ll_new( );
  tmplist = ll_new( );

  // get a pointer to the first item on the list and reset the current pointer
  current = ll_head( edges );

  // start at the first scanline and go until the active list is empty
  for(scan = current->yStart;scan < src->rows;scan++ ) {
    // count = 0;
    // grab all edges starting on this row
    // printf("scan: %d\n", scan);
    while( current != NULL && current->yStart == scan ) {
      // printf("adding edge at scanline %d\n", scan);
      count ++;
      ll_insert( active, current, compXIntersect );
      current = ll_next( edges );
    }
    //printf("edges in active %d.\n", count);
    // current is either null, or the first edge to be handled on some future scanline

    if( ll_empty(active) ) {
      break;
    }
      
    // if there are active edges
    // fill out the scanline
    // printf("  There are %d edges in the active list.\n", count);
    fillScan2( scan, active, src, c, x, y);
    count = 0;

    // remove any ending edges and update the rest
    for( tedge = ll_pop( active ); tedge != NULL; tedge = ll_pop( active ) ) {
      // printf("  edge from: %d to %d\n", tedge->yStart, tedge->yEnd);
      // printf ("if %d > %d keep it.\n", tedge->yEnd, scan);
      // keep anything that's not ending
      if( tedge->yEnd > scan+1 ) {
        float a = 1.0;

        // update the edge information with the dxPerScan values
        tedge->xIntersect += tedge->dxPerScan;
        tedge->zIntersect += tedge->dzPerScan;

        // adjust in the case of partial overlap
        if( tedge->dxPerScan < 0.0 && tedge->xIntersect < tedge->x1 ) {
          a = (tedge->xIntersect - tedge->x1) / tedge->dxPerScan;
          tedge->xIntersect = tedge->x1;
        }
        else if( tedge->dxPerScan > 0.0 && tedge->xIntersect > tedge->x1 ) {
          a = (tedge->xIntersect - tedge->x1) / tedge->dxPerScan;
          tedge->xIntersect = tedge->x1;
        }
        count ++;
        ll_insert( tmplist, tedge, compXIntersect );
      }
      else {
        // printf("  getting rid of edge\n");
      }
    }
    transfer = active;
    active = tmplist;
    tmplist = transfer;
  }

  // get rid of the lists, but not the edge records
  ll_delete(active, NULL);
  ll_delete(tmplist, NULL);

  return(0);
}

// draw the filled polygon using color c with the scanline rendering algorithm.
void scanline_drawFill2(Polygon *p, Image *src, Color *c){
    LinkedList *edges = NULL;

    // set up the edge list
    edges = setupEdgeList( p, src, 0 );
    if( !edges )
        return;
    
    // process the edge list (should be able to take an arbitrary edge list)

    processEdgeList2( edges, src, c, p->vertex[0].val[0], p->vertex[0].val[1]);

    // clean up
    ll_delete( edges, (void (*)(const void *))free );

    return;
}


// end sccanline
