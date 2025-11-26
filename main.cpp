#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

// ===== Structures =====
struct Vec { float x, y; };
struct Bullet { Vec pos, vel; int life; };
struct Asteroid { Vec pos, vel; float r; };
struct Ship { Vec pos, vel; float angle; int lives; };

// ===== Globals =====
Ship ship;
vector<Bullet> bullets;
vector<Asteroid> asteroids;
int score = 0;
bool keyLeft=false, keyRight=false, keyUp=false;

// ===== Utility =====
float frand(float a,float b){ return a+(b-a)*(rand()/(float)RAND_MAX); }
float length(Vec a,Vec b){ return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)); }

// ===== Game setup =====
void resetShip(){
    ship.pos = {0,0}; ship.vel = {0,0};
    ship.angle = 90; ship.lives = 3;
}
void spawnAsteroid(){
    Asteroid a;
    a.r = 0.1f;
    a.pos = {frand(-1,1), frand(-1,1)};
    a.vel = {frand(-0.01,0.01), frand(-0.01,0.01)};
    asteroids.push_back(a);
}

// ===== Drawing =====
void drawShip(){
    glPushMatrix();
    glTranslatef(ship.pos.x, ship.pos.y, 0);
    glRotatef(ship.angle,0,0,1);
    glBegin(GL_TRIANGLES);
      glColor3f(0,1,0);
      glVertex2f(0,0.05); glVertex2f(-0.03,-0.03); glVertex2f(0.03,-0.03);
    glEnd();
    glPopMatrix();
}
void drawBullet(Bullet &b){
    glColor3f(1,1,0);
    glBegin(GL_QUADS);
      glVertex2f(b.pos.x-0.01,b.pos.y-0.01);
      glVertex2f(b.pos.x+0.01,b.pos.y-0.01);
      glVertex2f(b.pos.x+0.01,b.pos.y+0.01);
      glVertex2f(b.pos.x-0.01,b.pos.y+0.01);
    glEnd();
}
void drawAsteroid(Asteroid &a){
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
    for(int i=0;i<20;i++){
        float th=2*M_PI*i/20;
        glVertex2f(a.pos.x+a.r*cos(th), a.pos.y+a.r*sin(th));
    }
    glEnd();
}
void drawText(float x,float y,string s){
    glRasterPos2f(x,y);
    for(char c:s) glutBitmapCharacter(GLUT_BITMAP_8_BY_13,c);
}

// ===== Game Update =====
void update(){
    // ship control
    if(keyLeft) ship.angle+=3;
    if(keyRight) ship.angle-=3;
    if(keyUp){
        ship.vel.x += 0.001*cos(ship.angle*M_PI/180);
        ship.vel.y += 0.001*sin(ship.angle*M_PI/180);
    }
    ship.pos.x+=ship.vel.x; ship.pos.y+=ship.vel.y;
    if(ship.pos.x>1) ship.pos.x=-1; if(ship.pos.x<-1) ship.pos.x=1;
    if(ship.pos.y>1) ship.pos.y=-1; if(ship.pos.y<-1) ship.pos.y=1;

    // bullets
    for(int i=0;i<bullets.size();i++){
        bullets[i].pos.x+=bullets[i].vel.x;
        bullets[i].pos.y+=bullets[i].vel.y;
        bullets[i].life--;
        if(bullets[i].life<=0){ bullets.erase(bullets.begin()+i); i--; }
    }

    // asteroids
    for(auto &a:asteroids){
        a.pos.x+=a.vel.x; a.pos.y+=a.vel.y;
        if(a.pos.x>1) a.pos.x=-1; if(a.pos.x<-1) a.pos.x=1;
        if(a.pos.y>1) a.pos.y=-1; if(a.pos.y<-1) a.pos.y=1;
    }

    // bullet vs asteroid
    for(int i=0;i<bullets.size();i++){
        for(int j=0;j<asteroids.size();j++){
            if(length(bullets[i].pos,asteroids[j].pos)<asteroids[j].r){
                bullets.erase(bullets.begin()+i); i--;
                asteroids.erase(asteroids.begin()+j);
                score+=10; spawnAsteroid();
                break;
            }
        }
    }

    // ship vs asteroid
    for(int j=0;j<asteroids.size();j++){
        if(length(ship.pos,asteroids[j].pos)<asteroids[j].r){
            ship.lives--;
            resetShip();
            if(ship.lives<=0){ score=0; asteroids.clear(); resetShip(); spawnAsteroid(); }
        }
    }
}

// ===== Rendering =====
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    drawShip();
    for(auto &b:bullets) drawBullet(b);
    for(auto &a:asteroids) drawAsteroid(a);
    drawText(-0.95,0.9,"Score: "+to_string(score));
    drawText(0.7,0.9,"Lives: "+to_string(ship.lives));
    glutSwapBuffers();
}

// ===== Keyboard =====
void keyDown(unsigned char k,int,int){
    if(k==27) exit(0);
    if(k==' '){
        Bullet b;
        b.pos=ship.pos;
        b.vel={0.02*cos(ship.angle*M_PI/180),0.02*sin(ship.angle*M_PI/180)};
        b.life=100;
        bullets.push_back(b);
    }
}
void specialDown(int k,int,int){
    if(k==GLUT_KEY_LEFT) keyLeft=true;
    if(k==GLUT_KEY_RIGHT) keyRight=true;
    if(k==GLUT_KEY_UP) keyUp=true;
}
void specialUp(int k,int,int){
    if(k==GLUT_KEY_LEFT) keyLeft=false;
    if(k==GLUT_KEY_RIGHT) keyRight=false;
    if(k==GLUT_KEY_UP) keyUp=false;
}

// ===== Timer =====
void tick(int){
    update();
    glutPostRedisplay();
    glutTimerFunc(16,tick,0);
}

// ===== Main =====
int main(int argc,char**argv){
    srand(time(0));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(600,600);
    glutCreateWindow("Asteroids Shooter (Simple)");
    glClearColor(0,0,0,1);
    resetShip();
    for(int i=0;i<3;i++) spawnAsteroid();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyDown);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutTimerFunc(16,tick,0);
    glutMainLoop();
    return 0;
}
