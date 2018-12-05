
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <GL/gl.h>     // openGL header
#include <GL/glu.h>   // glut header
#include <GL/glut.h>   // glut header
#include <vector>
#include <array>
#include <iostream>
#include <string.h>
#include "triangulate.h";

#define PI 3.1415926535898
#define Cos(th) cos(PI/180*(th))
#define Sin(th) sin(PI/180*(th))
double dim=500; /* dimension of orthogonal box */
/*  Various global state */
int toggleValues = 1; /* toggle values on and off */
int toggleMode = 0; /* projection mode */
int th = 0;   /* azimuth of view angle */
int ph = 0;   /* elevation of view angle */
int fov = 55; /* field of view for perspective */
float asp = 1.2;  /* aspect ratio */
int extrudeval = 0;
using namespace std;
void redrawPolygons();
void showAllTriangles();
void hideAllTriangles();
void drawAll3Dpolygons();
int ww=600;
int wh=500;
int polygonmode=0;
int polygonsnum=0;
int trianglesshown = 1;

void project() 
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (toggleMode) {
    /* perspective */
    gluPerspective(fov,asp,dim/4,4*dim);
  }
  else {
    /* orthogonal projection*/
    glOrtho(-dim*asp,+dim*asp, -dim,+dim, -dim,+dim);
    // glOrtho(-dim,+dim, -dim,+dim, -2*dim,+2*dim);
  }

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void reshape(int width,int height)
{
  asp = (height>0) ? (double)width/height : 1;
  glViewport(0,0, width,height);
  project();
}

void windowSpecial(int key,int x,int y)
{
  /*  Right arrow key - increase azimuth by 5 degrees */
  if (key == GLUT_KEY_RIGHT) th += 5;
  /*  Left arrow key - decrease azimuth by 5 degrees */
  else if (key == GLUT_KEY_LEFT) th -= 5;
  /*  Up arrow key - increase elevation by 5 degrees */
  else if (key == GLUT_KEY_UP) ph += 5;
  /*  Down arrow key - decrease elevation by 5 degrees */
  else if (key == GLUT_KEY_DOWN) ph -= 5;

  /*  Keep angles to +/-360 degrees */
  th %= 360;
  ph %= 360;

  project();
  glutPostRedisplay();
}

const int MAX_POINTS = 20;

void display()
{
   glClearColor(1.0,1.0,1.0,0.0);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_DEPTH_TEST);
   if (toggleMode) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    double Ex = -2*dim*Sin(th)*Cos(ph);
    double Ey = +2*dim        *Sin(ph);
    double Ez = +2*dim*Cos(th)*Cos(ph);
    /* camera/eye position, aim of camera lens, up-vector */
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
    // drawShape();
    drawAll3Dpolygons();
  }
  /*  Orthogonal - set world orientation */
  else {
    glRotatef(ph,1,0,0);
    glRotatef(th,0,1,0);
  }
   if(toggleMode==0){
     redrawPolygons();
   }
   // redrawPolygons();
   if(trianglesshown==0){
      showAllTriangles();
      // trianglesshown=1;
    }
    else if(trianglesshown==1){
      hideAllTriangles();
      // trianglesshown=0;
      // drawAllLines();
      // redrawPolygons();
    }
    if(extrudeval==1){
      glClearColor(0.0, 0.0, 0.0, 1.0);
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // clear display window
      // draw3Dpolygon(polygonsarray[0]);
      // drawShape();
      drawAll3Dpolygons();
    }
   glFlush();
   
   glutSwapBuffers ();
}

typedef struct Point2D{
  float x;
  float y;
  float z;
}Point2D;

bool isIntersecting(Point2D a, Point2D b, Point2D c, Point2D d);

float GR=1.0; // Fill polygon R value
float GG=1.0; // Fill polygon G value
float GB=1.0; // Fill polygon B value

float LGR=0.0; // Fill polygon R value
float LGG=0.0; // Fill polygon G value
float LGB=0.0; // Fill polygon B value

class Polygon{

  struct Point2D pointsarray[100];
  int numberOfPoints=0;
  Vector2dVector a;
  Vector2dVector result;
  Vector2dVector edges;
  int edgesnumber;
  int tricount;

  float R=1.0; // Fill polygon R value
  float G=1.0; // Fill polygon G value
  float B=1.0; // Fill polygon B value

  float LR=0.0; //Line R value
  float LG=0.0; //Line G value
  float LB=0.0; //Line B value

  public:
  float getR(){
    return R;
  }
  float getG(){
    return G;
  }
  float getB(){
    return B;
  }
  float getLR(){
    return LR;
  }
  float getLG(){
    return LG;
  }
  float getLB(){
    return LB;
  }
  void setRGB(float r,float g,float b){
    R=r;
    G=g;
    B=b;
  }
  void setLRGB(float r,float g,float b){
    LR=r;
    LG=g;
    LB=b;
  }
  void insertPoint(Point2D p){
    a.push_back( Vector2d(p.x,p.y));
    pointsarray[numberOfPoints] = p;
    numberOfPoints++;
  }
  int getNumberOfPoints(){
    return numberOfPoints;
  }
  Point2D * getpointsArray(){
    return pointsarray;
  }
  int getEdgesNumber(){
    return edgesnumber;
  }
  Vector2dVector getPolygonEdges(){
    return edges;
  }
  void triangulatePoly(){
    Triangulate::Process(a,result);
    int tcount = result.size()/3;
    for (int i=0; i<tcount; i++)
    {
      const Vector2d &p1 = result[i*3+0];
      const Vector2d &p2 = result[i*3+1];
      const Vector2d &p3 = result[i*3+2];
      edges.push_back(result[i*3+0]);
      edges.push_back(result[i*3+0]);
      edges.push_back(result[i*3+0]);
      edgesnumber+=3;
      tricount = tcount;
      printf("Triangle %d => (%0.0f,%0.0f) (%0.0f,%0.0f) (%0.0f,%0.0f)\n",i+1,p1.GetX(),p1.GetY(),p2.GetX(),p2.GetY(),p3.GetX(),p3.GetY());
    }
  }

  void showTriangles(){
    for (int i=0; i<tricount; i++)
    {
      const Vector2d &p1 = result[i*3+0];
      const Vector2d &p2 = result[i*3+1];
      const Vector2d &p3 = result[i*3+2];

      glBegin(GL_LINE_LOOP);

       glColor3f(0.5,1.0,0.5);
       glVertex2i(p1.GetX(),p1.GetY());
       glVertex2i(p2.GetX(),p2.GetY());
       glVertex2i(p3.GetX(),p3.GetY());

      glEnd();
      // glFlush();
      glutSwapBuffers ();
    }
  }

  void hideTriangles(){
    for (int i=0; i<tricount; i++)
    {
      const Vector2d &p1 = result[i*3+0];
      const Vector2d &p2 = result[i*3+1];
      const Vector2d &p3 = result[i*3+2];

      glBegin(GL_LINE_LOOP);

       glColor3f(R,G,B);
       glVertex2i(p1.GetX(),p1.GetY());
       glVertex2i(p2.GetX(),p2.GetY());
       glVertex2i(p3.GetX(),p3.GetY());

      glEnd();
      // glFlush();
      glutSwapBuffers ();
    }
  }

  void fillPolygon(float R,float G,float B){
     for(int i=0;i<tricount;i++){
       const Vector2d &p1 = result[i*3+0];
       const Vector2d &p2 = result[i*3+1];
       const Vector2d &p3 = result[i*3+2];

       glBegin(GL_POLYGON);
        setRGB(R,G,B);
        glColor3f(R,G,B);
        glVertex2i(p1.GetX(),p1.GetY());
        glVertex2i(p2.GetX(),p2.GetY());
        glVertex2i(p3.GetX(),p3.GetY());

       glEnd();
       // glFlush();
       glutSwapBuffers ();
     }
  }

  void drawPolygonLines(float LR,float LG,float LB){
    glBegin(GL_LINE_LOOP);
     for(int i=0;i<numberOfPoints;i++){
      setLRGB(LR,LG,LB);
       glColor3f(getLR(),getLG(),getLB());
       glVertex2i(pointsarray[i].x,pointsarray[i].y);
     }
    glEnd();
    // glFlush();
    glutSwapBuffers ();
  }

  bool doesIntersect(){
    for(int i=0;i<numberOfPoints;i++){
      for(int j=i+1;j<numberOfPoints-1;j++){
        if(isIntersecting(pointsarray[i],pointsarray[i+1],pointsarray[j],pointsarray[j+1])){
          return true;
        }
      }
      for(int i=numberOfPoints-1;i>=0;i--){
        for(int j=numberOfPoints-1;j>=0;j--){
          if(isIntersecting(pointsarray[i],pointsarray[i-1],pointsarray[j],pointsarray[j-1])){
            return true;
          }
        }
      }
      for(int i=0;i<numberOfPoints;i++){
        if(isIntersecting(pointsarray[0],pointsarray[numberOfPoints-1],pointsarray[i],pointsarray[i+1])){
          return true;
        }
      }
      return false;
    }
  }
};


// ______ GLOBAL VARIABLES _____
Polygon temppolygon;
Point2D temppoint;
Polygon polygonsarray[50];
Point2D previouspoint;
void suthHodgePolygon(Polygon poly);
void redrawPolygons();
//______ GLOBAL VARIABLES ______

void setGRGB(float r, float g ,float b){
   GR=r; // Fill polygon R value
   GG=g; // Fill polygon G value
   GB=b; // Fill polygon B value
}

void setGLRGB(float r,float g,float b){
  LGR=r; // Fill polygon R value
  LGG=g; // Fill polygon G value
  LGB=b; // Fill polygon B value
}
bool isIntersecting(Point2D p1, Point2D p2, Point2D q1, Point2D q2) {
    return (((q1.x-p1.x)*(p2.y-p1.y) - (q1.y-p1.y)*(p2.x-p1.x))
            * ((q2.x-p1.x)*(p2.y-p1.y) - (q2.y-p1.y)*(p2.x-p1.x)) < 0)
            &&
           (((p1.x-q1.x)*(q2.y-q1.y) - (p1.y-q1.y)*(q2.x-q1.x))
            * ((p2.x-q1.x)*(q2.y-q1.y) - (p2.y-q1.y)*(q2.x-q1.x)) < 0);
}

void drawpolygon()
{
   glBegin(GL_POLYGON);
    for(int i=0;i<temppolygon.getNumberOfPoints();i++){
      glColor3f(temppolygon.getLR(),temppolygon.getLG(),temppolygon.getLB());
      glVertex2i(temppolygon.getpointsArray()[i].x,temppolygon.getpointsArray()[i].y);
    }
   glEnd();
   // glFlush();
   glutSwapBuffers ();
}

void drawPoint(){
  // glutSwapBuffers ();
  glBegin(GL_POINTS);
  glColor3f(0.0,0.0,0.0); //default black color
  glVertex2i(temppoint.x,temppoint.y);
  glEnd();
  
  glutSwapBuffers ();

}

void drawPolygonsPoints(Polygon poly){
  glBegin(GL_POINTS);
  glColor3f(0.0,0.0,0.0); //default black color
  for(int i=0;i<poly.getNumberOfPoints();i++){
    glVertex2i(poly.getpointsArray()[i].x,poly.getpointsArray()[i].y);
  }
  glEnd();
  glutSwapBuffers ();
}

void erasePoint(float x,float y){
  glBegin(GL_POINTS);
  glColor3f(1.0,1.0,1.0);
  glVertex2i(x,y);
  glEnd();
  // glFlush();
  glutSwapBuffers ();
}

void showAllTriangles(){
  for(int i=0;i<polygonsnum;i++){
    polygonsarray[i].showTriangles();
  }
}

void hideAllTriangles(){
  for(int i=0;i<polygonsnum;i++){
    polygonsarray[i].fillPolygon(polygonsarray[i].getR(),polygonsarray[i].getG(),polygonsarray[i].getB());
  }
}

void drawAllLines(){
  for(int i=0;i<polygonsnum;i++){
    polygonsarray[i].drawPolygonLines(polygonsarray[i].getLR(),polygonsarray[i].getLG(),polygonsarray[i].getLB());
  }
}

void mouse(int btn, int state,int x,int y)
{
  glPointSize(5.0f);
  if(polygonmode==1){

    if(btn==GLUT_LEFT_BUTTON)
    {

        if(state==GLUT_DOWN)
        {
          previouspoint = temppoint;
          std::cout << "Point Added" << std::endl;
          temppoint.x=x;
          temppoint.y=(wh-y);

          temppolygon.insertPoint(temppoint);
          drawPoint();


        }
     }
     else if(btn == GLUT_MIDDLE_BUTTON){
       if(state == GLUT_DOWN){
         if(temppolygon.doesIntersect()){
           for(int i=0;i<temppolygon.getNumberOfPoints();i++){
             erasePoint(temppolygon.getpointsArray()[i].x,temppolygon.getpointsArray()[i].y);
             glutPostRedisplay();
           }
         }
         else{

           temppolygon.triangulatePoly();

           temppolygon.fillPolygon(GR,GG,GB);

           polygonsarray[polygonsnum]=temppolygon;
           polygonsarray[polygonsnum].drawPolygonLines(LGR,LGG,LGB);

           polygonsnum++;
           suthHodgePolygon(temppolygon);
           glutPostRedisplay();

           if(trianglesshown==1){
             showAllTriangles();
           }

           // glutPostRedisplay();

         }
         polygonmode=0;//reset polygon mode
         glutAttachMenu(GLUT_RIGHT_BUTTON);
         temppolygon = Polygon(); //refresh temp polygon to 0

       }
     }
  }
}



void createMenu();
void ActionMenu(int value){
  switch(value){
    case 1:
      polygonmode=1;
      glutMouseFunc(mouse);

      // createMenu();
      // glutDetachMenu(GLUT_RIGHT_BUTTON);
      
      break;
    case 2:
      // extrudeval=1;
      toggleMode=1;
      project();
      glutReshapeFunc(reshape);
      glutPostRedisplay();
      break;
    case 3:
      exit(0);
      break;
  }
}

void LineColorMenu(int value){
  switch(value){
    case 1:
      setGLRGB(1.0,1.0,1.0); //white
      break;
    case 2:
      setGLRGB(0.0,0.0,0.0); //black
      break;
    case 3:
      setGLRGB(1.0,0.0,0.0); //red
      break;
    case 4:
      setGLRGB(0.0,1.0,0.0); //green
      break;
    case 5:
      setGLRGB(0.0,0.0,1.0); //blue
      break;
    case 6:
      setGLRGB(1.0,0.0,1.0); //pink
      break;
    case 7:
      setGLRGB(0.5,0.0,1.0); //purple
      break;
    case 8:
      setGLRGB(1.0,1.0,0.0); //yellow
      break;
    case 9:
      setGLRGB(1.0,0.5,0.0); //orange
      break;
    case 10:
      setGLRGB(0.5,0.5,0.5); //gray
      break;
    case 11:
      setGLRGB(0.0,1.0,1.0); //light blue
      break;
    case 12:
      setGLRGB(0.0,0.5,0.0); //dark green
      break;
    case 13:
      setGLRGB(0.5,0.0,0.0); //dark red
      break;
    case 14:
      setGLRGB(0.96,0.80,0.69); //Flesh
      break;
    case 15:
      setGLRGB(0.52,0.39,0.39); // Dusty rose
      break;
    case 16:
      setGLRGB(0.55,0.09,0.09); // Scarlet
      break;
  }
}

void FillColorMenu(int value){
  switch(value){
    case 1:
      setGRGB(1.0,1.0,1.0); //white
      break;
    case 2:
      setGRGB(0.0,0.0,0.0); //black
      break;
    case 3:
      setGRGB(1.0,0.0,0.0); //red
      break;
    case 4:
      setGRGB(0.0,1.0,0.0); //green
      break;
    case 5:
      setGRGB(0.0,0.0,1.0); //blue
      break;
    case 6:
      setGRGB(1.0,0.0,1.0); //pink
      break;
    case 7:
      setGRGB(0.5,0.0,1.0); //purple
      break;
    case 8:
      setGRGB(1.0,1.0,0.0); //yellow
      break;
    case 9:
      setGRGB(1.0,0.5,0.0); //orange
      break;
    case 10:
      setGRGB(0.5,0.5,0.5); //gray
      break;
    case 11:
      setGRGB(0.0,1.0,1.0); //light blue
      break;
    case 12:
      setGRGB(0.0,0.5,0.0); //dark green
      break;
    case 13:
      setGRGB(0.5,0.0,0.0); //dark red
      break;
    case 14:
      setGRGB(0.96,0.80,0.69); //Flesh
      break;
    case 15:
      setGRGB(0.52,0.39,0.39); // Dusty rose
      break;
    case 16:
      setGRGB(0.55,0.09,0.09); // Scarlet
      break;
  }
}

void createMenu(){
  int mainMenu,actionMenu,lineColorMenu,fillColorMenu;

  fillColorMenu=glutCreateMenu(FillColorMenu);
  glutAddMenuEntry("White", 1);
  glutAddMenuEntry("Black", 2);
  glutAddMenuEntry("Red", 3);
  glutAddMenuEntry("Green", 4);
  glutAddMenuEntry("Blue", 5);
  glutAddMenuEntry("Pink", 6);
  glutAddMenuEntry("Purple", 7);
  glutAddMenuEntry("Yellow", 8);
  glutAddMenuEntry("Orange", 9);
  glutAddMenuEntry("Gray", 10);
  glutAddMenuEntry("Light Blue", 11);
  glutAddMenuEntry("Dark Green", 12);
  glutAddMenuEntry("Dark Red", 13);
  glutAddMenuEntry("Flesh", 14);
  glutAddMenuEntry("Dusty Rose", 15);
  glutAddMenuEntry("Scarlet", 16);

  lineColorMenu=glutCreateMenu(LineColorMenu);
  glutAddMenuEntry("White", 1);
  glutAddMenuEntry("Black", 2);
  glutAddMenuEntry("Red", 3);
  glutAddMenuEntry("Green", 4);
  glutAddMenuEntry("Blue", 5);
  glutAddMenuEntry("Pink", 6);
  glutAddMenuEntry("Purple", 7);
  glutAddMenuEntry("Yellow", 8);
  glutAddMenuEntry("Orange", 9);
  glutAddMenuEntry("Gray", 10);
  glutAddMenuEntry("Light Blue", 11);
  glutAddMenuEntry("Dark Green", 12);
  glutAddMenuEntry("Dark Red", 13);
  glutAddMenuEntry("Flesh", 14);
  glutAddMenuEntry("Dusty Rose", 15);
  glutAddMenuEntry("Scarlet", 16);

  actionMenu=glutCreateMenu(ActionMenu);
  glutAddMenuEntry("POLYGON", 1);
  glutAddMenuEntry("Extrude", 2);
  glutAddMenuEntry("Exit", 3);

  mainMenu=glutCreateMenu(ActionMenu);
  glutAddSubMenu("ACTION", actionMenu);
  glutAddSubMenu("LINE_COLOR",lineColorMenu);
  glutAddSubMenu("FILL_COLOR",fillColorMenu);


  glutAttachMenu(GLUT_RIGHT_BUTTON);

}

void processNormalKeys(unsigned char key, int x, int y){
  if(key==84 || key == 116){ // T KEY TO SHOW TRIANGLES
    if(trianglesshown==0){
      showAllTriangles();
      trianglesshown=1;
    }
    else if(trianglesshown==1){
      hideAllTriangles();
      trianglesshown=0;
      // drawAllLines();
      // redrawPolygons();
    }
    glutPostRedisplay();
  }
}

void resize(int width, int height) {
    // we ignore the params and do:
    glutReshapeWindow( 599, 499);
}

int x_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1*y2 - y1*x2) * (x3-x4) -
              (x1-x2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}

// Returns y-value of point of intersectipn of
// two lines
int y_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1*y2 - y1*x2) * (y3-y4) -
              (y1-y2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}

// This functions clips all the edges w.r.t one clip
// edge of clipping area
void clip(Point2D * pointsarray, int &poly_size,
          int x1, int y1, int x2, int y2)
{
    int new_poly_size = 0;
    struct Point2D new_points[MAX_POINTS] ;
    // (ix,iy),(kx,ky) are the co-ordinate values of
    // the points
    for (int i = 0; i < poly_size; i++)
    {
        // i and k form a line in polygon
        int k = (i+1) % poly_size;
        int ix = pointsarray[i].x, iy = pointsarray[i].y;
        int kx = pointsarray[k].x, ky = pointsarray[k].y;

        // Calculating position of first point
        // w.r.t. clipper line
        int i_pos = (x2-x1) * (iy-y1) - (y2-y1) * (ix-x1);

        // Calculating position of second point
        // w.r.t. clipper line
        int k_pos = (x2-x1) * (ky-y1) - (y2-y1) * (kx-x1);

        // Case 1 : When both points are inside
        if (i_pos < 0  && k_pos < 0)
        {
            //Only second point is added
            new_points[new_poly_size].x = kx;
            new_points[new_poly_size].y = ky;
            new_poly_size++;
        }

        // Case 2: When only first point is outside
        else if (i_pos >= 0  && k_pos < 0)
        {
            // Point of intersection with edge
            // and the second point is added
            new_points[new_poly_size].x = x_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_points[new_poly_size].y = y_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_poly_size++;

            new_points[new_poly_size].x = kx;
            new_points[new_poly_size].y = ky;
            new_poly_size++;
        }

        // Case 3: When only second point is outside
        else if (i_pos < 0  && k_pos >= 0)
        {
            //Only point of intersection with edge is added
            new_points[new_poly_size].x = x_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_points[new_poly_size].y = y_intersect(x1,
                              y1, x2, y2, ix, iy, kx, ky);
            new_poly_size++;
        }

        // Case 4: When both points are outside
        else
        {
            //No points are added
        }
    }

    // Copying new points into original array
    // and changing the no. of vertices
    poly_size = new_poly_size;
    for (int i = 0; i < poly_size; i++)
    {
        pointsarray[i].x = new_points[i].x;
        pointsarray[i].y = new_points[i].y;
    }
}

// Implements Sutherland–Hodgman algorithm
void suthHodgClip(Point2D * pointsarray, int poly_size,
                  Point2D * clipper_points, int clipper_size)
{
    // i and k are two consecutive indexes
    for (int i=0; i<clipper_size; i++)
    {
        int k = (i+1) % clipper_size;

        // We pass the current array of vertices, it's size
        // and the end points of the selected clipper line
        clip(pointsarray, poly_size, clipper_points[i].x,
             clipper_points[i].y, clipper_points[k].x,
             clipper_points[k].y);
    }

    // Printing vertices of clipped polygon
    std::cout << "SuthHodged" << std::endl;
    for (int i=0; i < poly_size; i++)
        std::cout << '(' << pointsarray[i].x <<
                ", " << pointsarray[i].y << ") ";
}

void suthHodgePolygon(Polygon poly){
  struct Point2D clipper_points[4];
  Point2D p1;
  p1.x = 150;
  p1.y= 150;
  Point2D p2;
  p1.x = 150;
  p1.y= 200;
  Point2D p3;
  p1.x = 200;
  p1.y= 200;
  Point2D p4;
  p1.x = 200;
  p1.y= 150;
  clipper_points[0]=p1;
  clipper_points[1]=p2;
  clipper_points[2]=p3;
  clipper_points[3]=p4;

  suthHodgClip(poly.getpointsArray(),poly.getNumberOfPoints(),clipper_points,4);
}
// CLIPPING END OF ALGORITHM ________________________________________

void redrawPolygons(){
  for (int i=0;i<polygonsnum;i++){
    drawPolygonsPoints(polygonsarray[i]);
  }
  drawAllLines();
}



void draw3Dpolygon(Polygon poly, int z){
    double xx;
    double yy;
    double xx2;
    double yy2;
      
      glColor3f(1.0,0.0,1.0);
      glBegin(GL_LINES);
      std::cout << "Creating points" << std::endl;
      for(int i=0; i<poly.getNumberOfPoints()-1; i++ ){
        glVertex3f(poly.getpointsArray()[i].x,poly.getpointsArray()[i].y,z);
        glVertex3f(poly.getpointsArray()[i+1].x,poly.getpointsArray()[i+1].y,z);
        glVertex3f(poly.getpointsArray()[i+1].x,poly.getpointsArray()[i+1].y,0);
        glVertex3f(poly.getpointsArray()[i+1].x,poly.getpointsArray()[i+1].y,z);
      }
      glVertex3f(poly.getpointsArray()[poly.getNumberOfPoints()-1].x,poly.getpointsArray()[poly.getNumberOfPoints()-1].y,z);
      glVertex3f(poly.getpointsArray()[0].x,poly.getpointsArray()[0].y,z);
    glEnd();

      glEnable(GL_CULL_FACE);
      glColor3f(1.0,0.0,1.0);
      glBegin(GL_QUADS);
        for(int i=poly.getNumberOfPoints()-1  ; i>=0;i--){
          if(i==0 ){
            xx=poly.getpointsArray()[i].x;
            yy=poly.getpointsArray()[i].y;
            xx2=poly.getpointsArray()[poly.getNumberOfPoints()-1].x;
            yy2=poly.getpointsArray()[poly.getNumberOfPoints()-1].y;

          }else{
            xx=poly.getpointsArray()[i].x;
            yy=poly.getpointsArray()[i].y;
            xx2=poly.getpointsArray()[i-1].x;
            yy2=poly.getpointsArray()[i-1].y;
          }
          glVertex3f(xx,yy,0);
          glVertex3f(xx2,yy2,0);
          glVertex3f(xx2,yy2,z);
          glVertex3f(xx,yy,z);

        }
        glEnd();
    
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_QUADS);
      for(int i=0 ; i<poly.getNumberOfPoints();i++){
        if(i==poly.getNumberOfPoints()-1){
          xx=poly.getpointsArray()[i].x;
          yy=poly.getpointsArray()[i].y;
          xx2=poly.getpointsArray()[0].x;
          yy2=poly.getpointsArray()[0].y;

        }else{
          xx=poly.getpointsArray()[i].x;
          yy=poly.getpointsArray()[i].y;
          xx2=poly.getpointsArray()[i+1].x;
          yy2=poly.getpointsArray()[i+1].y;
        }
        glVertex3f(xx,yy,0);
        glVertex3f(xx2,yy2,0);
        glVertex3f(xx2,yy2,z);
        glVertex3f(xx,yy,z);

      }
      glEnd();

      // glColor3f(1.0,0.0,1.0);
      // for(int i=0;i<poly.getNumberOfPoints()-1;i++){
      //   glBegin(GL_POLYGON);
      //     glVertex3f(poly.getpointsArray()[i].x,poly.getpointsArray()[i].y,0);
      //     glVertex3f(poly.getpointsArray()[i+1].x,poly.getpointsArray()[i+1].y,0);
      //     glVertex3f(poly.getpointsArray()[i+2].x,poly.getpointsArray()[i+2].y,0);
      //   glEnd();
      // }
}

void drawAll3Dpolygons(){
  for(int i=0;i<polygonsnum;i++){
    glColor3f(1.0,0.0,1.0);
    draw3Dpolygon(polygonsarray[i],100);
  }
}



int main(int argc, char** argv) {
  // init GLUT and create window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE |GLUT_DEPTH);
  glutInitWindowPosition(100,100);
	glutInitWindowSize(600,500);

  glutCreateWindow("Tron GL");
  gluOrtho2D(0.0,(GLdouble)ww,0.0,(GLdouble)wh);
  glutDisplayFunc(display);
  

  glutMouseFunc(mouse);
  glutSpecialFunc(windowSpecial);
  glutKeyboardFunc(processNormalKeys);
  createMenu();

  glutMainLoop();
  return 0;
}
