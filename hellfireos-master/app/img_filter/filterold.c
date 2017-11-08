#ifndef _FILTER_H_
#define _FILTER_H_

#include "image.h"

uint8_t gaussian(uint8_t buffer[5][5]){
	int32_t sum = 0, mpixel;
	uint8_t i, j;

	int16_t kernel[5][5] =	{	{2, 4, 5, 4, 2},
					{4, 9, 12, 9, 4},
					{5, 12, 15, 12, 5},
					{4, 9, 12, 9, 4},
					{2, 4, 5, 4, 2}
				};
	for (i = 0; i < 5; i++)
		for (j = 0; j < 5; j++)
			sum += ((int32_t)buffer[i][j] * (int32_t)kernel[i][j]);
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

// uint16_t myAtoi(uint8_t *str)
// {
//     uint16_t res = 0; // Initialize result
// 		uint32_t i = 0;
//
//     // Iterate through all characters of input string and
//     // update result
//     for (i; str[i] != '\0'; ++i)
//         res = res*10 + str[i] - '0';
//
//     // return result.
//     return res;
// }

void task_slave(void){
	uint32_t i, j, k = 0, sub_width, sub_height;
	uint8_t *sub_image, *img, *img2;
	uint32_t time;
	uint16_t cpu, task, size, val;

	sub_width = width;
	sub_height = height;

	printf("Teste3!!\n\n");

	uint8_t *port = (uint8_t *)hf_selfname();

	sub_image = (uint8_t *) malloc(sub_width * sub_height);

	if (hf_comm_create(hf_selfid(), 2000, 0))
		panic(0xff);

	delay_ms(50);

	while(1) {

		val = hf_recvack(&cpu, &task, sub_image, &size, 0);
		if (val)
			printf("error %d\n", val);
		else
			printf("[t3] %s (%d bytes)\n", sub_image, size);

		printf("Teste4!!\n\n");

		img = (uint8_t *) malloc(sub_height * sub_width);
		img2 = (uint8_t *) malloc(sub_height * sub_width);
		if (img == NULL || img2 == NULL){
			printf("\nmalloc() failed!\n");
			for(;;);
		}

		//printf("\n\nstart of processing!\n\n");

		time = _readcounter();

		do_gaussian(sub_image, img, sub_width, sub_height);
		do_sobel(img, img2, sub_width, sub_height);

		time = _readcounter() - time;

		printf("done in %d clock cycles.\n\n", time);

		/*
		printf("\n\nint32_t width = %d, height = %d;\n", sub_width, sub_height);
		printf("uint8_t image[] = {\n");
		for (i = 0; i < sub_height; i++){
			for (j = 0; j < sub_width; j++){
				printf("0x%x", img2[i * sub_width + j]);
				if ((i < sub_height-1) || (j < sub_width-1)) printf(", ");
				if ((++k % 16) == 0) printf("\n");
			}
		}
		printf("};\n");
*/

		val = hf_sendack(1, 1002, img2, size, 0, 500);
		if (val){
			printf("sender, hf_sendack(): error %d\n", val);
		}

		free(img);
		free(img2);

		printf("\n\nend of processing!\n");
		panic(0);
	}

}

void task_master(void){
	uint16_t i, j, k = 0;
	uint16_t block_height_std, block_width_std, block_height_rem, block_width_rem, current_height, current_width;
	uint16_t cpu, task, size, val;

	if(hf_comm_create(hf_selfid(), 1000, 0))
		panic(0xff);

	delay_ms(50);

	uint8_t *image2 = (uint8_t *) malloc((height + 4) * (width + 4));

	printf("Teste2!!\n\n");

	//set_matrix_borders(image, height, width, image2, height + 4, width + 4);

	printf("Teste7!!\n\n");

	block_height_std = height / NOC_HEIGHT;
	block_width_std = width / NOC_WIDTH;
	block_height_rem = block_height_std + height % NOC_HEIGHT;
	block_width_rem = block_width_std + width % NOC_WIDTH;

	int16_t w = 2;
  int16_t h = 2;

	/*
	for(i=0; i< NOC_WIDTH*NOC_HEIGHT; i++){
			if (i < NOC_WIDTH) {
				current_height = block_height_rem;
			} else {
				current_height = block_height_std;
			}

			if ((i + 1) % NOC_WIDTH == 0) {
				current_width = block_width_rem;
			} else {
				current_width = block_width_std;
			}
*/
			uint8_t *d = (uint8_t *)malloc(32 * 32);

			crop_sub_matrix(image2, d, height, width, h-2, w-2, 36, 36);
			//matrix_print(d, block_height+4, block_width+4);

			//if(i == 0){
			//	val = hf_sendack(i, 1001, image, (current_height * current_width), 0, 500);
			//} else{
			val = hf_sendack(1, 2000, image, (current_height * current_width), 0, 500);
			//}

			printf("Teste6!!\n\n");

			//printf("\n\n");

			w += current_width;

			if(w >= width){
					w = 2;
					h += current_height;

					if(h >= height){
						break;
					}
			}

	}

	//printf("Teste5!!\n\n");

	// val = hf_sendack(1, 2000, image, (height * width), 0, 500);
	/*
	for(i=0; i< NOC_WIDTH*NOC_HEIGHT; i++){
		if(i == 0){
			val = hf_recvack(&cpu, &task, d, &size, 0);
		} else if(i == 1){
			val = hf_recvack(&cpu, &task, d, &size, 0);
		} else{
			val = hf_recvack(&cpu, &task, d, &size, 0);
		}

		printf("Recebeu alguma coisa do %d!!\n\n", cpu);
		*/
		//set_sub_matrix(image, d, height, width, )

		//void set_sub_matrix(uint8_t *a, uint8_t *b, int16_t h, int16_t w, int16_t k, int16_t l, int16_t bh, int16_t bw);


	//}

	printf("Resultado!!\n\n");

	printf("\n\nint32_t width = %d, height = %d;\n", width, height);
	printf("uint8_t image[] = {\n");
	for (i = 0; i < height; i++){
		for (j = 0; j < width; j++){
			printf("0x%x", image[i * width + j]);
			if ((i < height-1) || (j < width-1)) printf(", ");
			if ((++k % 16) == 0) printf("\n");
		}
	}
	printf("};\n");
}

void app_main(void) {
	int8_t buf[20];
	sprintf(buf, "%d", (hf_cpuid()*1000));

	if (hf_cpuid() == 0){
		hf_spawn(task_master, 0, 0, 0, "master", 2048);
	}
	else if (hf_cpuid() == 1){
		hf_spawn(task_slave, 0, 0, 0, "2000", 2048);
	}
}

#endif
