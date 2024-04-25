#include <sox.h>
#include <stdio.h>

#define CHUNK_SIZE 2000
#define ENERGY_CUT 5.0

static double get_energy(sox_sample_t *buf, size_t len)
{
	double ret = 0;
	size_t i;
	for (i = 0; i < len; i++) {
		double c = ((double)buf[i]) / 2147483647;
		ret += c * c;
	}
	return ret;
}

int main(void)
{
	sox_sample_t buf[CHUNK_SIZE];
	sox_format_t *in;
	int in_knock = 0;
	double ofs = 0;
	double last_knock = 0;

	sox_format_init();

	in = sox_open_read("-", NULL, NULL, NULL);
	while (1) {
		size_t r;
		double energy;

		r = sox_read(in, buf, CHUNK_SIZE);
		if (!r)
			break;

		energy = get_energy(buf, r);
		if (energy > ENERGY_CUT) {
			if (!in_knock) {
				printf("%f\n", ofs);
				last_knock = ofs;
				in_knock = 1;
			}
		} else {
			in_knock = 0;
			if (last_knock && ofs - last_knock > 1.5) {
				printf("--\n");
				ofs = 0;
				last_knock = 0;
			}
		}

		fflush(stdout);
		ofs += (double)CHUNK_SIZE /
			(double)in->signal.rate /
			(double)in->signal.channels;
	}
	sox_close(in);

	return 0;
}
