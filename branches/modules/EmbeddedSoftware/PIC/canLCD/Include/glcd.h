/*********************************************************************
 *
 *                  Graphical LCD header file
 *
 *********************************************************************
 * FileName:        $HeadURL: http://svn.arune.se/svn/HomeAutomation/trunk/EmbeddedSoftware/PIC/canLCD/Include/lcd.h $
 * Last changed:	$LastChangedDate: 2006-11-21 10:28:39 +0100 (ti, 21 nov 2006) $
 * By:				$LastChangedBy: johboh $
 * Revision:		$Revision: 140 $
 ********************************************************************/


#ifndef __GLCD_H
#define __GLCD_H

#include <compiler.h>
#include <delays.h>
#include <stackTasks.h>
#include <typedefs.h>

void glcdInit(void);
void glcdSetStartLine(BYTE);
void glcdClearScr(void);

void glcdDisplayPic(BYTE,BYTE,const rom BYTE*);
void glcdPutrs(BYTE, BYTE, const rom BYTE *);
void glcdPuts(BYTE, BYTE, BYTE *);
void glcdPutc(BYTE);

#endif // __GLCD_H
