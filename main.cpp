#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

struct Vec {
    float x, y;
    Vec() : x(0), y(0) {}
    Vec(float _x, float _y) : x(_x), y(_y) {}
    Vec operator+(const Vec &o) const { return Vec(x+o.x, y+o.y); }
    Vec operator-(const Vec &o) const { return Vec(x-o.x, y-o.y); }
    Vec operator*(float s) const { return Vec(x*s, y*s); }
    float len() const { return std::sqrt(x*x + y*y); }
};
int winW = 800, winH = 600;
float toWorldX(int sx){ return (2.0f*sx/winW - 1.0f) * (float)winW/winH; }
float toWorldY(int sy){ return 1.0f - 2.0f*sy/winH; }

// ---------- Game objects ----------
struct Bullet {
    Vec pos, vel;
    int life; // frames
    Bullet(Vec p, Vec v): pos(p), vel(v), life(80) {}
};

struct Asteroid {
    Vec pos, vel;
    float radius;
    int size; // 3 = big, 2 = med, 1 = small
    float angle;
    Asteroid(Vec p, Vec v, int s): pos(p), vel(v), size(s) {
        radius = 20.0f * s; // tune radius
        angle = ((float)rand()/RAND_MAX)*2*3.14159f;
    }
};

struct Particle {
    Vec pos, vel;
    int life;
    Particle(Vec p, Vec v, int l): pos(p), vel(v), life(l) {}
};

// Ship
struct Ship {
    Vec pos;
    Vec vel;
    float angle; // radians (0 => pointing up)
    int lives;
    bool alive;
    Ship(){ reset(); }
    void reset(){ pos = Vec(0,0); vel = Vec(0,0); angle = 0; lives=3; alive=true; }
} ship;

std::vector<Bullet> bullets;
std::vector<Asteroid> asteroids;
std::vector<Particle> particles;

int score = 0;
bool keyLeft=false, keyRight=false, keyUp=false;

// ---------- Game management ----------
void spawnAsteroid(int size){
    // spawn on screen edge with inward velocity
    float edge = rand()%4; // 0: left,1:right,2:top,3:bottom
    float x,y;
    if(edge==0){ x = - (float)winW/winH*1.2f; y = ((float)rand()/RAND_MAX)*2-1; }
    else if(edge==1){ x = (float)winW/winH*1.2f; y = ((float)rand()/RAND_MAX)*2-1; }
    else if(edge==2){ x = ((float)rand()/RAND_MAX)*2-1; y = 1.2f; }
    else { x = ((float)rand()/RAND_MAX)*2-1; y = -1.2f; }
    Vec pos(x,y);
    // velocity towards center with random offset
    Vec dir = Vec((float)(-x + ((float)rand()/RAND_MAX-0.5f)), (float)(-y + ((float)rand()/RAND_MAX-0.5f)));
    float speed = 0.0025f + 0.0015f*rand()/RAND_MAX + 0.001f*(3-size);
    // normalize dir
    float l = dir.len(); if(l==0) l=1; dir = dir*(1.0f/l);
    Vec vel = dir * speed;
    asteroids.emplace_back(pos, vel, size);
}

void spawnInitialAsteroids(int n=5){
    asteroids.clear();
    for(int i=0;i<n;i++) spawnAsteroid(3);
}

// ---------- Drawing helpers ----------
void drawText(float x, float y, const std::string &s){
    glRasterPos2f(x,y);
    for(char c: s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void drawShip(const Ship &sh){
    // triangle pointing up (angle=0 upward)
    glPushMatrix();
    glTranslatef(sh.pos.x, sh.pos.y, 0);
    glRotatef(sh.angle * 180.0f / 3.14159f, 0,0,1);
    glBegin(GL_TRIANGLES);
    glColor3f(0.2f, 0.8f, 1.0f);
    glVertex2f(0, 0.035f);
    glVertex2f(-0.02f, -0.02f);
    glVertex2f(0.02f, -0.02f);
    glEnd();
    // flame when thrusting
    if(keyUp){
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f,0.6f,0.1f);
        glVertex2f(-0.01f, -0.02f);
        glVertex2f(0.01f, -0.02f);
        glVertex2f(0, -0.04f - (rand()%100)/5000.0f);
        glEnd();
    }
    glPopMatrix();
}

void drawBullet(const Bullet &b){
    glBegin(GL_QUADS);
    glColor3f(1,1,0);
    glVertex2f(b.pos.x-0.002f, b.pos.y-0.002f);
    glVertex2f(b.pos.x+0.002f, b.pos.y-0.002f);
    glVertex2f(b.pos.x+0.002f, b.pos.y+0.002f);
    glVertex2f(b.pos.x-0.002f, b.pos.y+0.002f);
    glEnd();
}

void drawAsteroid(const Asteroid &a){
    int steps = 16; // polygon approximation
    glPushMatrix();
    glTranslatef(a.pos.x, a.pos.y, 0);
    glRotatef(a.angle*180.0f/3.14159f,0,0,1);
    glBegin(GL_POLYGON);
    glColor3f(0.6f,0.6f,0.6f);
    for(int i=0;i<steps;i++){
        float theta = (float)i/steps * 2*3.14159f;
        float r = a.radius / 300.0f; // scale radius to world coords (tuned)
        float jitter = (rand()%1000)/5000.0f - 0.1f;
        float rr = r * (0.8f + fabs(jitter));
        glVertex2f(rr * cos(theta), rr * sin(theta));
    }
    glEnd();
    glPopMatrix();
}

void drawParticle(const Particle &p){
    float t = p.life/60.0f;
    glPointSize(2.0f + 2.0f*t);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.5f*t, 0.1f);
    glVertex2f(p.pos.x, p.pos.y);
    glEnd();
}

// ---------- Collision ----------
bool circleCollision(const Vec &aPos, float aR, const Vec &bPos, float bR){
    Vec d = aPos - bPos;
    float dist = d.len();
    return dist < (aR + bR);
}

// ---------- Update ----------
void createExplosion(const Vec &pos, int amount=15){
    for(int i=0;i<amount;i++){
        float ang = ((float)rand()/RAND_MAX)*2*3.14159f;
        float sp = 0.002f + ((float)rand()/RAND_MAX)*0.006f;
        Vec v(cos(ang)*sp, sin(ang)*sp);
        particles.emplace_back(pos, v, 30 + rand()%20);
    }
}

void splitAsteroid(int idx){
    if(idx<0 || idx>= (int)asteroids.size()) return;
    Asteroid a = asteroids[idx];
    Vec pos = a.pos; int sz = a.size;
    createExplosion(pos, 10);
    if(sz>1){
        // spawn two smaller asteroids
        for(int i=0;i<2;i++){
            Vec v(((float)rand()/RAND_MAX-0.5f)*0.01f, ((float)rand()/RAND_MAX-0.5f)*0.01f);
            asteroids.emplace_back(pos, v, sz-1);
        }
    }
    // remove original
    asteroids.erase(asteroids.begin()+idx);
}

void updateGame(){
    // ship control
    if(keyLeft) ship.angle += 0.05f;
    if(keyRight) ship.angle -= 0.05f;
    if(keyUp){
        // thrust forward based on ship.angle
        Vec forward(sin(ship.angle), cos(ship.angle));
        ship.vel = ship.vel + forward * 0.0009f;
    }
    // friction
    ship.vel = ship.vel * 0.999f;
    ship.pos = ship.pos + ship.vel;
    // screen wrap ship
    if(ship.pos.x > 1.2f) ship.pos.x = -1.2f;
    if(ship.pos.x < -1.2f) ship.pos.x = 1.2f;
    if(ship.pos.y > 1.2f) ship.pos.y = -1.2f;
    if(ship.pos.y < -1.2f) ship.pos.y = 1.2f;

    // bullets
    for(int i=(int)bullets.size()-1;i>=0;i--){
        bullets[i].pos = bullets[i].pos + bullets[i].vel;
        bullets[i].life--;
        if(bullets[i].life<=0) bullets.erase(bullets.begin()+i);
        else {
            // wrap bullets also
            if(bullets[i].pos.x > 1.2f) bullets[i].pos.x = -1.2f;
            if(bullets[i].pos.x < -1.2f) bullets[i].pos.x = 1.2f;
            if(bullets[i].pos.y > 1.2f) bullets[i].pos.y = -1.2f;
            if(bullets[i].pos.y < -1.2f) bullets[i].pos.y = 1.2f;
        }
    }

    // asteroids
    for(auto &a : asteroids){
        a.pos = a.pos + a.vel;
        a.angle += 0.005f * (1.0f + a.size);
        // wrap
        if(a.pos.x > 1.3f) a.pos.x = -1.3f;
        if(a.pos.x < -1.3f) a.pos.x = 1.3f;
        if(a.pos.y > 1.3f) a.pos.y = -1.3f;
        if(a.pos.y < -1.3f) a.pos.y = 1.3f;
    }

    // particles
    for(int i=(int)particles.size()-1;i>=0;i--){
        particles[i].pos = particles[i].pos + particles[i].vel;
        particles[i].life--;
        if(particles[i].life<=0) particles.erase(particles.begin()+i);
    }

    // collisions: bullets vs asteroids
    for(int i=(int)asteroids.size()-1;i>=0;i--){
        float ar = asteroids[i].radius/300.0f;
        for(int j=(int)bullets.size()-1;j>=0;j--){
            if(circleCollision(asteroids[i].pos, ar, bullets[j].pos, 0.004f)){
                // hit
                bullets.erase(bullets.begin()+j);
                score += 10 * asteroids[i].size;
                splitAsteroid(i);
                break; // asteroid changed, break to outer
            }
        }
    }

    // asteroid vs ship
    for(int i=(int)asteroids.size()-1;i>=0;i--){
        float ar = asteroids[i].radius/300.0f;
        if(circleCollision(asteroids[i].pos, ar, ship.pos, 0.03f)){
            // collision
            createExplosion(ship.pos, 25);
            ship.lives--;
            ship.pos = Vec(0,0);
            ship.vel = Vec(0,0);
            ship.angle = 0;
            if(ship.lives<=0){ ship.alive=false; }
            // remove asteroid
            splitAsteroid(i);
        }
    }

    // spawn more asteroids slowly
    static int frameCounter = 0;
    frameCounter++;
    if(frameCounter % 300 == 0){ // every ~5 seconds at 60fps: tune by timer rate
        spawnAsteroid(3);
    }
}

// ---------- Rendering ----------
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    // background stars
    glPointSize(1.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f,1.0f,1.0f);
    for(int i=0;i<50;i++){
        float sx = ((float)(i*37 % 101)/50.0f)-1.0f;
        float sy = ((float)(i*23 % 97)/50.0f)-1.0f;
        glVertex2f(sx, sy);
    }
    glEnd();

    // draw ship
    if(ship.alive) drawShip(ship);

    // draw bullets
    for(auto &b: bullets) drawBullet(b);
    // draw asteroids
    for(auto &a: asteroids) drawAsteroid(a);
    // draw particles
    for(auto &p: particles) drawParticle(p);

    // HUD
    glColor3f(1,1,1);
    drawText(-0.98f, 0.95f, "Score: "+std::to_string(score));
    drawText(0.4f, 0.95f, std::string("Lives: ")+std::to_string(ship.lives));
    if(!ship.alive){ drawText(-0.15f, 0.0f, "GAME OVER - Press R to Restart"); }

    glutSwapBuffers();
}

// ---------- Input ----------
void keyDown(unsigned char key, int x, int y){
    if(key==27) exit(0);
    if(key==' '){ // shoot
        if(ship.alive){
            Vec forward(sin(ship.angle), cos(ship.angle));
            Vec bpos = ship.pos + forward*0.04f;
            Vec bvel = ship.vel + forward*0.01f;
            bullets.emplace_back(bpos, bvel);
        }
    }
    if(key=='r' || key=='R'){
        // restart
        ship.reset(); score=0; bullets.clear(); asteroids.clear(); particles.clear(); spawnInitialAsteroids(5);
    }
}

void specialDown(int key, int x, int y){
    if(key==GLUT_KEY_LEFT) keyLeft=true;
    if(key==GLUT_KEY_RIGHT) keyRight=true;
    if(key==GLUT_KEY_UP) keyUp=true;
}
void specialUp(int key, int x, int y){
    if(key==GLUT_KEY_LEFT) keyLeft=false;
    if(key==GLUT_KEY_RIGHT) keyRight=false;
    if(key==GLUT_KEY_UP) keyUp=false;
}

// ---------- Timer ----------
void tick(int v){
    updateGame();
    glutPostRedisplay();
    glutTimerFunc(16, tick, 0); // ~60fps
}

// ---------- Window & reshape ----------
void reshape(int w, int h){
    winW = w; winH = h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w/h;
    // We'll keep world coords in -aspect..aspect x and -1..1 y
    glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv){
    srand((unsigned int)time(nullptr));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Asteroids Shooter - Simple OpenGL (GLUT)");

    glClearColor(0.02f, 0.02f, 0.06f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutTimerFunc(16, tick, 0);

    // initial state
    spawnInitialAsteroids(5);

    glutMainLoop();
    return 0;
}
