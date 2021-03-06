/*********************************************************************
 *
 *                  Vector remappings
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#ifndef BOOT_H
#define BOOT_H
 
#ifdef DEBUG_MODE
    #define RM_RESET_VECTOR             0x000000
    #define RM_HIGH_INTERRUPT_VECTOR    0x000008
    #define RM_LOW_INTERRUPT_VECTOR     0x000018
#else
    #define RM_RESET_VECTOR             0x003000
    #define RM_HIGH_INTERRUPT_VECTOR    0x003008
    #define RM_LOW_INTERRUPT_VECTOR     0x003018
#endif


    
#endif //BOOT_H
