#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306.h"
#include "Adafruit_INA219.h"


void * operator new(size_t size);
void operator delete(void * ptr);

void * operator new(size_t size)
{
  return malloc(size);
}

void operator delete(void * ptr)
{
  free(ptr);
}