#include <stdio.h>

extern long ce_exec_config[];

main(int argc, char *argv[])
{
	FILE *out, *in;
	int i, cnt, pos;
	unsigned char *lp;
	unsigned char buf[4096];
	if (argc != 3)
	{
		fprintf(stderr, "usage: %s <in-file> <out-file>\n", argv[0]);
		exit(1);
	}
	if ((out = fopen(argv[2], "w")) == (FILE *)0)
	{
		fprintf(stderr, "Can't create '%s'\n", argv[2]);
		exit(1);
	}
	if ((in = fopen(argv[1], "r")) == (FILE *)0)
	{
		fprintf(stderr, "Can't open '%s'\n", argv[1]);
		exit(1);
	}
	fprintf(out, "#\n");
	fprintf(out, "# Miscellaneous data structures:\n");
	fprintf(out, "# WARNING - this file is automatically generated!\n");
	fprintf(out, "#\n");
	fprintf(out, "\n");
	fprintf(out, "\t.data\n");
	fprintf(out, "\t.globl builtin_ramdisk_image\n");
	fprintf(out, "builtin_ramdisk_image:\n");
	pos = 0;
	while (fread(buf, sizeof(buf), 1, in) == 1)
	{
		cnt = 0;
		lp = (unsigned char *)buf;
		for (i = 0;  i < sizeof(buf);  i += 4)
		{
			if (cnt == 0)
			{
				fprintf(out, "\t.long\t");
			}
			fprintf(out, "0x%02X%02X%02X%02X", lp[0], lp[1], lp[2], lp[3]);
			lp += 4;
			if (++cnt == 4)
			{
				cnt = 0;
				fprintf(out, " # %x \n", pos+i-12);
				fflush(out);
			} else
			{
				fprintf(out, ",");
			}
		}
		pos += sizeof(buf);
	}
	fprintf(out, "\t.globl builtin_ramdisk_size\n");
	fprintf(out, "builtin_ramdisk_size:\t.long\t0x%x\n", pos);
	fflush(out);
	fclose(out);
	exit(0);
}

