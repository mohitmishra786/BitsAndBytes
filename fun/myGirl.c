#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

float eyeAngle = 0.0f;
float rotationAngle = 0.0f;

void drawSphere(float radius, int slices, int stacks) {
    GLUquadric *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluSphere(quadric, radius, slices, stacks);
    gluDeleteQuadric(quadric);
}

void drawHair() {
    glColor3f(0.5f, 0.3f, 0.7f); // Purple hair color
    glPushMatrix();
    glTranslatef(0.0f, 0.05f, 0.0f);
    glScalef(1.2f, 1.4f, 1.1f);
    drawSphere(0.2, 30, 30);
    glPopMatrix();
}

void drawFace() {
    glColor3f(1.0f, 0.9f, 0.85f); // Lighter anime-style skin tone
    glPushMatrix();
    glScalef(1.0f, 1.2f, 0.9f); // More oval face shape
    drawSphere(0.2, 30, 30);
    glPopMatrix();
}

void drawEye(float x) {
    glPushMatrix();
    glTranslatef(x, 0.05f, 0.17f);
    
    // Eye white
    glColor3f(1.0f, 1.0f, 1.0f);
    drawSphere(0.06f, 20, 20);
    
    // Iris
    glColor3f(0.2f, 0.7f, 0.8f); // Blue-green color
    glTranslatef(0.0f, 0.0f, 0.055f);
    drawSphere(0.04f, 20, 20);
    
    // Pupil
    glColor3f(0.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 0.035f);
    drawSphere(0.02f, 10, 10);
    
    // Highlight
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(0.01f, 0.01f, 0.005f);
    drawSphere(0.01f, 5, 5);
    
    glPopMatrix();
}

void drawMouth() {
    glColor3f(1.0f, 0.6f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, -0.08f, 0.17f);
    glRotatef(90, 1, 0, 0);
    glScalef(1.0f, 0.5f, 1.0f);
    GLUquadric *quadric = gluNewQuadric();
    gluQuadricDrawStyle(quadric, GLU_FILL);
    gluCylinder(quadric, 0.03, 0.03, 0.01, 20, 1);
    gluDeleteQuadric(quadric);
    glPopMatrix();
}

void drawHairAccessory(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(0.8f, 0.4f, 0.8f); // Light purple
    glutSolidSphere(0.03, 10, 10);
    glColor3f(0.6f, 0.2f, 0.6f); // Dark purple
    glutSolidSphere(0.02, 10, 10);
    glPopMatrix();
}

void drawClothing() {
    glColor3f(0.8f, 0.8f, 0.9f); // Light purple-gray
    glPushMatrix();
    glTranslatef(0.0f, -0.3f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.3, 0.4, 20, 20);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
    
    glRotatef(rotationAngle, 0, 1, 0);
    
    drawHair();
    drawFace();
    drawEye(-0.08f);
    drawEye(0.08f);
    drawMouth();
    drawHairAccessory(0.15f, 0.2f, 0.1f);
    drawHairAccessory(-0.15f, 0.2f, 0.1f);
    drawClothing();
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
}

void update(int value) {
    eyeAngle += 5.0f;
    if (eyeAngle > 360.0f) {
        eyeAngle -= 360.0f;
    }
    
    rotationAngle += 1.0f;
    if (rotationAngle > 360.0f) {
        rotationAngle -= 360.0f;
    }
    
    glutPostRedisplay();
    glutTimerFunc(25, update, 0);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("You're the one for me");
    
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);
    
    glutMainLoop();
    return 0;
}
