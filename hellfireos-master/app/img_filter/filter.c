#include "image.h"

#define BLOCK_WIDTH 3
#define BLOCK_HEIGHT 3

#define TOTAL_SLAVES 5

#define MASTER_NODE 0
#define SLV1_NODE 1
#define SLV2_NODE 2
#define SLV3_NODE 3
#define SLV4_NODE 4
#define SLV5_NODE 5

// Usado para gerencia dos blocos pelo mestre.
struct data_info {
	uint8_t x;
	uint8_t y;
	uint8_t w;
	uint8_t h;
};

// Header usado na troca de mensagens.
struct hdr_info {
	uint8_t w;
	uint8_t h;
};

uint8_t gaussian(uint8_t buffer[5][5]){
	int32_t sum = 0, mpixel;
	uint8_t i, j;

	int16_t kernel[5][5] =	{	{2, 4, 5, 4, 2},
					{4, 9, 12, 9, 4},
					{5, 12, 15, 12, 5},
					{4, 9, 12, 9, 4},
					{2, 4, 5, 4, 2}
				};
	for (i = 0; i < 5; i++){
		for (j = 0; j < 5; j++){
			sum += ((int32_t)buffer[i][j] * (int32_t)kernel[i][j]);
		}
	}
	mpixel = (int32_t)(sum / 159);

	return (uint8_t)mpixel;
}

uint32_t isqrt(uint32_t a){
	uint32_t i, rem = 0, root = 0, divisor = 0;

	for (i = 0; i < 16; i++){
		root <<= 1;
		rem = ((rem << 2) + (a >> 30));
		a <<= 2;
		divisor = (root << 1) + 1;
		if (divisor <= rem){
			rem -= divisor;
			root++;
		}
	}
	return root;
}

uint8_t sobel(uint8_t buffer[3][3]){
	int32_t sum = 0, gx = 0, gy = 0;
	uint8_t i, j;

	int16_t kernelx[3][3] =	{	{-1, 0, 1},
					{-2, 0, 2},
					{-1, 0, 1},
				};
	int16_t kernely[3][3] =	{	{-1, -2, -1},
					{0, 0, 0},
					{1, 2, 1},
				};
	for (i = 0; i < 3; i++){
		for (j = 0; j < 3; j++){
			gx += ((int32_t)buffer[i][j] * (int32_t)kernelx[i][j]);
			gy += ((int32_t)buffer[i][j] * (int32_t)kernely[i][j]);
		}
	}

	sum = isqrt(gy * gy + gx * gx);

	if (sum > 255) sum = 255;
	if (sum < 0) sum = 0;

	return (uint8_t)sum;
}

void do_gaussian(uint8_t *input, uint8_t *output, int32_t width, int32_t height){
	int32_t i = 0, j = 0, k, l;
	uint8_t image_buf[5][5];

	for(i = 0; i < height; i++){
		if (i > 1 && i < height-2){
			for(j = 0; j < width; j++){
				if (j > 1 && j < width-2){
					for (k = 0; k < 5; k++)
						for(l = 0; l < 5; l++)
							image_buf[k][l] = input[(((i + l-2) * width) + (j + k-2))];

					output[((i * width) + j)] = gaussian(image_buf);
				}else{
					output[((i * width) + j)] = input[((i * width) + j)];
				}
			}
		}else{
			output[((i * width) + j)] = input[((i * width) + j)];
		}
	}
}

void do_sobel(uint8_t *input, uint8_t *output, int32_t width, int32_t height){
	int32_t i = 0, j = 0, k, l;
	uint8_t image_buf[3][3];

	for(i = 0; i < height; i++){
		if (i > 2 && i < height-3){
			for(j = 0; j < width-1; j++){
				if (j > 2 && j < width-3){
					for (k = 0; k < 3; k++)
						for(l = 0; l < 3; l++)
							image_buf[k][l] = input[(((i + l-1) * width) + (j + k-1))];

					output[((i * width) + j)] = sobel(image_buf);
				}else{
					output[((i * width) + j)] = 0;
				}
			}
		}else{
			output[((i * width) + j)] = 0;
		}
	}
}

void print_image(uint8_t *final_image){
		uint8_t i=0, k = 0, j=0;

		printf("\n\nint32_t width = %d, height = %d;\n", width, height);

		// Joga imagem na tela (hex dump).
		printf("uint8_t image[] = {\n");
		for (i = 0; i < height; i++){
			for (j = 0; j < width; j++){

				printf("0x%x", final_image[i * width + j]);
				if ((i < height-1) || (j < width-1)) printf(", ");
				if ((++k % 16) == 0) printf("\n");

			}
		}
		printf("};\n");

}

uint8_t next_data(uint8_t *bag, uint8_t offset, uint8_t *x, uint8_t *y, uint8_t *w, uint8_t *h){

	// Armazena informações sobre o bloco de dados
	struct data_info *d = (struct data_info*)(bag + offset);
	d->x = *x;
	d->y = *y;
	d->w = *w;
	d->h = *h;

	printf("%d %d %d %d", d->x, d->y, d->w, d->h);

	uint8_t diff = 0;

	*x += *w;

	if(*x >= width){
		*x = 0;
		// trata y
		*y += *h;
		*w = BLOCK_WIDTH;

		if(*y >= height){

		}else{
			 diff = height - *y;
			// Diferença é menor que um bloco.
			if(diff < 2*BLOCK_HEIGHT)
				*h = diff;
		}
	}else{
		diff = width - *x;
		// Diferença é menor que um bloco.
		if(diff < 2*BLOCK_WIDTH)
			*w = diff;
	}

}

void master(void){

	uint32_t i, j, k = 0;

	uint32_t time;
	uint16_t cpu, task, size, val;

	if(hf_comm_create(hf_selfid(), 1000, 0))
		panic(0xff);

	delay_ms(80);

	uint8_t *final_image = malloc(width * height);

	uint8_t *sub_image = malloc(width * height * 4 + sizeof(struct hdr_info));

	// Total de sub-imagens a serem processadas (total de blocos).
	uint8_t total_sub_imgs = (width / BLOCK_WIDTH) * (height / BLOCK_HEIGHT);

	printf("Total de sub-imagens para processamento: %d\n", total_sub_imgs);

	// Informações sobre localização dos dados = (x, y, w, h)
	printf("Criando de tamanho %d\n", total_sub_imgs * sizeof(struct data_info));
	uint8_t *bag = malloc(total_sub_imgs * sizeof(struct data_info));
	uint8_t bag_offset = 0;
	// Informações sobre quem está com qual dado. O índice corresponde a tarefa e o valor ao id do dado.
	uint8_t *bag_control[TOTAL_SLAVES+1];
	// Índice para o bloco de dados atual.
	uint8_t actual_data_block = 0;

	// Inicia contagem do tempo de execução.
	time = _readcounter();

	uint8_t x = 0, y = 0;
  uint8_t w = BLOCK_WIDTH, h = BLOCK_HEIGHT;

	// Envia primeira rajada de dados. Os próximos dados são enviado.
	for(i=1; i<=TOTAL_SLAVES; i++){

		if(actual_data_block < total_sub_imgs-1){

			memset(sub_image, 0, (h+4) * (w+4) + sizeof(struct hdr_info));

			struct hdr_info *hr = (struct hdr_info*)sub_image;
			hr->w = w;
			hr->h = h;

			// Envia dados a serem processados.
			printf("\n\nEnviando bloco %d para o escravo %d.\n", actual_data_block, i);

			// Faz o recorte da imagem.
			get_sub_matrix(image, sub_image + sizeof(struct hdr_info), height, width, y-2, x-2, h+4, w+4);
			matrix_print(sub_image + sizeof(struct hdr_info), h+4, w+4);

			val = hf_sendack(i, 1000, sub_image, (h+4) * (w+4) + sizeof(struct hdr_info), 0, 500);
			//if (val)
			//	printf("sender, hf_sendack(): error %d\n", val);

			// Atualiza constrole de qual tarefa está atribuido cada dado.
			bag_control[i] = actual_data_block;

			// Atualiza bag e variáveis.
			next_data(bag, bag_offset, &x, &y, &w, &h);

			actual_data_block++;
			bag_offset += sizeof(struct data_info);

		}
	}

	uint8_t received = 0;
	//matrix_print(final_image, height, width);

	// Loop principal. Espera por respostas e envia mais trabalho para os escravos.
	while(1) {

		printf("\n\nRecebendo bloco ...\n");

		// Recebe os dados processados de um escravo.
		val = hf_recvack(&cpu, &task, sub_image, &size, 0);
		if (val)
			printf("error %d\n", val);

		struct hdr_info *hr = (struct hdr_info*)sub_image;
		matrix_print(sub_image + sizeof(struct hdr_info), hr->h, hr->w);

		// Busca informações da localização da sub-imagem.
		uint8_t block_index = bag_control[cpu];
		struct data_info *dt_hr = (struct data_info*)(bag + block_index * sizeof(struct data_info));

		// Atribui a sub-imagem a sua devida posicao na imagem.
		set_sub_matrix(final_image, sub_image+sizeof(struct hdr_info), height, width, dt_hr->y, dt_hr->x, dt_hr->h, dt_hr->w);
		matrix_print(final_image, height, width);

		received++;
		// Verifica se já recebeu todos os pacotes necessários.
		if(received == total_sub_imgs){
			printf("Foram recebidos todos os bloco da imagem.\n");
			break;
		}

		printf("%d %d \n", actual_data_block, total_sub_imgs);

		if(actual_data_block < total_sub_imgs){

			// Se existe bloco a ser enviado.
			// Pega o próximo bloco disponívvel e envia para o escravo.
			printf("\n\nEnviando bloco %d para o escravo %d.\n", actual_data_block, cpu);

			hr = (struct hdr_info*)sub_image;
			hr->w = w;
			hr->h = h;

			// Faz o recorte da imagem.
			get_sub_matrix(image, sub_image + sizeof(struct hdr_info), height, width, y-2, x-2, h+4, w+4);
			matrix_print(sub_image + sizeof(struct hdr_info), h+4, w+4);

			val = hf_sendack(cpu, 1000, sub_image, (h+4) * (w+4) + sizeof(struct hdr_info), 0, 500);
			if (val)
				printf("sender, hf_sendack(): error %d\n", val);

			// Atualiza constrole de qual tarefa está atribuido cada dado.
			// O índice corresponde a tarefa e o valor ao id do dado.
			bag_control[cpu] = actual_data_block;

			// Atualiza bag e variáveis.
			next_data(bag, bag_offset, &x, &y, &w, &h);
			actual_data_block++;
			bag_offset += sizeof(struct data_info);
	}
}

// Finaliza o calculo do tempo de execução.
time = _readcounter() - time;
printf("done in %d clock cycles.\n\n", time);

// Desenha imagem na tela.
print_image(final_image);

printf("\n\nFim do processamento!\n");
panic(0);

}

void slave(void){
	uint32_t i;
	uint32_t time;
	uint16_t cpu, task, size, val;

	// Inicializa porta 1000.
	if (hf_comm_create(hf_selfid(), 1000, 0)){
		panic(0xff);
	}

	uint8_t image_size = height * width * 4;

	// Aloca espaço para recebimento dos dados.
	uint8_t *sub_image = malloc(image_size);

	// Aloca espaço de memória para as imagem intermediária.
	uint8_t *sub_image_aux = malloc(image_size);

	if (sub_image_aux == NULL){
		printf("\nmalloc() failed!\n");
		panic(0);
	}

	while(1) {

		// Recebe os dados.
		val = hf_recvack(&cpu, &task, sub_image, &size, 0);
		//if (val)
		//	printf("error %d\n", val);
		//else
		//	printf("[master] %s (%d bytes)\n", sub_image, size);

		// TODO Faz processamento dos dados.

		struct hdr_info *hr = (struct hdr_info*)sub_image;

		printf("Recebido bloco com dimensoes h = %d e w = %d\n", hr->h+4, hr->w+4);

		matrix_print(sub_image + sizeof(struct hdr_info), hr->h+4, hr->w+4);

		// Aplica filtros na imagem.
		// Note também o uso das imagens intermediárias.

		// TODO Aplica filtros na imagem.

		//do_gaussian(sub_image, sub_image_aux, width, height);
		//do_sobel(sub_image_aux, sub_image, width, height);

		uint8_t *temp_matrix = malloc((hr->h+4) * (hr->w+4) + sizeof(struct hdr_info));

		memset(temp_matrix, hf_selfid(), (hr->h+4) * (hr->w+4) + sizeof(struct hdr_info));

		memcpy(temp_matrix, sub_image, sizeof(struct hdr_info));

		// Retira as bordas da imagem.
		//get_sub_matrix(sub_image + sizeof(struct hdr_info), temp_matrix + sizeof(struct hdr_info), hr->h+4, hr->w+4, 2, 2, hr->h, hr->w);

		matrix_print(temp_matrix + sizeof(struct hdr_info), hr->h, hr->w);

		// Envia dados processados.
		val = hf_sendack(MASTER_NODE, 1000, temp_matrix, hr->h * hr->w + sizeof(struct hdr_info), 0, 500);
		//if (val){
		//	printf("sender, hf_sendack(): error %d\n", val);
		//}

	}

	free(sub_image_aux);
	panic(0);

}

void app_main(void) {
	if(hf_cpuid() == MASTER_NODE){
		hf_spawn(master, 0, 0, 0, "master", 2048);
	}else if(hf_cpuid() == SLV1_NODE){
		hf_spawn(slave, 0, 0, 0, "slave1", 2048);
	}else if(hf_cpuid() == SLV2_NODE){
		hf_spawn(slave, 0, 0, 0, "slave2", 2048);
	}else if(hf_cpuid() == SLV3_NODE){
		hf_spawn(slave, 0, 0, 0, "slave3", 2048);
	}else if(hf_cpuid() == SLV4_NODE){
		hf_spawn(slave, 0, 0, 0, "slave4", 2048);
	}else if(hf_cpuid() == SLV5_NODE){
		hf_spawn(slave, 0, 0, 0, "slave5", 2048);
	}
}
