#include "main.h"

typedef struct _Current_Block
{
	UInt8* OriBlk;  //원본 블록 Original Block
	UInt8* PredBlk; //예측 블록 Prediction Block
	Int32* ResiBlk; //잔차 블록 Residual Block

	Int32* QuantBlk;
}CurrentBlock;

extern Int32 PicWid;
extern Int32 PicHei;
extern FILE* dpt;
#if !EntropyFlag
extern FILE* cp;
#endif
void EncoderFunc8x8(UInt8* OriLuma, UInt8* RecLuma, CurrentBlock *CurBlk, Int32 ctbX, Int32 ctbY, Int32 BlkSize)
{
	refBuf ref;
	Int32 candiRecBlk[modeNum][BlockSize * BlockSize]; //복원영상 임시저장 버퍼
	Int32 bistreamCandidate[modeNum][(BlockSize / 2) * (BlockSize / 2)]; //양자화된 계수 임시저장 버퍼


	Int32 Sad[modeNum] = { 0 };
	Int32 Sort[modeNum] = { 0 };
	Int32 BestSad, tmp, BestMode;

	ImgBuf Img; //엔트로피 코딩으로 들어가는 버퍼

	UInt8* RecPos, *OriPos; //복원화소 위치, 원본화소 위치, 현재 블록의 좌상단에 존재하는 화소를 가리킴 
	RecPos = RecLuma + (ctbY * BlkSize) * PicWid + (ctbX * BlkSize);
	OriPos = OriLuma + (ctbY * BlkSize) * PicWid + (ctbX * BlkSize);

	for (Int32 y = 0; y < BlkSize; y++)
		for (Int32 x = 0; x < BlkSize; x++)
			CurBlk->OriBlk[y * BlkSize + x] = OriPos[y * PicWid + x];

	ref = xFillReferenceSamples(RecPos, ctbX, ctbY, PicWid, BlkSize);
	for (UInt8 mode = 0; mode < modeNum; mode++)
	{
		PredictionFunc(CurBlk->PredBlk, ref, BlkSize, BlkSize, mode);
		for (Int32 pos = 0; pos < BlkSize * BlkSize; pos++)
			CurBlk->ResiBlk[pos] = CurBlk->OriBlk[pos] - CurBlk->PredBlk[pos];
#if TransQuantFlag
		TransQuantFunc(CurBlk->ResiBlk, BlkSize, CurBlk->QuantBlk);

		for (Int32 pos = 0; pos < (BlkSize / 2) * (BlkSize / 2); pos++)
			bistreamCandidate[mode][pos] = CurBlk->QuantBlk[pos];

		InvTransQuantFunc(CurBlk->QuantBlk, BlkSize, CurBlk->ResiBlk);
#endif
		for (Int32 pos = 0; pos < BlkSize * BlkSize; pos++)
			candiRecBlk[mode][pos] = CurBlk->ResiBlk[pos] + CurBlk->PredBlk[pos];
	}
	
	for (Int32 pos = 0; pos < BlkSize * BlkSize; pos++)
		for (UInt8 mode = 0; mode < modeNum; mode++)
			Sad[mode] += abs(CurBlk->OriBlk[pos] - candiRecBlk[mode][pos]);

	for (Int32 i = 0; i < modeNum; i++)
		Sort[i] = Sad[i];

	for (Int32 i = 1; i < modeNum; i++)
	{
		for (tmp = Sort[i], BestSad = i; BestSad > 0; BestSad--)
		{
			if (Sort[BestSad - 1] > tmp)
				Sort[BestSad] = Sort[BestSad - 1];
			else break;
		}
		Sort[BestSad] = tmp;
	}

	for (Int32 i = 0; i < modeNum; i++)
		if (Sort[0] == Sad[i])
		{
			BestMode = i;
			break;
		}

	for (Int32 i = 0; i < BlkSize; i++)
		for (Int32 j = 0; j < BlkSize; j++)
			RecLuma[(ctbY * BlkSize + i) * PicWid + (ctbX * BlkSize + j)] = (UInt8)candiRecBlk[BestMode][i * BlkSize + j];

	Img.Quant_blk = (Int32*)calloc((BlkSize / 2) * (BlkSize / 2), sizeof(Int32));

	Img.Best_Mode = BestMode;
	for (Int32 i = 0; i < BlkSize / 2; i++)
		for (Int32 j = 0; j < BlkSize / 2; j++)
			Img.Quant_blk[i * (BlkSize / 2) + j] = bistreamCandidate[BestMode][i * (BlkSize / 2) + j];

#if !EntropyFlag
	fprintf(cp, "%d\n", Img.Best_Mode);
	for (int i = 0; i < BlkSize / 2; i++)
	{
		for (int j = 0; j < BlkSize / 2; j++)
		{
			fprintf(cp, "%d\t", Img.Quant_blk[i * (BlkSize / 2) + j]);
		}
	}
	fprintf(cp, "\n");
#else
	EncoderEntropyCoding(BlkSize / 2, &Img);
#endif

	free(Img.Quant_blk);
}

void InitBlock(CurrentBlock *CurBlk, Int32 BlkSize)
{
	CurBlk->OriBlk  = (UInt8*)calloc(BlkSize * BlkSize, sizeof(UInt8));
	CurBlk->PredBlk = (UInt8*)calloc(BlkSize * BlkSize, sizeof(UInt8));
	CurBlk->ResiBlk = (Int32*)calloc(BlkSize * BlkSize, sizeof(Int32));
	CurBlk->QuantBlk = (Int32*)calloc(BlkSize * BlkSize, sizeof(Int32));
}

void DestroyBlock(CurrentBlock* CurBlk)
{
	free(CurBlk->OriBlk);
	free(CurBlk->PredBlk);
	free(CurBlk->ResiBlk);
	free(CurBlk->QuantBlk);
}

void EncodingProcess(UInt8* OriLuma, UInt8* RecLuma)
{
	CurrentBlock CurBlk;
	Int32 BlkSize = BlockSize;
	Int32 StopX, StopY, m_picWidthInCtu, m_picHeightInCtu;
	
	StopX = m_picWidthInCtu  = PicWid / BlkSize;
	StopY = m_picHeightInCtu = PicHei / BlkSize;

	InitBlock(&CurBlk, BlkSize);

	for (Int32 ctbY = 0; ctbY < StopY; ctbY++)
		for (Int32 ctbX = 0; ctbX < StopX; ctbX++) //8x8 블록 단위로 반복문이 실행됨
			EncoderFunc8x8(OriLuma, RecLuma, &CurBlk, ctbX, ctbY, BlkSize);
			
	DestroyBlock(&CurBlk);
}