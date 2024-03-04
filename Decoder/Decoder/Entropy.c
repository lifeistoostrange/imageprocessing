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

void print_array(int codes[], int n, Int32 Idx, ImgBuf* Img)
{
	for (int i = 0; i < n; i++)
	{
		Img->Huff_codes[Idx] <<= 1;
		if (codes[i] == 1)
		{
			Img->Huff_codes[Idx] |= 1;
		}
	}
	Img->Huff_length[Idx] = n;
}

void print_codes(TreeNode* root, int codes[], int top, ImgBuf* Img)
{
	//1�� �����ϰ� ��ȯȣ���Ѵ�.
	if (root->left)
	{
		codes[top] = 1;
		print_codes(root->left, codes, top + 1, Img);
	}
	//0�� �����ϰ� ��ȯȣ���Ѵ�.
	if (root->left)
	{
		codes[top] = 0;
		print_codes(root->right, codes, top + 1, Img);
	}
	//�ܸ�����̸� �ڵ带 ����Ѵ�.
	if (is_leaf(root))
	{
		Int32 Idx = 0;
		for (Int32 i = 0; i < 16; i++)
		{
			if (root->ch == Img->Huff_ch[i])
			{
				Idx = i;
				break;
			}
		}
		print_array(codes, top, Idx, Img);
	}
}

//������ �ڵ� ���� �Լ�
void huffman_tree(int freq[], int ch_list[], int n, ImgBuf* Img)
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
	print_codes(e.ptree, codes, top, Img);
	destroy_tree(e.ptree);
	free(Set);
}

void Huffman(ImgBuf* img)
{
	Int32 size = img->Huff_Size;

	huffman_tree(img->Huff_freq, img->Huff_ch, size, img);
}

void DecoderEntropyCoding(Int32 BLK, ImgBuf* Img)
{
	Int32 ALLZeroFlag = 0; //1�ΰ�� Img->Quant_blk(����ȭ�� ��ȯ ��� �� ��)�� ��� 0�� �� ����
						   //0�ΰ�� Img->Quant_blk(����ȭ�� ��ȯ ��� �� ��) �� 0�� �ƴ� ���� �� ����
	UInt8 Bitstream = 0;
	UInt8 BitCnt = 0;
	UInt8 tmp = 0;

	UInt8 CurInfo = 0;

	fread(&Bitstream, sizeof(UInt8), 1, dp);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////������� ���� ����
	////
	
	////������� ����
	if ((Bitstream & 0xc0) == 0)
	{
		Img->Best_Mode = 0;
		Bitstream <<= 2;
		BitCnt += 2;
	}
	else
	{
		Img->Best_Mode = ((Bitstream & 0xe0) >> 5) - 1;
		Bitstream <<= 3;
		BitCnt += 3;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////������� ���� ��
	if ((Bitstream & 0x80) != 0)
	{
		ALLZeroFlag = 1; //��� 0�� ���
		Bitstream <<= 1;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////����ȭ ��� ���� ����
	////ALLZeroFlag ����
	
	if (ALLZeroFlag) //��� 0�� ���
	{
		for (Int32 i = 0; i < BLK * BLK; i++)
			Img->Quant_blk[i] = 0;
	}
	else //0�� �ƴ� ȭ�Ұ� �ִ� ���
	{
		for (Int32 i = 0; i < 16; i++) //�ʱ�ȭ ���� //�ǵ��� ����
		{
			Img->Huff_ch[i] = 10000;
			Img->Huff_freq[i] = 0;
			Img->Huff_codes[i] = 0;
			Img->Huff_length[i] = 0;
		}
		Img->Huff_Size = 0; //�ʱ�ȭ ���� //�ǵ��� ����

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Img->Huff_Size ��������
		Int32 PrefixCnt = 0;
		while ((Bitstream & 0x80) == 0)
		{
			PrefixCnt++;
			Bitstream <<= 1;
			BitCnt++;
			if (BitCnt == 8)
			{
				fread(&Bitstream, sizeof(UInt8), 1, dp);
				BitCnt = 0;
			}
		}
		Bitstream <<= 1;
		BitCnt++;
		if (BitCnt == 8)
		{
			BitCnt = 0;
			fread(&Bitstream, sizeof(UInt8), 1, dp);
		}

		tmp = 0;
		for (Int32 i = PrefixCnt; i > 0; i--)
		{
			tmp <<= 1;
			if ((Bitstream & 0x80) == 0x80)
			{
				tmp |= 0x01;
			}
			Bitstream <<= 1;
			BitCnt++;
			if (BitCnt == 8)
			{
				BitCnt = 0;
				fread(&Bitstream, sizeof(UInt8), 1, dp);
			}
		}
		CurInfo = (UInt8)pow(2., (double)PrefixCnt) + tmp - 1;
		Img->Huff_Size = CurInfo;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Img->Huff_ch, Img->Huff_freq ��������
		for (Int32 i = 0; i < Img->Huff_Size; i++)
		{
			PrefixCnt = 0;
			while ((Bitstream & 0x80) == 0)
			{
				Bitstream <<= 1;
				BitCnt++;
				PrefixCnt++;
				if (BitCnt == 8)
				{
					BitCnt = 0;
					fread(&Bitstream, sizeof(UInt8), 1, dp);
				}
			}
			Bitstream <<= 1;
			BitCnt++;
			if (BitCnt == 8)
			{
				BitCnt = 0;
				fread(&Bitstream, sizeof(UInt8), 1, dp);
			}

			tmp = 0;
			for (Int32 j = PrefixCnt; j > 0; j--)
			{
				tmp <<= 1;
				if ((Bitstream & 0x80) == 0x80)
				{
					tmp |= 0x01;
				}
				Bitstream <<= 1;
				BitCnt++;
				if (BitCnt == 8)
				{
					BitCnt = 0;
					fread(&Bitstream, sizeof(UInt8), 1, dp);
				}
			}
			CurInfo = (UInt8)pow(2., (double)PrefixCnt) + tmp - 1;
			Img->Huff_ch[i] = CurInfo;

			PrefixCnt = 0;
			while ((Bitstream & 0x80) == 0)
			{
				Bitstream <<= 1;
				BitCnt++;
				PrefixCnt++;
				if (BitCnt == 8)
				{
					BitCnt = 0;
					fread(&Bitstream, sizeof(UInt8), 1, dp);
				}
			}
			Bitstream <<= 1;
			BitCnt++;
			if (BitCnt == 8)
			{
				BitCnt = 0;
				fread(&Bitstream, sizeof(UInt8), 1, dp);
			}

			tmp = 0;
			for (Int32 j = PrefixCnt; j > 0; j--)
			{
				tmp <<= 1;
				if ((Bitstream & 0x80) == 0x80)
				{
					tmp |= 0x01;
				}
				Bitstream <<= 1;
				BitCnt++;
				if (BitCnt == 8)
				{
					BitCnt = 0;
					fread(&Bitstream, sizeof(UInt8), 1, dp);
				}
			}
			CurInfo = (UInt8)pow(2., (double)PrefixCnt) + tmp - 1;
			Img->Huff_freq[i] = CurInfo;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		Huffman(Img);

		//����ȭ ȭ�Ұ� ����
		for (Int32 i = 0; i < BLK * BLK; i++)
		{
			Int32 flag = 0, length = 0;
			tmp = 0;
			while (flag == 0) //flag�� 1�� �Ǳ� ������ ��� ���ư�
			{
				tmp <<= 1;
				if ((Bitstream & 0x80) == 0x80)
					tmp |= 0x01;
				Bitstream <<= 1;
				BitCnt++;
				length++;
				if (BitCnt == 8)
				{
					Bitstream = 0;
					BitCnt = 0;
					fread(&Bitstream, sizeof(UInt8), 1, dp);
				}

				for (Int32 j = 0; j < Img->Huff_Size; j++)
				{
					if (tmp == Img->Huff_codes[j] && length == Img->Huff_length[j])
					{
						flag = 1;
						Img->Quant_blk[i] = Img->Huff_ch[j];
						break;
					}
				}
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////+,- ��ȣ
		for (Int32 i = 0; i < BLK * BLK; i++)
		{
			if ((Bitstream & 0x80) == 0x80)
			{
				Img->Quant_blk[i] = -Img->Quant_blk[i];
				Bitstream <<= 1;
				BitCnt++;
				if ((BitCnt == 8) && (i != (BLK * BLK - 1)))
				{
					BitCnt = 0;
					fread(&Bitstream, sizeof(UInt8), 1, dp);
				}
			}
			else
			{
				Bitstream <<= 1;
				BitCnt++;
				if ((BitCnt == 8) && (i != (BLK * BLK - 1)))
				{
					BitCnt = 0;
					fread(&Bitstream, sizeof(UInt8), 1, dp);
				}
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////����ȭ ��� ���� ��
}
#endif