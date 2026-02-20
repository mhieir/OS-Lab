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

static uint highlight_for_selection = 0;

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
  } 
  else if(c == KEY_RT) {
    if(pos % 80 != 79){
      pos++;
    }
  } 
  else if(c == KEY_LF) {
    if(pos > 0){
      --pos;
    }
  }
  else{
    if(highlight_for_selection){
      ushort current =  (c&0xff) | 0x0700;
      crt[pos++] =  (current & 0x00FF) | ((current & 0xFF00) ^ 0x7700);
    }
    else
      crt[pos++] = (c&0xff) | 0x0700;  // black on white
  }

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
  uint rtil; // read till
} input_state_t;

static input_state_t input;

#define C(x)  ((x)-'@')  // Control-x
//-------------------------------------------------------curser-movements------------------------------------------------------------
static input_state_t undo_history[MAX_UNDO_HISTORY];
static int hist_top = -1;
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
redraw_from_cursor(uint len)
{
  for (uint i = input.cur; i < input.e; i++)
     consputc(input.buf[i % INPUT_BUF]);
  for (uint i = 0; i < len; i++)
    consputc(' ');
      
  for (uint i = input.e + len - 1; i >= input.cur + 1; i--)
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
    redraw_from_cursor(1);
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
    consputc(KEY_RT);
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
//-------------------------------------------------------control +Z history-----------------------------------------------------------
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

//-------------------------------------------------------copy-pasting-----------------------------------------------------------------
static int selecting = 0;
static uint sel_start = 0;
static uint sel_end = 0;
static char clipboard[INPUT_BUF];


static void reach_graphic_cursor_to(uint reaching_point){
  if(input.cur > reaching_point)
    while( input.cur > reaching_point){
      consputc(KEY_LF);
      input.cur--;
    }
  else
    while(input.cur < reaching_point){
      consputc(KEY_RT);
      input.cur++;
  }
}

static void clear_selection(void) {
  if(selecting){
    uint old_cur = input.cur;
    reach_graphic_cursor_to(sel_start); 
    for(uint i = sel_start; i <= sel_end; i++){
      consputc(input.buf[i % INPUT_BUF]);
      input.cur++;
    }
    reach_graphic_cursor_to(old_cur);
  }
  selecting = 0;
  sel_start = sel_end = 0;
}


static void start_or_update_selection(void) {
  if (selecting == 0) {
    selecting = 1;
    sel_start = input.cur;
    sel_end = input.cur;
  } 
  else if(selecting == 1){
    sel_end = input.cur;
    selecting = 2;
    if (sel_start > sel_end) {
      sel_start ^= sel_end;
      sel_end ^= sel_start;
      sel_start ^= sel_end;
    }

    uint old_cur = input.cur;
    reach_graphic_cursor_to(sel_start);

    highlight_for_selection = 1;
    for (uint i = sel_start; i <= sel_end; i++) {
      consputc(input.buf[i % INPUT_BUF]);
      input.cur++;
    }
    highlight_for_selection = 0;
    reach_graphic_cursor_to(old_cur);
  }
  else{
    clear_selection();
  }
}


static void copy_selection(void) {
  if (selecting) {
    uint len = sel_end - sel_start + 1;
    for (uint i = 0; i < len /*&& i <= INPUT_BUF - 1*/; i++)
      clipboard[i] = input.buf[(sel_start + i) % INPUT_BUF];
    clipboard[len] = '\0';
  }
}

static void delete_selection(void) {
  if (selecting) {
    uint len = sel_end - sel_start+1;
    for (uint i = sel_start; i <= sel_end  && i < input.e; i++)
      input.buf[i % INPUT_BUF] = input.buf[(i + len) % INPUT_BUF];
    

    reach_graphic_cursor_to(sel_start);
    input.cur = sel_start;
    input.e -= len;
  
    redraw_from_cursor(len);
    clear_selection();
  }
}

static void paste_clipboard(void) {
  if (clipboard[0] != '\0') {
    for (uint i = 0; clipboard[i]!= '\0' /*&& input.e - input.r < INPUT_BUF*/; i++) {
      insert_char_at_cursor(clipboard[i]);
    }
  }
}

int cnt_tab = 0;
void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;
  int modified = 0;

  acquire(&cons.lock);
  while ((c = getc()) >= 0) {
    modified = 0; 
    if(c != '\t')
      cnt_tab = 0;
    switch (c) {
    case C('P'):
      doprocdump = 1;
      break;

    case C('U'): // Kill line
      kill_line();
      modified = 1;
      break;

    case C('H'): case BACKSPACE:
      if (selecting) {
        delete_selection();
        modified = 1;
      } else {
        handle_backspace();
        modified = 1;
      }
      break;

    case C('Z'):
      if(selecting)
        clear_selection();
      else
        undo_last_change();
      break;

    case KEY_LF:
      cursor_left();
      break;

    case KEY_RT:
      cursor_right();
      break;

    case C('D'):
      if(selecting)
        clear_selection();
      else
        cursor_next_word();
      break;

    case C('A'):
      if(selecting)
        clear_selection();
      else
        cursor_prev_word();
      break;
    case C('S'):
      start_or_update_selection();
      break;

    case C('C'):
      copy_selection();
      break;

    case C('V'):
      if(selecting)
        delete_selection();
      paste_clipboard();
      modified = 1;
      break;
    case '\t': // Tab key
      cnt_tab++;
      if(input.cur == input.e){
        if(cnt_tab == 1){
          input.buf[input.e++ % INPUT_BUF] = c;
          input.rtil = input.e;
        }
        else{
          input.buf[input.e++ % INPUT_BUF] = c;
          input.rtil = input.w;
          input.r = input.w;
        }
        // crt[0] = (('0' + input.r)&0xff) | 0x0700;
        // crt[1] = (('0' + input.w)&0xff) | 0x0700;
        // crt[2] = (('0' + input.cur)&0xff) | 0x0700;
        // crt[3] = (('0' + input.e)&0xff) | 0x0700;
        // crt[4] = (('0' + input.rtil)&0xff) | 0x0700;
        // crt[5] = ((c)&0xff) | 0x0700;
        wakeup(&input.r);
      }
      break;
    default:
    if (c != 0 && input.e - input.r < INPUT_BUF) {
      
      if(selecting)
      delete_selection();
        insert_char_at_cursor(c);
        modified = 1;

        if (c == '\n' || input.e == input.r + INPUT_BUF) {
          input.w = input.e;
          input.rtil = input.e;
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
    while(input.r == input.rtil){
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
    // if(c == '\t')
    //   continue;
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}
int tabcomplete = 0;
int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;
  char c;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++){
    c = buf[i] & 0xff;
    if(tabcomplete == 0 && c == '\t') {
      tabcomplete = 1;
      input.e--; // for the tab that is at the end of the buffer now
      continue;
    }
    if(tabcomplete == 1 && c != '\t') {
      crt[0] = (('0' + input.r)&0xff) | 0x0700;
      crt[1] = (('0' + input.w)&0xff) | 0x0700;
      crt[2] = (('0' + input.cur)&0xff) | 0x0700;
      crt[3] = (('0' + input.e)&0xff) | 0x0700;
      insert_char_at_cursor(c);
      continue;
    }
    if(tabcomplete == 1 && c == '\t') {
      crt[5] = (('-')&0xff) | 0x0700;
      tabcomplete = 0;
      input.r = input.w;
      input.rtil = input.w;
    
      continue;
    } 
    else {
      consputc(c);
    }
  }
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

