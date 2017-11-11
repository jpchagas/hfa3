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
	//memset(b, 0, bh * bw);
	printf("[get] sub matrix is a %dx%d, line %d column %d\n", bh, bw, k, l);
	int16_t i = 0;
	int16_t bindex = 0;
	while (i < bh){
		memcpy(&b[bindex], &a[(i + k) * w + l], bw*sizeof(uint8_t));
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
	 //printf("a[%d] = b[%d]\n", (i + k) * w + l, bindex);
   memcpy(&a[(i + k) * w + l], &b[bindex], bw*sizeof(uint8_t)   );
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
    memcpy(&b[bindex], &a[i * aw], aw); // Copia conteúdo da linha.
    i += 1;
    bindex += aw + 4;	// Pula duas posições da borda esquerda e duas da borda direita.
	}
}
