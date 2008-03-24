#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#define DEVICE "/dev/midi1"
#define BAUDRATE B115200
#define TRIG_LEVEL 128
#define SAMPLE_FREQ 1000

#define SAMPLES_PER_PLOT 200

struct termios oldtio,newtio;

int main(int argc, char* argv[]) {
	int input;
	FILE* gnuplot;
	int i;
	float out_data;
	unsigned char buf[SAMPLES_PER_PLOT];
	unsigned char trig = TRIG_LEVEL;
	unsigned freq = SAMPLE_FREQ;
	
	input = open(DEVICE, O_RDONLY | O_NOCTTY);
	if (input <0) {perror(DEVICE); return -1; }
	gnuplot = popen("gnuplot", "w");
	
	tcgetattr(input,&oldtio); /* save current port settings */
	
	// bzero not defined? moving newtio outside main to zero it. 
	//bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	
	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;
	
	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	newtio.c_cc[VMIN]     = SAMPLES_PER_PLOT;   /* blocking read until all samples received */
	
	tcflush(input, TCIFLUSH);
    tcsetattr(input,TCSANOW,&newtio);
	
	fprintf(gnuplot, "set yrange [-0.1:5.1]\n");
	while(1) {
		// Wait for trig
		do {
			if (read(input, buf, 1) != 1) return 0;
		} while (buf[0] >= trig);
		do {
			if (read(input, buf, 1) != 1) return 0;
		} while (buf[0] < trig);
		
		// Read rest of the buffer
		for (i=1; i<SAMPLES_PER_PLOT; i++) {
			if (read(input, buf+i, 1) != 1) return 0;
		}
		//read(input, buf+i, SAMPLES_PER_PLOT-1);
		
		fprintf(gnuplot, "plot '-' with lines\n");
		
		for (i=0; i<SAMPLES_PER_PLOT; i++) {
			out_data = buf[i]*5.0/256;
			fprintf(gnuplot, "%g %g\n", (float)i/freq, out_data);
		}
		fprintf(gnuplot, "e\n"); // Display output
		fflush(gnuplot);
	}
	
	tcsetattr(input,TCSANOW,&oldtio);
	close(input);
	pclose(gnuplot);
	printf("\n");
	return 0;
}
