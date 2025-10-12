// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "kbd.h"
static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

char get_char_at(int pos)
{
  if(pos < 0 || pos >= 25*80)
    return ' ';
  return crt[pos] & 0xFF;
}


int find_word_start(int current_pos)
{
  int pos = current_pos;
  int line_start = pos - (pos % 80);
  if(pos == line_start) return pos;
  
  while(pos > line_start && get_char_at(pos - 1) == ' ') {
    pos--;
  }
  
  while(pos > line_start && get_char_at(pos - 1) != ' ') {
    pos--;
  }
  
  return pos;
}


static void
cgaputc(int c)
{
  int pos;

  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);


  crt[pos] ^= 0x7700;

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
     crt[--pos] = ' ' | 0x0700;
  } else if(c == KEY_RT) {
    if(pos % 80 != 79){
      pos++;
    }
  } else if(c == KEY_LF) {
    if(pos > 0){
      --pos;
  } else if(c == C('D')) {
    // pos = find_word_start(pos);
  } 
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  ushort current = crt[pos];
  crt[pos] = (current & 0x00FF) | ((current & 0xFF00) ^ 0x7700);
  // crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

#define INPUT_BUF 128
#define MAX_UNDO_HISTORY 32
typedef struct {
  char buf[INPUT_BUF];
  uint r;
  uint w;
  uint e;
  uint cur;
} input_state_t;

static input_state_t input;
static input_state_t undo_history[MAX_UNDO_HISTORY];
static int hist_top = -1;

#define C(x)  ((x)-'@')  // Control-x

static void
shift_right(uint start, uint end)
{
  for (uint i = end; i > start; i--)
    input.buf[i % INPUT_BUF] = input.buf[(i - 1) % INPUT_BUF];
}

static void
shift_left(uint start, uint end)
{
  for (uint i = start; i < end - 1; i++)
    input.buf[i % INPUT_BUF] = input.buf[(i + 1) % INPUT_BUF];
}

static void
redraw_from_cursor(void)
{
  for (uint i = input.cur; i < input.e; i++)
    consputc(input.buf[i % INPUT_BUF]);
  consputc(' ');
  for (uint i = input.e; i >= input.cur + 1; i--)
    consputc(KEY_LF);
  consputc(KEY_LF);
}

static void
handle_backspace(void)
{
  if (input.cur > input.w) {
    shift_left(input.cur - 1, input.e);
    input.cur--;
    input.e--;
    consputc(BACKSPACE);
    redraw_from_cursor();
  }
}

static void
cursor_left(void)
{
  if (input.cur > input.w) {
    consputc(KEY_LF);
    input.cur--;
  }
}

static void
cursor_right(void)
{
  if (input.cur < input.e) {
    consputc(input.buf[input.cur % INPUT_BUF]);
    input.cur++;
  }
}

static void 
cursor_end_line(void) {
  while (input.cur < input.e) {
    consputc(input.buf[input.cur % INPUT_BUF]);
    input.cur++;
  }
}

static void 
cursor_prev_word(void) {
  while (input.cur > input.w && input.buf[(input.cur - 1) % INPUT_BUF] == ' ') {
    consputc(KEY_LF);
    input.cur--;
  }

  while (input.cur > input.w && input.buf[(input.cur - 1) % INPUT_BUF] != ' ') {
    consputc(KEY_LF);
    input.cur--;
  }
}

static void 
cursor_next_word(void) {
  while (input.cur < input.e && input.buf[input.cur % INPUT_BUF] != ' ')
  {
    consputc(input.buf[input.cur % INPUT_BUF]);
    input.cur++;
  }
  while (input.cur < input.e && input.buf[input.cur % INPUT_BUF] == ' ')
  {
    consputc(input.buf[input.cur % INPUT_BUF]);
    input.cur++;
  }
}

static void
insert_char_at_cursor(int c)
{
  if (c == '\r')
    c = '\n';

  if (input.cur == input.e) {
    input.buf[input.e++ % INPUT_BUF] = c;
    consputc(c);
    input.cur++;
    return;
  }


  for (uint i = input.e; i > input.cur; i--) {
    input.buf[i % INPUT_BUF] = input.buf[(i - 1) % INPUT_BUF];
  }

  input.buf[input.cur % INPUT_BUF] = c;
  consputc(c);
  input.e++;
  input.cur++;

  for (uint i = input.cur; i < input.e; i++) {
    consputc(input.buf[i % INPUT_BUF]);
  }

  for (uint i = input.e; i > input.cur; i--) {
    consputc(KEY_LF);
  }
}

static void
kill_line(void) {
  cursor_end_line();
  while (input.e != input.w &&
      input.buf[(input.e - 1) % INPUT_BUF] != '\n') {
    input.e--;
    input.cur = input.e;
    consputc(BACKSPACE);
  }
}


static void
clear_history(void) {
  hist_top = 0;
  memset(undo_history[0].buf, 0, INPUT_BUF);
  undo_history[0].e = input.e;
  undo_history[0].w = input.w;
  undo_history[0].cur = input.cur;
}

static void 
save_snapshot(void) {
  if (hist_top + 1 < MAX_UNDO_HISTORY) {
    hist_top++;
    memmove(undo_history[hist_top].buf, input.buf, INPUT_BUF);
    undo_history[hist_top].e = input.e;
    undo_history[hist_top].cur = input.cur;
  } else {
    for (int i = 0; i < MAX_UNDO_HISTORY - 1; i++) {
      memmove(undo_history[i].buf, undo_history[i + 1].buf, INPUT_BUF);
      undo_history[i].e = undo_history[i + 1].e;
      undo_history[i].cur = undo_history[i + 1].cur;
    }
    memmove(undo_history[MAX_UNDO_HISTORY - 1].buf, input.buf, INPUT_BUF);
    undo_history[MAX_UNDO_HISTORY - 1].e = input.e;
    undo_history[MAX_UNDO_HISTORY - 1].cur = input.cur;
    hist_top = MAX_UNDO_HISTORY - 1;
  }
}


static void 
undo_last_change(void) {
  if (hist_top < 0)
    return;
  kill_line();
  memmove(input.buf, undo_history[hist_top].buf, INPUT_BUF);
  input.e = undo_history[hist_top].e;
  input.cur = undo_history[hist_top].cur;
  hist_top--;

  for (uint i = input.w; i < input.e; i++) {
    consputc(input.buf[i % INPUT_BUF]);
  }
   for (uint i = input.e; i > input.cur; i--)
    consputc(KEY_LF);
}

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;
  int modified = 0;

  acquire(&cons.lock);
  while ((c = getc()) >= 0) {
    modified = 0; 
    switch (c) {
    case C('P'):
      doprocdump = 1;
      break;

    case C('U'): // Kill line
      kill_line();
      modified = 1;
      break;

    case C('H'): case BACKSPACE:
      handle_backspace();
      modified = 1;
      break;

    case C('Z'):
      undo_last_change();
      break;

    case KEY_LF:
      cursor_left();
      break;

    case KEY_RT:
      cursor_right();
      break;

    case C('D'):
      cursor_next_word();
      break;

    case C('A'):
      cursor_prev_word();
      break;

    default:
      if (c != 0 && input.e - input.r < INPUT_BUF) {
        insert_char_at_cursor(c);
        modified = 1;

        if (c == '\n' || input.e == input.r + INPUT_BUF) {
          input.w = input.e;
          input.cur = input.e;
          clear_history();
          modified = 0;
          wakeup(&input.r);
        }
      }
      break;
    }

    if (modified) {
      save_snapshot();
    }
  }
  release(&cons.lock);

  if (doprocdump) {
    procdump();
  }
}


int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

