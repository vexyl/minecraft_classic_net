#include "utils.h"

#include <zlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern uint8_t* map_buffer_compressed;
extern size_t map_buffer_compressed_size;

void decompress_buffer(uint8_t* map_buffer, uint8_t** out_buffer, size_t* out_buffer_size, size_t expected_buffer_size)
{
	assert(*out_buffer == NULL && map_buffer != NULL);

	*out_buffer = (uint8_t*)malloc(expected_buffer_size * sizeof(uint8_t));
	if (*out_buffer == NULL) {
		fprintf(stderr, "failed to allocate memory for map decompression\n");
		exit(1);
	}

	memset(*out_buffer, 0, expected_buffer_size);

	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = map_buffer_compressed_size;
	strm.next_in = map_buffer_compressed;

	int ret = inflateInit2(&strm, 16+MAX_WBITS);
	if (ret != Z_OK) {
		fprintf(stderr, "zlib inflate failed\n");
		exit(1);
	}

	strm.avail_out = expected_buffer_size;
	strm.next_out = (*out_buffer);

	ret = inflate(&strm, Z_FINISH);
	
	assert(ret != Z_STREAM_ERROR);
	
	switch (ret) {
	case Z_NEED_DICT:
	case Z_DATA_ERROR:
	case Z_MEM_ERROR:
		fprintf(stderr, "zlib inflate failed: %d\n", ret);
		exit(1);
	}

	inflateEnd(&strm);

	*out_buffer_size = (size_t)strm.total_out;
}
