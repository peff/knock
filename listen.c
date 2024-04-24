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

static double hash_knock(sox_format_t *in, sox_sample_t *buf,
			 size_t have, size_t len)
{
	double ret = 0;
	double average = 0;
	unsigned ctr = 0;

	if (have) {
		if (get_energy(buf, have) > ENERGY_CUT)
			average += 1;
		ctr++;
	}

	while (len > 0) {
		size_t r = sox_read(in, buf, len < CHUNK_SIZE ? len : CHUNK_SIZE);
		if (!r)
			break;
		len -= r;
		if (get_energy(buf, r) > ENERGY_CUT)
			average += 1;
		if (++ctr >= 4) {
			ret *= 1.1;
			ret += average / 4;
			ctr = 0;
		}
	}
	return ret;
}

int main(int argc, const char **argv)
{
	sox_sample_t buf[CHUNK_SIZE];
	double knock;
	size_t len;
	sox_format_t *in;

	sox_format_init();

	if (!*++argv) {
		fprintf(stderr, "usage: knock <audiofile>\n");
		return 1;
	}
	in = sox_open_read(*argv, NULL, NULL, NULL);
	len = in->signal.length;
	knock = hash_knock(in, buf, 0, len);
	sox_close(in);
	argv++;

	in = sox_open_read("-", NULL, NULL, NULL);
	while (1) {
		size_t r;
		double energy;
		double attempt;

		r = sox_read(in, buf, CHUNK_SIZE);
		if (!r)
			break;

		energy = get_energy(buf, r);
		if (energy < ENERGY_CUT)
			continue;

		printf("got knock\n");
		attempt = hash_knock(in, buf, r, len);

		printf("score = %f / %f\n", attempt, knock);
	}
	sox_close(in);

	return 0;
}
