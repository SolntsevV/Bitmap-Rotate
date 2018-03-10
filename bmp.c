#include "bmp.h"
#include "malloc.h"
#include "string.h"

image_t* read_bmp_image(const char *filename){
	FILE* fp;
	image_t* image;
	uint8_t *data_bmp = NULL;

	image = malloc(sizeof(image_t));
	
	/*проверка удалось ли прочитать файл*/
	if ((fp = fopen(filename, "rb")) == NULL)
		return NULL;
	
	/*считываем в структуру заголовка, если она пустая, то выходим */
	if (fread(&(image->header), sizeof(bmp_header_t), 1, fp) < 1) {
		fclose(fp);
		return NULL;
	}
	
	/* проверка заголовков изображения */
	if(validate_header(image->header) != 0) {
		fclose(fp);
		return NULL;
	}
	
	/*
	stream – указатель на управляющую таблицу потока данных. 
	offset – смещение позиции. 
	whence – точка отсчета смещения.
	
	0 – при успешной установки позиции. 
	*/
	if(fseek(fp, image->header.bOffBits, SEEK_SET) != 0){
		fclose(fp);
		return NULL;
	}

	data_bmp = malloc(image->header.biSizeImage*sizeof(pixel_t));

	if (fread(data_bmp, sizeof(uint8_t), image->header.biSizeImage, fp) < image->header.biSizeImage) {
		free(data_bmp);
		fclose(fp);
		return NULL;
	}

  image->data = deserialize_data(data_bmp, image->header.biHeight, image->header.biWidth);
  free(data_bmp);

  if (image->data == NULL) {
    fclose(fp);
    return NULL;
  }
  fclose(fp);
  return image;
}

pixel_t** deserialize_data(const uint8_t* data_bmp, uint32_t height, uint32_t width){
  uint32_t i;
  uint32_t padding = padding_size(width);
  uint32_t tmp_width = width + padding;
  pixel_t** data;
  if ((data = create_data(height, width)) == NULL)
    return NULL;
  for (i = 0; i < height; i++) {
    unsigned long offset = i*tmp_width*sizeof(pixel_t);
    memcpy(data[i], data_bmp+offset, width*sizeof(pixel_t));
  }
  return data;
}

pixel_t** create_data(uint32_t height, uint32_t width){
  uint32_t i;
  pixel_t** data;
  if ((data = malloc(height * sizeof(pixel_t*))) == NULL) {
    return NULL;
  }
  for (i = 0; i < height; i++) {
    if ((data[i] = malloc(width * sizeof(pixel_t))) == NULL){
      free_data(data, height);
      return NULL;
    }
  }
  return data;
}

uint8_t padding_size(uint32_t width){
  unsigned long tmp_width = width*sizeof(pixel_t);
  if (tmp_width%PADDING_SIZE == 0)
    return 0;
  return PADDING_SIZE-(tmp_width%PADDING_SIZE);
}

void free_data(pixel_t** data, uint32_t height){
  uint32_t i;
  for (i = 0; i < height; i++) {
    free(data[i]);
  }
  free(data);
}

int validate_header(bmp_header_t header){
  if (header.bfType != 0x4d42)
  {
    return -1;
  }
  /* проверка сколькоими битами кодируется изображение */
  if (header.biBitCount != COLOR_DEPTH)
  {
    return -1;
  }
  return 0;
}

int rotate_bmp_image(image_t* image, direction_t direction){
	uint32_t newwidth;
	uint32_t newheight;
	pixel_t** tmp;
	
	if(image == NULL)
		return -1;

	  newwidth = image->header.biHeight;
	  newheight = image->header.biWidth;

	  tmp = rotate_data((const pixel_t**)image->data,
				  image->header.biHeight, image->header.biWidth, direction);
	  if (tmp == NULL)
		return -1;

	  free_data(image->data, image->header.biHeight);
	  image->header.biWidth = newwidth;
	  image->header.biHeight = newheight;
	  image->data = tmp;
	  return 0;
}

pixel_t** rotate_data(const pixel_t** data,
              uint32_t height, uint32_t width,direction_t direction){
  uint32_t i;
  uint32_t j;
  uint32_t newwidth = height;
  uint32_t newheight = width;
  pixel_t** tmp;

  if ((tmp = create_data(newheight, newwidth)) == NULL) {
    return NULL;
  }

  for (i = 0; i < newheight; i++) {
    for (j = 0; j < newwidth; j++) {
        if(direction == RIGHT)
          tmp[i][j] = data[j][width - 1 - i];
        else
          tmp[i][j] = data[height - 1 - j][i];
    }
  }
  return tmp;
}


int write_bmp_image(const char* filename, const image_t* image){
	
  FILE* fp;
  uint8_t *data_bmp;

  if(image == NULL)
    return -1;

  if ((fp = fopen(filename, "wb")) == NULL)
    return -1;
	
	/* записываем заголово*/
  if (fwrite(&(image->header), sizeof(bmp_header_t), 1, fp) < 1){
    fclose(fp);
    return -1;
  }

  if(fseek(fp, image->header.bOffBits, SEEK_SET) != 0){
    fclose(fp);
    return -1;
  }

  data_bmp = serialize_data((const pixel_t**)image->data, image->header.biHeight, image->header.biWidth);

  if (fwrite(data_bmp, sizeof(uint8_t), image->header.biSizeImage, fp) < image->header.biSizeImage)
  {
    fclose(fp);
    return -1;
  }
  
  fclose(fp);
  return 0;
}

uint8_t* serialize_data(const pixel_t** data, uint32_t height, uint32_t width){
  uint32_t i;
  uint32_t padding = padding_size(width);
  uint32_t tmp_width = width + padding;
  uint8_t pbuf[4] = {0};
  uint8_t* data_bmp = malloc(height * width * sizeof(pixel_t) + height * padding);
  for (i = 0; i < height; i++) {
    unsigned long offset = i * tmp_width * sizeof(pixel_t);
    memcpy(data_bmp + offset, data[i], width * sizeof(pixel_t));
    memcpy(data_bmp + offset + width, pbuf, padding);
  }
  return data_bmp;
}