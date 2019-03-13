// paralel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

#define SIZE 2048

using namespace std;

void pthread_row_matrix_multiplication();

class Complex
{
public:
	void output()
	{
		cout <<  r << "+" << i << "i,\t";
	}
	void input()
	{
		cout << "Enter the real part of the complex number: ";
		cin >> r;
		cout << "Enter the imaginary part of the complex number: ";
		cin >> i;
	}
	
	

	void subtract(Complex n1, Complex n2)
	{
		r = n1.r - n2.r;
		i = n1.i - n2.i;
	}
	

	double r;
	double i;

};

Complex add(Complex n1, Complex n2)
{
	Complex z;
	z.r = n1.r + n2.r;
	z.i = n1.i + n2.i;
	return z;
}


Complex multiply(Complex n1, Complex n2)
{
	Complex z;
	z.r = n1.r * n2.r - n1.i * n2.i;
	z.i = n1.r * n2.i + n1.i * n2.r;
	return z;
}

void * matrix_row_multiply(void *);

#define THREAD_COUNT 4
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int S2 = SIZE * SIZE;
int **matrix;
//int *vektor;
//int *sonuc;
int *sayilar;
int uzunluk;
int *sayilarVektor;

volatile int str = 0;
string tempMatrix[SIZE][SIZE];
string tempVektor[SIZE];
Complex matris[SIZE][SIZE];
Complex vektor[SIZE];
Complex sonuc[SIZE];
Complex toplam;


void matris_oku()
{

	int satir = 0, sutun = 0, indis = 0, indis_e = 0;
	ifstream m;
	m.open("b2k2.txt", ios::in);
	char c;
	string str, line,reel,imajiner;
	while (getline(m, str))
	{
		sutun = 0;

		/*m.get(c);
		cout << c;*/
		stringstream  linestream(str);
		string         sayi;

		while (getline(linestream, sayi, ','))
		{
			indis_e = -1;
			uzunluk = sayi.length();
			indis_e = sayi.find('e');

			if (indis_e < 0)
			{

				if (sayi[uzunluk - 1] == 'i')
				{
					for (int k = uzunluk - 1; k > 0; k--)
					{

						if (sayi[k] == '+' || sayi[k] == '-')
						{
							indis = k;

							break;
						}
					}
					reel = sayi.substr(0, indis);
					if (sayi[indis] == '-')
						imajiner = sayi.substr(indis, uzunluk - indis - 1);																																														//yusuf elosman
					else
						imajiner = sayi.substr(indis + 1, uzunluk - indis - 1);

				}

				else{
					reel = sayi;
					imajiner = "0";
				}

			}

			else
			{
				if (sayi[uzunluk - 1] == 'i')
				{
					for (int k = 1; k < uzunluk - 1; k++)
					{

						if (sayi[k] == '+' || sayi[k] == '-')
						{
							indis = k;
							if (indis < indis_e)
								break;
						}
					}
					reel = sayi.substr(0, indis);
					if (sayi[indis] == '-')
						imajiner = sayi.substr(indis, uzunluk - indis - 1);
					else
						imajiner = sayi.substr(indis + 1, uzunluk - indis - 1);

				}

				else{
					reel = sayi;
					imajiner = "0";
				}
			}
			matris[satir][sutun].r = atof(reel.c_str());
			matris[satir][sutun].i = atof(imajiner.c_str());
			sutun++;
		}
		satir++;

		cout << "MATRIS OKUMA... \t" << "satir = " << satir << "\tsutun = " << sutun << endl;
	}
	m.close();


}

void vektor_oku()
{
	int satir = 0, uzunluk,indis;
	string sayi = "",reel,imajiner;
	ifstream m;
	m.open("e2k.txt", ios::in);

	while (!m.eof())
	{
		m >> sayi;
		uzunluk = sayi.length();

		if (sayi[uzunluk - 1] == 'i')
		{
			for (int k = uzunluk - 1; k > 0; k--)
			{
				if (sayi[k] == '+' || sayi[k] == '-')
				{
					indis = k;
					
					break;
				}
			}
			reel = sayi.substr(0, indis);
			if (sayi[indis] == '-')
				imajiner = sayi.substr(indis, uzunluk - indis - 1);
			else
				imajiner = sayi.substr(indis + 1, uzunluk - indis - 1);
		}

		else{
			reel = sayi;
			imajiner = "0";
		}

		vektor[satir].r = atof(reel.c_str());
		vektor[satir].i = atof(imajiner.c_str());
		
		cout << "VEKTOR OKUMA...\t"  << "satir = " << satir  << endl;
		satir++;

		
	}
	m.close();
}

void carpma()
{
	
	Complex toplam;
	toplam.r = 0;
	toplam.i = 0;
	auto BslTimer = chrono::high_resolution_clock::now(); // zamani oku
	for (int i = 0; i < SIZE; i++)
	{
		for (int k = 0; k < SIZE; k++)
		{
			toplam = add(toplam, multiply(matris[i][k], vektor[k]));			
		}
		sonuc[i].r = toplam.r;
		sonuc[i].i = toplam.i;

		toplam.r = 0;
		toplam.i = 0;

	//	cout << "CARPMA...\t" << "satir = " << i << endl;
 	}
	auto BtsTimer = std::chrono::high_resolution_clock::now(); // bittikten sonra zamani oku
	auto sure = BtsTimer - BslTimer; /// farki hesapla
	printf("Toplam sure %5d \n", chrono::duration_cast<std::chrono::milliseconds>(sure).count());

	

}

void dosyaya_yaz()
{
	ofstream dosya_yaz;
	dosya_yaz.open("b2k2_sonuc.txt");
	for (int i = 0; i < SIZE; i++)
	{
		if (sonuc[i].i > -1)
			dosya_yaz << sonuc[i].r << "+" << sonuc[i].i << "i" << endl;
		else
			dosya_yaz << sonuc[i].r << sonuc[i].i << "i" << endl;
	}

	dosya_yaz.close();
}


int main(int argc, char** argv)
{
	
	matris_oku();
	vektor_oku();
//	pthread_row_matrix_multiplication();
	carpma();	
	dosyaya_yaz();
	
	system("pause");


	return 0;
}


void pthread_row_matrix_multiplication(){

	pthread_t id[THREAD_COUNT];
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	for (int i = 0; i < THREAD_COUNT; i++) {
		pthread_create(&id[i], &attr, matrix_row_multiply, NULL);

	}

	for (int i = 0; i < THREAD_COUNT; i++) {
		pthread_join(id[i], NULL);
	}
}

void *matrix_row_multiply(void *){
	int t_str;

	pthread_mutex_lock(&mutex);  // lock the critical section

	t_str = str;
	str += SIZE / 4;

	pthread_mutex_unlock(&mutex); // unlock once you are done
	auto BslTimer = chrono::high_resolution_clock::now(); // zamani oku

	//printf("Thread id-> %d t_str: %d\n",(int)pthread_self() %7,t_str);
	for (int i = t_str; i < t_str + SIZE / 4; i++)
	{
		toplam.r = 0;
		toplam.i = 0;
		for (int k = 0; k < SIZE; k++)
		{
			toplam = add(toplam, multiply(matris[i][k], vektor[k]));
		}

		sonuc[i].r = toplam.r;
		sonuc[i].i = toplam.i;		

//		cout << "CARPMA...\t" << "satir = " << i << endl;
	}
			
	auto BtsTimer = std::chrono::high_resolution_clock::now(); // bittikten sonra zamani oku
	auto sure = BtsTimer - BslTimer; /// farki hesapla
	printf("Toplam sure %5d \n", chrono::duration_cast<std::chrono::milliseconds>(sure).count());
	return NULL;
}

