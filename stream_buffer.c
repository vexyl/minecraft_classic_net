#include "stream_buffer.h"

#include <stdio.h>
#include <stdlib.h>

void stream_initialize(struct stream_buffer* stream, unsigned char* buffer, size_t size)
{
	stream->buffer = NULL;
	stream->size = 0;
	
	if (size > 0)
		stream_resize(stream, size);
		
	if (buffer != NULL)
		memcpy(stream->buffer, buffer, size);
	
	stream->write_offset = 0;
	stream->head = 0;
}

void stream_free(struct stream_buffer* stream)
{
	if (stream->buffer != NULL)
		free(stream->buffer);
	stream->buffer = NULL;
	stream->size = 0;
	stream->head = 0;
	stream->write_offset = 0;
}

void stream_resize(struct stream_buffer* stream, size_t size)
{
	stream->size += size;
	stream->buffer = realloc(stream->buffer, stream->size);
	if (stream->buffer == NULL) {
		fprintf(stderr, "error: realloc failed\n");
		exit(1);
	}
}

void stream_read(struct stream_buffer* stream, void* buffer, size_t size)
{
	stream_resize(stream, size);
	
	if (stream->head + size > stream->size) {
		fprintf(stderr, "error: attempted to read passed stream buffer size\n");
		exit(1);
	}
	
	if (buffer != NULL)
		memcpy(buffer, stream->buffer + stream->head, size);
		
	stream->head += size;
}

void stream_write(struct stream_buffer* stream, void* buffer, size_t size)
{
	if (buffer == NULL)
		return;
		
	stream_resize(stream, size);
	memcpy(stream->buffer+stream->write_offset, buffer, size);
	stream->write_offset += size;
}

void stream_read_int32(struct stream_buffer* stream, int32_t* out_data)
{
	stream_read(stream, out_data, sizeof(*out_data));
}

void stream_read_int16(struct stream_buffer* stream, int16_t* out_data)
{
	stream_read(stream, out_data, sizeof(*out_data));
}

void stream_read_int8(struct stream_buffer* stream, int8_t* out_data)
{
	stream_read(stream, out_data, sizeof(*out_data));
}

void stream_read_uint32(struct stream_buffer* stream, uint32_t* out_data)
{
	stream_read(stream, out_data, sizeof(*out_data));
}

void stream_read_uint16(struct stream_buffer* stream, uint16_t* out_data)
{
	stream_read(stream, out_data, sizeof(*out_data));
}

void stream_read_uint8(struct stream_buffer* stream, uint8_t* out_data)
{
	stream_read(stream, out_data, sizeof(*out_data));
}

void stream_read_string(struct stream_buffer* stream, char* out_data, size_t size)
{
	stream_read(stream, out_data, size);
}

void stream_read_string64(struct stream_buffer* stream, char* out_data)
{
	stream_read_string(stream, out_data, 64);
}

void stream_write_int32(struct stream_buffer* stream, int32_t in_data)
{
	stream_write(stream, &in_data, sizeof(in_data));
}

void stream_write_int16(struct stream_buffer* stream, int16_t in_data)
{
	stream_write(stream, &in_data, sizeof(in_data));
}

void stream_write_int8(struct stream_buffer* stream, int8_t in_data)
{
	stream_write(stream, &in_data, sizeof(in_data));
}

void stream_write_uint32(struct stream_buffer* stream, uint32_t in_data)
{
	stream_write(stream, &in_data, sizeof(in_data));
}

void stream_write_uint16(struct stream_buffer* stream, uint16_t in_data)
{
	stream_write(stream, &in_data, sizeof(in_data));
}

void stream_write_uint8(struct stream_buffer* stream, uint8_t in_data)
{
	stream_write(stream, &in_data, sizeof(in_data));
}

void stream_write_string(struct stream_buffer* stream, char* in_data, size_t size)
{
	stream_write(stream, in_data, size);
}

void stream_write_string64(struct stream_buffer* stream, char* in_data)
{
	stream_write_string(stream, in_data, 64);
}
