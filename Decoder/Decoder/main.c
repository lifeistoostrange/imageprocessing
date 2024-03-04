#include "main.h"

Int32 PicWid = WIDTH;
Int32 PicHei = HEIGHT;
Int32 frame = FRAME;

#if !EntropyFlag
FILE* cp;
#else
FILE* dp;
#endif
void main()
{
	FILE* op, *fp;

	Dou64 mse = 0;

	UInt8* RecLuma, *EncLuma;
	Int32 PicSize = PicWid * PicHei; //영상 사이즈

	EncLuma = (UInt8*)malloc(sizeof(UInt8) * PicSize);
	RecLuma = (UInt8*)malloc(sizeof(UInt8) * PicSize);

	fopen_s(&op, "REC_Dec_BasketballDrill_832x480_50.y", "wb");
	fopen_s(&fp, "REC_Enc_BasketballDrill_832x480_50.y", "rb");
#if !EntropyFlag
	fopen_s(&cp, "TEST.txt", "r");
#else
	fopen_s(&dp, "Bitstream.raw", "rb");
#endif

	for (Int32 i = 0; i < frame; i++)
	{
		memset(EncLuma, MinVal, sizeof(UInt8) * PicSize);
		memset(RecLuma, MinVal, sizeof(UInt8) * PicSize);
		fread(EncLuma, sizeof(UInt8), PicSize, fp);

		DecodingProcess(RecLuma);

		for (Int32 y = 0; y < PicHei; y++)
			for (Int32 x = 0; x < PicHei; x++)
				mse += ((EncLuma[i] - RecLuma[i]) * (EncLuma[i] - RecLuma[i]));

		fwrite(RecLuma, sizeof(UInt8), PicSize, op);
		printf("Frame: %d\n", i);
	}

	mse = mse / (PicWid * PicHei * frame);
	printf("MSE : %lf\n", mse);

	free(RecLuma);
	free(EncLuma);

	fclose(op);
	fclose(fp);
#if !EntropyFlag
	fclose(cp);
#else
	fclose(dp);
#endif
}


