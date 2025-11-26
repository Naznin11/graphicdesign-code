#include<GL/glut.h>
#include<math.h>
float pi=3.141592;
int segments=400;
float radius=0.5;
void circle(){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    glColor3f(1,1,1);
    for(int i=10;i<segments;i++){
        float angle=2*pi*float(i)/float(segments);
        float x=radius*cos(angle);
        float y=radius*sin(angle);
        glVertex2f(x,y);

    }
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1,1,1);
    for(int i=10;i<segments;i++){
        float angle=2*pi*float(i)/float(segments);
        float x=0.25+radius*cos(angle);
        float y=0.25+radius*sin(angle);
        glVertex2f(x,y);

    }
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1,1,1);
    for(int i=10;i<segments;i++){
        float angle=2*pi*float(i)/float(segments);
        float x=0.45+radius*cos(angle);
        float y=-0.05+radius*sin(angle);
        glVertex2f(x,y);

    }
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(1,1,1);
    for(int i=10;i<segments;i++){
        float angle=2*pi*float(i)/float(segments);
        float x=-0.35+radius*cos(angle);
        float y=-0.35+radius*sin(angle);
        glVertex2f(x,y);

    }
    glEnd();
     glBegin(GL_POLYGON);
    glColor3f(1,1,1);
    for(int i=10;i<segments;i++){
        float angle=2*pi*float(i)/float(segments);
        float x=-0.35+radius*cos(angle);
        float y=0.35+radius*sin(angle);
        glVertex2f(x,y);

    }
    glEnd();
glFlush();

}
int main(int argc,char** argv){
glutInit(&argc,argv);
glutInitDisplayMode(GLUT_SINGLE);
glutCreateWindow("circle");

glutInitWindowSize(500,500);
glutInitWindowPosition(500,400);
glutDisplayFunc(circle);
glutMainLoop();



}
