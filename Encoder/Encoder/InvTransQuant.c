#include "main.h"

void InvTransQuantFunc(Int32* QuantBlk, Int32 BlkSize, Int32* InvResiBlk)
{
	Int32 Quant_Table[] =
					{ 8, 10, 12, 14,
					  14, 16, 18, 20,
					  18, 20, 22, 24,
					  20, 22, 24, 26
	};

	Dou64 PI = pi;
	Dou64 DCTBlk[BlockSize * BlockSize];
	Dou64 Recon_R;
	Dou64 DCT_Real;

	Int32 Blk4 = BlkSize / 2;

	for (Int32 i = 0; i < BlkSize; i++)
		for (Int32 j = 0; j < BlkSize; j++)
			DCTBlk[i * BlkSize + j] = 0;

	for (Int32 i = 0; i < Blk4; i++)
		for (Int32 j = 0; j < Blk4; j++)
			DCTBlk[i * BlkSize + j] = QuantBlk[i * Blk4 + j] * Quant_Table[i * Blk4 + j];

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//InvResiBlk에 Inverse DCT값 저장

	for (Int32 i = 0; i < BlkSize; i++)
	{
		for (Int32 j = 0; j < BlkSize; j++)
		{
			Recon_R = 0;
			for (Int32 k = 0; k < BlkSize; k++)
			{
				for (Int32 l = 0; l < BlkSize; l++)
				{
					DCT_Real = DCTBlk[k * BlkSize + l] * cos((2. * j + 1.) * (l)*PI / (2. * BlkSize)) * cos((2. * i + 1) * (k)*PI / (2. * BlkSize));//11
					if (k == 0 && l == 0)
						Recon_R += DCT_Real / BlkSize;
					else if (k == 0 || l == 0)
						Recon_R += (DCT_Real * (1.0 / sqrt(2.0))) / (BlkSize / 2);
					else
						Recon_R += DCT_Real / (BlkSize / 2);
				}
			}

			if (Recon_R < 0)
				Recon_R = (int)(Recon_R - 0.5);

			else
				Recon_R = (int)(Recon_R + 0.5);


			InvResiBlk[i * BlkSize + j] = (Int32)Recon_R;
		}
	}







	//////////////////////////////////////////////////////////////////////////////////////////////////
}