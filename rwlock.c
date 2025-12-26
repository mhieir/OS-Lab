#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "rwlock.h"

void rwlock_init(struct rwlock *rw, char *name) {
  initlock(&rw->lk, "rwlock_internal");
  rw->name = name;
  rw->read_count = 0;
  rw->writing = 0;
}

void rwlock_acquire_read(struct rwlock *rw) {
  acquire(&rw->lk);
  while (rw->writing > 0) {
    sleep(rw, &rw->lk);
  }
  rw->read_count++;
  release(&rw->lk);
}

void rwlock_release_read(struct rwlock *rw) {
  acquire(&rw->lk);
  rw->read_count--;
  if (rw->read_count == 0) {
    wakeup(rw);
  }
  release(&rw->lk);
}

void rwlock_acquire_write(struct rwlock *rw) {
  acquire(&rw->lk);
  while (rw->writing > 0 || rw->read_count > 0) {
    sleep(rw, &rw->lk);
  }
  rw->writing = 1;
  release(&rw->lk);
}

void rwlock_release_write(struct rwlock *rw) {
  acquire(&rw->lk);
  rw->writing = 0;
  wakeup(rw);
  release(&rw->lk);
}