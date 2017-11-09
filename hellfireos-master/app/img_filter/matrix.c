#include "matrix.h"

/*
 * Desenha matriz na tela.
 *
 * @param a		Matriz em formato de vetor.
 * @param h 	Altura da matriz.
 * @param w 	Largura da matriz.
 *
 */
void matrix_print(uint8_t *a, int16_t h, int16_t w){
	printf("[print] matrix is a %dx%d\n", h, w);
	int16_t i = 0;
	while (i < h){
		int16_t j = 0;
		while (j < w){
			printf("%d\t", a[i * w + j]);
			j += 1;
    }
		printf("\n");
		i += 1;
  }
}

/*
 * Desenha uma sub-matriz de um matriz 'a' na tela.
 *
 * @param a		Matriz em formato de vetor.
 * @param h 	Altura da matriz.
 * @param w 	Largura da matriz.
 * @param k 	Linha inicial da sub-matriz.
 * @param l 	Coluna inicial da matriz.
 * @param bh 	Altura da  sub-matriz.
 * @param bw 	Largura da sub-matriz.
 *
 */
void matrix_print_sub(uint8_t *a, int16_t h, int16_t w, int16_t k, int16_t l, int16_t bh, int16_t bw){
	printf("[print] sub matrix is a %dx%d, line %d column %d\n", bh, bw, k, l);
	int16_t i = 0;
	while (i < bh){
		int16_t j = 0;
		while (j < bw){
			printf("%d\t", a[(i + k) * w + (j + l)]);
			j += 1;
    }
		printf("\n");
		i += 1;
  }
}

 /*
	* Recorta uma sub-matriz da matriz 'a' e a copia para 'b'.
	*
	* @param a	 Matriz a em formato de vetor.
	* @param h 	 Altura da matriz a.
	* @param w 	 Largura da matriz a.
  * @param b   Vetor b que deve receber a sub-matriz.
	* @param k 	 Linha inicial da sub-matriz.
	* @param l 	 Coluna inicial da matriz.
	* @param bh  Altura da  sub-matriz.
	* @param bw  Largura da sub-matriz.
  *
  */
void get_sub_matrix(uint8_t *a, uint8_t *b, int16_t h, int16_t w, int16_t k, int16_t l, int16_t bh, int16_t bw){

	memset(b, 0, bh * bw);

	printf("[crop] sub matrix is a %dx%d, line %d column %d\n", bh, bw, k, l);

	int16_t i = 0;
	int16_t bindex = 0;

	uint8_t xi = 0, xe = 0;

	// índice de coluna émenosr que o tamanho da imagem.
	if(l < 0){
		xi = 2;
		l = 0;
	}

	// índice de coluna é maior que o tamanho da imagem
	if(l+bw > w)
		xe = 2;

	// índice da linha menor que o tamanho da imagem..
	if(k < 0){
		bindex = 2 * bw;
		k = 0;
	}
	// índice da linha maior que o tamanho da imagem..
	if(k+bh > h)
		bh -= 2;

	while (i < bh){
    memcpy(&b[bindex + xi], &a[(i + k) * w + l], (bw -xi - xe) );
		i += 1;
    bindex += bw;
  }
}

/*
 * Cola uma sub-matriz 'b' em uma matriz 'a'.
 *
 * @param a	   Matriz a em formato de vetor.
 * @param h 	 Altura da matriz a.
 * @param w 	 Largura da matriz a.
 * @param b    Matriz b que vai ser colada em a.
 * @param k 	 Linha inicial da sub-matriz.
 * @param l 	 Coluna inicial da matriz.
 * @param bh   Altura da  sub-matriz.
 * @param bw   Largura da sub-matriz.
 *
 */
void set_sub_matrix(uint8_t *a, uint8_t *b, int16_t h, int16_t w, int16_t k, int16_t l, int16_t bh, int16_t bw){
 printf("[set sub-matrix] sub matrix is a %dx%d, line %d column %d\n", bh, bw, k, l);
 int16_t i = 0;
 int16_t bindex = 0;
 while (i < bh){
   memcpy(&a[(i + k) * w + l], &b[bindex], bw);
   i += 1;
   bindex += bw;
 }
}

/*
 * Copia o conteúdo de uma matriz 'a' para uma matriz 'b' deixando
 * bordas de largura 2 com valor 0.
 *
 * @param a 	 Matriz original em formato de vetor.
 * @param ah   Altura da matriz a.
 * @param aw   Largura da matriz a.
 * @param b		 Matriz resultante b em formato de vetor.
 * @param bh   Altura da matriz b.
 * @param bw   Largura da matriz b.
 *
 */
void set_matrix_borders(uint8_t *a, int16_t ah, int16_t aw, uint8_t *b, int16_t bh, int16_t bw){

  int16_t i = 0;
  int16_t bindex = bw * 2 + 2; // Pula primeiras duas linhas e duas posições da borda direita.
  while (i < ah){
    memcpy(&b[bindex], &a[i * aw], aw*sizeof(int)); // Copia conteúdo da linha.
    i += 1;
    bindex += aw + 4;	// Pula duas posições da borda esquerda e duas da borda direita.
	}
}

// int16_t main(int16_t argc, int16_t *argv[]){
//
//   // Tamanho da mensagem que queremos enviar.
//   int16_t block_width = 3;
//   int16_t block_height = 3;
//
//   int16_t image_width = 9;
//   int16_t image_height = 9;
//
//   // Matriz a original. É a imagem original.
//   int16_t a[] = {
//     11, 12, 13, 13, 14, 15, 16, 17, 18,
//     21, 22, 23, 24, 25, 26, 27, 28, 20,
//     31, 32, 33, 34, 35, 36, 37, 38, 15,
//     41, 42, 43, 44, 45, 46, 47, 48, 16,
//     51, 52, 53, 54, 55, 56, 57, 58, 20,
//     61, 62, 63, 64, 65, 66, 67, 68, 20,
//     71, 72, 73, 74, 75, 76, 77, 78, 20,
//     81, 82, 83, 84, 85, 86, 87, 88, 20,
//     91, 92, 93, 94, 95, 96, 97, 98, 20
//   };
//
//   // Mostra matriz original.
//   matrix_print(a, image_height, image_width);
//
//   // Mostra uma sub-matriz.
//   matrix_print_sub(a, image_height, image_width, 1, 2, 3, 2);
//
//   // Recorta uma sub-matriz b.
//   uint8_t b = malloc(6*sizeof(int));
//
//   int16_t x[] = { 9, 9, 9, 9, 9, 9, 9, 9, 9 };
//   set_sub_matrix(a, x, image_height, image_width, 1, 2, 3, 3);
//   matrix_print(a, image_height, image_width);
//   crop_sub_matrix(a, b, image_height, image_width, 1, 2, 3, 2);
//   matrix_print(a, 3, 2);
//
//   matrix_print(b, 3, 2);
//
//   int16_t c_width = image_width + 4;
//   int16_t c_height = image_height + 4;
//
//   // Inicializa uma matriz c.
//   int16_t c_size = c_width * c_height * sizeof(int);
//   uint8_t c = malloc(c_size);
//   memset(c, 0, c_size);
//
//   // Matriz c recebe a e fica com bordas.
//   set_matrix_borders(a, image_height, image_width, c, c_height, c_width);
//   matrix_print(c, c_height, c_width);
//
//   int16_t w = 2;
//   int16_t h = 2;
//
//   uint8_t d = malloc((block_width + 4) * (block_width + 4) * sizeof(int) );
//
//   while(1){
//
//       crop_sub_matrix(c, d, c_height, c_width, h-2, w-2, block_height+4, block_width+4);
//       matrix_print(d, block_height+4, block_width+4);
//
//       printf("\n\n");
//
//       w += block_width;
//
//       if(w >= image_width){
//           w = 2;
//           h += block_height;
//
//           if(h >= image_height){
//             break;
//           }
//
//       }
//
//   }
//
// 	return 0;
//
// }
