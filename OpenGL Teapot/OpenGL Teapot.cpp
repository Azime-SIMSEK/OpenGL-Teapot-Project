#include <windows.h>
#include <math.h>

// To prevent GLUT related exit errors
#define GLUT_DISABLE_ATEXIT_HACK
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

// Window size
int screenWidth = 600;
int screenHeight = 600;

// Window IDs
int window1, window2;

// Rotation angles for window 1
float sphi = 0.0f;      // Rotation around the Y-axis
float stheta = 0.0f;    // Rotation around the X-axis

// Mouse state;all buttons for window 1
bool mouse_left = false;
bool mouse_middle = false;
bool mouse_right = false;

// To keep track of whether a selection has been made from the menu.
bool menuSelected = false;

int mouseX, mouseY;

// View modes for window 2
enum ViewMode { TOP, BOTTOM, LEFT, RIGHT };
ViewMode viewMode2 = TOP;

// ---------------- Initialization ----------------
void myInit()
{
    //background color(black)
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    // ----- BONUS: Lighting -----
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // the position of light 
    GLfloat lightPos[] = { 3.0f, 3.0f, 3.0f, 1.0f };
    // ambient light ; to prevent it from being too dark
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    // main light ; to illuminate the object
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

// ---------------- Display Window 1 ----------------
void displayWindow1()
{
    // Clears the screen and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection(camera lens)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);

    // ModelView (object rotations)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // To place the camera at a fixed point
    gluLookAt(0, 0, 5,
        0, 0, 0,
        0, 1, 0);

    // For rotations based on mouse movement
    glRotatef(stheta, 1, 0, 0);   // Rotation around the X-axis
    glRotatef(sphi, 0, 1, 0);     // Rotation around the Y-axis

    //  White wireframe teapot code
    glColor3f(1, 1, 1);
    glutWireTeapot(0.8);

    //swaps the back buffer with the front buffer in a double-buffered OpenGL context
    glutSwapBuffers();
}

// ---------------- Display Window 2 ----------------
void displayWindow2()
{
    // Clears the screen and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection(camera lens)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);

    // ModelView
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Fixed camera
    gluLookAt(0, 0, 5,
        0, 0, 0,
        0, 1, 0);
    if (!menuSelected)
    {
        // ===== DEFAULT INITIAL VIEW ====
        glTranslatef(0.0f, -1.5f, -0.5f);
        glRotatef(0, 0, 1, 0);
        // KIRMIZI
        glColor3f(1.0f, 0.0f, 0.0f);
    }
    else
    {

        // View-based rotation + color
        switch (viewMode2)
        {
        case TOP:
            glRotatef(-90, 1, 0, 0);
            // green
            glColor3f(0.0f, 1.0f, 0.0f);
            break;

        case BOTTOM:
            glRotatef(90, 1, 0, 0);
            // yellow
            glColor3f(1.0f, 1.0f, 0.0f);
            break;

        case LEFT:
            glRotatef(180, 0, 1, 0);
            // blue
            glColor3f(0.0f, 0.0f, 1.0f);
            break;

        case RIGHT:
            glRotatef(0, 0, 1, 0);
            // blue + green = cyan color
            glColor3f(0.0f, 1.0f, 1.0f);
            break;
        }
    }

    glutWireTeapot(0.8);
    glutSwapBuffers();
}

// ---------------- Menu Callback ----------------
void menuCallback(int option)
{
    menuSelected = true;

    switch (option)
    {
    case 1: viewMode2 = TOP;    break;
    case 2: viewMode2 = BOTTOM; break;
    case 3: viewMode2 = LEFT;   break;
    case 4: viewMode2 = RIGHT;  break;
    }

    glutSetWindow(window2);
    glutPostRedisplay();
}

// ---------------- myReshape ----------------
void myReshape(int width, int height)
{ // adjust the camera aspect ratio to match that of the viewport
    glViewport(0, 0, width, height); // update viewport
    //glOrtho(-width,width,-height,height,-1000,1000);
    glOrtho(-1, 1, -1, 1, -1, 1);
}


// ---------------- myKeyboard ----------------
//  to exit the window
void myKeyboard(unsigned char key, int x, int y)
{
    // in ASCII 27 corresponds to the ESC (Escape) key
    if (key == 27)
        exit(0);
}

// ---------------- myMouse ----------------
void myMouse(int button, int state, int x, int y)
{
    // Mouse interaction ONLY for window 1
    if (glutGetWindow() != window1)
        return;

    mouse_left = (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN);
    mouse_middle = (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN);
    mouse_right = (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN);

    mouseX = x;
    mouseY = y;
}

// ---------------- myMouseMove ----------------
void myMouseMove(int x, int y)
{
    // If the mouse is moved in window 2, do nothing.
    if (glutGetWindow() != window1)
        return;

    // Calculate how much the mouse moved since last frame

    float dx = x - mouseX;
    float dy = mouseY - y;  // y is reversed because screen coordinates increase downward, 
    // but we want upward movement to correspond to positive rotation.


// If any mouse button is pressed, apply rotation to the teapot
    if (mouse_left || mouse_middle || mouse_right)
    {
        sphi += dx * 0.5f;            // 0.5f is a sensitivity factor,smaller value provides slower rotation                                 
        stheta += dy * 0.5f;          // larger value provide faster rotation

    }

    // Store the current mouse position,so we can calculate movement next time
    mouseX = x;
    mouseY = y;

    // to make sure window 1 is active
    glutSetWindow(window1);
    // ekranı yeniden çizmesi için, eğer bu satırı silersem mouse açıları değişiyor ancak ekranda bir değişiklik olmuyor
    glutPostRedisplay();
}

// ---------------- main ----------------
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    // -------- Window 1 --------
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    window1 = glutCreateWindow("Teapot - Interactive View");

    myInit();
    glutDisplayFunc(displayWindow1);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(myKeyboard);
    glutMouseFunc(myMouse);
    glutMotionFunc(myMouseMove);

    // -------- Window 2 --------
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(750, 100);
    window2 = glutCreateWindow("Teapot - Menu View");

    myInit();
    glutDisplayFunc(displayWindow2);
    glutReshapeFunc(myReshape);

    // Menu (ONLY window 2)
    int menu = glutCreateMenu(menuCallback);
    glutAddMenuEntry("From Top", 1);
    glutAddMenuEntry("From Bottom", 2);
    glutAddMenuEntry("From Left", 3);
    glutAddMenuEntry("From Right", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    return 0;
}
