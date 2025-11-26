#include <GL/glut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>

struct Pipe
{
    float x, gapY;
    Pipe(float _x, float _gapY): x(_x), gapY(_gapY) {}
} ;


int winW=800, winH=600;
float birdY=0.0f, birdVel=0.0f;
float gravity=-0.0015f, jumpVel=0.03f;
std::vector<Pipe> pipes;
int score=0;
bool gameOver=false;
float wingAngle=0.0f;
bool wingUp=true;

void drawText(float x,float y,std::string s)
{
    glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);
}

void drawCircle(float cx,float cy,float r,int segs)
{
    glBegin(GL_TRIANGLE_FAN);
    for(int i=0; i<segs; i++)
    {
        float th=2.0f*M_PI*i/segs;
        glVertex2f(cx+r*cos(th),cy+r*sin(th));
    }
    glEnd();
}


void drawBird(float x,float y)
{
    glPushMatrix();
    glTranslatef(x,y,0);


    glColor3f(1,1,0);
    drawCircle(0,0,0.05f,30);

    glColor3f(1,1,1);
    drawCircle(0.02f,0.02f,0.015f,20);
    glColor3f(0,0,0);
    drawCircle(0.02f,0.02f,0.007f,20);

    glColor3f(1,0.5f,0);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.05f,0);
    glVertex2f(0.08f,0.01f);
    glVertex2f(0.08f,-0.01f);
    glEnd();

    if(wingUp)
    {
        wingAngle+=2;
        if(wingAngle>20) wingUp=false;
    }
    else
    {
        wingAngle-=2;
        if(wingAngle<-20) wingUp=true;
    }

    glPushMatrix();
    glTranslatef(-0.02f,0,0);
    glRotatef(wingAngle,0,0,1);
    glColor3f(1,0.8f,0);
    glBegin(GL_TRIANGLES);
    glVertex2f(0,0);
    glVertex2f(-0.05f,0.03f);
    glVertex2f(-0.05f,-0.03f);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

void resetGame()
{
    birdY=0.0f;
    birdVel=0.0f;
    pipes.clear();
    pipes.push_back(Pipe(1.0f,0.0f));
    score=0;
    gameOver=false;
    wingAngle=0;
    wingUp=true;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);


    glBegin(GL_QUADS);
    glColor3f(0.53f,0.81f,0.98f);
    glVertex2f(-1,1);
    glColor3f(0.25f,0.75f,0.90f);
    glVertex2f(1,1);
    glColor3f(0.90f,0.95f,1.0f);
    glVertex2f(1,-1);
    glColor3f(0.85f,0.95f,1.0f);
    glVertex2f(-1,-1);
    glEnd();

    drawBird(-0.08f,birdY);

    for(auto &p:pipes)
    {
        float w=0.15f,gapH=0.4f;


        glBegin(GL_QUADS);
        glColor3f(0.0f,0.6f,0.0f);
        glVertex2f(p.x,-1);
        glColor3f(0.0f,0.8f,0.0f);
        glVertex2f(p.x+w,-1);
        glColor3f(0.0f,0.8f,0.0f);
        glVertex2f(p.x+w,p.gapY-gapH/2);
        glColor3f(0.0f,0.6f,0.0f);
        glVertex2f(p.x,p.gapY-gapH/2);
        glEnd();

        glColor3f(0.0f,0.5f,0.0f);
        glBegin(GL_QUADS);
        glVertex2f(p.x-0.01f,p.gapY-gapH/2);
        glVertex2f(p.x+w+0.01f,p.gapY-gapH/2);
        glVertex2f(p.x+w+0.01f,p.gapY-gapH/2+0.05f);
        glVertex2f(p.x-0.01f,p.gapY-gapH/2+0.05f);
        glEnd();


        glBegin(GL_QUADS);
        glColor3f(0.0f,0.6f,0.0f);
        glVertex2f(p.x,p.gapY+gapH/2);
        glColor3f(0.0f,0.8f,0.0f);
        glVertex2f(p.x+w,p.gapY+gapH/2);
        glColor3f(0.0f,0.8f,0.0f);
        glVertex2f(p.x+w,1);
        glColor3f(0.0f,0.6f,0.0f);
        glVertex2f(p.x,1);
        glEnd();

        glColor3f(0.0f,0.5f,0.0f);
        glBegin(GL_QUADS);
        glVertex2f(p.x-0.01f,p.gapY+gapH/2-0.05f);
        glVertex2f(p.x+w+0.01f,p.gapY+gapH/2-0.05f);
        glVertex2f(p.x+w+0.01f,p.gapY+gapH/2);
        glVertex2f(p.x-0.01f,p.gapY+gapH/2);
        glEnd();
    }


    glColor3f(1,1,1);
    drawText(-0.95f,0.9,"Score: "+std::to_string(score));
    if(gameOver) drawText(-0.3f,0,"GAME OVER! Press R to Restart");

    glutSwapBuffers();
}


void update(int v)
{
    if(!gameOver)
    {
        birdVel+=gravity;
        birdY+=birdVel;
        if(birdY<-1 || birdY>1) gameOver=true;

        for(auto &p:pipes) p.x-=0.01f;
        if(!pipes.empty() && pipes.front().x<-1.2f)
        {
            pipes.erase(pipes.begin());
            float newGapY=(rand()%100/100.0f*1.2f-0.6f);
            pipes.push_back(Pipe(1.0f,newGapY));
            score++;
        }
        for(auto &p:pipes)
        {
            float bx1=-0.1f,bx2=-0.03f,by1=birdY-0.05f,by2=birdY+0.05f;
            float w=0.15f,gapH=0.4f;
            if(bx2>p.x && bx1<p.x+w)
            {
                if(by1<p.gapY-gapH/2 || by2>p.gapY+gapH/2)
                    gameOver=true;
            }
        }

    }

    glutPostRedisplay();
    glutTimerFunc(16,update,0);
}

void key(unsigned char k,int x,int y)
{
    if(k==' ') birdVel=jumpVel;
    if(k=='r'||k=='R') resetGame();
}

int main(int argc,char**argv)
{
    srand(time(0));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(winW,winH);
    glutCreateWindow("Flappy Bird - With Wing Animation");

    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    resetGame();
    glutTimerFunc(16,update,0);

    glutMainLoop();
    return 0;
}
