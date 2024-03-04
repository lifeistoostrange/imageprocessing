#include "main.h"

typedef struct _Current_Block
{
	UInt8* ReconBlk;
	UInt8* PredBlk; //예측 블록 Prediction Block
	Int32* ResiBlk; //잔차 블록 Residual Block

	Int32* QuantBlk;
}CurrentBlock;

extern Int32 PicWid;
extern Int32 PicHei;
#if !EntropyFlag
extern FILE* cp;
#endif
void DecoderFunc8x8(UInt8* RecLuma, CurrentBlock* CurBlk, Int32 ctbX, Int32 ctbY, Int32 BlkSize)
{
	refBuf ref;

	ImgBuf Img; //엔트로피 코딩으로 들어가는 버퍼

	UInt8* RecPos; //복원화소 위치, 원본화소 위치, 현재 블록의 좌상단에 존재하는 화소를 가리킴 
	RecPos = RecLuma + (ctbY * BlkSize) * PicWid + (ctbX * BlkSize);

	Img.Quant_blk = (Int32*)calloc((BlkSize / 2) * (BlkSize / 2), sizeof(Int32));
#if !EntropyFlag
	fscanf(cp, "%d\n", &Img.Best_Mode);
	for (int i = 0; i < BlkSize / 2; i++)
	{
		for (int j = 0; j < BlkSize / 2; j++)
		{
			fscanf(cp, "%d\t", &Img.Quant_blk[i * (BlkSize / 2) + j]);
		}
	}
	fscanf(cp, "\n");
#else 
	DecoderEntropyCoding(BlkSize / 2, &Img);
#endif

	for (Int32 pos = 0; pos < (BlkSize / 2) * (BlkSize / 2); pos++)
		CurBlk->QuantBlk[pos] = Img.Quant_blk[pos];

	ref = xFillReferenceSamples(RecPos, ctbX, ctbY, PicWid, BlkSize);
	PredictionFunc(CurBlk->PredBlk, ref, BlkSize, BlkSize, Img.Best_Mode);
	InvTransQuantFunc(CurBlk->QuantBlk, BlkSize, CurBlk->ResiBlk);

	for (Int32 pos = 0; pos < BlkSize * BlkSize; pos++)
		CurBlk->ReconBlk[pos] = CurBlk->ResiBlk[pos] + CurBlk->PredBlk[pos];

	for (Int32 i = 0; i < BlkSize; i++)
		for (Int32 j = 0; j < BlkSize; j++)
			RecLuma[(ctbY * BlkSize + i) * PicWid + (ctbX * BlkSize + j)] = CurBlk->ReconBlk[i * BlkSize + j];

	free(Img.Quant_blk);
}

void InitBlock(CurrentBlock* CurBlk, Int32 BlkSize)
{
	CurBlk->ReconBlk = (UInt8*)calloc(BlkSize * BlkSize, sizeof(UInt8));
	CurBlk->PredBlk = (UInt8*)calloc(BlkSize * BlkSize, sizeof(UInt8));
	CurBlk->ResiBlk = (Int32*)calloc(BlkSize * BlkSize, sizeof(Int32));
	CurBlk->QuantBlk = (Int32*)calloc(BlkSize * BlkSize, sizeof(Int32));
}

void DestroyBlock(CurrentBlock* CurBlk)
{
	free(CurBlk->ReconBlk);
	free(CurBlk->PredBlk);
	free(CurBlk->ResiBlk);
	free(CurBlk->QuantBlk);
}

void DecodingProcess(UInt8* RecLuma)
{
	CurrentBlock CurBlk;

	Int32 BlkSize = BlockSize;
	Int32 StopX, StopY, m_picWidthInCtu, m_picHeightInCtu;

	StopX = m_picWidthInCtu = PicWid / BlkSize;
	StopY = m_picHeightInCtu = PicHei / BlkSize;
	
	InitBlock(&CurBlk, BlkSize);

	for (Int32 ctbY = 0; ctbY < StopY; ctbY++)
		for (Int32 ctbX = 0; ctbX < StopX; ctbX++) //8x8 블록 단위로 반복문이 실행됨
		{
			//printf("\n(%d, %d)", ctbX, ctbY);
			DecoderFunc8x8(RecLuma, &CurBlk, ctbX, ctbY, BlkSize);
		}
	
	DestroyBlock(&CurBlk);
}