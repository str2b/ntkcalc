#ifndef LEIO_H
#define LEIO_H

#include <stdint.h>

/*
* IO functions for little endian encoded files
* Copyright 2015 Tobias Straubinger
*/

extern uint32_t read_dword_at(FILE *, long);
extern uint32_t read_dword(FILE *);
extern uint16_t read_word_at(FILE *, long); 
extern uint16_t read_word(FILE *);
extern uint8_t  read_byte_at(FILE *, long);
extern uint8_t  read_byte(FILE *);

extern void write_dword_at(FILE *, uint32_t, long);
extern void write_dword(FILE *, uint32_t);
extern void write_word_at(FILE *, uint16_t, long);
extern void write_word(FILE *, uint16_t);
extern void write_byte_at(FILE *, uint8_t, long);
extern void write_byte(FILE *, uint8_t);

#endif
