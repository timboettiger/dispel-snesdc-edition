/* main.c
 * DisPel 65816 Disassembler
 * James Churchill
 * Created 20000924
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#include <sys/uio.h>
#else
#ifdef __linux__
#include <sys/io.h>
#else
#include <io.h>
#endif
#endif

#include "dispel.h"

int silent_mode=0,verbose_mode=0,patch_mode=0;

// Implementierung der usage-Funktion
void Usage(void) {
    printf("\nDisPel - DEADC0DE Special Edition\n");
    printf("Usage: dispel [options] <infile>\n");
    printf("Options:\n");
    printf("  -n                Skip $200 byte SMC header.\n");
    printf("  -t                Don't output addresses/hex dump.\n");
    printf("  -v                Verbose: More detailed output.\n");
    printf("  -s                Silent: No output.\n");
    printf("  -h/-l             Force HiROM/LoROM memory mapping.\n");
    printf("  -a                Start in 8-bit accumulator mode.\n");
    printf("  -x                Start in 8-bit X/Y mode.\n");
    printf("  -e                Disable bank-boundary enforcement.\n");
    printf("  -d <width>        Produce a hexdump with <width> bytes/line.\n");
    printf("  -o <outfile>      Set output file. Default is stdout.\n");
    printf("  <infile>          File to disassemble.\n\n");
    exit(1);
}

int AllASCII(unsigned char *b, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (b[i] < 32 || b[i] > 126)
		{
			return 0;
		}
	}
	return 1;
}

unsigned long ConvertToFastROM(unsigned long address) {
    return address | 0x800000; // Setzt das FastROM-Bit
}

unsigned long ConvertToSlowROM(unsigned long address) {
    return address & ~0x800000; // Entfernt das FastROM-Bit
}

int ScoreHiROM(unsigned char *data)
{
	int score = 0;

	if ((data[0xFFDC] + data[0xFFDD]*256 + data[0xFFDE] + data[0xFFDF]*256) == 0xFFFF)
	{
		score += 2;
	}

	if (data[0xFFDA] == 0x33)
	{
		score += 2;
	}
	if ((data[0xFFD5] & 0xf) < 4)
	{
		score += 2;
	}
	if (!(data[0xFFFD] & 0x80))
	{
		score -= 4;
	}
	if ((1 << (data[0xFFD7] - 7)) > 48)
	{
		score -= 1;
	}
	if (!AllASCII(&data[0xFFB0], 6))
	{
		score -= 1;
	}
	if (!AllASCII(&data[0xFFC0], 20))
	{
		score -= 1;
	}

	return (score);
}

int ScoreLoROM(unsigned char *data)
{
	int score = 0;

	if ((data[0x7FDC] + data[0x7FDD]*256 + data[0x7FDE] + data[0x7FDF]*256) == 0xFFFF)
	{
		score += 2;
	}
	if (data[0x7FDA] == 0x33)
	{
		score += 2;
	}
	if ((data[0x7FD5] & 0xf) < 4)
	{
		score += 2;
	}
	if (!(data[0x7FFD] & 0x80))
	{
		score -= 4;
	}
	if ((1 << (data[0x7FD7] - 7)) > 48)
	{
		score -= 1;
	}
	if (!AllASCII(&data[0x7FB0], 6))
	{
		score -= 1;
	}
	if (!AllASCII(&data[0x7FC0], 20))
	{
		score -= 1;
	}

	return (score);
}

int hexdump(unsigned char *data,unsigned long pos,unsigned long rpos, unsigned long len,char *inst, unsigned char dwidth)
{
	int i;
    sprintf(inst, "%02lX/%04lX:\t", (pos >> 16) & 0xFF, pos & 0xFFFF);
    for(i=0; i<dwidth && i+rpos<len; i++)
    {
    	sprintf(inst + i*2 + 9, "%02X", data[rpos+i]);
    }
    return dwidth;
}

int main(int argc, char *argv[]) {
	FILE *fin,*fout;
	char infile[BUFSIZ],outfile[BUFSIZ],inst[521];
	unsigned char dmem[4],flag=0,*data;
	unsigned long len,pos=0,origin=0x1000000,start=0,end=0,rpos;
	unsigned char opt,skip=0,hirom=2,shadow=2,bound=1,tsrc=0;
	unsigned int offset,bank=0x100,i,tmp,dwidth=0;
	int hiscore,loscore;

	outfile[0]=0;

    if (argc < 2) {
        Usage();
    }

    for (i = 1; i < (argc - 1); i++) {
        if (sscanf(argv[i], "-%c", &opt) == 0) {
            Usage();
        }

        switch (opt) {
            case 'n':
                skip = 1;
                break;
            case 't':
                tsrc |= 1;
                break;
            case 'v':
                verbose_mode = 1;
                break;
            case 's':
                silent_mode = 1;
                break;
            case 'h':
                hirom = 1;
                break;
            case 'l':
                hirom = 0;
                break;
            case 'a':
                flag |= 0x20;
                break;
            case 'x':
                flag |= 0x10;
                break;
            case 'e':
                break;
            case 'p':
                tsrc |= 2;
                break;
            case 'd':
                i++;
                if ((sscanf(argv[i], "%2X", &dwidth) == 0) || dwidth == 0) {
                    Usage();
                }
                break;
            case 'b':
                i++;
                if (sscanf(argv[i], "%2X", &bank) == 0) {
                    Usage();
                }
                break;
            case 'r':
                i++;
                if (sscanf(argv[i], "%6lX-%6lX", &start, &end) == 0) {
                    Usage();
                }
                break;
            case 'g':
                i++;
                if (sscanf(argv[i], "%6lX", &origin) == 0) {
                    Usage();
                }
                break;
            case 'o':
                i++;
                strcpy(outfile, argv[i]);
                break;
            default:
                Usage();
        }
    }

    strcpy(infile, argv[i]);
    fin = fopen(infile, "rb");
    if (!fin) {
        if (!silent_mode)
            printf("Cannot open %s for reading.\n", infile);
        exit(1);
    }

    if (outfile[0] == 0) {
        strcpy(outfile, "STDOUT");
        fout = stdout;
    } else {
        fout = fopen(outfile, "w");
        if (!fout) {
            if (!silent_mode)
                printf("Cannot open %s for writing.\n", outfile);
            exit(1);
        }
    }

#if defined(__APPLE__) || defined(__linux__)
	fseek(fin, 0L, SEEK_END); //Apple and linux code
	len = ftell(fin);
	fseek(fin, 0L, SEEK_SET);
#else
	len = filelength(fileno(fin)); //Windows code
#endif

    if (len <= 392) {
        if (verbose_mode && !silent_mode)
            printf("File size <= 392 bytes. Activating 'Action Replay Patch Mode'.\n");

        patch_mode = 1;
    }
    else if (len < 0x8000 || (skip == 1 && len < 0x8200)) {
        if (!silent_mode)
            printf("This file looks too small to be a legitimate ROM image.\n");
    }

	// Allocate mem for file. Extra 3 bytes to prevent segfault during memcpy
	if ((data = malloc(len+3)) == NULL)
	{
	    if (!silent_mode)
			printf("Cant alloc %ld bytes.\n", len+3);
		exit(1);
	}
	if (skip)
	{
		len -= 0x200;
		fread(data, 0x200, 1, fin);
	}

	fread(data, len, 1, fin);
	fclose(fin);

	// Autodetect the HiROM/LoROM state

	if (hirom==2)
	{
		hiscore = ScoreHiROM(data);
		loscore = ScoreLoROM(data);
		if (hiscore>loscore)
		{
//			fprintf(stderr,"Autodetected HiROM image.\n");
			hirom = 1;
		}
		else
		{
//			fprintf(stderr,"Autodetected LoROM image.\n");
			hirom = 0;
		}
	}

	// Unmangle the address options

	pos = start;

	// If shadow addresses given, convert to unshadowed and set shadow on.
	if ((bank == 0x100 && start & 0x800000) | (bank & 0x80))
	{
		shadow = 1;
	}

	// If HiROM addresses given, convert to normal and set hirom on.
	if ((bank == 0x100 && start & 0x400000) | (bank & 0x40))
	{
		hirom=1;
	}
	bank &= 0x13F;
	start &= 0x3FFFFF;
	end &= 0x3FFFFF;

	// Autodetect shadow

	if(shadow == 2)
	{
//		fprintf(stderr,"%02X\n",data[hirom?0xFFD5:0x7FD5]);
		if(data[hirom?0xFFD5:0x7FD5] & 0x30)
		{
			shadow=1;
		}
		else
		{
			shadow=0;
		}
	}

	// If the bank byte is set, apply it to the address range
	if (bank < 0x100)
	{
		if(hirom)
		{
			pos = bank << 16;
			start = pos;
			end = start | 0xFFFF;
		}
		else
		{
			pos = (bank << 16) + 0x8000;
			start = bank * 0x8000;
			end = start + 0x7FFF;
		}
	}
	else
	{
		if(!hirom)
		{
			// Convert the addresses to offsets
			if ((start & 0xFFFF) < 0x8000)
			{
				start += 0x8000;
			}
			pos = start;
			start = ((start >> 16) & 0xFF) * 0x8000 + (start & 0x7FFF);
			end = ((end >> 16) & 0xFF) * 0x8000 + (end & 0x7FFF);
		}
	}

	// If end isn't after start, set end to end-of-file.
	if(end <= start)
	{
		end = len-1;
	}

	// If new origin set, apply it.
	if (origin<0x1000000)
	{
		pos=origin;
	}

	// If shadow set, apply it
	if (shadow)
	{
		pos |= 0x800000;
	}

	// If hirom, apply the mapping
	if (hirom)
	{
		pos |= 0x400000;
	}

	if (verbose_mode && !silent_mode) {
    	fprintf(stderr,"Start: $%06lX End: $%06lX Pos: $%06lX\n", start, end, pos);
    	fprintf(stderr,"Input: %s\nOutput: %s\n", infile, outfile);
    	if(shadow)
    	{
    		fprintf(stderr,"Autodetected FastROM.\n");
    	}
    	else
    	{
    		fprintf(stderr,"Autodetected SlowROM.\n");
    	}
	}

	// Begin disassembly

	rpos = start;

	while (rpos < len && rpos <= end)
	{
		// copy some data to the staging area
		memcpy(dmem, data+rpos, 4);

        // deadcode patch handler
        if (rpos == 0 && rpos + 7 < len && patch_mode) {
            // deadcode patch check
            if (data[rpos] == 0xDE && data[rpos + 1] == 0xAD &&
                data[rpos + 2] == 0xC0 && data[rpos + 3] == 0xDE) {

                // extracting address and length
                unsigned long bank_shifted = (unsigned long)data[rpos + 4] << 16;
                unsigned long high_offset_shifted = (unsigned long)data[rpos + 5] << 8;
                unsigned long low_offset = (unsigned long)data[rpos + 6];
                unsigned long address = bank_shifted | high_offset_shifted | low_offset;

                unsigned char length = data[rpos + 7];
                unsigned long actual_bytes = len - rpos - 8;
                unsigned long max_bytes = length ? (length * 4) : 0;

                // output information
                if (!silent_mode)
                    printf("; Action Replay dead code patches %lu bytes from 0x%06lX:\n", length == 0 ? actual_bytes : max_bytes, address);
                if (!silent_mode && !length)
                    printf("; No length defined. Dynamic mode.\n");

                if (max_bytes && (actual_bytes != max_bytes)) {
                    // show warning if patch length missmatch
                    if (!silent_mode)
                        printf("; Warning: Patch length (%lu bytes) mismatch. Allowed length: %lu bytes.\n", actual_bytes, max_bytes);
                }

                rpos += 8;  // Magic Word (4) + Address (3) + Length (1)
                pos = address;
                continue;
            }
        }

		// disassemble one instruction, or produce one line of hexdump
		if (dwidth == 0)
		{
			offset = disasm(dmem, pos, &flag, inst, tsrc);
		}
		else
		{
			offset = hexdump(data, pos, rpos, len, inst, dwidth);
		}

		// Check for a file/block overrun
		if ((rpos + offset) > len || (rpos + offset) > (end+1))
		{
			// print out remaining bytes and finish
			fprintf(fout,"%02lX/%04lX:\t", (pos >> 16) & 0xFF, pos & 0xFFFF);
			for (i=rpos; i<len && i<=end; i++)
			{
				fprintf(fout,"%02X", data[rpos]);
			}
			fprintf(fout,"\n");
			break;
		}

		// Check for a bank overrun
		if (bound && ((pos & 0xFFFF) + offset) > 0x10000)
		{
			// print out remaining bytes
			fprintf(fout, "%02lX/%04lX:\t",(pos >> 16) & 0xFF, pos & 0xFFFF);
			tmp = 0x10000 - (pos & 0xFFFF);
			for (i=0; i<tmp; i++)
			{
				fprintf(fout, "%02X", data[rpos+i]);
			}
			fprintf(fout, "\n");
			// Move to next bank
			if(!hirom)
			{
				pos = (pos & 0xFF0000) + 0x18000;
			}
			else
			{
				pos += tmp;
			}
			rpos += tmp;
			continue;
		}

		fprintf(fout, "%s\n", inst);

		// Move to next instruction
		if (!hirom && ((pos & 0xFFFF) + offset) > 0xFFFF)
		{
			pos = (pos & 0xFF0000) + 0x18000;
		}
		else
		{
			pos += offset;
		}
		rpos+=offset;
	}

	fclose(fout);

	return 0;
}
