#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<stdio.h>
#include<string>
#include<algorithm>

#define buf_size 50000
using namespace std;
int input_file_size;

int cmp(void const *a, void const *b) {
	return *(short*)a - *(short*)b;
}
void copy(FILE *a, FILE *b, int pos, int cnt) {    // copy cnt elements from file 1 to file 2
	int buf[buf_size];
	int pos1 = ftell(a);
	int pos2 = ftell(b);
	int cop = 0;   // no. of elements to be copied
	fseek(a, pos * sizeof(short), SEEK_SET);
	cop = cnt>buf_size ? buf_size : cnt;
	while (cnt>0) {
		fread(buf, sizeof(short), cop, a);
		fwrite(buf, sizeof(short), cop, b);
		cnt -= cop;
	}
	fseek(a, pos1, SEEK_SET);
	fseek(b, pos2, SEEK_SET);
}
void merge(FILE *a, int lo, int mi, int hi) {
	int cnt1 = mi - lo + 1;     //first half
	int cnt2 = hi - mi;   //second half
	int p1 = ftell(a);
	int g = 0;   // g & h to maintain elements written to file after merging
	int h = 0;
	FILE *temp1, *temp2;
	temp1 = fopen("t1.bin", "w+b");
	temp2 = fopen("t2.bin", "w+b");
	fseek(a, lo * sizeof(short), SEEK_SET);
	copy(a, temp1, lo, cnt1);
	copy(a, temp2, mi + 1, cnt2);
	fseek(a, lo * sizeof(short), SEEK_SET);
	short input_buf1[buf_size];
	short input_buf2[buf_size];
	short output_buf[2 * buf_size];
	int c1 = cnt1, c2 = cnt2;  //to maintain no. of elements left
	int i = 0;
	int no_elements1 = 0;   //to maintain elements read at particular time
	int no_elements2 = 0;
	int x = 0, y = 0, z = 0; //to maintain array indexes
	while (i<cnt1 + cnt2) {
		if (x == no_elements1&&no_elements2 == y&&c1 == 0 && c2 == 0) {
			i = i + g + h;
			fwrite(output_buf, sizeof(short), g + h, a);
			g = 0; h = 0;
		}
		if (x == no_elements1) {
			g += no_elements1;
			no_elements1 = c1>buf_size ? buf_size : c1;
			x = 0;

			if (no_elements1 != 0) {
				c1 -= no_elements1;
				fread(input_buf1, sizeof(short), no_elements1, temp1);
			}
		}
		if (y == no_elements2) {
			no_elements2 = c2>buf_size ? buf_size : c2;
			y = 0;
			h += no_elements2;
			if (no_elements2 != 0) {
				c2 -= no_elements2;
				fread(input_buf2, sizeof(short), no_elements2, temp2);
			}

		}



		while (x<no_elements1&&y<no_elements2) {
			if (input_buf1[x] <= input_buf2[y])
				output_buf[z++] = input_buf1[x++];
			else
				output_buf[z++] = input_buf2[y++];

			if (z == 2 * buf_size) {
				fwrite(output_buf, sizeof(short), 2 * buf_size, a);
				z = 0;
				memset(output_buf, 0, 2 * buf_size * sizeof(short));
				i = i + 2 * buf_size;
				g -= buf_size;
				h -= buf_size;
			}

		}
		if (x == 0 && no_elements1 == 0) {
			while (y<no_elements2)output_buf[z++] = input_buf2[y++];
		}
		if (y == 0 && no_elements2 == 0) {
			while (x<no_elements1)output_buf[z++] = input_buf1[x++];
		}
		if (z == 2 * buf_size) {
			fwrite(output_buf, sizeof(short), 2 * buf_size, a);
			z = 0;
			memset(output_buf, 0, 2 * buf_size * sizeof(short));
			i = i + 2 * buf_size;
		}
	}
	fclose(temp1);
	fclose(temp2);
	remove("t1.bin");
	remove("t2.bin");
}
void accelerate(FILE *a, int lo, int hi) {      //Function to apply direct sorting in case no. of elements is less than buffer size

	int cnt = hi - lo + 1;
	short buf[buf_size];
	fseek(a, lo * sizeof(short), SEEK_SET);
	fread(buf, sizeof(short), cnt, a);
	qsort(buf, cnt, sizeof(short), cmp);
	fseek(a, lo * sizeof(short), SEEK_SET);
	fwrite(buf, sizeof(short), cnt, a);

}
void mergesort(FILE *a, int lo, int hi) {     // actual meresort program
	if (hi - lo + 1 <= buf_size) {
		accelerate(a, lo, hi);
	}
	else
	{
		int mi = lo + (hi - lo) / 2;
		mergesort(a, lo, mi);
		mergesort(a, mi + 1, hi);
		merge(a, lo, mi, hi);

	}

}

int main() {
	char input[10], output[10];
	FILE *a, *b;
	cout<< "Enter the name of input file";
	cin >> input;
	cout << "Enter the no. of elements which you want to sort";
	cin >> input_file_size;

	a = fopen(input, "rb");
	cout << "Enter the name of output file";
	cin >> output;
	b = fopen(output, "wb");
	copy(a, b, 0, input_file_size);
	fclose(b);
	cout << endl;
	fclose(a);
	b = fopen(output, "r+b");
	fseek(b, 0, SEEK_SET);

	mergesort(b, 0, input_file_size - 1);
	rewind(b);
	fclose(b);
	cout << "Success";
	cin >> input_file_size;


}
