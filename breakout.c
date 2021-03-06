/**
 * breakout.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * An implementation of the 1976 Atari game Breakout.
 * Requires the Stanford Portable Library.
 * 
 * Usage: ./breakout
 */

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 550
#define WIDTH 400

// number of rows and columns of bricks
#define ROWS 5
#define COLS 8

// size of bricks in pixels
#define BRICKGAP 5
#define BRICKWIDTH  (WIDTH - ((COLS + 1) * BRICKGAP)) / COLS
#define BRICKHEIGHT 12

// radius of ball in pixels
#define BALLRADIUS 10

// size and yposition of paddle in pixels
#define PADWIDTH 80
#define PADHEIGHT 10
#define PADY HEIGHT-50

// lives
#define LIVES 3

// prototypes
void pauseUntilClick(void);
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

	/** ball movement */
	double hVelocity = drand48() + 2.0;
	double vVelocity = 2.0;	
	// object ball collides with
	GObject object;

	// game begins when user clicks
	waitForClick();

    // keep playing until game over
    while (lives && bricks)
    {
        /** paddle movement */
		GEvent event = getNextEvent(MOUSE_EVENT);
		if (event != NULL)
		{
			if (getEventType(event) == MOUSE_MOVED) 
			{
				double halfPadWidth = getWidth(paddle)/2;
				double x = getX(event);	
				// keep the paddle within window bounds
				if (x + halfPadWidth > WIDTH)
				{
					x = WIDTH - PADWIDTH;
				}
				else if (x - halfPadWidth < 0)
				{
					x = 0;
				} 
				else 
				{
					x -= halfPadWidth;
				}
				setLocation(paddle, x, PADY); 
			}
		}		

		// move ball
        move(ball, hVelocity, vVelocity);

        // bounce off right edge of window
        if (getX(ball) + getWidth(ball) >= WIDTH)
        {
            hVelocity = -hVelocity;
        }

        // bounce off left edge of window
        else if (getX(ball) <= 0)
        {
            hVelocity = -hVelocity;
        }

		// bounce off top edge of window
		else if (getY(ball) <= 0)
		{
			vVelocity = -vVelocity;	
		}

		// collision with bottom edge of window
		else if (getY(ball) >= HEIGHT)
		{
			lives--;
			double bx = (WIDTH / 2) - BALLRADIUS*2;	
			double by = (HEIGHT / 1.5) - BALLRADIUS*2;
			setLocation(ball, bx, by);
			if (lives) waitForClick();
		}
				
		/* bounce off objects */ 
		object = detectCollision(window, ball);
		
		// bounce off paddle
		if (object == paddle)
		{
			vVelocity = -vVelocity;
		}
	
		// bounce off bricks
		else if (object != NULL && (strcmp(getType(object), "GRect") == 0) &&
				(strcmp(getType(object), "GLabel") != 0))
		{
			vVelocity = -vVelocity;
			// delete brick
			removeGWindow(window, object);
			bricks--; 
			// update score
			points++;
			updateScoreboard(window, label, points);
		}

        // linger before moving again
        pause(10);
    }

	/* GAME OVER */
	removeGWindow(window, paddle);
	removeGWindow(window, ball);

	// display game over
	GLabel gameover = newGLabel("GAME OVER");
	setFont(gameover, "SansSerif-50");
	setColor(gameover, "LIGHT_GRAY");
	double gox = (getWidth(window) - getWidth(gameover)) / 2;
    double goy = (getHeight(window) - getHeight(gameover));
    setLocation(gameover, gox, goy);
    add(window, gameover);

    // wait for click before exiting
    waitForClick();
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
	// colors for the bricks
	char *brickColors[]	= {"RED", "ORANGE", "GREEN", "BLUE", "CYAN"};	

    int i, j, x, y, color;
	x = BRICKGAP;
	y = 10;
	color = 0;
	for (i = 0; i < ROWS; i++)
	{
		for (j = 0; j < COLS; j++)
		{
			GRect brick = newGRect(x, y, BRICKWIDTH, BRICKHEIGHT);
			setColor(brick, brickColors[color]);
			setFilled(brick, true);
			add(window, brick);
			x += BRICKWIDTH + BRICKGAP;
		}
		color++;
		x = BRICKGAP;
		y += BRICKHEIGHT + BRICKGAP;
	}	
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
	// find centre position
	double x = (WIDTH / 2) - BALLRADIUS;	
	double y = (HEIGHT / 1.5) - BALLRADIUS;

	// set up ball
    GOval ball = newGOval(x, y, BALLRADIUS*2, BALLRADIUS*2);
	setColor(ball, "BLACK");
	setFilled(ball, true);
	add(window, ball);
	return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // determine location
	double x = (WIDTH / 2) - (PADWIDTH / 2);
	double y = PADY;
    
	//set up the paddle
	GRect paddle = newGRect(x, y, PADWIDTH, PADHEIGHT);
	setColor(paddle, "BLACK");
	setFilled(paddle, true);

	add(window, paddle);
	return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("0");
	setFont(label, "SansSerif-50");
	setColor(label, "LIGHT_GRAY");
	updateScoreboard(window, label, 0);  	
    add(window, label);
	sendToBack(label);
	return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * BALLRADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * BALLRADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * BALLRADIUS, y + 2 * BALLRADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
