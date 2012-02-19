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

int offset = 0;
void display(void)
{
/* clear all pixels  */
   glClear (GL_COLOR_BUFFER_BIT);

/* draw white polygon (rectangle) with corners at
 * (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)  
 * NOTE: We declare the color BEFORE the vertex location
 * If you want a different color for each vertex, make a glColor3f() statement before the glVertex3f statement.
 */
   glColor3f (1.0, 1.0, 1.0);
   glBegin(GL_POLYGON);
      glVertex3f (offset+100, 100, 0.0);
      glVertex3f (offset+300, 100, 0.0);
      glVertex3f (offset+300, 300, 0.0);
      glVertex3f (offset+100, 300, 0.0);
   glEnd();

   glRasterPos2f(0, 24);
   char name[] = "Ross";
   for (int i = 0; i < sizeof(name); i++)
   {
	   glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, name[i]);
   }
   glRasterPos2f(0, 0);
   char name2[] = "Anderson";
      for (int i = 0; i < sizeof(name2); i++)
   {
	   glutBitmapCharacter(GLUT_BITMAP_9_BY_15, name2[i]);
   }
/* don't wait!  
 * start processing buffered OpenGL routines 
 */
   glFlush ();
}



void init (void) 
{
/* select clearing color 	*/
   glClearColor (0.0, 0.0, 0.0, 0.0);



/* initialize viewing values  */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0, 400.0, 0.0, 400.0, -1.0, 1.0);
}

/* 
 * Declare initial window size, position, and display mode
 * (single buffer and RGBA).  Open window with "hello"
 * in its title bar.  Call initialization routines.
 * Register callback function to display graphics.
 * Enter main loop and process events.
 */

void my_idle (void)
{
	offset++;

	if (offset > 400)
	{
		offset = -300;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (400, 400); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("hello");
   init ();
   glutIdleFunc(my_idle);
   glutDisplayFunc(display); 
   glutMainLoop();
   return 0;   /* ANSI C requires main to return int. */
}
