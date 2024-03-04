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
	FILE* fp, *op; //원본 영상 파일과 복원 영상 파일

	Dou64 mse = 0;

	UInt8* OriLuma, * RecLuma; //원본 영상과 복원 영상 저장 버퍼
	Int32 PicSize = PicWid * PicHei; //영상 사이즈

	OriLuma = (UInt8*)calloc(PicSize, sizeof(UInt8));
	RecLuma = (UInt8*)calloc(PicSize, sizeof(UInt8));

	fopen_s(&fp, "BasketballDrill_832x480_50.y", "rb");
	fopen_s(&op, "REC_Enc_BasketballDrill_832x480_50.y", "wb");

#if !EntropyFlag
	fopen_s(&cp, "TEST.txt", "w");
#else
	fopen_s(&dp, "Bitstream.raw", "wb");
#endif

	for (Int32 i = 0; i < frame; i++)
	{
		memset(OriLuma, MinVal, sizeof(UInt8) * PicSize);
		memset(RecLuma, MinVal, sizeof(UInt8) * PicSize);
		fread(OriLuma, sizeof(UInt8), PicSize, fp);

		EncodingProcess(OriLuma, RecLuma);

		for (Int32 y = 0; y < PicHei; y++)
			for (Int32 x = 0; x < PicHei; x++)
				mse += ((OriLuma[i] - RecLuma[i]) * (OriLuma[i] - RecLuma[i]));

		fwrite(RecLuma, sizeof(UInt8), PicSize, op);
		printf("Frame: %d\n", i);
	}

	mse = mse / (PicWid * PicHei * frame);
	printf("MSE : %lf\n", mse);

	free(OriLuma);
	free(RecLuma);

	fclose(fp);
	fclose(op);
#if !EntropyFlag
	fclose(cp);
#else
	fclose(dp);
#endif
}