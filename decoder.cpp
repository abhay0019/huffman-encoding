/**************************************************************************************************************************************

						Copywrite@ABHAYSINGH 
			   			   	(^_^)


***************************************************************************************************************************************/



#include <iostream>
#include <unordered_map>
#include <string.h>
using namespace std;

#define FILE_NAME_SIZE 1000
 
unordered_map < string, char> codes;
unordered_map < string, char> :: iterator it1;
int last_line = 0;
char file[FILE_NAME_SIZE];
char decodedFile[FILE_NAME_SIZE];
void printBIT(char byte)
{
	for(int i=0;i<8;i++)
	{
		printf("%d",!!(byte & (1<<i)));
	}
	printf("\n");
	return;
}
void readHeader(FILE *fp)
{
	char ch;	
	fread(&ch, sizeof(char), 1, fp);
	last_line = (int)ch;
	if(last_line == 0)last_line = 8;
	char size, code;
	
	fread(&ch, sizeof(char), 1, fp);
	int N =(int)ch;
	
	printf("\t\tNUMBER OF DIFFERENT CHARATCTERS : %d\n\n",N);
	printf("\t\tLAST BYTE SIZE : %d\n\n",last_line);
	for(int i=0;i<N;i++)
	{
		fread(&ch, sizeof(char), 1, fp);
		fread(&size, sizeof(char), 1, fp);
		int l = size/8;
		string encode="";
		//printf("Encode size %d\n",size);
		for(int j=0; j<l;j++)
		{
			fread(&code, sizeof(char), 1, fp);
			//printf("read : ");
			//printBIT(code);
			for(int i=0;i<8;i++)
			{
				if( 1 & (code>>i) )
				{
					encode += '1';
				}
				else
				{
					encode += '0';
				}
			}
		}
		if(size%8 != 0)
		{
			fread(&code, sizeof(char), 1, fp);
			//printf("read : ");
			//printBIT(code);
			for(int i=0;i<(int)(size%8);i++)
			{
				if( 1 & (code>>i) )
				{
					encode += '1';
				}
				else
				{
					encode += '0';
				}
			}
		}
		//cout << "Encode : "<< encode << endl;
		codes[encode] = ch;
	}
	return;
}
string encoded ="";
string read = "";
void readDecoded(char ch, FILE *fp, FILE *f, int last)
{ 
	int bit;
		
	if(last == 0)
	{
		for(int i=0;i<8;i++)
		{
			bit = !!(ch & (1<<(7-i)));
			encoded += (bit?'1':'0');
			read += (bit?'1':'0');
			if( codes.find(encoded) != codes.end() )
			{
				fwrite(&codes[encoded], sizeof(char), 1, f);
				encoded = "";
			}
		}
	}
	else
	{
		for(int i=0;i<last_line;i++)
		{
			bit = !!(ch & (1<<(7-i)));
			encoded += (bit?'1':'0');
			read += (bit?'1':'0');
			if( codes.find(encoded) != codes.end() )
			{
				fwrite(&codes[encoded], sizeof(char), 1, f);
				encoded = "";
			}
		}
	}
	return;
}
void usage()
{
	printf("\n\t\tusage: ./decode [fileToDecode] [DecodedFileName]\n\n");
	return;
}
void printEncoding()
{
	FILE *fp =fopen("1.txt","w");
	it1 = codes.begin();
	while(it1 != codes.end())
	{
		fprintf(fp,"%c%s\n",it1->second,it1->first.c_str());
		cout << it1->second << " --> "<< it1->first <<endl;
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
		strcpy( decodedFile, strcat(argv[1], ".txt") );
	}
	else if	(argc == 3)
	{
		strcpy(file,argv[1]);
		strcpy( decodedFile, argv[2] );		
	}
	else
	{
		usage();
		return 0;
	}
	
	FILE *fp = fopen(file, "rb");	
	
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	FILE *f = fopen(decodedFile,"wb");
	
	printf("\t\tReading Header ...\n\n");
	
	readHeader(fp);
	
	size -= ftell(fp);
	
	char ch,ch1;
	int rd = 0;
	printf("\t\tDecoding Data ...\n\n");
	long long int decompressed = 0;
	int prev = 9;	
	int fl = 0;
	while(fread(&ch,sizeof(char),1, fp)>0)
	{
		decompressed++;
		if(fread(&ch1,sizeof(char),1, fp)<=0)
		{
			readDecoded(ch, fp, f, 1);				//LAST LINE
		}
		else
		{
			readDecoded(ch, fp, f, 0);
			fseek(fp, -1, SEEK_CUR);
		}
		int per = ((float)decompressed/size)*100;		
		if( per > prev )
		{
			if(per >= 100)fl=1;		
			printf("\t\t%d %%  decompressed ..\n",per);
			prev = (per/10+1)*10;
		}
		
	}
	if(fl == 0)printf("\t\t%d %%  compressed ..\n",100);
	fclose(f);
	fclose(fp);
	//printEncoding();
	return 0;
}
