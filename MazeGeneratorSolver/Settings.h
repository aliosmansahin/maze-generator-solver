#pragma once

/*

This file contains configurable parameters, constants and preprocessor definitions used by the application.

Author: Ali Osman ÞAHÝN

Date: December 7, 2025

*/

/* ------- SETTINGS ------- */

/*
Change these values to modify the window dimensions and title used in the application.
*/
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Maze Generator and Solver"

/* Change this value to set simulation interval, for example, it updates every 0.01 seconds by default */
#define SIMULATION_INTERVAL 0.01

/* Change this value to set camera sensitivity */
#define CAMERA_SENSITIVITY 1

/*
Change these values to edit maze width and height
WARNING: Use odd numbers
*/
#define MAZE_WIDTH 11
#define MAZE_HEIGHT 11

/*
Change this value to generate maze from a seed.
If it is commented, the seed will be generated randomly
*/
//#define MAZE_SEED 0

/*
Change this value to 
WARNING: Works only if maze is being generated randomly,
	DON'T UNCOMMENT THIS IF YOU ARE USING RANDOM GENERATION

WARNING: Making it more than 9 might make app unstable,
	because, seed is an unsigned int, and max value of it is 4294967295 which is a 10 digid value,
	which can be fault as an overflow
*/
#define MAZE_SEED_RIGID_COUNT 9

/* ------- DEBUG ------- */

//#define DEBUG_CLEAR_COLOR // Uncomment this line to enable debug clear color (red) during rendering.
//#define DEBUG_PRINT_MAZE_INTO_CONSOLE // Uncomment this line to print maze into console after generation (might not work with large mazes)