#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h>

#define PATTERN_LENGTH 4
int ReadWord32(FILE * f) {
  unsigned char buf[4];
  fread(buf, 4, 1, f);
  return (((unsigned int) buf[0]) << 24) +
    (((unsigned int) buf[1]) << 16) +
    (((unsigned int) buf[2]) << 8) +
    (unsigned int) buf[3];
}

int find_magic(FILE * h, long offset) {
  fseek(h, offset, SEEK_SET);

  int ch, f1 = 0, f2 = 0, f3 = 0, f4 = 0;
  while ((ch = fgetc(h)) != EOF) {
    switch (ch) {
    case 'B':
      f1 = 1;
      break;
    case 'C':
      if (f1) {
        f2 = 1;
      } else {
        f1 = 0;
      }
      break;
    case 'L':
      if (f1 && f2) {
        f3 = 1;
      } else {
        f1 = 0;
        f2 = 0;
      }
      break;
    case '1':
      if (f1 && f2 && f3) {
        return (int) ftell(h) - PATTERN_LENGTH;
      } else {
        f1 = 0;
        f2 = 0;
        f3 = 0;
      }
      break;
    default:
      f1 = 0;
      f2 = 0;
      f3 = 0;
      f4 = 0;
      break;
    }
  }

  return -1;
}

int main(int argc, char * argv[]) {
  printf("+-------------------------------------------------------+\n\
| NtkFwInfo 0.1 fw binary information utility by Tobi@s |\
\n+-------------------------------------------------------+\n\n");
  if (argc != 2) {
    printf("Usage: %s filename\n", argv[0]);
    return -1;
  }
  FILE * h;
  h = fopen(argv[1], "rb");
  if (!h) {
    perror(argv[1]);
    return -1;
  }
  fseek(h, 0, SEEK_END);
  int length = ftell(h);

  int offset = -1;
  int last_offset = -1;
  int hits = 0;
  do {
    offset = find_magic(h, offset + 1);
    if (offset != -1) {
      last_offset = offset;
      printf("BCL compressed partition %d at\t%08x\n", hits++, offset);
    }
  } while (offset != -1);
  if (hits == 0) {
    printf("No compressed partitions found...\nGuessed firmware type: NonComp single partition or invalid firmware binary\n");
  } else if (hits == 1 && last_offset == 0) {
    printf("\nGuessed firmware type: FullComp single partition\n");
  } else if (hits == 1 && last_offset == 0x000B0000) {
    printf("\nGuessed firmware type: PartComp single partition\n");
  } else {
    printf("\nGuessed firmware type: FullComp multi partition\n");
  }

  fclose(h);
  return 0;
}
