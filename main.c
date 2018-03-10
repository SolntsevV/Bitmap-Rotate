#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "malloc.h"
#include "bmp.h"

int main() {
	
	image_t* image;
	direction_t direction;
	int direction_inp;
	const char* filename = "test_empty.bmp";
	
	printf("Enter direction of turn:\n1 - Right\n2-Left\n");
	scanf("%d", &direction_inp);
	
	if(direction_inp == 1)
		direction = RIGHT;
	else
		direction = LEFT;
	
	image = read_bmp_image(filename);
	
	if(image == NULL) {
		printf("Could not read file\n");
		return READ_ERROR;
	}
	
	if(rotate_bmp_image(image, direction) == -1){
		fputs("Could not rotate picture.\n", stderr);
		return ROTATION_ERROR;
	}

	if(write_bmp_image(filename, image) == -1){
		fputs("Could not write picture to disk.\n", stderr);
		return WRITE_ERROR;
	}
	
	return 0;
}