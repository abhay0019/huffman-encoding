/**************************************************************************************************************************************

						Copywrite@ABHAYSINGH 
			   			   	(^_^)


***************************************************************************************************************************************/

#include <iostream>
#include <unordered_map>
#include <queue> 
#include <string.h> 
#include <stdio.h>
using namespace std;

#define FILE_NAME_SIZE 1000  
#define MAX_BIT 100010
#define BIT_SIZE(SIZE) (SIZE/8+!!(SIZE%8))
typedef pair<long long int, char> ic;

struct HuffNode{
	char ch;
	long long int freq;
	struct HuffNode *left;
	struct HuffNode *right;
};
struct comp
{
	bool operator()(const HuffNode *a, const HuffNode *b)
	{
		return a->freq > b->freq;
	}
};

class Huffman{
	
	public:
		HuffNode *buildTree();
		HuffNode *newNode(int, char);
};

char  file[FILE_NAME_SIZE];
char  compressedFile[FILE_NAME_SIZE];

char byte = 0;
int cnt = 0;
unordered_map <char, long long int> frequency;
unordered_map <char, long long int> :: iterator it;
unordered_map <char, string>  codes;
unordered_map <char, string>  :: iterator it1;
priority_queue < HuffNode *, vector<HuffNode *>, comp>Q;

char code[MAX_BIT];

HuffNode * Huffman::newNode(int freq, char c)
{
	HuffNode * temp = (struct HuffNode *)malloc(sizeof(HuffNode));
	temp->left = temp->right = NULL;
	temp->ch = c;
	temp->freq = freq;
	return temp;
}

HuffNode* Huffman::buildTree()
{
	HuffNode *nw,*temp,*temp1;
	while(Q.size()>1)
	{
		temp = Q.top();
		Q.pop();
		temp1 = Q.top();
		Q.pop();
		
		nw = newNode(temp->freq + temp1->freq,'@');
		nw->left = temp;
		nw ->right = temp1;
		Q.push( nw );
	}
	return Q.top();
}
void storeCodes(HuffNode *ROOT, char code[], int i)
{
	if(ROOT ==  NULL)
	{
		return;
	}
	if(ROOT->left ==  NULL && ROOT->right ==  NULL)
	{
		code[i]='\0';
		codes[ROOT->ch] = string(code);
		return;
	}
	code[i] = '0';
	storeCodes(ROOT->left, code, i+1);
	code[i] = '1';
	storeCodes(ROOT->right, code, i+1);	
	return;
}
string write = "";
void writeBIT(int b, FILE *fp)
{
	int temp = 1;
	
	if(b == 1)
	{
		write+='1';
		temp=(1<<(7-cnt));
		byte |= temp;
	}
	else
	{
		write+='0';
	}
	cnt++;
	if(cnt == 8)
	{
		fwrite(&byte, sizeof(char), 1, fp);
		cnt = 0;
		byte = 0;
	}
	return;
}

void writeEncode(char c, FILE *fp)
{
	string encode = codes[c];
	
	for(int i=0;i<encode.size();i++)
	{
		if(encode[i] == '0')
		{
			writeBIT(0,fp);	
		}
		else
		{
			writeBIT(1,fp);
		}
	}
	return;
}
void setBIT(char BITARRAY[],int i)
{
	BITARRAY[i/8] |= (1 << (i%8));
	return;
}
void unsetBIT(char BITARRAY[],int i)
{
	BITARRAY[i/8] &= ~(1 << (i%8));
	return;
}
int getBIT(char BITARRAY[],int i)
{
	return 1 & (BITARRAY[i/8] >> ( (i%8) ) );
}
void printBIT(char byte)
{
	for(int i=0;i<8;i++)
	{
		printf("%d",!!(byte & (1<<i)));
	}
	printf("\n");
	return;
}
void writeHeader(FILE *fp, int new_size)
{
	char last_bytes = (char)(new_size%8);
	
	fwrite(&last_bytes, sizeof(char), 1, fp);
	
	char N = (char)codes.size();
	
	it1 = codes.begin();
	
	if( N == 256)
	{
		N = 0;
	}
	fwrite(&N, sizeof(char), 1, fp);
	printf("NUMBER OF CHARACTER %d\n ",N);
	while(it1!=codes.end())
	{
		char ch = it1->first;
		
		fwrite(&ch, sizeof(char), 1, fp);
		
		string encode = it1->second;
		
		char BITARRAY[BIT_SIZE(encode.size())];
		
		memset(BITARRAY, sizeof(BITARRAY), 0);
		
		char encode_size = (char)encode.size();
		
		fwrite(&encode_size, sizeof(char), 1, fp);
		for(int i=0;i<encode.size();i++)
		{
			unsetBIT(BITARRAY,i);
		}
		for(int i=0;i<encode.size();i++)
		{
			if(encode[i] == '1')
			{
				setBIT(BITARRAY, i);
			}
		}
		it1++;
		char byte1 = 0; 
		int count = 0;
		//cout << "Encode Size : "<< encode.size() << endl;
		//cout << "Encode : "<< encode << endl;
		for(int i=0;i<encode.size();i++)
		{
			if(count == 8 )
			{
				//cout << "write : ";
				//printBIT(byte1);
				fwrite(&byte1, sizeof(char), 1, fp);
				count = 0;
				byte1 = 0;
			}
			if(getBIT(BITARRAY, i) == 1)
			{
				byte1 |= (1 << (i%8));	
			}
			count++;
		}
		if(count != 0 )
		{
			//cout << "write : ";
			//printBIT(byte1);
			fwrite(&byte1, sizeof(char), 1, fp);
		}
	}
	return;
}
void usage()
{
	printf("\n\t\tusage: ./encode [fileToEncode] [encodedFileName]\n\n");
	return;
}
void printEncoding()
{		
	FILE *fp =fopen("2.txt","w");
	it1 = codes.begin();
	while(it1 != codes.end())
	{
		fprintf(fp,"%c%s\n",it1->first,it1->second.c_str());
		cout << it1->first << " --> "<< it1->second <<endl;
		it1++;
	}
	fclose(fp);
	return;
}
int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		usage();
		return 0;
	}
	else if(argc == 2)
	{
		strcpy(file,argv[1]);
		strcpy( compressedFile, strcat(argv[1], ".huff") );
	}
	else if	(argc == 3)
	{
		strcpy(file,argv[1]);
		strcpy( compressedFile, argv[2] );		
	}
	else
	{
		usage();
		return 0;
	}
	FILE *fp ;	
	char ch;
	fp = fopen(file, "rb");
	printf("\t\tReading File ....\n\n");
	while(fscanf(fp,"%c", &ch)>0)
	{
		if( frequency.find(ch) == frequency.end() )
		{
			frequency[ch] = 1;
		}
		else
		{
			frequency[ch]++;
		}
	}
	fclose(fp);
	it = frequency.begin();
	HuffNode * temp;
	Huffman obj;
	long long int ini =0, nw = 0;
	
	printf("\t\tBuilding Huffman Tree ....\n\n");
	
	while(it != frequency.end())
	{
		long long int freq = it->second;
		char c = it->first;
		HuffNode * temp = obj.newNode(freq, c);
		Q.push( temp );
		it++;
		ini += freq*8;
	}
	
	HuffNode *ROOT = obj.buildTree();
	
	storeCodes(ROOT, code, 0);
	
	it1 = codes.begin();
	
	while(it1 != codes.end())
	{
		nw += frequency[it1->first]*it1->second.size();
		it1++;
	}
	
	FILE *fp1 = fopen(compressedFile, "wb"); 
	printf("\t\tCompression Started ...\n\n");
	
	printf("\t\tWriting Header ....\n\n");
	
	writeHeader(fp1, nw);
	
	fp = fopen(file, "rb");
	
	printf("\t\tCompressing Data ....\n\n");
	
	long long int compressed = 0;
	int prev = 9;
	ini = ini / 8;
	int fl = 0;
	while(fscanf(fp,"%c", &ch)>0)
	{
		compressed++;
		writeEncode(ch, fp1);
		int per = ((float)compressed/ini)*100;
		if( per > prev )
		{
			if(per >= 100)fl=1;
			printf("\t\t%d %%  compressed ..\n",per);
			prev = (per/10 + 1)*10;
		}
	}
	if(byte!=0)
	{
		fwrite(&byte, sizeof(char), 1, fp1);
	}
	if(fl == 0)
		printf("\t\t%d %%  compressed ..\n",100);
	fclose(fp);
	fclose(fp1);
	//printEncoding();
	return 0;
}
