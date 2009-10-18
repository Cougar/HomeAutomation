using System;
using System.Collections.Generic;
using System.Text;

namespace canConfigurator
{
    class crc16
    {
        // *****************************************************************************
        // Update the CRC for transmitted and received data using
        // the CCITT 16bit algorithm (X^16 + X^12 + X^5 + X^0)
        //
        // Adapted by R Andrag from an 
        // asm routine by John C. Wren (google to find the original code)
        // roland.andrag at gmail.com
        //
        // To check your crc implementation use:
        // http://www.zorc.breitbandkatze.de/crc.html
        // Web calculator by Sven Reifegerste
        // CRC order = 16
        // CRC polynomial = 1021 hex
        // Initial value = FFFF hex
        // direct CRC
        // Final XOR value = 0
        // Don't reverse data bytes or CRC result
        // calculator allows you to enter hex bytes by prefixing %
        // append 00 00 to your mesage when you calulate the crc
        // i.e. crc should work out to zero on: message:00:00:crc_h:crc_l
        // e.g. %01%02%00%00 gives a crc of 9c14
        //      %01%02%00%00%9c%14 gives a crc of 0000
        // %01%02%9c%14 gives a crc of 9327 hex
        // *****************************************************************************

        private static ushort update_crc(ushort p_crc, byte data)
        {

            p_crc = (ushort)((p_crc >> 8) | (p_crc << 8));
            p_crc ^= data;
            p_crc ^= (ushort)(((ushort)p_crc & 0xff) >> 4);
            p_crc ^= (ushort)(p_crc << 12);
            p_crc ^= (ushort)((p_crc & 0xff) << 5);

            return p_crc;
        }

        public static ushort calc_crc(byte[] p_data, ushort n_bytes)
        {
            ushort crc = 0xFFFF;

            for (int i=0; i<n_bytes; i++)
            {
                crc = update_crc(crc, p_data[i]);
            }

            return crc;
        }


        /*
        unsafe public static void update_crc(ushort* p_crc, byte data)
        {
            *p_crc = (*p_crc >> 8) | (*p_crc << 8);
            *p_crc ^= data;
            *p_crc ^= (*p_crc & 0xff) >> 4;
            *p_crc ^= *p_crc << 12;
            *p_crc ^= (*p_crc & 0xff) << 5;
        }


        unsafe public static ushort calc_crc(byte* p_data, ushort n_bytes)
        {
            ushort crc = 0xFFFF;

            for (; n_bytes > 0; n_bytes--)
            {
                update_crc(&crc, *(p_data++));
            }

            update_crc(&crc, 0);
            update_crc(&crc, 0);

            return crc;
        }
        */

    }
}
