// Tema4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

typedef struct node
{
	unsigned int weight;
	unsigned char symb;
	node *left;
	node *right;
	node *parent;
	int index;
};

void InitTree();
void Archive();
void Extract();
void AddToTree(char s);
void UpdateTree(int index);
void UpdateVector(int leftNodeIndex, int rightNodeIndex);
void UpdateCodes(node *nNode, string code);
void UpdateIndexes(node *nNode);

void RemoveWeight(node *nRem);
void UpdateWeight(node *nAdd);

string aSymbs[256];
int aIndex[516];

node *nTree;	// quick reference to root
node *nESC;		// reference to the ESC node	
node *nEOS;		// reference to the EOS node

string sCodeESC;
string sCodeEOS;

// vector of nodes
vector<node*> vNodes; // holds in descending order references to the nodes;  

// our tree
node aTree[516];	// the array of nodes
int iIndex;			// where the next node is to be inserted

int initialSize;
int finalSize = 0;

//folder path
string fFolder = "C:\\Users\\Marius\\Desktop\\Semestrul I\\PS\\Tema4\\pack\\";
string fFile = "text.txt";				// original file
string fArchive = "text.small";			// archived file name
string fExtracted = "text_x.txt";		// extracted file name

//string fFile = "speech_01.wav";		// original file
//string fArchive = "speech_01.small";	// archived file name
//string fExtracted = "speech_01_x.wav";	// extracted file name

//string fFile = "sound_01.wav";		// original file
//string fArchive = "sound_01.small";	// archived file name
//string fExtracted = "sound_01_x.wav";	// extracted file name

//string fFile = "Y04.M";		// original file
//string fArchive = "Y04.small";	// archived file name
//string fExtracted = "Y04_x.M";	// extracted file name

int _tmain(int argc, _TCHAR* argv[])
{
	InitTree();
	Archive();
	Extract();

	getchar();
	return 0;
}

void InitTree()
{
	iIndex = 0;
	vNodes.clear();
	node nNode;

	// create the level0 tree
	nNode.weight = 2;
	nNode.parent = NULL;
	aTree[iIndex++] = nNode;
	vNodes.push_back(&aTree[0]); // root
	vNodes[0]->index = 0;
	nTree = vNodes[0];

	nNode.weight = 1;
	nNode.left = NULL;
	nNode.right = NULL;
	nNode.parent = nTree;
	aTree[iIndex++] = nNode;
	vNodes.push_back(&aTree[1]); // EOS
	nEOS = vNodes[1];
	nEOS->index = 1;

	aTree[iIndex++] = nNode;
	vNodes.push_back(&aTree[2]); // ESC
	nESC = vNodes[2];
	nESC->index = 2;

	nTree->left = nEOS;
	nTree->right = nESC;

	sCodeEOS = "0";
	sCodeESC = "1";
}

void Archive()
{
	time_t start, end;
	start = time(NULL);

	// open the channel for input (a file, in this case)
	int i;

	FILE *fRead = fopen((fFolder + fFile).c_str() , "rb");
	if (fRead != NULL)
	{
		cout << "Archiving started: " << (fFolder + fFile) << endl;
		
		// create the output file
		FILE *fWrite = fopen((fFolder + fArchive).c_str() , "wb" );
		
		// get input file size
		fseek (fRead , 0 , SEEK_END);
		i = ftell (fRead);
		initialSize = i;
		rewind (fRead);
		
		unsigned char r, w;
		const char *cpSymb;
		int dim = 0;
		r = 0;
		w = 0;
		int nrBits = 0;
		int mask = 128;
		
		while (i > 0)
		{
			fread(&r, 1, 1, fRead); // go through each byte
			i--;
			
			dim = aSymbs[r].size();
			if (dim > 0)
			{
				// write to output the old code for the char
				cpSymb = aSymbs[r].c_str();
				
				while ((*cpSymb) != '\0')
				{
					w *= 2;
					if((*cpSymb) % 2 != 0)
					{
						w++;
					}

					cpSymb++;
					nrBits++;
					if (nrBits == 8)
					{
						fwrite (&w , 1 , sizeof(char) , fWrite );
						w = 0;
						nrBits = 0;
						finalSize++; // contorize the size of the archive
					}
				}

				// increase weight of existing symbol c
				vNodes[aIndex[r]]->weight++;
				UpdateTree(aIndex[r]);
				// update the values of mSymbs
				string prefix;
				UpdateCodes(nTree, prefix);
			}
			else
			{
				// create a new symbol
				char s = r;

				// write to output the ESC char
				int j = 0;
				while (j < sCodeESC.size())
				{
					w *= 2;
					if(sCodeESC[j] == '1')
					{
						w++;
					}

					j++;
					nrBits++;
					if (nrBits == 8)
					{
						fwrite (&w , 1 , sizeof(char) , fWrite );
						finalSize++; // contorize the size of the archive
						w = 0;
						nrBits = 0;
					}
				}
				// write to output ASCII code of char
				j = 0;
				while (j < 8)
				{
					w *= 2;
					if((r & mask) == mask)
					{
						w++;
					}
					r *= 2;

					j++;
					nrBits++;
					if (nrBits == 8)
					{
						fwrite (&w , 1 , sizeof(char) , fWrite );
						finalSize++; // contorize the size of the archive
						w = 0;
						nrBits = 0;
					}
				}

				// add to tree; 
				AddToTree(s);
				
				// update the values of aSymbs
				string prefix;
				UpdateCodes(nTree, prefix);
			}
		}
		// write the EOS char
		int j = 0;
		while (j < sCodeEOS.size())
		{
			w *= 2;
			if(sCodeEOS[j] == '1')
			{
				w++;
			}

			j++;
			nrBits++;
			if (nrBits == 8)
			{
				fwrite (&w , 1 , sizeof(char) , fWrite );
				w = 0;
				nrBits = 0;
				finalSize++; // contorize the size of the archive
			}
		}
		if (nrBits < 8) // compress the last byte
		{
			unsigned char unusedBits = 8 - nrBits; // the number of unused bits on the last byte
			while (nrBits < 8)
			{
				w *= 2;
				nrBits++;
			}
			fwrite (&w , 1 , sizeof(char) , fWrite );
			finalSize++; // contorize the size of the archive
		}

		fclose(fRead);
		fclose(fWrite);
	}
	end = time(NULL);
	int min;
	int sec;
	string sOut;
	min = ((end - start) / 60);
	sec = ((end - start) % 60);
	if (min != 0)
	{
		sOut = to_string(long double(min));
		sOut += " minutes ";
	}
	if (sec >= 0)
	{
		sOut += to_string(long double(sec));
		sOut += " seconds ";
	}
	cout << "Archiving completed" << endl;
	cout << "Archiving duration: " << sOut << endl;
	cout << "Compression factor: " << (finalSize / float(initialSize) ) * 100 << " %" << endl;
}

void AddToTree(char s)
{
	node nNode;
	node *pNode, *pSymb;
	int index = iIndex;
	nNode.weight = 1;
	aTree[iIndex++] = nNode; 
	aTree[iIndex++] = nNode; 
	pNode = &aTree[index++];
	pSymb = &aTree[index];

	// create a node for the new symbol
	pSymb->symb = s;
	pSymb->weight = 1;
	pSymb->left = NULL;
	pSymb->right = NULL;
	pSymb->parent = pNode;

	pNode->parent = nESC->parent;
	pNode->left = pSymb;
	pNode->right = nESC;
	pNode->weight = 2;

	// take ESC's place in the tree
	if (nESC->parent->left == nESC)
	{
		nESC->parent->left = pNode;
	}
	else
	{
		nESC->parent->right = pNode;
	}
	nESC->parent = pNode;

	// add in vNodes too
	vNodes[nESC->index] = pNode;
	pNode->index = nESC->index;
	
	index = vNodes.size();
		
	vNodes.push_back(pSymb);
	pSymb->index = index++;

	vNodes.push_back(nESC);
	nESC->index = index;

	UpdateTree(pNode->index); // check the fraternity of the nodes starting from the latest adition
}

void UpdateTree(int index)
{
	// check the fraternity
	int i = index - 1;
	int actualIndex = index;
	
	while ((vNodes[i]->weight < vNodes[index]->weight) && (i != 0))
	{
		if ((vNodes[i]->left != NULL) && (vNodes[i]->right != NULL))
		{
			// this is a node
			RemoveWeight(vNodes[i]);
			vNodes[index]->weight--; // just until it's removed
			RemoveWeight(vNodes[index]);

			node *auxNode = vNodes[index];
			node *auxParent = auxNode->parent;
			auxNode->index = i;
			vNodes[i]->index = index;

			if (vNodes[index]->parent->left == vNodes[index])
			{
				vNodes[index]->parent->left = vNodes[i];
				vNodes[index]->parent = vNodes[i]->parent;
			}
			else
			{
				vNodes[index]->parent->right = vNodes[i];
				vNodes[index]->parent = vNodes[i]->parent;
			}

			if (vNodes[i]->parent->left == vNodes[i])
			{
				vNodes[i]->parent->left = auxNode;
				vNodes[i]->parent = auxParent;
			}
			else
			{
				vNodes[i]->parent->right = auxNode;
				vNodes[i]->parent = auxParent;
			}
			UpdateWeight(vNodes[i]);
			UpdateWeight(vNodes[index]);
			vNodes[index]->weight++;
			actualIndex = i;
		}
		
		i--;
	}

	i++; 
	if ((i != index) && (actualIndex != i))
	{
		// swap them
		vNodes[i]->parent->weight -= vNodes[i]->weight;
		vNodes[index]->weight--; // just until it's removed
		RemoveWeight(vNodes[index]);
		vNodes[index]->weight++;

		node *auxNode = vNodes[index];
		node *auxParent = auxNode->parent;
		auxNode->index = i;
		vNodes[i]->index = index;

		if (vNodes[index]->parent->left == vNodes[index])
		{
			vNodes[index]->parent->left = vNodes[i];
			vNodes[index]->parent = vNodes[i]->parent;
		}
		else
		{
			vNodes[index]->parent->right = vNodes[i];
			vNodes[index]->parent = vNodes[i]->parent;
		}

		if (vNodes[i]->parent->left == vNodes[i])
		{
			vNodes[i]->parent->left = auxNode;
			vNodes[i]->parent = auxParent;
		}
		else
		{
			vNodes[i]->parent->right = auxNode;
			vNodes[i]->parent = auxParent;
		}

		UpdateWeight(vNodes[i]);
		if (vNodes[index]->parent != nTree)
		{
			vNodes[index]->parent->weight += vNodes[index]->weight;
			UpdateTree(vNodes[index]->parent->index);
		}
		else
		{
			nTree->weight += vNodes[index]->weight;
		}
	}
	else
	{
		if (vNodes[index]->parent != nTree)
		{
			vNodes[index]->parent->weight++;
			UpdateTree(vNodes[index]->parent->index);
		}
		else
		{
			nTree->weight++;
		}
	}
	
	UpdateVector(0, 0);
}

void UpdateVector(int leftNodeIndex, int rightNodeIndex)
{
	bool recursiveCall = false;
	int nextLevelIndex = ++rightNodeIndex;
	int nextLeftNodeIndex = nextLevelIndex;
	int nextRightNodeIndex;
	for(int i = leftNodeIndex; i < rightNodeIndex; i++)
	{
		if (vNodes[i]->left != NULL)
		{
			vNodes[nextLevelIndex] = vNodes[i]->left;
			vNodes[i]->left->index = nextLevelIndex;
			nextLevelIndex++;

			if (recursiveCall == false)
			{
				if ((vNodes[i]->left->left != NULL)  || 
					(vNodes[i]->left->right != NULL))
				{
					recursiveCall = true;
				}
			}

		}
		if (vNodes[i]->right != NULL)
		{
			vNodes[nextLevelIndex] = vNodes[i]->right;
			vNodes[i]->right->index = nextLevelIndex;
			nextLevelIndex++;

			if (recursiveCall == false)
			{
				if ((vNodes[i]->right->left != NULL)  || 
					(vNodes[i]->right->right != NULL))
				{
					recursiveCall = true;
				}
			}

		}
	}
	nextRightNodeIndex = --nextLevelIndex;

	if (recursiveCall == true)
	{
		UpdateVector(nextLeftNodeIndex, nextRightNodeIndex);
	}
}

void RemoveWeight(node *nRem)
{
	int w = nRem->weight;
	while (nRem->parent != NULL)
	{
		nRem->parent->weight -= w;
		nRem = nRem->parent;
	}
}

void UpdateWeight(node *nAdd)
{
	int w = nAdd->weight;
	while (nAdd->parent != NULL)
	{
		nAdd->parent->weight += w;
		nAdd = nAdd->parent;
	}
}

void UpdateCodes(node *nNode, string code)
{
	// start from root, call recursively until leaf is reached and assign code
	bool leaf = true;
	if (nNode->left != NULL)
	{
		leaf = false;
		string lCode = code;
		lCode.push_back('0');
		UpdateCodes(nNode->left, lCode);
	}

	if (nNode->right != NULL)
	{
		leaf = false;
		string rCode = code;
		rCode.push_back('1');
		UpdateCodes(nNode->right, rCode);
	}
	
	if (leaf == true)
	{
		
		if (nNode == nESC)		// check if ESC
		{
			sCodeESC = code;
		}
		else if (nNode == nEOS) // check if EOS
		{
			sCodeEOS = code;
		}
		else					// it's normal char
		{
			aSymbs[nNode->symb] = code;
			aIndex[nNode->symb] = nNode->index;
		}
	}
}

void UpdateIndexes(node *nNode)
{
	// start from root, call recursively until leaf is reached and assign index
	bool leaf = true;
	if (nNode->left != NULL)
	{
		leaf = false;
		UpdateIndexes(nNode->left);
	}

	if (nNode->right != NULL)
	{
		leaf = false;
		UpdateIndexes(nNode->right);
	}
	
	if (leaf == true)
	{
		
		if (nNode == nESC)		// check if ESC
		{
			return;
		}
		else if (nNode == nEOS) // check if EOS
		{
			return;
		}
		else					// it's normal char
		{
			aIndex[nNode->symb] = nNode->index;
		}
	}
}

void Extract()
{
	time_t start, end;
	start = time(NULL);

	// get the compressed input from channel (a file, in this case)	
	string sPath = fFolder;
	FILE *fRead = fopen((sPath + fArchive).c_str() , "rb");
	FILE *fWrite = fopen((sPath + fExtracted).c_str(), "wb");
	if (fRead != NULL)
	{
		cout << endl << "Extraction started: " << (sPath+fArchive) << endl;

		// create the level0 tree
		InitTree();

		node *nCurrent;
		char r, w;
		w = 0;
		int nrBits = 0;
		int mask = 128;
		fread(&r, 1, 1, fRead); // go through each byte
		
		nCurrent = nTree;
		if ((r & mask) == mask)
		{
			nCurrent = nCurrent->right;
		}
		else
		{
			nCurrent = nCurrent->left;
		}
		r *= 2;
		nrBits++;

		while (nCurrent != nEOS)
		{
			if (nCurrent == nESC)
			{
				nCurrent = nTree; // reset

				// here comes the next char
				int j = 0;
				w = 0;
				while (j < 8)
				{
					w *= 2;
					w += ((r & mask) == mask);
					r *= 2;
					j++;
					nrBits++;
					if (nrBits == 8) // read the next byte
					{
						fread(&r, 1, 1, fRead); // go through each byte
						nrBits = 0;
					}
				}
				// add the char to the tree
				AddToTree(w);
				UpdateIndexes(nTree);
				// write the char to output
				fwrite(&w, 1, sizeof(char), fWrite);
			}
			else if ((nCurrent->left == NULL) && (nCurrent->right == NULL))
			{
				// char; write to ouput
				fwrite(&nCurrent->symb, 1, sizeof(char), fWrite);

				// increase weight of existing symbol c
				vNodes[aIndex[nCurrent->symb]]->weight++;
				UpdateTree(aIndex[nCurrent->symb]);
				UpdateIndexes(nTree);
				nCurrent = nTree; // reset
			}

			// keep reading bits
			if ((r & mask) == mask)
			{
				nCurrent = nCurrent->right;
			}
			else
			{
				nCurrent = nCurrent->left;
			}
			r *= 2;
			nrBits++;
			if (nrBits == 8) // read the next byte
			{
				fread(&r, 1, 1, fRead); // go through each byte
				nrBits = 0;
			}

		}
		cout << "Extraction completed."<<endl;
		fclose(fWrite);
		fclose(fRead);
	}
	end = time(NULL);
	int min, sec;
	string sOut;
	min = ((end - start) / 60);
	sec = ((end - start) % 60);
	if (min != 0)
	{
		sOut = to_string(long double(min));
		sOut += " minutes ";
	}
	if (sec >= 0)
	{
		sOut += to_string(long double(sec));
		sOut += " seconds ";
	}
	cout << "Extraction duration: " << sOut << endl;
}