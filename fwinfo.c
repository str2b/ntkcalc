#include <stdio.h>
#include <stdlib.h>
#include "lib/leio.h"

#define CODEENTRY_OFFS 0x00
#define BININFO_SIZE 9
#define BININFO1_OFFS 0x50
#define BININFO2_OFFS 0x58
#define BININFO3_OFFS 0x60
#define BINLEN_OFFS 0x68
#define CHECKSUM_OFFS 0x6c
#define CRCLENCHECK_OFFS 0x70
#define BINCTRL_OFFS 0x78
#define CRCBINTAG_OFFS 0x7C

struct HEADINFO
{
    uint32_t CodeEntry;   ///< [0x00] fw CODE entry (4) ----- r by Ld
    uint32_t Resv1[19];   ///< [0x04~0x50] reserved (4*19) -- reserved, its mem value will filled by Ld
    char BinInfo_1[BININFO_SIZE];  ///< [0x50~0x58] CHIP-NAME (8) ---- r by Ep
    char BinInfo_2[BININFO_SIZE];  ///< [0x58~0x60] SDK version (8)
    char BinInfo_3[BININFO_SIZE];  ///< [0x60~0x68] SDK releasedate (8)
    uint32_t BinLength;   ///< [0x68] Bin File Length (4) --- w by Ep/bfc
    uint32_t Checksum;    ///< [0x6c] Check Sum or CRC (4) ----- w by Ep/Epcrc
    uint32_t CRCLenCheck; ///< [0x70~0x74] Length check for CRC (4) ----- w by Epcrc (total len ^ 0xAA55)
    uint32_t Resv2;       ///< [0x74~0x78] reserved (4) --- reserved for other bin tools
    uint32_t BinCtrl;     ///< [0x78~0x7C] Bin flag (4) --- w by bfc
                        ///<             BIT 0.compressed enable (w by bfc)
    uint32_t CRCBinaryTag;///< [0x7C~0x80] Binary Tag for CRC (4) ----- w by Epcrc (0xAA55 + "NT")
} HEADERINFO;

int main(int argc, char * argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <firmware.bin>\n", argv[0]);
		return -1;
	}
	FILE *fp = fopen(argv[1], "rb");
	if(!fp) {
		fprintf(stderr, "'%s' not found or not a file\n", argv[1]);
		return -1;
	}
	
	HEADERINFO.CodeEntry = read_dword_at(fp, CODEENTRY_OFFS);
	fseek(fp, BININFO1_OFFS, SEEK_SET);
	fgets(HEADERINFO.BinInfo_1, BININFO_SIZE, fp);
	fseek(fp, BININFO2_OFFS, SEEK_SET);
	fgets(HEADERINFO.BinInfo_2, BININFO_SIZE, fp);
	fseek(fp, BININFO3_OFFS, SEEK_SET);
	fgets(HEADERINFO.BinInfo_3, BININFO_SIZE, fp);
	HEADERINFO.BinLength = read_dword_at(fp, BINLEN_OFFS);
	HEADERINFO.Checksum = read_dword_at(fp, CHECKSUM_OFFS);
	HEADERINFO.CRCLenCheck =read_dword_at(fp, CRCLENCHECK_OFFS);
	HEADERINFO.BinCtrl = read_dword_at(fp, BINCTRL_OFFS);
	HEADERINFO.CRCBinaryTag = read_dword_at(fp, CRCBINTAG_OFFS);
	
	printf("[%04x] Entry point:	%08x\n", CODEENTRY_OFFS, HEADERINFO.CodeEntry);
	printf("[%04x] Chip name:	%s\n", BININFO1_OFFS, HEADERINFO.BinInfo_1);
	printf("[%04x] SDK Version:	%s\n", BININFO2_OFFS, HEADERINFO.BinInfo_2);
	printf("[%04x] SDK release:	%s\n", BININFO3_OFFS, HEADERINFO.BinInfo_3);
	printf("[%04x] File length:	%d\n", BINLEN_OFFS, HEADERINFO.BinLength);
	printf("[%04x] Checksum:	%08x\n", CHECKSUM_OFFS, HEADERINFO.Checksum);
	printf("[%04x] CRC len chk:	%08x\n", CRCLENCHECK_OFFS, HEADERINFO.CRCLenCheck);
	printf("[%04x] Bin flag:	%08x\n", BINCTRL_OFFS, HEADERINFO.BinCtrl);
	printf("[%04x] Tag for crc:	%08x\n", CRCBINTAG_OFFS, HEADERINFO.CRCBinaryTag);
	fclose(fp);
}