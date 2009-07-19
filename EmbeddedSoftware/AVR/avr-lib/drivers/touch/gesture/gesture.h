/** 
 * @defgroup Touch gesture driver
 * @code #include <drivers/touch/gesture/gesture.h> @endcode
 * 
 * @brief Implementation of 'A new gesture recognition algorithm and segmentation method of Korean scripts for gesture-allowed ink editor' by Mi Gyung Cho
 * 
 *
 * @author	Linus Lundin, Jonas Andersson, Anders Runeson
 * @date	2009
 */

/**@{*/


#ifndef GESTURE_H_
#define GESTURE_H_
#include <stdio.h>
#include <config.h>

/* */
typedef struct
{
	uint8_t x;
	uint8_t y;
} point;

/* */
typedef struct
{
	uint8_t f1;
	uint8_t f2;
	uint8_t f3;
	uint8_t f4;
	uint8_t f5;
	uint8_t f6;
	uint8_t f7;
	uint8_t f8;
	uint8_t f9;
} gesture;

/* */
static __inline__ uint8_t min(uint8_t val1, uint8_t val2) {
  if (val1 > val2)
  {
  	return val2;
  }
  return val1;
}

/* */
static __inline__ uint8_t max(uint8_t val1, uint8_t val2) {
  if (val1 > val2)
  {
  	return val1;
  }
  return val2;
}

#define F8_OFFSET	15

/**
 * Touch gesture parser
 * Implementation of 'A new gesture recognition algorithm and segmentation 
 * method of Korean scripts for gesture-allowed ink editor' by Mi Gyung Cho
 * 
 * Call function parseBuffer with a buffer containing x/y coordinates, the buffer start and end.
 * Returns function results that describe the gesture in 24bits
 * 
 * @param buffer
 * 		Pointer to buffer to where to data to parse is stored
 * @param startIndex
 * 		First value in buffer
 * @param endIndex
 * 		Last value in buffer
 * @return
 * 		Struct with parsed function results
 */
 gesture parseBuffer(point *buffer, uint8_t startIndex, uint8_t endIndex);

/**@}*/
#endif /*GESTURE_H_*/
