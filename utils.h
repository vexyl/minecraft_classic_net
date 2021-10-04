#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <string.h>

void decompress_buffer(uint8_t* map_buffer, uint8_t** out_buffer, size_t* out_buffer_size, size_t expected_buffer_size);

#endif // UTILS_H_
