#include "main.h"
#include "timer.h"
#include "ball.h"
#include "trampoline.h"
#include "ground.h"
#include "pacs.h"
#include "flying.h"
#include "magnet.h"
#include "pond.h"
#include "spikes.h"
#include <ctime>

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

Ball ball;
Trampoline trampolinemax;
Ground layer1,layer2,layer3,layer4,layer5;
Pacs pacs[32];
Flying flying;
Magnet magnet;
Pond pond;
Spikes spikes;
int flag;

float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;

Timer t60(1.0 / 60);
float v[32],V[32];
/* Render the scene with openGL */
/* Edit this function according to your assignment */
bool detect_collision_flyingup(bounding_box_t flying, bounding_box_t ball)
{
  return ((abs(flying.x-ball.x)<=1.05)&&(abs(flying.y-ball.y)<=0.3)&&(ball.y>flying.y));
}
bool detect_collision_flyingdown(bounding_box_t flying, bounding_box_t ball)
{
  return ((abs(flying.x-ball.x)<=1.05)&&(abs(flying.y-ball.y)<=0.3)&&(ball.y<flying.y));
}
bool detect_up_collision(bounding_box_t trampoline, bounding_box_t ball)
{
  return ((abs(trampoline.y - ball.y) < 0.25 ) && (abs(trampoline.x - ball.x) < 1.0));
}
bool detect_side_collision(bounding_box_t trampoline, bounding_box_t ball)
{
  return ((abs(trampoline.x-ball.x )<= 1.10) && (abs(ball.y -layer5.position.y)< 1.00) && (ball.x-trampoline.x<0));
}
bool detect_right_collision(bounding_box_t trampoline, bounding_box_t ball)
{
  return ((abs(ball.x-trampoline.x) <= 1.10)&&(ball.y - layer5.position.y < 1.00)&&(ball.x-trampoline.x>0));
}

void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    // glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    // glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    // glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    glm::mat4 MVP;  // MVP = Projection * View * Model
    // Don't change unless you are sure!!

    // Scene render


    layer1.draw(VP);
    layer2.draw(VP);
    layer3.draw(VP);
    layer4.draw(VP);
    layer5.draw(VP);
    pond.draw(VP);
    magnet.draw(VP);
    for(int c=0; c<32 ; c++)
    {
      int var=0;
      if(c<16)
      {
        var=1;
      }
      if(pacs[c].pacflag == 1)
      {
        pacs[c].draw(VP,var);
      }
    }
    spikes.draw(VP);
    flying.draw(VP);
    trampolinemax.draw(VP);
    ball.draw(VP);
}

void tick_input(GLFWwindow *window) {
    int left  = glfwGetKey(window, GLFW_KEY_LEFT);
    int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int up = glfwGetKey(window, GLFW_KEY_UP);
    //collision between pacs and ball
    for(int i=0;i<32;i++)
    {
      if(pacs[i].pacflag)
      {
          if(detect_collision(pacs[i].bounding_box(),ball.bounding_box()  ))
          {
            if(pacs[i].a != 1 || pacs[i].a == 1)
            {
                pacs[i].pacflag = 0;
                ball.speedy = 6;
                ball.collide=1;
            }
          /*  else if(pacs[i].a == 1)
            {
              pacs[i].pacflag = 0;
              ball.speedy = rand()/10 + (-5);
              ball.speed = rand()/4 +(-2);
              ball.random = 1;
            }*/
          }
      }
    }
    if(detect_collision_flyingup(flying.bounding_box(),ball.bounding_box()))
    {
      ball.speedy=3;
      ball.speed=0;
      ball.goingup=1;
    }
    if(detect_collision_flyingdown(flying.bounding_box(),ball.bounding_box()))
    {
      ball.speedy=0;
      ball.speed=0;
      ball.goingup=1;
    }
    if(abs(ball.position.x)<=0.75 && (ball.position.y <= -1.2))
    {
      ball.position.x=0;
      ball.position.y=-2.2;
      ball.inwater=1;
    }

  //collision between trampoline and ball
   if (detect_up_collision(trampolinemax.bounding_box(), ball.bounding_box()))
    {
      //printf("tramp\n");
        ball.speedy = 10;
        ball.tramp=1;
    }
    else if(detect_side_collision(trampolinemax.bounding_box(),ball.bounding_box()))
    {
      ball.position.x -= 0.1;
    }
    else if(detect_right_collision(trampolinemax.bounding_box(),ball.bounding_box()))
    {
      ball.position.x += 0.1;
    }
    else
    {
      if (up && ball.goingup != 1 )
      {
        //printf("k11");
        if(ball.inwater != 1)
        {
          ball.speedy = 6.5;
          ball.goingup=1;
        }
        else if(ball.inwater == 1)
        {
        //  printf("kk");
          ball.speedy = 8.5;
          ball.goingup=1;
        }
      }
        //left key
        if (left)
        {
          ball.position +=glm::vec3(-.03,0,0);
        }
        //up key


        //right key
        if(right)
        {
          ball.position +=glm::vec3(.03,0,0);
        }
    }
    //circulating the balls

    for(int ll=0;ll<32;ll++)
    {
      if(pacs[ll].position.x>4 && pacs[ll].pacflag)
      {
        pacs[ll].position.x=-5.0;
      }
    }
    if(magnet.presence)
    {
      ball.speed=0.004;
    }
}

void tick_elements() {
    ball.tick();
        if(ball.position.y <= -1.2 && ball.goingup == 1)
        {
          ball.speedy=0;
          ball.goingup=0;
        }
        if(ball.position.y <= -1.2 && ball.tramp == 1 && (abs(ball.position.x-trampolinemax.position.x) > 1.1))
        {
          ball.speedy=0;
          ball.tramp=0;
        }
        if(ball.position.y <= -1.2 && ball.collide == 1 && (abs(ball.position.x-trampolinemax.position.x) > 1.1))
        {
          ball.speedy=0;
          ball.collide=0;
        }
        if((ball.position.x >= 4.0 || ball.position.x <= -4.0) && ball.random == 1 && (abs(ball.position.x-trampolinemax.position.x) > 1.1))
        {
          ball.speed=0;
        }
        if(ball.position.y <= -1.2 && ball.random == 1 && (abs(ball.position.x-trampolinemax.position.x) > 1.1))
        {
          ball.speedy=0;
          ball.speed=0;
          ball.random=0;
        }
        spikes.tick();
        //ball.position += glm::vec3(0,ball.speedy*t + 0.5*0.25*t*t,0);
        //ball.speedy -= 0.25*t;
        flying.tick();
        if(flying.position.x >= 5.0)
        {
          flying.position.x=-5.0;
        }
        for(int c=0;c<32;c++)
        {
          if(pacs[c].pacflag==1)
          {
            pacs[c].tick();
          }
        }
    /*if (detect_collision(trampolinemax.bounding_box(), ball.bounding_box())) {

    }*/

}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    flag=1;
    for(int r=0;r<32;r++)
    {
      v[r]=float(rand()%50+50)/660;
    }
    for(int r=0;r<32;r++)
    {
      V[r]=flag*v[r];
    }
    for(int c=0;c<32;c++)
    {
      color_t paccolours[5]={COLOR_PAC1,COLOR_PAC2,COLOR_PAC3,COLOR_PAC4,COLOR_PAC5};

      int getter = rand()%5;
      int var=0;
      if(c<16)
        var=1;
      pacs[c]=Pacs(-4.2,float(rand()%40)/10-0.25,paccolours[getter],v[c],double(((rand()%8)+0.75)/100),var);
    }
    layer1 = Ground(0 , -3.75 , COLOR_BROWN);
    layer2 = Ground(0 , -3.25 , COLOR_BROWN);
    layer3 = Ground(0 , -2.75 , COLOR_BROWN);
    layer4 = Ground(0 , -2.25 , COLOR_BROWN);
    layer5 = Ground(0 , -1.75 , COLOR_GRASS);
    flying = Flying(-5,2.25, COLOR_FLYING);
    flying.speed = 0.0356988 ;
    trampolinemax = Trampoline(2.5, -0.5 , COLOR_RED);
    trampolinemax.radius = 0.75 ;
    spikes = Spikes(-3.5, -1.5, COLOR_LRED);
    ball       = Ball(-2, -1.25, COLOR_RED);
    ball.speed = 0;
    ball.inwater=0;
    ball.speedy = 0;
    pond = Pond(0,-1.5,COLOR_WATER,1.0);
    magnet=Magnet(3,3,COLOR_BLACK);
    magnet.presence=1;
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("resources/Sample_GL.vert", "resources/Sample_GL.frag");
    // Get a handle for our "MVP" uniform

    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 600;
    int height = 600;

    window = initGLFW(width, height);

    initGL (window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }

        // Poll for Keyflying and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x)  < (0.5)) &&
           (abs(a.y - b.y)  < (0.5));
}


void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
