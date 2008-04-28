/*********************************************************************
 *
 *                  LCD header file
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/


#ifndef __LCD_H
#define __LCD_H

#include <compiler.h>
#include <delays.h>
#include <stackTasks.h>
#include <typedefs.h>

void lcdClear(void);
void lcdPuts(char * s);
void lcdPutrs(const rom char *s);
void lcdPutc(char c);
void lcdGoto(BYTE pos);
void lcdInit(void);

#endif // __LCD_H
