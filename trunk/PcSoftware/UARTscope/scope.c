#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#define SAMPLES_PER_PLOT 200

int main(int argc, char* argv[]) {
	int input;
	FILE* gnuplot;
	int i;
	float out_data;
	unsigned char buf[SAMPLES_PER_PLOT];
	unsigned char trig = 128;
	unsigned freq = 1000;
	
	input = open("/dev/midi1", O_RDONLY);
	gnuplot = popen("gnuplot", "w");
	
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
	
	close(input);
	pclose(gnuplot);
	printf("\n");
	return 0;
}
