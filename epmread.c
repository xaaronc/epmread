#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "epm_adc.h"

/* APQ8064 */
#define NUM_DEVICES 2
#define NUM_CHANS 16

static int
epm_init(int fd)
{
	uint32_t ret;

	if (ioctl(fd, EPM_ADC_INIT, &ret)) {
		perror("ioctl failed");
		return 1;
	}

	if (ret) {
		fprintf(stderr, "epm_init failed: %i\n", (int)ret);
		return 2;
	}

	return 0;
}

static int
epm_deinit(int fd)
{
	uint32_t ret;

	if (ioctl(fd, EPM_ADC_DEINIT, &ret)) {
		perror("ioctl failed");
		return 1;
	}

	if (ret) {
		fprintf(stderr, "epm_deinit failed: %i\n", (int)ret);
		return 2;
	}

	return 0;
}

static int
epm_read(int fd, int dev, int chan, int32_t *data)
{
	struct epm_chan_request req = {
		.device_idx = dev,
		.channel_idx = chan,
	};

	if (ioctl(fd, EPM_ADC_REQUEST, &req)) {
		perror("epm_req failed");
		return 1;
	}

	*data = req.physical;

	return 0;
}

static int
read_avg(int fd, int id, int nsamps, int32_t *data)
{
	int i;
	int64_t total = 0;

	for (i = 0; i < nsamps; i++) {
		int32_t sample;

		if (epm_read(fd, id / 16, id % 16, &sample)) {
			return 1;
		}
		total += sample;
	}

	*data = total / nsamps;
	return 0;
}

int
main(int argc, char *argv[])
{
	int i;
	int fd;
	char *fname = "/dev/epm_adc";
	int nsamps = 1;

	for (i = 1; i < argc; i++) {
		if (!strcmp("--dev", argv[i]) || !strcmp("-d", argv[i])) {
			fname = argv[++i];
		} else if (!strcmp("--samples", argv[i]) || !strcmp("-s", argv[i])) {
			nsamps = strtol(argv[++i], NULL, 0);
		} else {
			break;
		}
	}

	if (i == argc) {
		fprintf(stderr,
				"usage: %s [--dev <file>] [--samples <int>] chan [chan..]\n",
				argv[0]);
		return 1;
	}

	fd = open(fname, O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		perror("open failed");
		return errno;
	}

	if (epm_init(fd)) {
		goto out;
	}

	for (; i < argc; i++) {
		int32_t data;
		int id = strtol(argv[i], NULL, 10);

		if (read_avg(fd, id, nsamps, &data)) {
			break;
		}
		printf("%li\n", (long)data);
	}

	epm_deinit(fd);

out:
	close(fd);
	return 0;
}

