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

//생성 함수
HeapType* create()
{
	return (HeapType*)malloc(sizeof(HeapType));
}

//초기화 함수
void init(HeapType* h)
{
	h->heap_size = 0;
}

//현재 요소의 개수가 heap_size인 히프 h에 item을 삽입한다.
//삽입 함수
void insert_min_heap(HeapType* h, element item)
{
	int i;
	i = ++(h->heap_size);

	//트리를 거슬러 올라가면서 부모 노드와 비교하는 과정
	while ((i != 1) && (item.key < h->heap[i / 2].key)) {
		h->heap[i] = h->heap[i / 2];
		i /= 2;
	}
	h->heap[i] = item; // 새로운 노드 삽입
}

//삭제 함수
element delete_min_heap(HeapType* h)
{
	int parent, child;
	element item, temp;

	item = h->heap[1];
	temp = h->heap[(h->heap_size)--];
	parent = 1;
	child = 2;
	while (child <= h->heap_size) {
		//현재 노드의 자식노드중 더 작은 자식노드를 찾는다.
		if ((child < h->heap_size) && (h->heap[child].key) > h->heap[child + 1].key)
			child++;
		if (temp.key < h->heap[child].key) break;

		//한 단계 아래로 이동
		h->heap[parent] = h->heap[child];
		parent = child;
		child *= 2;
	}
	h->heap[parent] = temp;
	return item;
}

//이진 트리 생성 함수
TreeNode* make_tree(TreeNode* left, TreeNode* right) {
	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	node->left = left;
	node->right = right;
	return node;
}

//이진 트리 제거 함수
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
	//1을 저장하고 순환호출한다.
	if (root->left)
	{
		codes[top] = 1;
		print_codes(root->left, codes, top + 1, Img);
	}
	//0을 저장하고 순환호출한다.
	if (root->left)
	{
		codes[top] = 0;
		print_codes(root->right, codes, top + 1, Img);
	}
	//단말노드이면 코드를 출력한다.
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

//허프만 코드 생성 함수
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
		//최소값을 가지는 두 개의 노드를 삭제
		e1 = delete_min_heap(Set);
		e2 = delete_min_heap(Set);

		//두개의 노드를 합친다. 
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
	Int32 ALLZeroFlag = 0; //1인경우 Img->Quant_blk(양자화된 변환 블록 내 값)이 모두 0이 들어가 있음
						   //0인경우 Img->Quant_blk(양자화된 변환 블록 내 값) 중 0이 아닌 값이 들어가 있음
	UInt8 Bitstream = 0;
	UInt8 BitCnt = 0;
	UInt8 tmp = 0;

	UInt8 CurInfo = 0;

	fread(&Bitstream, sizeof(UInt8), 1, dp);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////예측모드 복원 시작
	////
	
	////예측모드 저장
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
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////예측모드 복원 끝
	if ((Bitstream & 0x80) != 0)
	{
		ALLZeroFlag = 1; //모두 0인 경우
		Bitstream <<= 1;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////양자화 블록 복원 시작
	////ALLZeroFlag 복원
	
	if (ALLZeroFlag) //모두 0인 경우
	{
		for (Int32 i = 0; i < BLK * BLK; i++)
			Img->Quant_blk[i] = 0;
	}
	else //0이 아닌 화소가 있는 경우
	{
		for (Int32 i = 0; i < 16; i++) //초기화 과정 //건들지 말것
		{
			Img->Huff_ch[i] = 10000;
			Img->Huff_freq[i] = 0;
			Img->Huff_codes[i] = 0;
			Img->Huff_length[i] = 0;
		}
		Img->Huff_Size = 0; //초기화 과정 //건들지 말것

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Img->Huff_Size 복원시작
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
		//Img->Huff_ch, Img->Huff_freq 복원시작
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

		//양자화 화소값 복원
		for (Int32 i = 0; i < BLK * BLK; i++)
		{
			Int32 flag = 0, length = 0;
			tmp = 0;
			while (flag == 0) //flag가 1이 되기 전까지 계속 돌아감
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
		////+,- 부호
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
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////양자화 블록 복원 끝
}
#endif