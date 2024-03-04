#include "main.h"

#define MAX_ELEMENT 200

#if EntropyFlag
extern FILE* dp;

typedef struct TreeNode {
	int CNT;
	int ch;
	struct TreeNode* left;
	struct TreeNode* right;
} TreeNode;

typedef struct {
	TreeNode* ptree;
	int ch;
	int key;
} element;

typedef struct {
	element heap[MAX_ELEMENT];
	int heap_size;
} HeapType;

//���� �Լ�
HeapType* create()
{
	return (HeapType*)malloc(sizeof(HeapType));
}

//�ʱ�ȭ �Լ�
void init(HeapType* h)
{
	h->heap_size = 0;
}

//���� ����� ������ heap_size�� ���� h�� item�� �����Ѵ�.
//���� �Լ�
void insert_min_heap(HeapType* h, element item)
{
	int i;
	i = ++(h->heap_size);

	//Ʈ���� �Ž��� �ö󰡸鼭 �θ� ���� ���ϴ� ����
	while ((i != 1) && (item.key < h->heap[i / 2].key)) {
		h->heap[i] = h->heap[i / 2];
		i /= 2;
	}
	h->heap[i] = item; // ���ο� ��� ����
}

//���� �Լ�
element delete_min_heap(HeapType* h)
{
	int parent, child;
	element item, temp;

	item = h->heap[1];
	temp = h->heap[(h->heap_size)--];
	parent = 1;
	child = 2;
	while (child <= h->heap_size) {
		//���� ����� �ڽĳ���� �� ���� �ڽĳ�带 ã�´�.
		if ((child < h->heap_size) && (h->heap[child].key) > h->heap[child + 1].key)
			child++;
		if (temp.key < h->heap[child].key) break;

		//�� �ܰ� �Ʒ��� �̵�
		h->heap[parent] = h->heap[child];
		parent = child;
		child *= 2;
	}
	h->heap[parent] = temp;
	return item;
}

//���� Ʈ�� ���� �Լ�
TreeNode* make_tree(TreeNode* left, TreeNode* right) {
	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	node->left = left;
	node->right = right;
	return node;
}

//���� Ʈ�� ���� �Լ�
void destroy_tree(TreeNode* root)
{
	if (root == NULL) return;
	destroy_tree(root->left);
	destroy_tree(root->right);
	free(root);
}

int is_leaf(TreeNode* root)
{
	return !(root->left) && !(root->right);
}

void print_array(int codes[], int n, Int32 Idx, ImgBuf* img)
{
	for (int i = 0; i < n; i++)
	{
		img->Huff_codes[Idx] <<= 1;
		if (codes[i] == 1)
		{
			img->Huff_codes[Idx] |= 1;
		}
	}
	img->Huff_length[Idx] = n;
}

void print_codes(TreeNode* root, int codes[], int top, ImgBuf* img)
{
	//1�� �����ϰ� ��ȯȣ���Ѵ�.
	if (root->left)
	{
		codes[top] = 1;
		print_codes(root->left, codes, top + 1, img);
	}
	//0�� �����ϰ� ��ȯȣ���Ѵ�.
	if (root->left)
	{
		codes[top] = 0;
		print_codes(root->right, codes, top + 1, img);
	}
	//�ܸ�����̸� �ڵ带 ����Ѵ�.
	if (is_leaf(root))
	{
		Int32 Idx = 0;
		for (Int32 i = 0; i < 16; i++)
		{
			if (root->ch == img->Huff_ch[i])
			{
				Idx = i;
				break;
			}
		}
		print_array(codes, top, Idx, img);
	}
}

//������ �ڵ� ���� �Լ�
void huffman_tree(int freq[], int ch_list[], int n, ImgBuf* img)
{
	TreeNode* node, * x;
	HeapType* Set;
	element e, e1, e2;
	int codes[100];
	int top = 0;

	Set = create();
	init(Set);
	for (int i = 0; i < n; i++) {
		node = make_tree(NULL, NULL);
		e.ch = node->ch = ch_list[i];
		e.key = node->CNT = freq[i];
		e.ptree = node;
		insert_min_heap(Set, e);
	}

	for (int i = 1; i < n; i++) {
		//�ּҰ��� ������ �� ���� ��带 ����
		e1 = delete_min_heap(Set);
		e2 = delete_min_heap(Set);

		//�ΰ��� ��带 ��ģ��. 
		x = make_tree(e1.ptree, e2.ptree);
		e.key = x->CNT = e1.key + e2.key;
		e.ptree = x;
		insert_min_heap(Set, e);
	}
	e = delete_min_heap(Set);
	print_codes(e.ptree, codes, top, img);
	destroy_tree(e.ptree);
	free(Set);
}

void Huffman(ImgBuf* img)
{
	Int32 size = 0;

	for (Int32 i = 0; i < 16; i++)
	{
		img->Huff_ch[i] = 10000;
		img->Huff_freq[i] = 0;
		img->Huff_codes[i] = 0;
		img->Huff_length[i] = 0;
	}
	img->Huff_Size = 0;

	for (Int32 i = 0; i < 16; i++)
	{
		Int32 flag = 0;
		img->Quant_blk[i];
		for (Int32 j = 0; j < 16; j++)
		{
			if (img->Quant_blk[i] == img->Huff_ch[j] && flag == 0)
			{
				flag = 1;
				img->Huff_freq[j]++;
			}
		}
		if (flag == 0)
		{
			img->Huff_ch[size] = img->Quant_blk[i];
			img->Huff_freq[size++]++;
		}
	}

	huffman_tree(img->Huff_freq, img->Huff_ch, size, img);
	img->Huff_Size = size;
}


void EncoderEntropyCoding(Int32 BLK, ImgBuf* Img)
{
	Int32 ALLZeroFlag = 1; //1�ΰ�� Img->Quant_blk(����ȭ�� ��ȯ ��� �� ��)�� ��� 0�� �� ����
	                       //0�ΰ�� Img->Quant_blk(����ȭ�� ��ȯ ��� �� ��) �� 0�� �ƴ� ���� �� ����
	UInt8 Bitstream = 0;
	UInt8 BitCnt = 0;
	UInt8 tmp = 0;

	UInt8 CurInfo = 0; //���� ��Ʈ��Ʈ������ ������� �ϴ� ����

	///////////////////////////////////////////////////////////////////////////////////
	//Img->Best_Mode ����
	switch (Img->Best_Mode)
	{
	case 0:
		Bitstream |= 0x00;
		BitCnt += 2;
		break;
	case 1:
		Bitstream |= 0x02;
		BitCnt += 3;
		break;
	case 2:
		Bitstream |= 0x03;
		BitCnt += 3;
		break;
	case 3:
		Bitstream |= 0x04;
		BitCnt += 3;
		break;
	case 4:
		Bitstream |= 0x05;
		BitCnt += 3;
		break;
	case 5:
		Bitstream |= 0x06;
		BitCnt += 3;
		break;
	case 6:
		Bitstream |= 0x07;
		BitCnt += 3;
		break;
	}
	///////////////////////////////////////////////////////////////////////////////////
	for (Int32 i = 0; i < BlockSize * BlockSize / 4; i++)
	{
		if (Img->Quant_blk[i] != 0)
			ALLZeroFlag = 0;
	}

	if (ALLZeroFlag == 1) //����ȭ ��� �� ȭ�Ұ� ��� 0�� ���
	{
		///////////////////////////////////////////////////////////////////////////////////
		Bitstream <<= 1;
		BitCnt++;
		Bitstream |= 0x01;
		///////////////////////////////////////////////////////////////////////////////////
	}
	else //����ȭ ��� �� 0�� �ƴ� ȭ�Ұ� �ִ� ���
	{
		Int32 QuantBuf[BlockSize * BlockSize / 4] = { 0 };
		for (Int32 i = 0; i < BlockSize * BlockSize / 4; i++)
		{
			QuantBuf[i] = Img->Quant_blk[i];
			Img->Quant_blk[i] = abs(Img->Quant_blk[i]);
		}

		Huffman(Img);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////Img->Huff_Size
		// Huff_Size���� Golomb code�� ��ȯ �� Bitstream�� ����
		CurInfo = Img->Huff_Size;
		Int32 PrefixCnt = (Int32)log2((double)(CurInfo + 1));

		// Prefix code(0)�� PrefixCnt ������ŭ Bitstream�� ����
		for (Int32 i = 0; i < PrefixCnt; i++)
		{
			Bitstream <<= 1;
			BitCnt++;
			if (BitCnt == 8)
			{
				fwrite(&Bitstream, sizeof(UInt8), 1, dp);
				BitCnt = 0;
				Bitstream = 0;
			}
		}

		// Center code(1)�� Bitstream�� ����
		Bitstream <<= 1;
		BitCnt++;
		Bitstream |= 0x01;
		if (BitCnt == 8)
		{
			fwrite(&Bitstream, sizeof(UInt8), 1, dp);
			BitCnt = 0;
			Bitstream = 0;
		}

		// Suffix code�� Bitstream�� ����
		Int32 SuffixCode = CurInfo + 1 - (Int32)(pow(2.0, (double)PrefixCnt));   // SuffixCode=CurInfo+1-pow(2,SuffixCnt)������ SuffixCode�� ���
		for (Int32 i = PrefixCnt; i > 0; i--)
		{
			Bitstream <<= 1;
			BitCnt++;
			tmp = (UInt8)(SuffixCode & ((Int32)pow(2., ((double)i - 1.0))));
			tmp >>= (UInt8)(i - 1);
			Bitstream |= tmp;
			if (BitCnt == 8)
			{
				fwrite(&Bitstream, sizeof(UInt8), 1, dp);
				BitCnt = 0;
				Bitstream = 0;
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////Img->Huff_ch, Img->Huff_freq
		for (Int32 i = 0; i < Img->Huff_Size; i++)
		{
			// Huff_ch[i]���� Golomb code�� ��ȯ �� Bitstream�� ����
			CurInfo = Img->Huff_ch[i];
			PrefixCnt = (int)log2((double)(CurInfo + 1));

			// Prefix code(0)�� PrefixCnt ������ŭ Bitstream�� ����
			for (Int32 j = 0; j < PrefixCnt; j++)
			{
				Bitstream <<= 1;
				BitCnt++;
				if (BitCnt == 8)
				{
					fwrite(&Bitstream, sizeof(UInt8), 1, dp);
					BitCnt = 0;
					Bitstream = 0;
				}
			}

			// Center code(1)�� Bitstream�� ����
			Bitstream <<= 1;
			BitCnt++;
			Bitstream |= 0x01;
			if (BitCnt == 8)
			{
				fwrite(&Bitstream, sizeof(UInt8), 1, dp);
				BitCnt = 0;
				Bitstream = 0;
			}

			// Suffix code�� Bitstream�� ����
			SuffixCode = CurInfo + 1 - (Int32)(pow(2.0, (double)PrefixCnt));   // SuffixCode=CurInfo+1-pow(2,SuffixCnt)������ SuffixCode�� ���
			for (Int32 j = PrefixCnt; j > 0; j--)
			{
				Bitstream <<= 1;
				BitCnt++;
				tmp = (UInt8)(SuffixCode & ((Int32)pow(2.0, ((double)j - 1.0))));
				tmp >>= (UInt8)(j - 1);
				Bitstream |= tmp;
				if (BitCnt == 8)
				{
					fwrite(&Bitstream, sizeof(UInt8), 1, dp);
					BitCnt = 0;
					Bitstream = 0;
				}
			}
			// Huff_freq[i]���� Golomb code�� ��ȯ �� Bitstream�� ����
			CurInfo = Img->Huff_freq[i];
			PrefixCnt = (int)log2((double)(CurInfo + 1));
			// Prefix code(0)�� PrefixCnt ������ŭ Bitstream�� ����
			for (Int32 j = 0; j < PrefixCnt; j++)
			{
				Bitstream <<= 1;
				BitCnt++;
				if (BitCnt == 8)
				{
					fwrite(&Bitstream, sizeof(UInt8), 1, dp);
					BitCnt = 0;
					Bitstream = 0;
				}
			}

			// Center code(1)�� Bitstream�� ����
			Bitstream <<= 1;
			BitCnt++;
			Bitstream |= 0x01;
			if (BitCnt == 8)
			{
				fwrite(&Bitstream, sizeof(UInt8), 1, dp);
				BitCnt = 0;
				Bitstream = 0;
			}

			// Suffix code�� Bitstream�� ����
			SuffixCode = CurInfo + 1 - (Int32)(pow(2.0, (double)PrefixCnt));   // SuffixCode=CurInfo+1-pow(2,SuffixCnt)������ SuffixCode�� ���
			for (Int32 j = PrefixCnt; j > 0; j--)
			{
				Bitstream <<= 1;
				BitCnt++;
				tmp = (UInt8)(SuffixCode & ((Int32)pow(2., ((double)j - 1.0))));
				tmp >>= (UInt8)(j - 1);
				Bitstream |= tmp;
				if (BitCnt == 8)
				{
					fwrite(&Bitstream, sizeof(UInt8), 1, dp);
					BitCnt = 0;
					Bitstream = 0;
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////img->Quant_blk[i]
		UInt32 Curcodes = 0;
		UInt32 Curlength = 0;

		for (Int32 i = 0; i < BlockSize * BlockSize / 4; i++)
		{
			CurInfo = Img->Quant_blk[i];
			for (Int32 j = 0; j < Img->Huff_Size; j++)
			{
				if (CurInfo == Img->Huff_ch[j])
				{
					Curcodes = Img->Huff_codes[j];
					Curlength = Img->Huff_length[j];
					break;
				}
			}
			Curcodes <<= (8 - Curlength);
			for (Int32 j = 0; j < (Int32)Curlength; j++)
			{
				if ((Curcodes & 0x80) == 0x80)
				{
					Bitstream <<= 1;
					BitCnt++;
					Bitstream |= 0x01;
					Curcodes <<= 1;
					if (BitCnt == 8)
					{
						fwrite(&Bitstream, sizeof(UInt8), 1, dp);
						BitCnt = 0;
						Bitstream = 0;
					}
				}
				else
				{
					Bitstream <<= 1;
					BitCnt++;
					Curcodes <<= 1;
					if (BitCnt == 8)
					{
						fwrite(&Bitstream, sizeof(UInt8), 1, dp);
						BitCnt = 0;
						Bitstream = 0;
					}
				}
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////+,- ��ȣ
		for (Int32 i = 0; i < BlockSize * BlockSize / 4; i++)
		{
			if (QuantBuf[i] < 0)
			{
				Bitstream <<= 1;
				Bitstream |= 0x01;
				BitCnt++;
				if (BitCnt == 8)
				{
					fwrite(&Bitstream, sizeof(UInt8), 1, dp);
					BitCnt = 0;
					Bitstream = 0;
				}
			}
			else
			{
				Bitstream <<= 1;
				BitCnt++;
				if (BitCnt == 8)
				{
					fwrite(&Bitstream, sizeof(UInt8), 1, dp);
					BitCnt = 0;
					Bitstream = 0;
				}
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	// Bitstream�� �� ������ �����ϸ� �� ������ 0���� ä��
	if (BitCnt != 0)
	{
		Bitstream <<= (UInt8)(8 - BitCnt);
		fwrite(&Bitstream, sizeof(UInt8), 1, dp);
	}
}
#endif