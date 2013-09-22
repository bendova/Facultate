// Project1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace std;

struct symbol
{
	char value;
	int freq;
};

void OpenFile();
void Order();
void WriteArchive();
void ReadArchive();
void BuildTree(vector<symbol> *v, string prefix);

vector<symbol> symbols;
vector<char> original;
map<char, string> tree;
map<char, string>::iterator itTree;
map<string, char> rTree;

//folder path
string folder = "C:\\Users\\Marius\\Desktop\\Semestrul I\\PS\\Teme\\Tema2\\pack\\";

//string file = "quartz.dll";			// original file
//string archive = "quartz.small";	// archived file name
//string extracted = "quartz_x.dll";	// extracted file name

//string file = "sound_01.wav";			// original file
//string archive = "sound_01.small";	// archived file name
//string extracted = "sound_01_x.wav";	// extracted file name

//string file = "docu.doc";				// original file
//string archive = "docu.small";		// archived file name
//string extracted = "docu_x.doc";		// extracted file name

//string file = "speech_01.wav";			// original file
//string archive = "speech_01.small";		// archived file name
//string extracted = "speech_01_x.wav";	// extracted file name

//string file = "WINZIP32.EXE";			// original file
//string archive = "WINZIP32.small";		// archived file name
//string extracted = "WINZIP32_x.EXE";	// extracted file name

string file = "Y04.M";				// original file
string archive = "Y04.small";		// archived file name
string extracted = "Y04_x.M";		// extracted file name

//string file = "test.txt";				// original file
//string archive = "test.small";		// archived file name
//string extracted = "test2.txt";		// extracted file name

int _tmain(int argc, _TCHAR* argv[])
{
	string prefix;
	//
	OpenFile();
	Order(); // sort the arrays
	BuildTree(&symbols, prefix); // build the binary tree
	WriteArchive();
	ReadArchive();

	getchar();
	
	return 0;
}

void OpenFile()
{
	char c;
	int lSize;
	int i;
	
	string path = folder;
	const char *fileName = (path.append(file)).c_str();
	FILE *f = fopen(fileName, "rb");
	cout << "Reading file: " << path << endl;
	// read the contents of the file
	if (f != NULL)
	{
		bool newSymbol;
		// get file size
		fseek (f , 0 , SEEK_END);
		lSize = ftell (f);
		rewind (f);
		
		i = lSize;
		while (i > 0)
		{
			fread(&c, 1, 1, f); // go through each byte
			i--;

			newSymbol = true;

			for(int j = 0; j < symbols.size(); j++)
			{
				if (symbols[j].value == c)
				{
					symbols[j].freq++;
					newSymbol = false;
					break;
				}
			}
			
			if (newSymbol == true)
			{
				// add the new symbol
				symbol s;
				s.value = c;
				s.freq = 1;
				symbols.push_back(s);
			}
		}
	}

	cout << "File read complete. Nr of symbols: " << symbols.size() << endl;
}

void Order()
{
	int iAux;
	char cAux;
	int n = symbols.size();
	bool swaped =  true;

	while(swaped)
	{
		swaped = false;

		for(int i = 1 ; i < n; i++)
		{
			if (symbols[i-1].freq < symbols[i].freq)
			{
				// swap the values
				iAux = symbols[i-1].freq;
				symbols[i-1].freq = symbols[i].freq;
				symbols[i].freq = iAux;

				cAux = symbols[i-1].value;
				symbols[i-1].value = symbols[i].value;
				symbols[i].value = cAux;
				
				swaped = true;
			}
			else if (symbols[i-1].freq == symbols[i].freq) // ensure that the lexical order is correct
			{
				if (symbols[i-1].value > symbols[i].value)
				{
					cAux = symbols[i-1].value;
					symbols[i-1].value = symbols[i].value;
					symbols[i].value = cAux;
				}
			}
		}
		n--;
	}
}

void BuildTree(vector<symbol> *v, string prefix)
{
	// if the vector has only 1 symbol, add it to the tree
	if (v->size() == 1)
	{
		tree[(*v)[0].value] = prefix;
	}
	else
	{
		// split the vector into 2 vectors of equal weights
		vector<symbol> left;
		vector<symbol> right;
		vector<symbol>::iterator itL = left.begin(); 
		vector<symbol>::iterator itR = right.begin(); 
		int wLeft = 0;
		int wRight = 0;
		int L = 0;
		int R = v->size() - 1;
		while (L <= R)
		{
			left.push_back((*v)[L]);
			wLeft += (*v)[L].freq;
			L++;
			while ((wRight <= wLeft) && (L <= R))
			{
				right.insert(itR, 1, (*v)[R]);
				itR = right.begin(); 
				wRight += (*v)[R].freq;
				R--;
			}
		}

		string prefixL = prefix;
		string prefixR = prefix;
		prefixL.push_back('0');
		prefixR.push_back('1');
		if (left.size() > 0)
		{
			BuildTree(&left, prefixL);
		}
		if (right.size() > 0)
		{
			BuildTree(&right, prefixR);
		}
	}
}

void WriteArchive()
{
	cout << "Archive write started:" << endl;
	FILE * pFile;
	// build the name of the archived file
	string name = "";
	int i = 0;
	int dim = file.size();
	char c = file[i];
	while ((c != '.') && (i < dim))
	{
		name.append(1, c);
		i++;
		c = file[i];
	}
	name.append(".small");

	string pathw = folder;
	// create the file
	pFile = fopen ((pathw.append(name)).c_str() , "wb" );

	// AUXILIARY INFORMATION
	// first write as a full byte the number of distinct symbols used
	c = symbols.size();

	fwrite (&c, 1, sizeof(char), pFile );

	int nrBits = 0;
	c = 0;
	int mask = 128;

	// start writing the symbol[8bits] / NrOfBits [5bits] / new code [NrOfBits bits]
	for(itTree = tree.begin(); itTree != tree.end(); itTree++)
	{
		// write the ASCII code for the symbol
		if (nrBits == 0)
		{
			c = itTree->first;
			fwrite (&c , 1 , sizeof(char) , pFile );
			c = 0;
			nrBits = 0;
		}
		else 
		{
			int j = 0;
			int s = itTree->first;
			while (j < 8)
			{
				c *= 2;
				if ((s & mask) == mask)
				{
					c++;
				}
				s *= 2;

				j++;
				nrBits++;
				if (nrBits == 8 )
				{
					fwrite (&c , 1 , sizeof(char) , pFile );
					c = 0;
					nrBits = 0;
				}
			}
		}

		// write on 5 bits the number of bits on which the symbol is represented
		int j = 0;
		int mask5 = 16;
		int s = (*itTree).second.size();
		while (j < 5)
		{
			c *= 2;
			if ((s & mask5) == mask5)
			{
				c++;
			}
			s *= 2;

			j++;
			nrBits++;
			if (nrBits == 8 )
			{
				fwrite (&c , 1 , sizeof(char) , pFile );
				c = 0;
				nrBits = 0;
			}
		}
		
		// write the new binary representation of the number
		for(int i = 0; i < (*itTree).second.size() ; i++)
		{
			char s = (*itTree).second[i];
			
			c *= 2;
			if (s == '1')
			{
				c++;
			}

			nrBits++;
			if (nrBits == 8)
			{
				fwrite (&c , 1 , sizeof(char) , pFile );
				c = 0;
				nrBits = 0;
			}
		}
		
	}

	int countIters = 0;
	string path = folder;
	int fSize;
	// Start writing the compressed data
	FILE *rFile = fopen((path.append(file)).c_str() , "rb");
	if (rFile != NULL)
	{
		// get file size
		fseek (rFile , 0 , SEEK_END);
		fSize = ftell (rFile);
		rewind (rFile);
		i = fSize;
		int j;
		char or; // original char
		string oc; // compressed char
	
		fread(&or, sizeof(char), 1, rFile); // go through each byte
		while (i > 0)
		{
			if (tree.count(or) != 0)
			{
				oc = tree[or];
			}
			else
			{
				cout << "valeu";
			}
			
			j = 0;
			while (j < oc.size())
			{
				char s = oc[j];
			
				c *= 2;
				if (s == '1')
				{
					c++;
				}

				j++;
				nrBits++;
				if (nrBits == 8)
				{
					fwrite (&c , 1 , sizeof(char) , pFile );
					c = 0;
					nrBits = 0;
					
				}
			}
			fread(&or, sizeof(char), 1, rFile); // go through each byte
			i--;
		}

		if (nrBits == 0)
		{
			unsigned char unusedBits = 0;
			fwrite (&unusedBits , 1 , sizeof(char) , pFile ); // write the number of unused bits in the last byte; this counts as auxiliary information
		}
		else if (nrBits < 8) // compress the last byte
		{
			unsigned char unusedBits = 8 - nrBits; // the number of unused bits on the last byte
			while (nrBits < 8)
			{
				c *= 2;
				nrBits++;
			}
			fwrite (&c , 1 , sizeof(char) , pFile ); // write the last byte
			fwrite (&unusedBits , 1 , sizeof(char) , pFile ); // write the number of unused bits in the last byte; this counts as auxiliary information
			countIters++;
		}
		fclose (rFile);
	}

	fclose (pFile);
	cout << "Archive write complete." << endl;
}

void ReadArchive()
{
	// read the archive and rebuild the map
	FILE *fFile;
	int msk = 128;
	int minCode = 32; // the minimum nr of bits used to represent a symbol
	string path = folder;
	fFile = fopen((path.append(archive)).c_str(), "rb" );

	// read the contents of the file
	if (fFile != NULL)
	{
		cout << "Archive read started:" << endl;
		// get file size
		fseek (fFile , 0 , SEEK_END);
		int tSize = ftell(fFile);
		rewind (fFile);

		int countP;
		char c;
		char sym; // the ascii code of the symbol
		int rep; // the nr of bits used for representation
		string bin; // the binary code used to represent the symbol

		int j = 0;
		int nrBits = 0;
		int i = tSize; // the last byte is the stop byte
		
		if (i > 0)
		{
			// read the first byte
			// it indicates the number of pairs in the map
			fread(&c, 1, 1, fFile);
			nrBits = 0;
			countP = (unsigned char)c;
			if (countP == 0)
			{
				countP = 256;
			}
			i--;
		}

		fread(&c, sizeof(char), 1, fFile);
		i--;
		while (countP > 0) // read each pair
		{
			// read the ascii code
			sym = 0;
			int j = 0;
			while (j < 8)
			{
				sym *= 2;
				if ((c & msk) == msk)
				{
					sym++;
				}
				c *= 2;
				j++;
				nrBits++;
				if (nrBits == 8)
				{
					fread(&c, sizeof(char), 1, fFile);
					i--;
					nrBits = 0;
				}
			} 
			
			// get the nr of bits used for representation			
			rep = 0;
			j = 0;
			while(j < 5)
			{
				rep *= 2;
				if ((c & msk) == msk)
				{
					rep++;
				}
				c *= 2;

				j++;
				nrBits++;
				if (nrBits == 8)
				{
					fread(&c, sizeof(char), 1, fFile);
					i--;
					nrBits = 0;
				}
			}
			if (minCode > rep)
			{
				minCode = rep;
			}

			// get the binary representation
			bin = "";
			j = 0;
			while (j < rep)
			{
				if ((c & msk) == msk)
				{
					bin.push_back('1');
				}
				else
				{
					bin.push_back('0');
				}
				c *= 2;
				j++;
				nrBits++;
				if (nrBits == 8)
				{
					fread(&c, sizeof(char), 1, fFile);
					i--;
					nrBits = 0;
				}
			}

			// set the tree item
			rTree[bin] = sym;
			countP--;
		}

		// create a file to extract to
		string writePath = folder;
		FILE *wFile = fopen((writePath.append(extracted)).c_str(), "wb" );

		int mask = 128;
		string key;
		while(i > 0) 
		{
			while (key.size() < minCode)
			{
				if ((c & mask) == mask)
				{
					key.push_back('1');
				}
				else
				{
					key.push_back('0');
				}
				c *= 2;
				nrBits++;

				if (nrBits == 8)
				{
					nrBits = 0;
					i--;
					fread(&c, sizeof(char), 1, fFile);
					
					if (i == 1)
					{
						break;
					}
				}
			}

			if (i == 1)
			{
				break;
			}

			while (rTree.count(key) == 0)
			{
				if ((c & mask) == mask)
				{
					key.push_back('1');
				}
				else
				{
					key.push_back('0');
				}
				c *= 2;
				nrBits++;

				if (nrBits == 8)
				{
					nrBits = 0;
					i--;
					fread(&c, sizeof(char), 1, fFile);
					
					if (i == 1)
					{
						break;
					}
				}
			}

			if (i == 1)
			{
				break;
			}

			char check = rTree[key];
			
			fwrite (&rTree[key] , 1 , sizeof(char) , wFile );
			key = "";
		}
		
		unsigned char unusedBits;
		fread(&unusedBits, sizeof(char), 1, fFile);

		if (unusedBits > 0) 
		{
			nrBits += unusedBits;
			// write the last char
			while(nrBits < 8)
			{
				while (rTree.count(key) == 0)
				{
					if ((c & mask) == mask)
					{
						key.push_back('1');
					}
					else
					{
						key.push_back('0');
					}
					c *= 2;
					nrBits++;
				}
				fwrite (&rTree[key] , 1 , sizeof(char) , wFile );
				key = "";
			}
		}
		
		fclose (wFile);
	}

	fclose (fFile);
	cout << "File extracted." << endl; 
}

