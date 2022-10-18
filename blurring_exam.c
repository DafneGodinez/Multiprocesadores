#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define NUM_THREADS 2

void blurring(char outputImage[], int sizeM);

int main(){
  #pragma omp parallel
    {
        #pragma omp sections
        {
        #pragma omp section
            blurring("blurred1_exam2.bmp", 15);
         #pragma omp section
            blurring("blurred2_exam2.bmp", 23);
        #pragma omp section
            blurring("blurred2_exam2.bmp", 35);
        }
    }
  return 0;
}

void blurring(char outputImageName[], int sizeM){
  FILE *image, *outputImage, *lecturas;
  image = fopen("toy.bmp","rb");                 //Imagen original a transformar
  outputImage = fopen(outputImageName,"wb");      //Imagen transformada
  long ancho;
  long alto;
  unsigned char r, g, b;    //Pixel
  unsigned char* ptr;

  unsigned char xx[54];
  long cuenta = 0, anchoR = 0, altoR = 0, anchoM = 0, altoM = 0;
  long sum;
  int iR, jR;
  for(int i = 0; i < 54; i++) {
    xx[i] = fgetc(image);
    fputc(xx[i], outputImage);      //Copia cabecera a nueva imagen
  }
  ancho = (long)xx[20]*65536 + (long)xx[19]*256 + (long)xx[18];
  alto = (long)xx[24]*65536 + (long)xx[23]*256 + (long)xx[22];
  printf("largo img %li\n",alto);
  printf("ancho img %li\n",ancho);

  ptr = (unsigned char*)malloc(alto*ancho*3* sizeof(unsigned char));
  omp_set_num_threads(NUM_THREADS);
    
  unsigned char foto[alto][ancho], fotoB[alto][ancho];
  unsigned char pixel;
  
    for(int i = 0; i < alto; i++){
      for(int j = 0; j < ancho; j++){
        b = fgetc(image);
        g = fgetc(image);
        r = fgetc(image);

        pixel = 0.21*r+0.72*g+0.07*b;
        foto[i][j] = pixel;
        fotoB[i][j] = r;
      }
    }

    anchoR = ancho/sizeM;
    altoR = alto/sizeM;
    anchoM = ancho%sizeM;
    altoM = alto%sizeM;

    int inicioX,inicioY,cierreX,cierreY,ladoX,ladoY;
    //casi toda la imagen se representa aquí
    for(int i = 0; i < alto; i++){
      //iR=i*sizeM;
      for(int j = 0; j < ancho; j++){
        //jR=j*sizeM;
        if(i < sizeM){
          inicioX = 0;
          cierreX = i+sizeM;
          ladoX = i+sizeM;
        } else if(i >= alto-sizeM){
          inicioX = i-sizeM;
          cierreX = alto;
          ladoX = alto-i+sizeM;
        }else{
          inicioX = i-sizeM;
          cierreX = i+sizeM;
          ladoX = sizeM*2+1;
        }
    
        if(j < sizeM){
          inicioY = 0;
          cierreY = j+sizeM;
          ladoY = j+sizeM;
        }else if(j >= ancho-sizeM){
          inicioY = j-sizeM;
          cierreY = ancho;
          ladoY = ancho-j+sizeM;
        }else{
          inicioY = j-sizeM;
          cierreY = j+sizeM;
          ladoY = sizeM*2+1;
        }
        sum = 0;
        for(int x = inicioX; x < cierreX; x++){
          for(int y = inicioY; y < cierreY; y++){
            sum += foto[x][y];
          }
        }
        sum = sum/(ladoX*ladoY);
        fotoB[i][j] = sum;
      }
    }
   
  //asignacion a imagen
  cuenta = 0;
  for(int i = 0; i < alto; i++){
    for(int j = 0; j < ancho; j++){
      ptr[cuenta] = fotoB[i][j]; //b
      ptr[cuenta+1] = fotoB[i][j]; //g
      ptr[cuenta+2] = fotoB[i][j]; //r
      cuenta++;
    }
  }       

  //Grises
  const double startTime = omp_get_wtime();
  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
    for (int i = 0; i < alto*ancho; ++i) {
      fputc(b, outputImage);
      fputc(ptr[i+1], outputImage);
      fputc(r, outputImage);
    }
  }

  const double endTime = omp_get_wtime();
  free(ptr);
  fclose(image);
  fclose(outputImage);
  printf("Tiempo= %f\n", endTime-startTime);
}