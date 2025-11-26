#include <GL/glut.h>


void init() {
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
}


void drawGrid() {
    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2f( 0, -1); glVertex2f( 0,  1);
    glVertex2f(-1,  0); glVertex2f( 1,  0);
    glEnd();
}


void drawX(float x, float y) {
    float s = 0.2;
    glBegin(GL_LINES);
    glVertex2f(x - s, y - s); glVertex2f(x + s, y + s);
    glVertex2f(x - s, y + s); glVertex2f(x + s, y - s);
    glEnd();
}


void drawPlus(float x, float y) {
    float s = 0.2;
    glBegin(GL_LINES);
    glVertex2f(x - s, y); glVertex2f(x + s, y);
    glVertex2f(x, y - s); glVertex2f(x, y + s);
    glEnd();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawGrid();
    glColor3f(0, 0, 0);

    drawX(-0.5,  0.5);
    drawPlus(0.5,  0.5);
    drawPlus(-0.5, -0.5);
    drawX(0.5, -0.5);

    glFlush();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Simple Grid X and +");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
