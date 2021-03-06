/*
$Id:$

ST7565 LCD library!

Copyright (C) 2010 Limor Fried, Adafruit Industries

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

// some of this code was written by <cstone@pobox.com> originally; it is in the public domain.
*/

//#include <Wire.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

#include "ST7565.h"

#define ST7565_STARTBYTES 0

#define LCDWIDTH 132
#define LCDHEIGHT 64
















uint8_t is_reversed = 0;

// a handy reference to where the pages are on the screen // pages 0-7
const uint8_t pagemap[] = {3, 2, 1, 0, 7, 6, 5, 4};

// a 5x7 font table
extern uint8_t PROGMEM const font[];


// the memory buffer for the LCD // Adafruit Logo

uint8_t st7565_buffer[1024] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
0x03, 0x07, 0x07, 0x0e, 0x0e, 0x0c, 0x18, 0x18, 0x18, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x18, 0x18, 0x18, 0x0c, 0x0e, 0x0e, 0x07, 0x07,
0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7e, 0xf8, 0xf0, 0xe0,
0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0xc0, 0xe0, 0xf0, 0xf8, 0x7e, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x03, 0x1f, 0x7f, 0xff, 0xff, 0xff, 0xfe, 0xe0, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x3f,
0x7f, 0x3f, 0x1f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xfe, 0xff, 0xff, 0xff, 0x7f, 0x1f, 0x03, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x0f, 0x03, 0x00, 0x00, 0x00,
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0x0f, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x80, 0xf0, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x3f,
0x1f, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f,
0x1f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0x80, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe0, 0xf8, 0xfc, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfc, 0xf8, 0xe0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xe0,
0xf0, 0xf0, 0xf8, 0xf8, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0xe0, 0xf0, 0xf8,
0xf8, 0xf8, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xf8, 0xf8, 0xf0,
0xf0, 0xe0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

// reduces how much is refreshed, which speeds it up!
// originally derived from Steve Evans/JCW's mod but cleaned up and
// optimized
#define enablePartialUpdate

#ifdef enablePartialUpdate
static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif



static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) {
#ifdef enablePartialUpdate
    if (xmin < xUpdateMin) xUpdateMin = xmin;
    if (xmax > xUpdateMax) xUpdateMax = xmax;
    if (ymin < yUpdateMin) yUpdateMin = ymin;
    if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}

void ST7565::drawbitmap(uint8_t x, uint8_t y, const uint8_t* bitmap, uint8_t w, uint8_t h, uint8_t color) {
    for (uint8_t j = 0; j < h; j++) {
        for (uint8_t i = 0; i < w; i++) {
            if (pgm_read_byte(bitmap + i + (j / 8) * w) & _BV(j % 8)) {
                my_setpixel(x + i, y + j, color);
            }
        }
    }

    updateBoundingBox(x, y, x + w, y + h);
}


//===============================================================================================================================Character


void ST7565::drawstring(uint8_t x, uint8_t line, char* c) {
    while (c[0] != 0) {
        drawchar(x, line, c[0]);
        c++;
        x += 6; // 6 pixels wide
        if (x + 6 >= LCDWIDTH) {
            x = 0;    // ran out of this line
            line++;
        }
        if (line >= (LCDHEIGHT / 8))
            return;        // ran out of space :(
    }
}


void ST7565::drawstring_P(uint8_t x, uint8_t line, const char* str) {
    while (1) {
        char c = pgm_read_byte(str++);
        if (!c)
            return;
        drawchar(x, line, c);
        x += 1; // 6 pixels wide
        if (x + 6 >= LCDWIDTH) {
            x = 0;    // ran out of this line
            line++;
        }
        if (line >= (LCDHEIGHT / 8))
            return;        // ran out of space :(
    }
}

void  ST7565::drawchar(uint8_t x, uint8_t line, char c) {
    for (uint8_t i = 0; i < 5; i++) {
        st7565_buffer[x + (line * 128)] = pgm_read_byte(font + (c * 5) + i);
        x++;
    }

    updateBoundingBox(x, line * 8, x + 5, line * 8 + 8);
}


//===============================================================================================================================Character





// bresenham's algorithm - thx wikpedia
void ST7565::drawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
    uint8_t color) {
    uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    // much faster to put the test here, since we've already sorted the points
    updateBoundingBox(x0, y0, x1, y1);

    uint8_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int8_t err = dx / 2;
    int8_t ystep;

    if (y0 < y1) {
        ystep = 1;
    }
    else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            my_setpixel(y0, x0, color);
        }
        else {
            my_setpixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

// filled rectangle
void ST7565::fillrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {

    // stupidest version - just pixels - but fast with internal buffer!
    for (uint8_t i = x; i < x + w; i++) {
        for (uint8_t j = y; j < y + h; j++) {
            my_setpixel(i, j, color);
        }
    }

    updateBoundingBox(x, y, x + w, y + h);
}

// draw a rectangle
void ST7565::drawrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
    uint8_t color) {
    // stupidest version - just pixels - but fast with internal buffer!
    for (uint8_t i = x; i < x + w; i++) {
        my_setpixel(i, y, color);
        my_setpixel(i, y + h - 1, color);
    }
    for (uint8_t i = y; i < y + h; i++) {
        my_setpixel(x, i, color);
        my_setpixel(x + w - 1, i, color);
    }

    updateBoundingBox(x, y, x + w, y + h);
}

// draw a circle outline
void ST7565::drawcircle(uint8_t x0, uint8_t y0, uint8_t r,
    uint8_t color) {
    updateBoundingBox(x0 - r, y0 - r, x0 + r, y0 + r);

    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    my_setpixel(x0, y0 + r, color);
    my_setpixel(x0, y0 - r, color);
    my_setpixel(x0 + r, y0, color);
    my_setpixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        my_setpixel(x0 + x, y0 + y, color);
        my_setpixel(x0 - x, y0 + y, color);
        my_setpixel(x0 + x, y0 - y, color);
        my_setpixel(x0 - x, y0 - y, color);

        my_setpixel(x0 + y, y0 + x, color);
        my_setpixel(x0 - y, y0 + x, color);
        my_setpixel(x0 + y, y0 - x, color);
        my_setpixel(x0 - y, y0 - x, color);

    }



}

void ST7565::fillcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {
    updateBoundingBox(x0 - r, y0 - r, x0 + r, y0 + r);

    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    for (uint8_t i = y0 - r; i <= y0 + r; i++) {
        my_setpixel(x0, i, color);
    }

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        for (uint8_t i = y0 - y; i <= y0 + y; i++) {
            my_setpixel(x0 + x, i, color);
            my_setpixel(x0 - x, i, color);
        }
        for (uint8_t i = y0 - x; i <= y0 + x; i++) {
            my_setpixel(x0 + y, i, color);
            my_setpixel(x0 - y, i, color);
        }
    }
}

void ST7565::my_setpixel(uint8_t x, uint8_t y, uint8_t color) {
    if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
        return;

    // x is which column
    if (color)
        st7565_buffer[x + (y / 8) * 128] |= _BV(7 - (y % 8));
    else
        st7565_buffer[x + (y / 8) * 128] &= ~_BV(7 - (y % 8));
}

// the most basic function, set a single pixel
void ST7565::setpixel(uint8_t x, uint8_t y, uint8_t color) {
    if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
        return;

    // x is which column
    if (color)
        st7565_buffer[x + (y / 8) * 128] |= _BV(7 - (y % 8));
    else
        st7565_buffer[x + (y / 8) * 128] &= ~_BV(7 - (y % 8));

    updateBoundingBox(x, y, x, y);
}


// the most basic function, get a single pixel
uint8_t ST7565::getpixel(uint8_t x, uint8_t y) {
    if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
        return 0;

    return (st7565_buffer[x + (y / 8) * 128] >> (7 - (y % 8))) & 0x1;
}

void ST7565::begin(uint8_t contrast) {
    st7565_init();
    st7565_command(CMD_DISPLAY_ON);
    st7565_command(CMD_SET_ALLPTS_NORMAL);
    st7565_set_brightness(contrast);
}

void ST7565::st7565_init(void) {
    // set pin directions
    pinMode(sid, OUTPUT);
    pinMode(sclk, OUTPUT);
    pinMode(a0, OUTPUT);
    pinMode(rst, OUTPUT);
    pinMode(cs, OUTPUT);

    // toggle RST low to reset; CS low so it'll listen to us
    if (cs > 0)
        digitalWrite(cs, LOW);

    digitalWrite(rst, LOW);
    _delay_ms(500);
    digitalWrite(rst, HIGH);

    // LCD bias select
    st7565_command(CMD_SET_BIAS_7);
    // ADC select
    st7565_command(CMD_SET_ADC_REVERSE);
    // SHL select
    st7565_command(CMD_SET_COM_NORMAL);
    // Initial display line
    st7565_command(CMD_SET_DISP_START_LINE);
    


    // turn on voltage converter (VC=1, VR=0, VF=0)
    st7565_command(CMD_SET_POWER_CONTROL | 0x4);
    // wait for 50% rising
    _delay_ms(50);

    // turn on voltage regulator (VC=1, VR=1, VF=0)
    st7565_command(CMD_SET_POWER_CONTROL | 0x6);
    // wait >=50ms
    _delay_ms(50);

    // turn on voltage follower (VC=1, VR=1, VF=1)
    st7565_command(CMD_SET_POWER_CONTROL | 0x7);
    // wait
    _delay_ms(10);

    // set lcd operating voltage (regulator resistor, ref voltage resistor)
    st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);

    // initial display line
    // set page address
    // set column address
    // write display data

    // set up a bounding box for screen updates

    updateBoundingBox(0, 0, LCDWIDTH - 1, LCDHEIGHT - 1);
}

inline void ST7565::spiwrite(uint8_t c) {
    shiftOut(sid, sclk, MSBFIRST, c);
    /*
    int8_t i;
    for (i=7; i>=0; i--) {
      SCLK_PORT &= ~_BV(SCLK);
      if (c & _BV(i))
        SID_PORT |= _BV(SID);
      else
        SID_PORT &= ~_BV(SID);
      SCLK_PORT |= _BV(SCLK);
    }
    */

    /*
    // loop unwrapped! too fast doesnt work :(

    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(7))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);

    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(6))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);

    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(5))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);

    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(4))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);

    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(3))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);

    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(2))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);


    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(1))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);

    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(0))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);
  */

}
void ST7565::st7565_command(uint8_t c) {
    digitalWrite(a0, LOW);

    spiwrite(c);
}

void ST7565::st7565_data(uint8_t c) {
    digitalWrite(a0, HIGH);

    spiwrite(c);
}
void ST7565::st7565_set_brightness(uint8_t val) {
    st7565_command(CMD_SET_VOLUME_FIRST);
    st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}


//=========================================================================================================================Display

void ST7565::display(void) {
    uint8_t col, maxcol, p;

    /*
    Serial.print("Refresh ("); Serial.print(xUpdateMin, DEC);
    Serial.print(", "); Serial.print(xUpdateMax, DEC);
    Serial.print(","); Serial.print(yUpdateMin, DEC);
    Serial.print(", "); Serial.print(yUpdateMax, DEC); Serial.println(")");
    */

    for (p = 0; p < 8; p++) {
        /*
          putstring("new page! ");
          uart_putw_dec(p);
          putstring_nl("");
        */
#ifdef enablePartialUpdate
        // check if this page is part of update
        if (yUpdateMin >= ((p + 1) * 8)) {
            continue;   // nope, skip it!
        }
        if (yUpdateMax < p * 8) {
            break;
        }
#endif

        st7565_command(CMD_SET_PAGE | pagemap[p]);


#ifdef enablePartialUpdate
        col = xUpdateMin;
        maxcol = xUpdateMax;
#else
        // start at the beginning of the row
        col = 0;
        maxcol = LCDWIDTH - 1;
#endif

        st7565_command(CMD_SET_COLUMN_LOWER | ((col + ST7565_STARTBYTES) & 0xf));
        st7565_command(CMD_SET_COLUMN_UPPER | (((col + ST7565_STARTBYTES) >> 4) & 0x0F));
        st7565_command(CMD_RMW);

        for (; col <= maxcol; col++) {
            //uart_putw_dec(col);
            //uart_putchar(' ');
            st7565_data(st7565_buffer[(128 * p) + col]);
        }
    }

#ifdef enablePartialUpdate
    xUpdateMin = LCDWIDTH - 1;
    xUpdateMax = 0;
    yUpdateMin = LCDHEIGHT - 1;
    yUpdateMax = 0;
#endif
}

//=========================================================================================================================Display


// clear everything
void ST7565::clear(void) {
    memset(st7565_buffer, 0, 1024);
    updateBoundingBox(0, 0, LCDWIDTH - 1, LCDHEIGHT - 1);
}


// this doesnt touch the buffer, just clears the display RAM - might be handy
void ST7565::clear_display(void) {
    uint8_t p, c;

    for (p = 0; p < 8; p++) {
        /*
          putstring("new page! ");
          uart_putw_dec(p);
          putstring_nl("");
        */

        st7565_command(CMD_SET_PAGE | p);
        for (c = 0; c < 129; c++) {
            //uart_putw_dec(c);
            //uart_putchar(' ');
            st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
            st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
            st7565_data(0x0);
        }
    }
}
