#include "stdint.h"

#define PADDING_SIZE 4
#define COLOR_DEPTH 24

/* направления поворота */
typedef enum {
  RIGHT,
  LEFT
} direction_t;

#pragma pack(push, 1) /* Помещает текущее значение выравнивания упаковки во внутренний стек компилятора */
/* структура заголовочного файла */
typedef struct {
  uint16_t bfType; /* Отметка для отличия формата от других (сигнатура формата) 4D42 */
  uint32_t bfileSize; /* Размер файла в байтах */
  uint32_t bfReserved; /* Зарезервировано и должен содержать ноль */
  uint32_t bOffBits; /* Положение пиксельных данных относительно начала данной структуры (в байтах) */
  uint32_t biSize; /* количество байтов, требуемых структурой */
  uint32_t biWidth; /* ширина изображения в пикселях */
  uint32_t biHeight; /* высота изображения в пикселях */
  uint16_t biPlanes; /* В BMP допустимо только значение 1 */
  uint16_t biBitCount; /* сколько битов используется для кодирования картинки */
  uint32_t biCompression; /*Тип сжатия для сжатого растрового изображения */
  uint32_t biSizeImage; /* размер изображения в байтах */
  uint32_t biXPelsPerMeter; /* Горизонтальное разрешение в пикселях на метр */
  uint32_t biYPelsPerMeter; /* Горизонтальное разрешение в пикселях на метр */
  uint32_t biClrUsed; /* Количество индексов цвета в таблице цветов, которые фактически используются изображением */
  uint32_t biClrImportant; /* Количество индексов цвета, необходимых для отображения изображения */
} bmp_header_t;


/* структура пикселя*/
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixel_t;

/* структура изображения */
typedef struct {
  bmp_header_t header;
  pixel_t** data;
} image_t;

enum write_status {
	READ_ERROR = 5,
	ROTATION_ERROR,
	WRITE_ERROR
};

image_t* read_bmp_image(const char*);
int write_bmp_image(const char*, const image_t*);
uint8_t padding_size(uint32_t);
void free_data(pixel_t**, uint32_t);
int rotate_bmp_image(image_t*, direction_t);
pixel_t** rotate_data(const pixel_t**,uint32_t, uint32_t,direction_t);
pixel_t** create_data(uint32_t, uint32_t);
uint8_t* serialize_data(const pixel_t**, uint32_t, uint32_t);
pixel_t** deserialize_data(const uint8_t*, uint32_t, uint32_t);
int validate_header(bmp_header_t);