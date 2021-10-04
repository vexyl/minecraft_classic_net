#ifndef _CLASSIC_PROTOCOL_H
#define _CLASSIC_PROTOCOL_H

#include <stdint.h>
#include <string.h>

struct stream_buffer;
struct protocol;

struct server_id_packet {
	uint8_t version;
	uint8_t server_name[65], server_motd[65];
	uint8_t user_type;
};

struct level_finalized_packet
{
	uint8_t* buffer;
	size_t size;
	int16_t x_size, y_size, z_size;
};

struct set_block_packet
{
	int16_t x, y, z;
	uint8_t block_type;
};

struct spawn_packet
{
	int8_t pid;
	char name[64];
	int16_t x, y, z;
	uint8_t yaw, pitch;
};

struct position_orientation_packet
{
	int8_t pid;
	int16_t x, y, z;
	uint8_t yaw, pitch;
};

void classic_protocol_initialize(struct protocol* proto);

void opcode_handle_server_id(struct stream_buffer* stream);
void opcode_handle_ping(struct stream_buffer* stream);
void opcode_handle_level_init(struct stream_buffer* stream);
void opcode_handle_level_data(struct stream_buffer* stream);
void opcode_handle_level_final(struct stream_buffer* stream);
void opcode_handle_set_block(struct stream_buffer* stream);
void opcode_handle_spawn(struct stream_buffer* stream);
void opcode_handle_position_orientation(struct stream_buffer* stream);
void opcode_handle_position_orientation_update(struct stream_buffer* stream);
void opcode_handle_position_update(struct stream_buffer* stream);
void opcode_handle_orientation_update(struct stream_buffer* stream);
void opcode_handle_despawn(struct stream_buffer* stream);
void opcode_handle_message(struct stream_buffer* stream);
void opcode_handle_disconnect(struct stream_buffer* stream);
void opcode_handle_user_type(struct stream_buffer* stream);

#endif // _CLASSIC_PROTOCOL_H
