#ifndef STREAM_BUFFER_H_
#define STREAM_BUFFER_H_

#include <stdint.h>
#include <string.h>

struct stream_buffer {
	unsigned char* buffer;
	size_t size;
	int head, write_offset;
};

void stream_initialize(struct stream_buffer* stream, unsigned char* buffer, size_t size);
void stream_free(struct stream_buffer* stream);

void stream_resize(struct stream_buffer* stream, size_t size);

void stream_read(struct stream_buffer* stream, void* buffer, size_t size);
void stream_write(struct stream_buffer* stream, void* buffer, size_t size);

void stream_read_int32(struct stream_buffer* stream, int32_t* out_data);
void stream_read_int16(struct stream_buffer* stream, int16_t* out_data);
void stream_read_int8(struct stream_buffer* stream, int8_t* out_data);
void stream_read_uint32(struct stream_buffer* stream, uint32_t* out_data);
void stream_read_uint16(struct stream_buffer* stream, uint16_t* out_data);
void stream_read_uint8(struct stream_buffer* stream, uint8_t* out_data);
void stream_read_string(struct stream_buffer* stream, char* out_data, size_t size);

void stream_write_int32(struct stream_buffer* stream, int32_t in_data);
void stream_write_int16(struct stream_buffer* stream, int16_t in_data);
void stream_write_int8(struct stream_buffer* stream, int8_t in_data);
void stream_write_uint32(struct stream_buffer* stream, uint32_t in_data);
void stream_write_uint16(struct stream_buffer* stream, uint16_t in_data);
void stream_write_uint8(struct stream_buffer* stream, uint8_t in_data);
void stream_write_string(struct stream_buffer* stream, char* in_data, size_t size);

#endif // STREAM_BUFFER_H_
