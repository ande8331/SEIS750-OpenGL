/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/*
 * Nathan Gossett Comments
 * hello.c
 * This is a simple, introductory OpenGL program.  This is OpenGL 1.* code, and uses no GLSL or advanced features
 * We'll be moving beyond this style of code soon, but for now this is simple and easy to work with.
 *
 * This version of the code has some viewing code to make our conversion from GL coordinates to GLUT coordinates easier
 */
#include <GL/glut.h>
#include "target.h"
#include <stdio.h>

#define CROSSHAIR_LENGTH 9
#define CROSSHAIR_WIDTH 1
#define CROSSHAIR_HOLE 1
#define TARGET_COUNT 6
#define FLASH_INTERVAL 45

int windowWidth = 400;
int windowHeight = 400;
bool targetsMoving = true;
target targetArray[TARGET_COUNT];
float mouseX = 0;
float mouseY = 0;

// Print a null terminated string starting at the specified x/y position.
void printString(char * str, float x, float y)
{
   glRasterPos2f(x, y);
   for (int i = 0; str[i] != '\0'; i++)
   {
	   glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
   }
}

// Draw a single frame
void display(void)
{
    /* clear all pixels  */
   glClear (GL_COLOR_BUFFER_BIT);

    // Draw Targets
    bool crossHairsOverTarget = false;
    int targetsAlive = 0;
    for (int i = 0; i < TARGET_COUNT; i++)
    {
		// only process targets still alive
        if (targetArray[i].targetDisplayable())
        {
            targetsAlive++;

            // Paint the targets yellow
            glColor3f(1.0, 1.0, 0.0);

            // Advance position if we are in motion
            if (targetsMoving)
            {
                targetArray[i].updatePosition(windowWidth, windowHeight);
            }

            // Paint the rectangle
            glBegin(GL_POLYGON);
                glVertex3f (targetArray[i].getX(), targetArray[i].getY(), 0.0);
                glVertex3f (targetArray[i].getX() + targetArray[i].getLength(), targetArray[i].getY(), 0.0);
                glVertex3f (targetArray[i].getX() + targetArray[i].getLength(), targetArray[i].getY() + targetArray[i].getHeight(), 0.0);
                glVertex3f (targetArray[i].getX(), targetArray[i].getY() + targetArray[i].getHeight(), 0.0);
            glEnd();

            // Determine if the crosshairs are over this target
            if (targetArray[i].targetAtPosition(mouseX, mouseY))
            {
                crossHairsOverTarget = true;
            }
        }
    }

    // Set the color for the crosshairs
    if (crossHairsOverTarget == true)
    {
		// red crosshairs if over target
        glColor3f(1.0, 0.0, 0.0);
    }
    else
    {
		// otherwise green
        glColor3f(0.0, 1.0, 0.0);
    }

    // Draw Crosshairs    
    glBegin(GL_POLYGON);
        glVertex3f(mouseX-CROSSHAIR_HOLE, mouseY - CROSSHAIR_WIDTH, 0.0);
        glVertex3f(mouseX-CROSSHAIR_HOLE-CROSSHAIR_LENGTH, mouseY - CROSSHAIR_WIDTH, 0.0);
        glVertex3f(mouseX-CROSSHAIR_HOLE-CROSSHAIR_LENGTH, mouseY + CROSSHAIR_WIDTH, 0.0);
        glVertex3f(mouseX-CROSSHAIR_HOLE, mouseY + CROSSHAIR_WIDTH, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex3f(mouseX+CROSSHAIR_HOLE, mouseY - CROSSHAIR_WIDTH, 0.0);
        glVertex3f(mouseX+CROSSHAIR_HOLE+CROSSHAIR_LENGTH, mouseY - CROSSHAIR_WIDTH, 0.0);
        glVertex3f(mouseX+CROSSHAIR_HOLE+CROSSHAIR_LENGTH, mouseY + CROSSHAIR_WIDTH, 0.0);
        glVertex3f(mouseX+CROSSHAIR_HOLE, mouseY + CROSSHAIR_WIDTH, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex3f(mouseX-CROSSHAIR_WIDTH, mouseY + CROSSHAIR_HOLE, 0.0);
        glVertex3f(mouseX-CROSSHAIR_WIDTH, mouseY + CROSSHAIR_HOLE + CROSSHAIR_LENGTH, 0.0);
        glVertex3f(mouseX+CROSSHAIR_WIDTH, mouseY + CROSSHAIR_HOLE + CROSSHAIR_LENGTH, 0.0);
        glVertex3f(mouseX+CROSSHAIR_WIDTH, mouseY + CROSSHAIR_HOLE, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex3f(mouseX-CROSSHAIR_WIDTH, mouseY - CROSSHAIR_HOLE, 0.0);
        glVertex3f(mouseX-CROSSHAIR_WIDTH, mouseY - CROSSHAIR_HOLE - CROSSHAIR_LENGTH, 0.0);
        glVertex3f(mouseX+CROSSHAIR_WIDTH, mouseY - CROSSHAIR_HOLE - CROSSHAIR_LENGTH, 0.0);
        glVertex3f(mouseX+CROSSHAIR_WIDTH, mouseY - CROSSHAIR_HOLE, 0.0);
    glEnd();

	// Provide game status
	glColor3f(0.0, 1.0, 0.0);
    char msg[100];
	if (targetsAlive == 1)
	{
		sprintf_s(msg, "%d targets down, %d target to go.", TARGET_COUNT-targetsAlive, targetsAlive);
	}
	else if (TARGET_COUNT-targetsAlive == 1)
	{
		sprintf_s(msg, "%d target down, %d targets to go.", TARGET_COUNT-targetsAlive, targetsAlive);
	}
	else
	{
		sprintf_s(msg, "%d targets down, %d targets to go.", TARGET_COUNT-targetsAlive, targetsAlive);
	}
	printString(msg, 10, 10);

	// Display completion message
    if (targetsAlive == 0)
    {
		static int flashCount = 0;

		// Cycle the message on and off
		if (flashCount < FLASH_INTERVAL)
		{
			sprintf(msg, "You are awesome!");
			glColor3f(1.0, 0.5, 0.0);
			printString(msg, windowWidth/2-(4.5*strlen(msg)), windowHeight/2);

			sprintf(msg, "Press F2 to Restart");
			glColor3f(1.0, 0.5, 0.0);
			printString(msg, windowWidth/2-(4.5*strlen(msg)), windowHeight/2-16);
		}
		else if (flashCount > FLASH_INTERVAL*1.75)
		{
			flashCount = 0;
		}

		flashCount++;
    }

/* don't wait!  
 * start processing buffered OpenGL routines 
 */
   glFlush ();
   glutSwapBuffers();
}


/* Initialize */
void init (void) 
{
    /* set clearing color 	*/
   glClearColor (0.0, 0.0, 0.0, 0.0);

    /* Create the targets */
    for (int i = 0; i < TARGET_COUNT; i++)
    {
        targetArray[i] = target();
    }

	targetsMoving = true;
}

/* Function call for each time a shot is fired */
void gun_fire(float x, float y)
{
    // Send the shot coordinates to each target and let them
    // determine if they were hit
    for (int i = 0; i < TARGET_COUNT; i++)
    {
        targetArray[i].takingFire(x, y);
    }
}

/* Function call for game reset */
void my_special(int key, int x, int y) 
{
    if (key == GLUT_KEY_F2)
    {
		init();
    }
}

/* Function call to handle other key presses */
void my_key(unsigned char key, int x, int y) 
{
    // Fire
    if (key == ' ')
    {
        gun_fire(mouseX, mouseY);
    }

    // Start/Stop motion
    if ((key == 'm') || (key == 'M'))
    {
        targetsMoving = !targetsMoving;
    }

    //glutPostRedisplay();
}

/* Monitor mouse clicks */
void my_mouse(int button, int state, int x, int y) 
{
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
    {
        int glX = x;
        int glY = windowHeight - y;
        gun_fire(glX, glY);

        mouseX = glX;
        mouseY = glY;
    }
}

/* Monitor mouse position */
void my_mouse_motion(int x, int y)
{
    int glX = x;
    int glY = windowHeight - y;

    mouseX = glX;
    mouseY = glY;
}

/* Take in resized dimensions */
void my_reshape(int w, int h) 
{
    windowWidth = w;
    windowHeight = h;
  
	// Use ortho to set the window to map mouse inputs pixel for pixel  
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, windowWidth, 0.0, windowHeight, -1.0, 1.0);
   glViewport(0, 0, windowWidth, windowHeight);
   glMatrixMode(GL_MODELVIEW);
}

void my_timer(int v) 
{
	/* calls the display function v times a second */
	glutPostRedisplay();
	glutTimerFunc(1000/v, my_timer, v);
}

/* 
 * Declare initial window size, position, and display mode
 * (single buffer and RGBA).  Open window with "hello"
 * in its title bar.  Call initialization routines.
 * Register callback function to display graphics.
 * Enter main loop and process events.
 */


int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (windowWidth, windowHeight); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Assignment 1 - Ross Anderson");   
   glutKeyboardFunc(my_key);
   glutSpecialFunc(my_special);
   glutMouseFunc(my_mouse);
   glutPassiveMotionFunc(my_mouse_motion);
   glutReshapeFunc(my_reshape);
   glutTimerFunc(500, my_timer, 60);

   init ();

   glutDisplayFunc(display); 
   glutMainLoop();
   return 0;   /* ANSI C requires main to return int. */
}
