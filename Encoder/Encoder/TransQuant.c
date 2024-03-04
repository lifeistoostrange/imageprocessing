#include "main.h"

void TransQuantFunc(Int32* ResiBlk, Int32 BlkSize, Int32* QuantBlk)
{
	Int32 Quant_Table[] =
					{ 8, 10, 12, 14,
					  14, 16, 18, 20,
					  18, 20, 22, 24,
					  20, 22, 24, 26
	};

	Int32 Simple_LPF[64] = {
		1, 1, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0
	};
	Dou64 PI = pi;
	//Dou64 Temp, Spec;
	Dou64 DCTBlk[BlockSize * BlockSize] = { 0 }; //DCT적용한 값 저장
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	for (Int32 i = 0; i < BlkSize; i++)
	{
		for (Int32 j = 0; j < BlkSize; j++)
		{
			for (Int32 k = 0; k < BlkSize; k++)
			{
				for (Int32 l = 0; l < BlkSize; l++)
				{
					DCTBlk[i * BlkSize + j] += ResiBlk[k * BlkSize + l] * (cos((2.0 * l + 1.0) * j * PI / (2.0 * BlkSize))) * (cos((2.0 * k + 1.0) * i * PI / (2.0 * BlkSize)));
				}
			}
			if (i == 0 && j == 0)
				DCTBlk[i * BlkSize + j] /= BlkSize;
			else if (i == 0 || j == 0)
				DCTBlk[i * BlkSize + j] = (DCTBlk[i * BlkSize + j] * (1.0 / sqrt(2.0))) / (BlkSize / 2);
			else
				DCTBlk[i * BlkSize + j] = DCTBlk[i * BlkSize + j] / (BlkSize / 2);
		}
	}




	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (Int32 i = 0; i < BlkSize; i++)
		for (Int32 j = 0; j < BlkSize; j++)
			DCTBlk[i * BlkSize + j] *= Simple_LPF[i * BlkSize + j];

	Int32 Blk4 = BlkSize / 2;
	for (Int32 i = 0; i < Blk4; i++)
		for (Int32 j = 0; j < Blk4; j++)
			QuantBlk[i * Blk4 + j] = (int)(DCTBlk[i * BlkSize + j] / Quant_Table[i * Blk4 + j] + 0.5);

}