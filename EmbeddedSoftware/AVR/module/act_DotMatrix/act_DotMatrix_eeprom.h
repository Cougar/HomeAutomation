#ifndef ACT_DOTMATRIX_EEPROM
#define ACT_DOTMATRIX_EEPROM

#include "act_DotMatrix.h"
// Do not change anything below this line.
// ----------------------------------------------------------------

	#include <drivers/misc/eeprom_crc8.h>
	#include <avr/eeprom.h>
	#define EEDATA 			(uint8_t*)&eeprom_act_DotMatrix.Data
	#define EEDATA16		(uint16_t*)&eeprom_act_DotMatrix.Data
	#define EEDATA32		(uint32_t*)&eeprom_act_DotMatrix.Data
	#define EEDATA_CALC_CRC		eeprom_crc8((uint8_t*)&eeprom_act_DotMatrix.Data,sizeof(struct act_DotMatrix_Data))

#if ((__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ == 6 && __AVR_LIBC_REVISION__ >= 7)||(__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ > 6)||__AVR_LIBC_MAJOR__ > 1)
	#define EEDATA_UPDATE_CRC	eeprom_update_byte((uint8_t*)&eeprom_act_DotMatrix.crc,eeprom_crc8((uint8_t*)&eeprom_act_DotMatrix.Data,sizeof(struct act_DotMatrix_Data)))
#else
	#define EEDATA_UPDATE_CRC	eeprom_write_byte((uint8_t*)&eeprom_act_DotMatrix.crc,eeprom_crc8((uint8_t*)&eeprom_act_DotMatrix.Data,sizeof(struct act_DotMatrix_Data)))
#endif

	#define EEDATA_OK		EEDATA_CALC_CRC == EEDATA_STORED_CRC
	#define EEDATA_STORED_CRC 	eeprom_read_byte((uint8_t*)&eeprom_act_DotMatrix.crc)
	
	struct eeprom_act_DotMatrix{
		struct act_DotMatrix_Data Data;
		uint8_t crc;
	};
	extern struct eeprom_act_DotMatrix EEMEM eeprom_act_DotMatrix;
	
	#define WITH_CRC	1
	#define WITHOUT_CRC	0


#if ((__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ == 6 && __AVR_LIBC_REVISION__ >= 7)||(__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ > 6)||__AVR_LIBC_MAJOR__ > 1)
	static __inline__ void eeprom_write_byte_crc(uint8_t *__p, uint8_t __value, uint8_t crc) {
	    eeprom_update_byte(__p, __value);
	    if (crc == WITH_CRC)
	    {
	      EEDATA_UPDATE_CRC;
	    }
	}
	static __inline__ void eeprom_write_word_crc(uint16_t *__p, uint16_t __value, uint8_t crc) {
	    eeprom_update_word(__p, __value);
	    if (crc == WITH_CRC)
	    {
	      EEDATA_UPDATE_CRC;
	    }
	}
	static __inline__ void eeprom_write_dword_crc(uint32_t *__p, uint32_t __value, uint8_t crc) {
	    eeprom_update_dword(__p, __value);
	    if (crc == WITH_CRC)
	    {
	      EEDATA_UPDATE_CRC;
	    }
	}
#else
	static __inline__ void eeprom_write_byte_crc(uint8_t *__p, uint8_t __value, uint8_t crc) {
	  if (__value != eeprom_read_byte(__p))
	  {
	    eeprom_write_byte(__p, __value);
	    if (crc == WITH_CRC)
	    {
	      EEDATA_UPDATE_CRC;
	    }
	  }
	}
	static __inline__ void eeprom_write_word_crc(uint16_t *__p, uint16_t __value, uint8_t crc) {
	  if (__value != eeprom_read_word(__p))
	  {
	    eeprom_write_word(__p, __value);
	    if (crc == WITH_CRC)
	    {
	      EEDATA_UPDATE_CRC;
	    }
	  }
	}
	static __inline__ void eeprom_write_dword_crc(uint32_t *__p, uint32_t __value, uint8_t crc) {
	  if (__value != eeprom_read_dword(__p))
	  {
	    eeprom_write_dword(__p, __value);
	    if (crc == WITH_CRC)
	    {
	      EEDATA_UPDATE_CRC;
	    }
	  }
	}
#endif

#endif
