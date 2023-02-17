#include<iostream>
#include<fstream>
#include <conio.h>
#include <string>
#include <bitset>
#include "Compressor.h"
using namespace std;

void randomFileGenerator(int bytes, string fname)
{
	srand(time(0));
	ofstream wrt(fname);
	int newline = 25 + (rand() % (35 - 25 + 1));
	for (int i = 0; i < bytes; i++)
	{
		int c = 48 + (rand() % (120 - 48 + 1));
		wrt << (char)c;
		if (i % newline == 0)
			wrt << endl, newline = 25 + rand() % 35;
	}
}


int main()
{
	string filename;
	char choice;
	cout << "Enter Filename: ";
	cin >> filename;

	cout << "Do you want to generate the file?(Y)";
	choice = _getch();
	if (choice == 'Y' || choice == 'y')
	{
		int size;
		cout << "Enter Size For File In KiloBytes: ";
		cin >> size;
		randomFileGenerator(size * 1000, filename);
	}
	system("cls");
	compressor testComp;
	testComp.setInputFile(filename);
	testComp.Compress();
	cout << "File Compressed....\n";
	system("pause");
	testComp.DeCompress();
	cout << "File Decompressed....\n";
	system("pause");

}