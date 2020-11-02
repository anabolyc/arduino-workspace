#ifndef _MAIN_H
#define _MAIN_H

void setup(void);
void loop(void);


// void * operator new(size_t size);
// void operator delete(void * ptr);

// void * operator new(size_t size)
// {
//   return malloc(size);
// }

// void operator delete(void * ptr)
// {
//   free(ptr);
// }

int main(void)
{
  setup();
  for (;;)
  {
    loop();
  }
}

#endif