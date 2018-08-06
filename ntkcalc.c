#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "lib/leio.h"

#define FILESIZE_ADDRESS 0x68
#define CHECKSUM_ADDRESS 0x6e
#define CHECKSUM_ADDRESS_V2 0x46e
#define BCLCKSM_ADDRESS 0x04
#define CSMAGIC_ADDRESS 0x6c
#define CSMAGIC_ADDRESS_V2 0x46c
#define BCL_MAGIC 0x314c4342
#define EP_MAGIC 0x80000400
#define CHECKSUM_MAGIC 0xaa55

#define PROG_INFO "+-------------------------------+\n"\
				  "| Ntk checksum calculator v0.7  |\n"\
				  "|           by Tobi@s          |\n"\
				  "| https://dashcamtalk.com/forum |\n"\
				  "| https://www.goprawn.com/forum |\n"\
				  "|   Donate: dc.p-mc.eu/donate   |\n"\
				  "+-------------------------------+\n"

void read_file_to_mem(FILE *, uint16_t *, uint32_t);
uint16_t calc_checksum(uint16_t *, uint32_t, uint32_t, uint16_t);
void print_usage(int, char **);

int main(int argc, char *argv[]) {
  uint32_t filesize;
  uint16_t checksum;
  uint16_t startval = 0;
  uint32_t magic;
  uint32_t skip_at;
  uint16_t read_checksum;
  char *read_mode = "rb";
  enum modes { BASEVAL, CHECKSUM, CHECKSUM2 } mode;
  enum formats { BCL, RAW, RAWv2 } format;
  
  if(argc != 3 && argc != 4) {
    print_usage(argc, argv);
  }
  
  if(argv[1][1] == 'b' && argc == 3) {
    mode = BASEVAL;
  } else if(argv[1][1] == 'c') {
    if(argv[1][2] == 'w') {
      read_mode = "rb+";  	
    }
    if(argc == 4) {
      startval = (uint16_t) strtol(argv[3], NULL, 0);
      printf("startval: %04x\n", startval);
    }
    mode = CHECKSUM;
  } else {
    fprintf(stderr, "Unknown mode: '%s' or wrong arguments!\n", argv[1]);
    print_usage(argc, argv);
    exit(-1);
  }  

  printf("%s\n", PROG_INFO);

  FILE *fp = fopen(argv[2], read_mode);
  if(!fp) {
    perror(argv[2]);
    exit(-1);
  }  
  
  /*
  * Detect firmware format
  */
  magic = read_dword_at(fp, 0);
  if(magic == BCL_MAGIC) {
    printf("Detected BCL header (FullComp)\n");
    format = BCL;
    skip_at = BCLCKSM_ADDRESS;
  } else {
    magic = read_word_at(fp, CSMAGIC_ADDRESS);
    if(magic != CHECKSUM_MAGIC) {
      fprintf(stdout, "[INSECURE] Checksum magic is (%04x).\n", magic & 0xFFFF);
      //return -1;
    }
	if(magic == 0xFFFF) {
		printf("Detected raw eCos firmware image (NonComp)\n");
		format = RAWv2;
		skip_at = CHECKSUM_ADDRESS_V2;
	} else {
		printf("Detected raw nvt firmware image (NonComp)\n");
		format = RAW;
		skip_at = CHECKSUM_ADDRESS;
	}

  }

  if(mode == BASEVAL) {
    if(format == BCL) 
      read_checksum = read_word_at(fp, BCLCKSM_ADDRESS);
    else
      read_checksum = read_word_at(fp, CHECKSUM_ADDRESS);
  } 

  /*
  * Read file to mem
  */
  fseek(fp, 0, SEEK_END);
  filesize = ftell(fp);
  uint16_t * mem = malloc((filesize/2) * sizeof(uint16_t));
  read_file_to_mem(fp, mem, filesize);
  
  /*
  * Calculate checksum
  */
  checksum = calc_checksum(mem, filesize/2, skip_at, startval);
  
  /*
  * Handle result
  */
  if(mode == BASEVAL) {
	printf("baseval: %04x\n", (~(checksum - 1) + read_checksum) & 0xFFFF);
  } else if(mode == CHECKSUM) {
    printf("checksum: %04x\n", checksum);
    if(read_mode[2] == '+') /* write mode */ {
      printf("Writing at 0x%08x\n", skip_at);
      write_word_at(fp, checksum, skip_at);
    }	
  } else {
    fprintf(stderr, "Invalid mode!?\n");
  }
  
  free(mem);
  fclose(fp);
  return 0;
}

void print_usage(int argc, char * argv[]) {
  fprintf(stderr, "Usage: %s [mode] filename [start val]\n", argv[0]);
  fprintf(stderr, "  modes:\n");
  fprintf(stderr, "    -b filename 	baseval calculation\n");
  fprintf(stderr, "    -c[w] filename 	checksum calculation, w writes checksum to binary\n");  
  fprintf(stderr, "    -c[w] filename val	checksum calculation with custom start value (hex)\n"); 
  fprintf(stderr, "\n");
  fprintf(stderr, "Author: Tobi@s\n");
  fprintf(stderr, "Disclaimer: Commercial use strictly prohibited!\n");
  exit(-1);
}

void read_file_to_mem(FILE *fp, uint16_t mem[], uint32_t filesize) {
  rewind(fp);
  for(int i = 0, o = 0; i < filesize; i += 2, o++) {
    mem[o] = read_word_at(fp, i);
  }
}

uint16_t calc_checksum(uint16_t data[], uint32_t length, uint32_t skip_at, uint16_t startval) {
  uint32_t pos = 0;
  uint16_t checksum = 0;
  while(pos < length) {
    if(pos * 2 == skip_at) {
      checksum += pos;
    } else {
      checksum += pos + data[pos];      	
    }
    pos++;
  }	
  return startval + (~checksum & 0xFFFF) + 1;
}
