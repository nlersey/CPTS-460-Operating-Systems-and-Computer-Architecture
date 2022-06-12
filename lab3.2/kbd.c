#include "keymap"
#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10
#define CAPSLOCK 0x58
#define LSHIFT 0x12
#define RSHIFT 0x59
#define LCTRL 0x14
#define RCTRL 0xE0

extern int kputc(char c);


typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;
int count;
// state variables
int shifted = 0; 
int release = 0;
int control = 0;
int capslock = 0;

int kbd_init()
{
  char scode;

  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10;    // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;       // ARM manual says clock=8
  kp->head = kp->tail = 0;       // circular buffer char buf[128]
  kp->data = 0; kp->room = 128;

  count = 0;  // number of KBD interrupts
  release = 0;
}

void kbd_handler()
{
  u8 scode, c;

  KBD *kp = &kbd;
  color = YELLOW;
  scode = *(kp->base + KDATA);  // get scan code from KDATA reg => clear IRQ


  if (scode == 0xF0){       // key release 
     release = 1;           // set flag
     return;
  }

  if ( (scode == 0x12 || scode == 0x59) && (shifted == 1) ) {       // shift pressed
    shifted = 0;
    release = 0;
    return;
  }

  if ( (scode == 0x14) && (control == 1) ) {       // Control pressed
    control = 0;
    release = 0;
    return;
  }

  if (scode == 0x12 || scode == 0x59){
    shifted = 1;
    return;
  }

  if (scode == 0x14 ){
    control = 1;
    return;
  }

  if (release && scode){    // next scan code following key release
     release = 0;           // clear flag 
     return;
  }

  if (scode == CAPSLOCK) {
    capslock = !capslock;
    return;
  }

  if (control && (scode == 0x21)){ //Ctrl + C
    printf("CTRL C\n");
  }

  if (control && (scode == 0x23)){ //Ctrl + D
    printf("CTRL D\n");
    c = 0x04; 
  }

  if ((shifted && scode)||capslock){
    c = utab[scode];
  }
  else{
    c = ltab[scode];
  }

  // Store char in buf[] for task to get

 if (kp->data == 128) // buf FULL, ignore current key
   return;
 if (c != '\r')
   printf("kbd interrupt: c=%x %c\n", c, c);
  kp->buf[kp->head++] = c;
  kp->head %= 128;

  kp->data++;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();                          // unmask IRQ in case it was masked out
  //while(kp->data == 0);              // BUSY wait while kp->data is 0 
  while(kp->data <= 0); // wait for data; READONLY

  lock();                            // mask out IRQ
    c = kp->buf[kp->tail++]; 
    kp->tail %= 128;                 /*** Critical Region ***/
    kp->data--; kp->room++;
    unlock();                        // unmask IRQ
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    *s = c;
    s++;
  }
  *s = 0;
}

