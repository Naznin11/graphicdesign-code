#include <GL/glut.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Function to draw a filled semicircle
void drawSemiCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; i++) {
        float theta = M_PI * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Umbrella canopy (yellow semicircle)
    glColor3f(1.0f, 1.0f, 0.0f);
    drawSemiCircle(0.0f, 0.0f, 0.7f, 200);

    // Scalloped bottom edges (cut-out effect using white)
 glColor3f(1.0f, 1.0f, 1.0f);
    drawSemiCircle(-0.15f, 0.0f, 0.23f, 100);
    drawSemiCircle(0.20f, 0.0f, 0.23f, 100);
    drawSemiCircle(0.50f, 0.0f, 0.23f, 100);
   drawSemiCircle(-0.50f, 0.0f, 0.23f, 100);


    // Umbrella stick (brown line)
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.18f);
    glVertex2f(0.0f, -0.7f);
    glEnd();


    // Curved handle (J-shape)
    glBegin(GL_LINE_STRIP);
    for (int i = -90; i <= 90; i++) {
        float theta = i * M_PI / 180.0f;
        float x = 0.15f * cosf(theta);
        float y = -0.8f + 0.15f * sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();

    glFlush();
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Ted's Lost Umbrella");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
