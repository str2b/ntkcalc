/**
* v0.4
*/

#include <stdio.h>
#include <stdint.h>
#include "leio.h"

/*
* reads little endian dword at abssolute offset from fp
*/
uint32_t read_dword_at(FILE * fp, long abs_offset) {
	fseek(fp, abs_offset, SEEK_SET);
	return read_dword(fp);
}

/*
* reads little endian dword at current position indicator from fp
*/
uint32_t read_dword(FILE * fp) {
	uint32_t res = 0;
	res += fgetc(fp) << 0;
	res += fgetc(fp) << 8;
	res += fgetc(fp) << 16;
	res += fgetc(fp) << 24;
	return res;
}

/*
* reads little endian word at abssolute offset from fp
*/
uint16_t read_word_at(FILE * fp, long abs_offset) {
	fseek(fp, abs_offset, SEEK_SET);
	return read_word(fp);
}

/*
* reads little endian word at current position indicator from fp
*/
uint16_t read_word(FILE * fp) {
	uint16_t res = 0;
	res += fgetc(fp) << 0;
	res += fgetc(fp) << 8;
	return res;
}

/*
* reads byte at abssolute offset from fp
*/
uint8_t read_byte_at(FILE * fp, long abs_offset) {
	fseek(fp, abs_offset, SEEK_SET);
	return read_byte(fp);
}

/*
* reads byte at current position indicator from fp
*/
uint8_t read_byte(FILE * fp) {
	return (uint8_t)fgetc(fp);
}

/*
* writes little endian dword at absolute offset from fp
*/
void write_dword_at(FILE * fp, uint32_t dword, long abs_offset) {
	fseek(fp, abs_offset, SEEK_SET);
	write_dword(fp, dword);
}

/*
* writes little endian dword at current position indicator from fp
*/
void write_dword(FILE * fp, uint32_t dword) {
	uint32_t mask = 0xFF;
	fputc((dword >> 0) & mask, fp);
	fputc((dword >> 8) & mask, fp);
	fputc((dword >> 16) & mask, fp);
	fputc((dword >> 24) & mask, fp);
}

/*
* writes little endian word at absolute offset from fp
*/
void write_word_at(FILE * fp, uint16_t word, long abs_offset) {
	fseek(fp, abs_offset, SEEK_SET);
	write_word(fp, word);
}

/*
* writes little endian word at current position indicator from fp
*/
void write_word(FILE * fp, uint16_t word) {
	uint8_t mask = 0xFF;
	fputc((word >> 0) & mask, fp);
	fputc((word >> 8) & mask, fp);
}

/*
* writes byte at absolute offset from fp
*/
void write_byte_at(FILE * fp, uint8_t byte, long abs_offset) {
	fseek(fp, abs_offset, SEEK_SET);
	write_byte(fp, byte);
}

/*
* writes byte at current position indicator from fp
*/
void write_byte(FILE * fp, uint8_t byte) {
	fputc(byte, fp);
}
