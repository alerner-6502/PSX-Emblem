/*
Armored-Core Emblem Implementor v0.2

Copyright (c) 2019 Anatoly Lerner

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
 
#define STBI_NO_JPEG
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_LINEAR
#define STBI_NO_HDR  // Exclude unused methods

#define STBI_FAILURE_USERMSG  // Enable user friendly messages

#define STB_IMAGE_IMPLEMENTATION // Enable this library
#include "stb_image.h"

#include "exoquant.h" // image quantizer

#define UCHAR unsigned char

#define CLR while(getchar() != '\n')

void TerminateProgram(char str[]){
	printf("%s%s%s", "\nError: ", str, " Terminating.\nPress any key to exit...");
	getchar();
	exit(1);
}
	
void FormatPath(char Str[]){
	if(*Str == '"'){ 
		strcpy(Str, Str+1);
		*(Str + strlen(Str) - 1) = '\0';
	}
}

int TransparencyTest(UCHAR *img, int width, int height){
	int i, r = 0;
	for(i = 0; i < width*height; i++){
		if(*(img + i*4 + 3) < 0x80){
			// binary transparency
			*(img + i*4 + 3) = 0x00;
			// transparent_pixel_found flag
			r = 1;
		}
		// binary transparency
		else{ *(img + i*4 + 3) = 0xff;}
	}
	return r;
}

void ImageInput(UCHAR **img){
	char filename[100]; int i, width, height, channels;
	
	puts("    Drag and drop your image file into this window (or enter the full path).");
	puts("    Requirements:\n       Format: BMP or PNG (transparency is supported)\n       Dimensions: 64 by 64 pixels");
	do{
		i = 0;
		printf("%s","    File: "); gets(filename);
		FormatPath(filename);
		*img = stbi_load(filename, &width, &height, &channels, 4);
		if(*img == NULL){
			printf("%s%s%s\n", "       Error: ", stbi_failure_reason(), ". Please try again."); 
			i = 1;
		}
		else if(width != 64 || height != 64){
			printf("%s\n", "       Error: Unsupported image dimensions. Please try again.");
			stbi_image_free(*img);
			i = 1;
		}
	}while(i != 0);
}

void QuantizeImage(UCHAR *img, UCHAR **img_idx){
	
	unsigned char palette[] = {
		0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x79, 0x00, 0xFF, 0xFF, 0xFB, 0x00, 0xFF,
		0x7B, 0xFB, 0x00, 0xFF, 0x00, 0xFB, 0x00, 0xFF, 0x00, 0xFB, 0x7B, 0xFF, 0x00, 0xFB, 0xFF, 0xFF,
		0x00, 0x79, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x7B, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
		0xFF, 0x00, 0x7B, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x7B, 0x79, 0x7B, 0xFF, 0xFF, 0xFB, 0xFF, 0xFF
	};
	
	*img_idx = (UCHAR*)malloc(sizeof(UCHAR)*64*64);
	
	// check for transparency and quantize it
	int trn = TransparencyTest(img, 64, 64);
	
	exq_data *pExq = exq_init(); // initialize the quantizer (per image)
	exq_feed(pExq, img, 64*64); // feed pixel data (32bpp)
	exq_set_palette(pExq, palette, 16); // feed palette data
	exq_map_image(pExq, 64*64, img, *img_idx);
	exq_free(pExq);
}

void SjisToAscii(char inp[], int len, char out[]){
	const unsigned char SjisLUT[] = {
		0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x30,
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
		0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
		0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
		0x3F, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
	};
	char *uj = out; int i, n;
	unsigned char tmp_1, tmp_2;
	
	for(i = 0; i < len; i += 2){
		tmp_1 = (unsigned char)inp[i];
		tmp_2 = (unsigned char)inp[i+1];
		if(tmp_1 == 0x82){
			n = (int)(tmp_2) - 64;
			n = abs(n); n = n % 96;
			*uj = SjisLUT[n];
		}
		else if((tmp_1 == 0x81 && tmp_2 == 0x40) ||
				(tmp_1 == 0x00 && tmp_2 == 0x00) ||
				(tmp_1 == 0x3E && tmp_2 == 0x00)){ *uj = '_';}
		else{ *uj = '?';}
		uj++;
	}
	*uj = '\0';
	
	
}

unsigned int BuffToUINT(char buff[], int len){
	unsigned int r; int i;
	for(i = len-1, r = 0; i >= 0; i--){
		r = r << 8;
		r = r + (unsigned char)(buff[i]);
	}
	return r;
}

void UINTtoBuff(unsigned char buff[], unsigned int num){
	int i;
	for(i = 0; i < 4; i++){
		buff[i] = (unsigned char)(num%256);
		num /= 256;
	}
}

void ReadMemoryCard(int *game, int *slot, char *filename){
	
	FILE *fin; int i, k; long j; unsigned int n;
	char buff[128], buff2[128];
	
	int AcSectors[16]; // sector record. [0..2] = AC game type, -1 = Empty
	for(i = 0; i < 16; i++){ AcSectors[i] = -1;}
	int AcExists = 0;
	
	const char GameCodes[][20] = {"BASCUS-94182", "BASLUS-00670", "BASLUS-01030"};
	const int GameMemType[] = {0,1,2}; // games use different memory mappings
	const char GameNames[][20] = {"1", "Project Phantasma", "Master of the Arena"};
		
	puts("Drag and drop your PS1 memory card file into this window (or enter the full path).");
	puts("Requirements:\n   Extension: .MCR or .MCD\n   Compression: None");
	do{
		do{
			i = 0;
			printf("%s","File: "); gets(filename);
			FormatPath(filename);
			fin = fopen(filename, "rb");

			if(fin == NULL){
				printf("%s%s%s\n", "   Error: \"", filename, "\"file not found. Please try again."); 
				i = 1;
			}
			else{
				fseek(fin, 0L, SEEK_END); // file size
				if(ftell(fin) != 131072){
					puts("Invalid file size. Please try again.");
					fclose(fin);
					i = 1;
				}
			}
		}while(i != 0);
		
		puts("\nArmored core games on the memory card:\n");
		
		for(j = 1; j < 16; j ++){ // read all the sector codes
		
			fseek(fin, 10 + j*128, SEEK_SET);
			fread(buff, 1, 13, fin); buff[12] = '\0';
			
			for(k = 0; k < 3; k++){
				if(strcmp(buff, GameCodes[k]) == 0){
					
					// record the sector game
					AcSectors[j] = k;
					AcExists = 1;
					
					// print sector name
					fseek(fin, 4 + j*8192, SEEK_SET);
					fread(buff, 1, 64, fin);
					SjisToAscii(buff, 64, buff2);	
					printf("[%ld] %s\n", j, buff2);
					
					// print game name
					printf("        Game: AC %s (%s)\n", GameNames[GameMemType[k]], GameCodes[k]);
					
					// get pilot name
					if(GameMemType[k] < 2){ fseek(fin, 534 + j*8192, SEEK_SET);}
					if(GameMemType[k] == 2){ fseek(fin, 406 + j*8192, SEEK_SET);}
					fread(buff, 1, 18, fin);
					SjisToAscii(buff, 18, buff2);
					printf("        Pilot: %s\n", buff2);
					
					// get sortie number
					fread(buff, 1, 4, fin);
					printf("        Sortie: %d\n", (unsigned int)buff[0]);
					
					// get credit number
					fread(buff, 1, 4, fin);
					n = BuffToUINT(buff, 4);
					printf("        Credit: %d\n", n);
					
					// done
					printf("\n");
					k = 3; // exit the loop immidiately
				}
			}
		}
		if(AcExists == 0){ puts("--- None ---\n\nPlease provide some other memory card.\n");}
	}while(AcExists == 0);
	
	puts("Please enter the sector number (the number in the square brackets) of the game you want to modify.");
	do{
		printf("Number: ");
		i = scanf("%d", slot);
		CLR;
		if(i != 1 || *slot < 1 || *slot > 15 || AcSectors[*slot] == -1){
			puts("Invalid Input. Please try again.");
		}
	}while(i != 1 || *slot < 1 || *slot > 15 || AcSectors[*slot] == -1);
	*game = AcSectors[*slot];
	
	fclose(fin);
	
}

void InsertChecksum(int game, int slot, char filename[]){
	
	const int ChecksumOffset[] = {512, 512, 384};
	const int ChecksumRangeStart[] = {560, 560, 432};
	const int ChecksumRangeLen[] = {3040, 3232, 3240};
	unsigned int sum = 0; int i;
	unsigned char buff[4];
	
	FILE *fmd = fopen(filename, "r+b");
	fseek(fmd, ChecksumRangeStart[game] + slot*8192, SEEK_SET);
	
	for(i = 0, sum = 0; i < ChecksumRangeLen[game]; i++){
		fread(buff, 1, 1, fmd);
		sum += buff[0];
	}
	if(game == 2){  // master of the arena exception
		sum += 0xCA8;
		sum = 0 - sum;
	}
	
	fseek(fmd, ChecksumOffset[game] + slot*8192, SEEK_SET);
	UINTtoBuff(buff, sum);
	fwrite(buff, 1, 4, fmd);
	fclose(fmd);
}

void EmbedImage(int game, int slot, char filename[]){
	char buff[128];
	
	printf("\nWould you like to modify the emblem of your game? (y/n): ");
	gets(buff);
	if(strcmp(buff, "y") == 0){
		UCHAR *img, *img_idx, tmp;
		const int EmblemOffset[] = {1080, 1272, 1152};
		int i, j;
		
		ImageInput(&img);
		QuantizeImage(img, &img_idx);
		
		FILE *fmd = fopen(filename, "r+b");
		if(fmd == NULL){ TerminateProgram("The specified memory card file is no longer there.");}
		
		printf("    Modifying the memory card...");
		
		fseek(fmd, EmblemOffset[game] + slot*8192, SEEK_SET);
		for(i = 0; i < 64*32; i++){ // number of bytes
			tmp = 0x00;
			for(j = 0; j < 2; j++){ // two pixels per byte
				tmp = tmp + (*img_idx << j*4);
				img_idx++;
			}
			fwrite(&tmp, 1, 1, fmd);
		}
		
		fclose(fmd);
		stbi_image_free(img);
		
		InsertChecksum(game, slot, filename);
		puts("Done");
	}
}

void ApplyCheats(int game, int slot, char filename[]){
	
	char buff[128]; int i, j; unsigned char tmp;
	
	printf("\nWould you like to enable the overweight mode? (y/n): ");
	gets(buff);
	if(strcmp(buff, "y") == 0){
		
		const int OverweightOffset[] = {3402, 3594, 3566};
		
		FILE *fmd = fopen(filename, "r+b");
		if(fmd == NULL){ TerminateProgram("The specified memory card file is no longer there.");}
		
		printf("    Modifying the memory card...");
		
		fseek(fmd, OverweightOffset[game] + slot*8192, SEEK_SET);
		tmp = 0x01;
		fwrite(&tmp, 1, 1, fmd);
		fclose(fmd);
		
		InsertChecksum(game, slot, filename);
		puts("Done");
	}
	
	printf("\nWould you like to enable human plus abileties? (y/n): ");
	gets(buff);
	if(strcmp(buff, "y") == 0){
		
		puts("    Up to what level? (0 to 6) (Enter 6 to unlock all human plus abileties)");
		do{
			printf("    Level: ");
			i = scanf("%d", &j);
			CLR;
			if(i != 1 || j < 0 || j > 6){ puts("    Invalid Input. Please try again.");}
		}while(i != 1 || j < 0 || j > 6);
		
		const int HumanOffset[] = {3400, 3592, 3564};
		
		FILE *fmd = fopen(filename, "r+b");
		if(fmd == NULL){ TerminateProgram("The specified memory card file is no longer there.");}
		
		printf("    Modifying the memory card...");
		
		fseek(fmd, HumanOffset[game] + slot*8192, SEEK_SET);
		tmp = (unsigned char)(j);
		fwrite(&tmp, 1, 1, fmd);
		fclose(fmd);
		
		InsertChecksum(game, slot, filename);
		puts("Done");
	}
	
}

int main(){
	
	puts("=== Armored-Core Emblem Creator (PSX) ===\n");
	
	int game, slot; char filename[256];
	
	ReadMemoryCard(&game, &slot, filename);
	
	EmbedImage(game, slot, filename);
	
	ApplyCheats(game, slot, filename);
	
	puts("\nHave fun :). Press any key to exit...");
	getchar();
	return 0;
}


