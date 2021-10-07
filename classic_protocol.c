#include "classic_protocol.h"
#include "stream_buffer.h"
#include "protocol.h"
#include "utils.h"

#ifdef __linux__
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdlib.h>

uint8_t* map_buffer;
size_t map_buffer_size;

uint8_t* map_buffer_compressed;
size_t map_buffer_compressed_size;

void (*classic_on_server_id)(struct server_id_packet packet);
void (*classic_on_level_finalized)(struct level_finalized_packet packet);
void (*classic_on_set_block)(struct set_block_packet packet);
void (*classic_on_spawn)(struct spawn_packet packet);
void (*classic_on_position_orientation)(struct position_orientation_packet);
void (*classic_on_position_orientation_update)(struct position_orientation_update_packet);
void (*classic_on_position_update)(struct position_update_packet);
void (*classic_on_orientation_update)(struct orientation_update_packet);
void (*classic_on_message)(struct message_packet);
void (*classic_on_despawn)(struct despawn_packet);

void classic_protocol_initialize(struct protocol* proto)
{
	proto->opcode_table[0x00] = opcode_table_entry_new(&opcode_handle_server_id, 131);
	proto->opcode_table[0x01] = opcode_table_entry_new(&opcode_handle_ping, 1);
	proto->opcode_table[0x02] = opcode_table_entry_new(&opcode_handle_level_init, 1);
	proto->opcode_table[0x03] = opcode_table_entry_new(&opcode_handle_level_data, 1028);
	proto->opcode_table[0x04] = opcode_table_entry_new(&opcode_handle_level_final, 7);
	proto->opcode_table[0x06] = opcode_table_entry_new(&opcode_handle_set_block, 8);
	proto->opcode_table[0x07] = opcode_table_entry_new(&opcode_handle_spawn, 74);
	proto->opcode_table[0x08] = opcode_table_entry_new(&opcode_handle_position_orientation, 10);
	proto->opcode_table[0x09] = opcode_table_entry_new(&opcode_handle_position_orientation_update, 7);
	proto->opcode_table[0x0a] = opcode_table_entry_new(&opcode_handle_position_update, 5);
	proto->opcode_table[0x0b] = opcode_table_entry_new(&opcode_handle_orientation_update, 4);
	proto->opcode_table[0x0c] = opcode_table_entry_new(&opcode_handle_despawn, 2);
	proto->opcode_table[0x0d] = opcode_table_entry_new(&opcode_handle_message, 66);
	proto->opcode_table[0x0e] = opcode_table_entry_new(&opcode_handle_disconnect, 65);
	proto->opcode_table[0x0f] = opcode_table_entry_new(&opcode_handle_user_type, 2);
	
	classic_on_server_id = NULL;
	classic_on_level_finalized = NULL;
	classic_on_set_block = NULL;
}

void opcode_handle_server_id(struct stream_buffer* stream)
{
	struct server_id_packet packet;
	
	packet.server_name[64] = 0;
	packet.server_motd[64] = 0;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_uint8(stream, &packet.version);
	stream_read_string64(stream, (char*)packet.server_name);
	stream_read_string64(stream, (char*)packet.server_motd);
	stream_read_uint8(stream, &packet.user_type);
	
	printf("protocol version: %d\nname: %s\nmotd: %s\nuser type: %d\n", packet.version, packet.server_name, packet.server_motd, packet.user_type);
	
	if (classic_on_server_id != NULL)
		classic_on_server_id(packet);
}

void opcode_handle_ping(struct stream_buffer* stream)
{

}

void opcode_handle_level_init(struct stream_buffer* stream)
{
	printf("got level init\n");
	map_buffer_compressed = NULL;
	map_buffer_compressed_size = 0;
}

void opcode_handle_level_data(struct stream_buffer* stream)
{
	int16_t chunk_length;
	uint8_t chunk_data[1024];
	uint8_t percent;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_int16(stream, &chunk_length);
	stream_read(stream, &chunk_data, 1024);
	stream_read_uint8(stream, &percent);
	
	chunk_length = ntohs(chunk_length);
	
	printf("chunk length: %d\n", chunk_length);
	
	size_t offset = map_buffer_compressed_size;
	
	map_buffer_compressed_size += chunk_length;
	map_buffer_compressed = (uint8_t*)realloc(map_buffer_compressed, map_buffer_compressed_size * sizeof(uint8_t));
	if (map_buffer_compressed == NULL) {
		fprintf(stderr, "error map_buffer realloc()\n");
		exit(1);
	}
	
	memcpy(map_buffer_compressed + offset, chunk_data, chunk_length);
	
	printf("%d%%\n", percent);
}

void opcode_handle_level_final(struct stream_buffer* stream)
{
	int16_t x_size, y_size, z_size;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_int16(stream, &x_size);
	stream_read_int16(stream, &y_size);
	stream_read_int16(stream, &z_size);
	
	x_size = ntohs(x_size);
	y_size = ntohs(y_size);
	z_size = ntohs(z_size);

	printf("map dimensions: %dx%dx%d\n", x_size, y_size, z_size);
	
	decompress_buffer(map_buffer_compressed, &map_buffer, &map_buffer_size, x_size * y_size * z_size);
	
	printf("map buffer size: %d\n", (int)map_buffer_size);
	
	free(map_buffer_compressed);
	
	FILE* fp = fopen("map_buffer.dat", "wb");
	fwrite(map_buffer, sizeof(uint8_t), map_buffer_size, fp);
	fclose(fp);
	
	struct level_finalized_packet packet;
	
	packet.buffer = map_buffer;
	packet.size = map_buffer_size;
	packet.x_size = x_size;
	packet.y_size = y_size;
	packet.z_size = z_size;
	
	if (classic_on_level_finalized != NULL)
		classic_on_level_finalized(packet);
}

void opcode_handle_set_block(struct stream_buffer* stream)
{
	struct set_block_packet packet;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_int16(stream, &packet.x);
	stream_read_int16(stream, &packet.y);
	stream_read_int16(stream, &packet.z);
	stream_read_uint8(stream, &packet.block_type);
	
	packet.x = ntohs(packet.x);
	packet.y = ntohs(packet.y);
	packet.z = ntohs(packet.z);

	if (classic_on_set_block != NULL)
		classic_on_set_block(packet);
}

void opcode_handle_spawn(struct stream_buffer* stream)
{
	struct spawn_packet packet;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_uint8(stream, &packet.pid);
	stream_read_string64(stream, (char*)&packet.name);
	stream_read_int16(stream, &packet.x);
	stream_read_int16(stream, &packet.y);
	stream_read_int16(stream, &packet.z);
	stream_read_uint8(stream, &packet.yaw);
	stream_read_uint8(stream, &packet.pitch);
	
	packet.x = ntohs(packet.x);
	packet.y = ntohs(packet.y);
	packet.z = ntohs(packet.z);
	
	if (classic_on_spawn != NULL)
		classic_on_spawn(packet);
}

void opcode_handle_position_orientation(struct stream_buffer* stream)
{
	struct position_orientation_packet packet;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_uint8(stream, &packet.pid);
	stream_read_int16(stream, &packet.x);
	stream_read_int16(stream, &packet.y);
	stream_read_int16(stream, &packet.z);
	stream_read_uint8(stream, &packet.yaw);
	stream_read_uint8(stream, &packet.pitch);
	
	packet.x = ntohs(packet.x);
	packet.y = ntohs(packet.y);
	packet.z = ntohs(packet.z);
	
	if (classic_on_position_orientation != NULL)
		classic_on_position_orientation(packet);
}

void opcode_handle_position_orientation_update(struct stream_buffer* stream)
{
	struct position_orientation_update_packet packet;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_uint8(stream, &packet.pid);
	stream_read_int8(stream, &packet.dx);
	stream_read_int8(stream, &packet.dy);
	stream_read_int8(stream, &packet.dz);
	stream_read_uint8(stream, &packet.yaw);
	stream_read_uint8(stream, &packet.pitch);

	packet.dx = packet.dx;
	packet.dy = packet.dy;
	packet.dz = packet.dz;
	
	if (classic_on_position_orientation_update != NULL)
		classic_on_position_orientation_update(packet);
}

void opcode_handle_position_update(struct stream_buffer* stream)
{
	struct position_update_packet packet;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_uint8(stream, &packet.pid);
	stream_read_int8(stream, &packet.dx);
	stream_read_int8(stream, &packet.dy);
	stream_read_int8(stream, &packet.dz);

	packet.dx = packet.dx;
	packet.dy = packet.dy;
	packet.dz = packet.dz;
	
	if (classic_on_position_update != NULL)
		classic_on_position_update(packet);
}

void opcode_handle_orientation_update(struct stream_buffer* stream)
{
	struct orientation_update_packet packet;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_uint8(stream, &packet.pid);
	stream_read_uint8(stream, &packet.yaw);
	stream_read_uint8(stream, &packet.pitch);
	
	if (classic_on_orientation_update != NULL)
		classic_on_orientation_update(packet);
}

void opcode_handle_despawn(struct stream_buffer* stream)
{
	struct despawn_packet packet;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_uint8(stream, &packet.pid);
	
	if (classic_on_despawn != NULL)
		classic_on_despawn(packet);
}

void opcode_handle_message(struct stream_buffer* stream)
{
	struct message_packet packet;
	
	stream_read_uint8(stream, NULL); // skip opcode
	stream_read_uint8(stream, &packet.type);
	stream_read_string64(stream, (char*)&packet.message);
	
	if (classic_on_message != NULL)
		classic_on_message(packet);
}

void opcode_handle_disconnect(struct stream_buffer* stream)
{
	printf("got disconnect\n");
	exit(1);
}

void opcode_handle_user_type(struct stream_buffer* stream)
{
	printf("got user_type\n");
}
