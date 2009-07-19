/**
 * Touch gesture driver
 * 
 * @date	2009-07-19
 * 
 * @author	Linus Lundin, Jonas Andersson, Anders Runeson
 *   
 */

#include "gesture.h"

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
gesture parseBuffer(point *buffer, uint8_t startIndex, uint8_t endIndex)
{
	int16_t f1_k = 0, f3_tmp = 0, f4_tmp = 0, f5_tmp = 0;
	uint8_t f7_tmp = 0, f7_sign = 0, f1_sign = 0, f3_yref = 0, xMin, xMax, yMin, yMax, f9_tmp;
	gesture functionData;
	functionData.f1=0;
	functionData.f7=0;
	
	/* Function 6 indicates the sign value of the y coordinates of the last point minus y coordinates of the first point */
	functionData.f6 = 0;
	if ((int16_t)buffer[endIndex].y - (int16_t)buffer[startIndex].y > 0)
	{
		functionData.f6 = 1;
	}
	
	xMin = buffer[startIndex].x;
	xMax = buffer[startIndex].x;
	yMin = buffer[startIndex].y;
	yMax = buffer[startIndex].y;
	f3_yref = (buffer[startIndex].y + buffer[endIndex].y)/2;
	f1_k = ((buffer[endIndex].y-buffer[startIndex].y)<<4)/(buffer[endIndex].x-buffer[startIndex].x);
	f1_sign = 2;

	for (uint8_t i = startIndex+1; i < endIndex-1; i++)
	{
		/* Function 1 indicates the number of intersection points between an input gesture g(x) and the straight line f(x) 
		which connects the first and last point of the gesture */
		if (((f1_k*(buffer[i].x-buffer[0].x))>>4) + buffer[0].y < buffer[i].y) 
		{
			if (f1_sign != 1)
			{
				functionData.f1++;
			}
			f1_sign = 1;
		}
		else 
		{
			if (f1_sign != 0)
			{
				functionData.f1++;
			}
			f1_sign = 0;
		}
		
		/* Function 3 indicates the sign value of area gap between g(x) and f(x) */
		if (buffer[i].x > min(buffer[startIndex].x, buffer[endIndex].x) && buffer[i].x < max(buffer[startIndex].x, buffer[endIndex].x)) 
		{
		    f3_tmp += f3_yref - (buffer[i].y + buffer[i-1].y)/2;
		}
		
		/* find min and max of x and y */
		if (xMin > buffer[i].x)
		{
			xMin = buffer[i].x;
		}
		if (xMax < buffer[i].x)
		{
			xMax = buffer[i].x;
		}
		if (yMin > buffer[i].y)
		{
			yMin = buffer[i].y;
		}
		if (yMax < buffer[i].y)
		{
			yMax = buffer[i].y;
		}
		
		/* Function 4 represents the sign value of the sum of the x coordinates of all the points that constitute strokes 
		minus the x coordinates of the last point */
		f4_tmp += (int16_t)buffer[i].x - (int16_t)buffer[endIndex].x;
		
		/* Function 5 checks the sign value of the sum of the x coordinates of all the points that constitute strokes 
		minus x coordinates of the first point */
		f5_tmp += (int16_t)buffer[i].x - (int16_t)buffer[startIndex].x;
	}
	
	/* Function 7 indicates the number of intersection points between an input gesture g(x) and all horizontal lines yi, 
	where i is between 0 and n, that constitute a gesture*/
	for (uint8_t j = yMin; j < yMax-1; j++)
	{
		f7_tmp = 0xff;
		f7_sign = 0;
		for (uint8_t i = startIndex+1; i < endIndex-1; i++)
		{
			if (j > buffer[i].y) 
			{
				if (f7_sign != '+')
				{
					f7_tmp++;
				}
				f7_sign = '+';
			}
			else {
				if (f7_sign != '-')
				{
					f7_tmp++;
				}
				f7_sign = '-';
			}
		}
		if (f7_tmp > functionData.f7)
		{
			functionData.f7 = f7_tmp;
		}
	}
	
	functionData.f1--;
	/* Function 2 checks if the y coordinates of the points which exist between the intersection point and the last point are 
	greater than the y coordinates of the intersection point */
	functionData.f2 = f1_sign;
	if (functionData.f1 == 0)
	{
		functionData.f2 = 2;
	}
	
	/* Function 3 indicates the sign value of area gap between g(x) and f(x) */
	functionData.f3 = 1;
	if (f3_tmp > 0)
	{
		functionData.f3 = 0;
	}
		
	/* Function 9 represents the sign value of the multiplication of x coordinates of the first point and the last point 
	minus the mediate value of x coordinates of all the points */
	//f9_tmp = ((buffer[startIndex].x - (xMax+xMin)/2)*(buffer[endIndex].x - (xMax+xMin)/2));
	f9_tmp = ((buffer[startIndex].x - (xMax+xMin)/2 > 0) ^ (buffer[endIndex].x - (xMax+xMin)/2 > 0));
	functionData.f9 = 0;
	if (!f9_tmp)
	{
		functionData.f9 = 1;
	}
	
	/* Function 5 checks the sign value of the sum of the x coordinates of all the points that constitute strokes 
	minus x coordinates of the first point */
	functionData.f5 = 0;
	if (f5_tmp > 0)
	{
		functionData.f5 = 1;
	}
	
	/* Function 4 represents the sign value of the sum of the x coordinates of all the points that constitute strokes 
	minus the x coordinates of the last point */
	functionData.f4 = 0;
	if (f4_tmp > 0)
	{
		functionData.f4 = 1;
	}
	
	/* Function 8 checks whether or not x coordinates of all the points except for the first point and the last point is between x0 and xn */
	functionData.f8 = 1;
	if (xMin+F8_OFFSET < min(buffer[startIndex].x,buffer[endIndex].x) || xMax > max(buffer[startIndex].x,buffer[endIndex].x)+F8_OFFSET)
	{
		functionData.f8 = 0;
	}
		
	//printf("1%u2%c3%c4%c5%c6%c7%u8%c9%c\n", f1, f2, f3, f4, f5, f6, f7, f8, f9);

	return functionData;
}


