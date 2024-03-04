#include "main.h"

typedef struct _refsample
{
	UInt8(*at)(refBuf, Int32, Int32);
}refSam;

UInt8 refSample(refBuf ref, Int32 side, Int32 pos)
{
	return ref.refPix[side][pos];
}

refBuf xFillReferenceSamples(UInt8* recPos, Int32 ctbX, Int32 ctbY, Int32 stride, Int32 BlkSize)
{
	Int32 StopX = stride / BlkSize;
	refBuf ref;
	recPos -= stride;
	recPos -= 1;

	////////////////////////////////////////////////////////////////////////////////////////////////
	//recPos 이용 ==> ref.refPix[][] = recPos[];                          //ref.refPix[0][]은 상단 참조화소, ref.refPix[1][]은 좌측 참조화소
	if (ctbY != 0 && ctbX != 0)
	{
		for (Int32 i = 0; i <= BlkSize * 2; i++)
		{
			ref.refPix[0][i] = recPos[i];
		}
		for (Int32 i = 0; i < BlkSize; i++)
		{
			ref.refPix[1][i] = recPos[i * stride];
		}

	}
	else if (ctbX == 0 && ctbY != 0)
	{
		for (Int32 i = 0; i <= BlkSize * 2; i++)
		{
			ref.refPix[0][i] = i == 0 ? 128 : recPos[i];
			ref.refPix[1][i] = 128;
		}
	}

	else if (ctbX == 0 && ctbY == 0)
	{
		for (Int32 i = 0; i <= BlkSize * 2; i++)
		{
			ref.refPix[0][i] = 128;
			ref.refPix[1][i] = 128;
		}
	}

	else
	{
		for (Int32 i = 0; i <= BlkSize * 2; i++)
		{
			ref.refPix[0][i] = 128;
		}
		for (Int32 i = 0; i <= BlkSize; i++)
		{
			ref.refPix[1][i] = i == 0 ? 128 : recPos[i * stride];//BLockSize까지? 아니면 BlockSize+1까지???
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	for (Int32 i = BlkSize; i < BlkSize * 2; i++)
	{
		ref.refPix[1][i + 1] = 128;
	}

	return ref;
}

void PredictionFunc(UInt8* Pred, refBuf ref, Int32 BlkSize, Int32 BlkStride, UInt8 mode)
{
	UInt8* refMain;
	UInt8* refSide;

	UInt8 refAbove[2 * BlockSize + 1];
	UInt8 refLeft[2 * BlockSize + 1];

	Bool bIsModeVer = (mode == HORIZONTAL || mode == HDIAGONAL) ? 0 : 1;
	Int32 intraPredAngle = mode == DIAGONAL ? -32 : mode == VDIAGONAL || mode == HDIAGONAL ? 32 : 0;
	UInt8 predBuf[BlockSize * BlockSize];

	refSam pSrc;
	pSrc.at = refSample;

	if (intraPredAngle >= 0)
	{
		for (Int32 x = 0; x <= BlkSize * 2; x++)
			refAbove[x] = pSrc.at(ref, 0, x);
			
		for (Int32 y = 0; y <= BlkSize * 2; y++)
			refLeft[y] = pSrc.at(ref, 1, y);

		refMain = bIsModeVer ? refAbove : refLeft;
		refSide = bIsModeVer ? refLeft : refAbove;
	}

	else
	{
		for (Int32 x = 0; x <= BlkSize; x++)
			refAbove[x + BlkSize] = pSrc.at(ref, 0, x);

		for (Int32 y = 0; y <= BlkSize; y++)
			refLeft[y + BlkSize] = pSrc.at(ref, 1, y);

		refMain = bIsModeVer ? refAbove + BlkSize : refLeft + BlkSize;
		refSide = bIsModeVer ? refLeft + BlkSize : refAbove + BlkSize;

		// Extend the Main reference to the left.
		Int32 sizeSide = BlkSize;
		Int32 sidestart = BlkSize;
		for (Int32 k = -sizeSide; k <= -1; k++)
		{
			refMain[k] = refSide[sidestart];
			sidestart -= 1;
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////

	Pel* pDsty = predBuf;
	if (mode == PLANAR)
	{
		for (Int32 y = 0; y < BlkSize; y++)
		{
			for (Int32 x = 0; x < BlkSize; x++)
			{
				predBuf[y * BlkSize + x] =
					((BlkSize - 1 - x) * ref.refPix[1][y + 1] + (x + 1) * ref.refPix[0][BlkSize] + (BlkSize - 1 - y) * ref.refPix[0][x + 1] + (y + 1) * ref.refPix[1][BlkSize] + BlkSize) >> (Int32)(log2(BlkSize) + 1);
			}
		}
	}

	else if (mode == VERTICAL)
	{
		for (Int32 y = 0; y < BlkSize; y++)
		{
			for (Int32 x = 0; x < BlkSize; x++)
			{
				predBuf[y * BlkSize + x] = ref.refPix[0][x + 1];
			}
		}
	}
	else if (mode == HORIZONTAL)
	{
		for (Int32 y = 0; y < BlkSize; y++)
		{
			for (Int32 x = 0; x < BlkSize; x++)
			{
				predBuf[y * BlkSize + x] = ref.refPix[1][y + 1];
			}
		}
	}
	else if (mode == DIAGONAL)
	{

		for (int i = 0; i < BlkSize; i++)
		{
			for (int y = 0; y < BlkSize; y++)
				for (int x = 0; x < BlkSize; x++)
				{
					if ((abs(x - y) == i) && ((x - y) >= 0))
						predBuf[y * BlkSize + x] = ref.refPix[0][i];
					else if ((abs(x - y) == i) && ((x - y) < 0))
						predBuf[y * BlkSize + x] = ref.refPix[1][i];
				}
		}
	}
	else if (mode == VDIAGONAL)
	{
		for (Int32 y = 0; y < BlkSize; y++)
		{
			for (Int32 x = 0; x < BlkSize; x++)
			{
				predBuf[y * BlkSize + x] = ref.refPix[0][x + y + 2];
			}
		}
	}
	else if (mode == HDIAGONAL)
	{
		for (Int32 y = 0; y < BlkSize; y++)
		{
			for (Int32 x = 0; x < BlkSize; x++)
			{
				predBuf[y * BlkSize + x] = ref.refPix[1][x + y + 2];
			}
		}
	}
	else
	{
		Int32 AVG = 0;

		for (Int32 i = 1; i <= BlkSize; i++)
		{
			AVG += ref.refPix[0][i];
		}
		for (Int32 i = 1; i <= BlkSize; i++)
		{
			AVG += ref.refPix[1][i];
		}
		for (Int32 i = 0; i < BlkSize * BlkSize; i++)
		{
			predBuf[i] = (Int32)((float)AVG / (float)(BlkSize * 2) + 0.5);
		}



	}
	////////////////////////////////////////////////////////////////////////////////////////////////

	for (Int32 y = 0; y < BlkSize; y++)
		for (Int32 x = 0; x < BlkSize; x++)
			Pred[y * BlkStride + x] = predBuf[y * BlkStride + x];
			//Pred[y * BlkStride + x] = bIsModeVer ? predBuf[y * BlkStride + x] : predBuf[x * BlkStride + y]; //예측블록 최종저장
}