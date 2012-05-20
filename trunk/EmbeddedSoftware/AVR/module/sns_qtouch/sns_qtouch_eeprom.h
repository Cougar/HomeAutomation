#ifndef SNS_QTOUCH_EEPROM
#define SNS_QTOUCH_EEPROM

#include "sns_qtouch.h"
// Do not change anything below this line.
// ----------------------------------------------------------------

	#include <drivers/misc/eeprom_crc8.h>
	#include <avr/eeprom.h>
	#define EEDATA 			(uint8_t*)&eeprom_sns_qtouch.Data
	#define EEDATA16		(uint16_t*)&eeprom_sns_qtouch.Data
	#define EEDATA32		(uint32_t*)&eeprom_sns_qtouch.Data
	#define EEDATA_CALC_CRC		eeprom_crc8((uint8_t*)&eeprom_sns_qtouch.Data,sizeof(struct sns_qtouch_Data))

#if ((__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ == 6 && __AVR_LIBC_REVISION__ >= 7)||(__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ > 6)||__AVR_LIBC_MAJOR__ > 1)
	#define EEDATA_UPDATE_CRC	eeprom_update_byte((uint8_t*)&eeprom_sns_qtouch.crc,eeprom_crc8((uint8_t*)&eeprom_sns_qtouch.Data,sizeof(struct sns_qtouch_Data)))
#else
	#define EEDATA_UPDATE_CRC	eeprom_write_byte((uint8_t*)&eeprom_sns_qtouch.crc,eeprom_crc8((uint8_t*)&eeprom_sns_qtouch.Data,sizeof(struct sns_qtouch_Data)))
#endif

	#define EEDATA_OK		EEDATA_CALC_CRC == EEDATA_STORED_CRC
	#define EEDATA_STORED_CRC 	eeprom_read_byte((uint8_t*)&eeprom_sns_qtouch.crc)
	
	struct eeprom_sns_qtouch{
		struct sns_qtouch_Data Data;
		uint8_t crc;
	};
	extern struct eeprom_sns_qtouch EEMEM eeprom_sns_qtouch;
	
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
