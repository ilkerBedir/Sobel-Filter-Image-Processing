#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define LO(num)	((num) & 0x000000FF)


int width, height, maxval,maxgy,maxgx;//resimlerdeki geniþlik,uzunluk, ve maksimum piksel parametreleri
int image[2000*2000];//dizi halinde orijinal resim
int image_matrix[2000][2000];//orijinal resim matris
int image_matrix_tmp[2000][2000];//deðiþken matrisi
int image_matrix_gx[2000][2000];//gxin deðiþken matrisi
int image_matrix_gy[2000][2000];//y yönünde uygulan filtrenin deðiþken matrisi
void readP2();//P2 dosyasýný okuma fonksiyonu
void arrayToMatrix();//diziyi matrise çevirme fonksiyonu
void sobelFilter();//Konvolüsyonun uygulandýðý yer
void normalizasyon();//normaliztonun yapýldýðý fonksiyon
void writeOutputP2();//p2 dosyasýný yazdýrma
void writeOutputP5();//p5 dosyasýný yardýrma
void readP5();//p5 dosyasýný okuma

int main(int argc, char* argv[])
{
	int choice;
	char input[100],output[100];
	printf("PGM DOSYASININ TIPINI SECINIZ\n1-)P2\n2-)P5\n");
	scanf("%d",&choice);
	printf("INPUT DOSYASINI ISMINI YAZINIZ.(.PGM HALINDE)\n");
	scanf("%s",input);
	printf("OUTPUT DOSYASINI ISMINI YAZINIZ.(.PGM HALINDE)\n");
	scanf("%s",output);
	if(choice==1){
		readP2(input);
		arrayToMatrix();
		sobelFilter();
		normalizasyon();
		writeOutputP2(output);
	}
	else{
		readP5(input);
		sobelFilter();
		normalizasyon();
		writeOutputP5(output);
	}
	
return 0;
}

void readP5(char input[100]){
	int i,a,j;
	char line[100];
    FILE *fp;
 
    fp = fopen(input, "rb");
    if (fp == NULL) {
        printf("could not open image file\n");
        exit(1);
         }
              
    fgets(line, sizeof(line), fp);
    if (strcmp(line, "P5\n") != 0) {
        printf("image is not in PGM format!\n");
        fclose(fp);
        exit(1);
    }
   
   fscanf(fp, "%d", &width);
    fscanf(fp, "%d", &height);
   fscanf(fp, "%d \n", &maxval); 
 
    if (maxval > 255) {
        printf("sorry, only 8-bit PGMs are supported at this time!\n");
        fclose(fp);
        exit(1);
    }
    i=0;
    a=0;
    for (i = 0; i < height; ++i) {
            for (j = 0; j < width; ++j) {
                a = fgetc(fp);
                image_matrix[i][j] = a;
            }
        }
    fclose(fp);
}
void readP2(char input[100]){
	char line[100];
    int i,a,j;
    FILE *fd;
    fd = fopen(input, "rb");
    if (fd == NULL) {
        printf("could not open image file\n");
        exit(1);
    }
    fgets(line, sizeof(line), fd);
    printf("%s",line);
    if (strcmp(line, "P2\n") != 0){
        printf("image is not in PGM format!\n");
        fclose(fd);
        exit(1);
    }
    fgets(line, sizeof(line), fd);
    if (fscanf(fd, "%d %d %d\n", &width, &height, &maxval) != 3) {
        printf("invalid header\n");
        fclose(fd);
        exit(1);
    }
    if (maxval > 255) {
        printf("sorry, only 8-bit PGMs are supported at this time!\n");
        fclose(fd);
        exit(1);
    }
    i=0;
   while(!feof (fd))    {
    	fscanf(fd,"%d",&a);
    	image[i]=a;
    	i++;
	}
	fclose(fd);
}
void arrayToMatrix(){//okuduðum dosyayý ilk olarak diziye yazdýrdýðýmýz için o diziyi matrise çevirme width ve height deðerelerinide bildiðimiz için for da onlarý kullandým
	int i,j,a=0;	
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
		image_matrix[i][j]=image[a];
		a++;
		}
	}
}

void sobelFilter(){//x yönünde ve y yönündeki filtreleri ayrý ayrý uyguladým ,orijinal resim içiniseardýndan kareleri toplayýp kökünü aldýk 
	int i,j;
	int mx[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1,0,1}
	};
	int my[3][3] = 
	{
		{-1,-2,-1},
		{0,0,0},
		{1,2,1}
	};
		for(i=0;i<height;i++)//önce deðiþken matrislere 0 atadýk kenarlarý bozuk çýkmasýn diye
	{
		for(j=0;j<width;j++)
		{
		image_matrix_tmp[i][j]=0;
		image_matrix_gx[i][j]=0;
		image_matrix_gy[i][j]=0;
		}
	}
	int gx,gy,gz;
	for(i=1;i<height-1;i++)//kenarlarýmýz bozulmasý için orijinal matrisin sýnýrlarý daralttýk.
	{
		for(j=1;j<width-1;j++)//konvolüsyon iþlemleri
		{
			gx=image_matrix[i-1][j-1]*mx[0][0]+image_matrix[i-1][j]*mx[0][1]+image_matrix[i-1][j+1]*mx[0][2]+image_matrix[i][j-1]*mx[1][0]+
			image_matrix[i+1][j-1]*mx[2][0]+image_matrix[i][j]*mx[1][1]+image_matrix[i][j+1]*mx[1][2]+image_matrix[i+1][j]*mx[2][1]+image_matrix[i+1][j+1]*mx[2][2];
			image_matrix_gx[i][j]=gx;
			gx=pow(gx,2);
			gy=image_matrix[i-1][j-1]*my[0][0]+image_matrix[i-1][j]*my[0][1]+image_matrix[i-1][j+1]*my[0][2]+image_matrix[i][j-1]*my[1][0]+
			image_matrix[i+1][j-1]*my[2][0]+image_matrix[i][j]*my[1][1]+image_matrix[i][j+1]*my[1][2]+image_matrix[i+1][j]*my[2][1]+image_matrix[i+1][j+1]*my[2][2];
			image_matrix_gy[i][j]=gy;
			gy=pow(gy,2);
			gz=sqrt(gx+gy);
			image_matrix_tmp[i][j]=gz;
		}
   }
   for(i=0;i<width;i++){//0 yaptýðýmýz kenarlarý orijinal resimden alarak kenar deðerlerini bozmadýk
   	image_matrix_tmp[0][i]=image_matrix[0][i];
   	image_matrix_gx[0][i]=image_matrix[0][i];
   	image_matrix_gy[0][i]=image_matrix[0][i];
   	image_matrix_tmp[height-1][i]=image_matrix[height-1][i];
   	image_matrix_gx[height-1][i]=image_matrix[height-1][i];
   	image_matrix_gy[height-1][i]=image_matrix[height-1][i];
   }
    for(i=0;i<height;i++){
   	image_matrix_tmp[i][0]=image_matrix[i][0];
   	image_matrix_gx[i][0]=image_matrix[i][0];
   	image_matrix_gy[i][0]=image_matrix[i][0];
   	image_matrix_tmp[i][width-1]=image_matrix[i][width-1];
   	image_matrix_gx[i][width-1]=image_matrix[i][width-1];
   	image_matrix_gy[i][width-1]=image_matrix[i][width-1];
   }	
}
void normalizasyon(){// normalizyonun ise (deger-min)/(max-min)*255 olarak yapýldý.
	int min,max,mingx,mingy,i,j,a,b,ax,bx,ay,by;
	min=max=image_matrix_tmp[0][0];
	mingx=maxgx=image_matrix_gx[0][0];
	mingy=maxgy=image_matrix_gy[0][0];
    for(i=0; i<height; i++)
    {
    	for(j=0;j<width;j++)
		{
		if(min>image_matrix_tmp[i][j]){
		  	min=image_matrix_tmp[i][j];
		}     
		if(max<image_matrix_tmp[i][j]){
		    max=image_matrix_tmp[i][j];
		    a=i;
		    b=j;
		}
		if(mingx>image_matrix_gx[i][j]){
			mingx=image_matrix_gx[i][j];	
		}
		if(maxgx<image_matrix_gx[i][j]){
			maxgx=image_matrix_gx[i][j];
			ax=i;
			bx=j;
		}
		if(mingy>image_matrix_gy[i][j]){
			mingy=image_matrix_gy[i][j];
		}
		if(maxgy<image_matrix_gy[i][j]){
			maxgy=image_matrix_gy[i][j];
			ay=i;
			by=j;
		}
    	}
	}
    for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			image_matrix_tmp[i][j]=(double)((double)(image_matrix_tmp[i][j]-min)/(double)(max-min))*255;
			image_matrix_gx[i][j]=(double)((double)(image_matrix_gx[i][j]-mingx)/(double)(maxgx-mingx))*255;
			image_matrix_gy[i][j]=(double)((double)(image_matrix_gy[i][j]-mingy)/(double)(maxgy-mingy))*255;
		}
	}
	maxval=image_matrix_tmp[a][b];
	maxgx=image_matrix_gx[ax][bx];
	maxgy=image_matrix_gy[ay][by];
}

void writeOutputP2(char output[100]){
	
   FILE* fd;
   FILE* fdx;
   FILE* fdy;
   char gx[100]="gx";
   char	gy[100]="gy";
   char output1[100];
   strcpy(output1,output);
   fd = fopen(output, "wb"); //write the file in binary mode
   strcat(gx,output);
   fdx=fopen(gx, "wb");
   strcat(gy,output1);
   fdy=fopen(gy,"wb");
   int i,j;
    fprintf(fd, "P2\n");
    fprintf(fd, "%d %d\n", width, height);
	fprintf(fd, "%d\n", maxval);
	fprintf(fdx, "P2\n");
    fprintf(fdx, "%d %d\n", width, height);
	fprintf(fdx, "%d\n", maxgx);
	fprintf(fdy, "P2\n");
    fprintf(fdy, "%d %d\n", width, height);
	fprintf(fdy, "%d\n", maxgy);
	
   for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
         fprintf(fd, "%d ", image_matrix_tmp[i][j]); 
         fprintf(fdx, "%d " , image_matrix_gx[i][j]);
         fprintf(fdy, "%d " , image_matrix_gy[i][j]);
      }
      fprintf(fd, "\n");
      fprintf(fdx, "\n");
      fprintf(fdy, "\n");
   }
   fclose(fd);
   fclose(fdx);
   fclose(fdy);
}
void writeOutputP5(char output[100]){
	FILE *fp;
    int i, j;
    int a;
    char gx[100]="gx";
    char gy[100]="gy";
    char output1[100];
    strcpy(output1,output);
    fp = fopen(output, "wb");
    if (fp == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", maxval);
        for (i = 0; i < height; ++i) {
            for (j = 0; j < width; ++j) {
                a = LO(image_matrix_tmp[i][j]);
                fputc(a, fp);
            }
        }
    fclose(fp);
    strcat(gx,output);
    fp = fopen(gx, "wb");
    if (fp == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", maxgx);
        for (i = 0; i < height; ++i) {
            for (j = 0; j < width; ++j) {
                a = LO(image_matrix_gx[i][j]);
                fputc(a, fp);
            }
        }
    fclose(fp);
    strcat(gy,output1);
    fp = fopen(gy, "wb");
    if (fp == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", width, height);
    fprintf(fp, "%d\n", maxgy);
        for (i = 0; i < height; ++i) {
            for (j = 0; j < width; ++j) {
                a = LO(image_matrix_gy[i][j]);
                fputc(a, fp);
            }
        }
    fclose(fp);
}



