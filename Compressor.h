#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <string>
#include <bitset>
using namespace std;

class compressor
{
	class node
	{
		char symbol;
		int freq;
		string code;
		node* left;
		node* right;
		friend class compressor;
	public:
		node() : symbol(' '), freq(0), code(""), left(nullptr), right(nullptr) {}
		node(char sym, int fr) : symbol(sym), freq(fr), code(""), left(nullptr), right(nullptr){}
		friend ostream& operator<<(ostream& cout, node* n)
		{
			cout << n->symbol << ": " << n->freq;
			return cout;
		}

	};
	class compare {
		friend class compressor;
		friend class node;
	public:
		bool operator()(node* a, node* b)
		{
			return (a->freq > b->freq);
		}
	};

	node* root;
	int charFreq[128]{};
	string codes[128]{};
	priority_queue<node*, vector<node*>, compare> pq;
	string bitString;
	string inFileName;
	string outFileName;

	void codesRecursive(node* root, string code)
	{
		if (root->left == NULL && root->right == NULL)
		{
			root->code = code;
			codes[int(root->symbol)] = code;
		}
		else
		{
			codesRecursive(root->left, code + '0');
			codesRecursive(root->right, code + '1');
		}
	}
	string removeExtension(string fname)
	{
		int newsize;
		for (int i = fname.size() - 1 ; i >= 0; i--)
		{
			if (fname[i] == '.')
			{
				newsize = i;
				break;
			}
		}
		fname.resize(newsize);
		return fname;
	}
	unsigned char bitsetToChar(bitset<8> bitIn)
	{
		int ret = bitIn.to_ulong();
		return (unsigned char)ret;
	}
	string charToBitString (int in)
	{
		string temp = "";
		string result = "";
		while (in)
		{
			temp += ('0' + in % 2);
			in /= 2;
		}
		result.append(8 - temp.size(), '0');
		for (int i = temp.size()-1; i >= 0; i--)
		{
			result += temp[i];
		}
		return result;
	}

public:

	void Compress()
	{
		readFrequencies();
		createPQ();
		createTree();
		createCodes();
		createBitString();
		encode();
	}
	void DeCompress()
	{

		decode();
		createDecoded();
	}

	void setInputFile(string fname)
	{
		inFileName = fname;
	}
	void readFrequencies()
	{
		ifstream rdr(inFileName);
		int code;
		while (!rdr.eof())
		{
			code = (int)rdr.get();
			if (rdr)
				charFreq[code]++;
		}
		rdr.close();
	}
	void createPQ()
	{
		node* nptr = nullptr;
		for (int i = 0; i < 127; i++)
		{
			if (charFreq[i] != 0)
			{
				nptr = new node(i, charFreq[i]);
				pq.push(nptr);
			}
		}
	}
	void createTree()
	{
		priority_queue<node*, vector<node*>, compare> tempPQ(pq);
		while (tempPQ.size() > 1)
		{
			node* l = tempPQ.top();
			tempPQ.pop();
			node* r = tempPQ.top();
			tempPQ.pop();

			root = new node(0, r->freq + l->freq);
			root->left = l;
			root->right = r;

			tempPQ.push(root);
		}
		
	}
	void createCodes()
	{
		string code = "";
		codesRecursive(root, code);
	}
	void createBitString()
	{
		ifstream rdr(inFileName);
		bitString.clear();
		int code;
		while (!rdr.eof())
		{
			code = (int)rdr.get();
			if (rdr)
				bitString += codes[code];
		}
		while (bitString.size() % 8 != 0)
			bitString += '0';

		rdr.close();
	}
	void encode()
	{
		outFileName = removeExtension(inFileName) + ".cmp";
		ofstream wrt(outFileName, ios::binary);

		bitset<8> oneSet;
		oneSet.set();
		int j = 7;
		string out = "";
		for (int i = 0; i < bitString.size(); i++)
		{
			oneSet.set(j, (int)(bitString[i] - '0'));
			j--;
			if (j == 0)
			{
				j = 7;
				unsigned char c = bitsetToChar(oneSet);
				out += c;
				oneSet.set();
			}
		}
		wrt.write(out.c_str(), out.size());
		wrt.close();
	}
	void decode()
	{
		ifstream rdr(outFileName, ios::binary);
		bitString.clear();

		unsigned char textseg;
		while (!rdr.eof())
		{
			rdr.read(reinterpret_cast<char*>(&textseg), 1);
			if (rdr.eof())
				break;
			bitString += charToBitString(textseg);
		}
		rdr.close();
	}
	void createDecoded()
	{
		ofstream wrt("[output]" + removeExtension(outFileName) + ".txt");
		node* itr = root;
		for (int i = 0; i < bitString.size(); i++)
		{
			if (bitString[i] == '0')
				itr = itr->left;
			else
				itr = itr->right;

			if (!itr->left && !itr->right)
			{
				wrt << itr->symbol;
				itr = root;
			}
		}
	}
};