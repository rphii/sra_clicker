#ifndef MAIN_H
#define MAIN_H

#ifndef UNICODE
#define UNICODE
#endif

/********************/
/*** I NEED THOSE ***/
/********************/

#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/**********************/
/*** USER LIBRARIES ***/
/**********************/

#include "mouse.h"
#include "keyboard.h"
#include "locate.h"

/******************************/
/*** FIXING MY HIGHLIGHTING ***/
/******************************/

typedef uint8_t uint8_t;
typedef uint16_t uint16_t;
typedef uint32_t uint32_t;

#endif
