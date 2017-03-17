#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "lib/leio.h"

#define FILESIZE_ADDRESS 0x68
#define CHECKSUM_ADDRESS 0x6e
#define BCLCKSM_ADDRESS 0x04
#define CSMAGIC_ADDRESS 0x6c
#define BCL_MAGIC 0x314c4342
#define EP_MAGIC 0x80000400
#define CHECKSUM_MAGIC 0xaa55

#define PROG_INFO "+--------------------------------------------+\n"\
				  "| Ntk base value calculator v0.1.1 by Tobi@s |\n"\
				  "|       https://dashcamtalk.com/forum/       |\n"\
				  "|       https://www.goprawn.com/forum/       |\n"\
				  "+--------------------------------------------+\n"

void read_file_to_mem(FILE *, uint16_t *, uint32_t);
uint16_t calc_baseval(uint16_t *, uint32_t, uint32_t, uint16_t);

int main(int argc, char *argv[]) {
  uint32_t filesize;
  uint16_t checksum;
  uint16_t baseval;
  uint16_t startval;
  uint32_t magic;
  uint32_t skip_at;

  if(argc != 2) {
    fprintf(stderr, "Usage: %s filename\n", argv[0]);
    return -1;
  }

  printf("%s\n", PROG_INFO);

  FILE *fp = fopen(argv[1], "rb");
  if(!fp) {
    perror(argv[1]);
    return -1;
  }
  
  
  magic = read_dword_at(fp, 0);
  if(magic == BCL_MAGIC) {
    printf("Detected BCL header (FullComp)\n");
	skip_at = BCLCKSM_ADDRESS;
	checksum = read_word_at(fp, BCLCKSM_ADDRESS);
  } else {
    magic = read_word_at(fp, CSMAGIC_ADDRESS);
    if(magic != CHECKSUM_MAGIC) {
      fprintf(stderr, "Checksum magic invalid (%04x).\n", magic & 0xFFFF);
      return -1;
    } 
    printf("Detected raw firmware image (NonComp)\n");
    skip_at = CHECKSUM_ADDRESS;
	checksum = read_word_at(fp, CHECKSUM_ADDRESS);
  }
  
  fseek(fp, 0, SEEK_END);
  filesize = ftell(fp);
  uint16_t * mem = malloc((filesize/2) * sizeof(uint16_t));

  read_file_to_mem(fp, mem, filesize);
  
  baseval = calc_baseval(mem, filesize/2, skip_at, checksum);
  printf("baseval: %04x\n", baseval);
  
  free(mem);
  fclose(fp);
  return 1;
}

void read_file_to_mem(FILE *fp, uint16_t mem[], uint32_t filesize) {
  rewind(fp);
  for(int i = 0, o = 0; i < filesize; i += 2, o++) {
    mem[o] = read_word_at(fp, i);
  }
}

uint16_t calc_baseval(uint16_t data[], uint32_t length, uint32_t skip_at, uint16_t checksum) {
  uint32_t pos = 0;
  uint16_t new_sum = 0;
  while(pos < length) {
    if(pos * 2 == skip_at) {
      new_sum += pos;
    } else {
      new_sum += pos + data[pos];      	
    }
    pos++;
  }	
  return (checksum + new_sum) & 0xFFFF;
}
