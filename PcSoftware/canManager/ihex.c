#include "ihex.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

unsigned char cksum;

unsigned char hex2byte(const unsigned char *p) {
	int i;
	unsigned char c, ret=0;
	
	for (i=0; i<2; i++) {
		if (*p>='0' && *p<='9')
			c = *p-'0';
		else if (*p>='A' && *p<='F')
			c = *p-'A'+10;
		else if (*p>='a' && *p<='f')
			c = *p-'a'+10;
		else
			printf("Bad file format.\n");
		ret = (ret << 4) + c;
		p++;
	}
	cksum += ret;
	return ret;
}

int ihexLoadFile(char* file, unsigned char* buf, long *start_addr, long *len) {
	int fd;
	char read_char;
	unsigned char hex_byte[2];
	unsigned char reclen, rectyp;
	unsigned int offset, expected_offset=-1;
	unsigned char hex_buf[512], *p;
	int i, segments=0;
	int start_addr_set = 0;
	
	fd = open(file, O_RDONLY);
	*len = 0;
	
	while (1) {
		do {
			if (read(fd, &read_char, 1) != 1) return 0;
		} while (read_char != ':');
		cksum = 0;
		
		if (read(fd, &hex_byte, 2) != 2) return 0;
		reclen = hex2byte(hex_byte);
		
		if (read(fd, &hex_byte, 2) != 2) return 0;
		offset = hex2byte(hex_byte)<<8;
		
		if (read(fd, &hex_byte, 2) != 2) return 0;
		offset += hex2byte(hex_byte);
		
		if (read(fd, &hex_byte, 2) != 2) return 0;
		rectyp = hex2byte(hex_byte);
		
		switch (rectyp) {
		case 0:
			if (offset != expected_offset) {
				// new segment
				printf("New segment at offset 0x%x. Previous ended at 0x%x.\n", offset, expected_offset);
				segments++;
			}
				
			if (read(fd, &hex_buf, reclen*2) != reclen*2) return 0;
			p = hex_buf;
			for (i = 0; i < reclen; i++) {
				buf[(*len)++] = hex2byte(p);
				p += 2;
			}
			expected_offset = offset + reclen;
			if (!start_addr_set) {
				*start_addr = offset;
				start_addr_set = 1;
			}
			break;
		case 1:
			//*len = expected_offset - *start_addr;
			close(fd);
			return segments;
		case 2:
		case 3:
		case 4:
		case 5:
			if (read(fd, &hex_buf, reclen*2) != reclen*2) return 0;
			p = hex_buf;
			for (i = 0; i < reclen; i++) {
				hex2byte(p);
				p += 2;
			}
			break;
		default:
			printf("Error in hex-file!\n");
		}
		if (read(fd, &hex_byte, 2) != 2) return 0;
		hex2byte(hex_byte);
		if (cksum != 0) {
			printf("Invalid checksum!\n");
			return 0;
		}
	}
	return 0;
}
