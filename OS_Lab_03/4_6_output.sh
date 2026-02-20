4_6_test
Tick: PID 3,cpu : Even ,ticks_run 1, ticks =599 
Tick: PID 3,cpu : Even ,ticks_run 2, ticks =600 
Tick: PID 3,cpu : Even ,ticks_run 3, ticks =601 
YIELD: PID 3,

Tick: PID 3,cpu : Even ,ticks_run 1, ticks =602 
--- Process Information ---
--------------------------------------
PID: 1 STATUS: SLEEPING  PARENT: 0 NAME: init ELAPSED(ticks): 0 CPU: 0 Algorithm: RR
PID: 2 STATUS: SLEEPING  PARENT: 1 NAME: sh ELAPSED(ticks): 558 CPU: 0 Algorithm: RR
PID: 3 STATUS: RUNNING   PARENT: 2 NAME: 4_6_test ELAPSED(ticks): 8 CPU: 0 Algorithm: RR
Tick: PID 3,cpu : Even ,ticks_run 2, ticks =603 
Tick: PID 3,cpu : Even ,ticks_run 3, ticks =604 
YIELD: PID 3,
--------------------------------------


--- Process Information ---
--------------------------------------
PID: 1 STATUS: SLEEPING  PARENT: 0 NAME: init ELAPSED(ticks): 0 CPU: 0 Algorithm: RR
PID: 2 STATUS: SLEEPING  PARENT: 1 NAME: sh ELAPSED(ticks): 561 CPU: 0 Algorithm: RR
PID: 3 STATUS: RUNNING   PARENT: 2 NAME: 4_6_test ELAPSED(ticks): 11 CPU: 0 Algorithm: RR
PID: 4 STATUS: RUNNABLE  PARENT: 3 NAME: 4_6_test ELAPSED(ticks): 0 CPU: 0 Algorithm: RR
Tick: PID 3,cpu : Even ,ticks_run 1, ticks =606 
--------------------------------------

Tick: PID 3,cpu : Even ,ticks_run 2, ticks =607 
Tick: PID 3,cpu : Even ,ticks_run 3, ticks =608 
YIELD: PID 3,
Tick: PID 4,cpu : Even ,ticks_run 1, ticks =610 
Tick: PID 4,cpu : Even ,ticks_run 2, ticks =611 
Tick: PID 4,cpu : Even ,ticks_run 3, ticks =612 
YIELD: PID 4,

Tick: PID 3,cpu : Even ,ticks_run 1, ticks =613 
Tick: PID 3,cpu : Even ,ticks_run 2, ticks =614 
--- Process Information ---
--------------------------------------
PID: 1 STATUS: SLEEPING  PARENT: 0 NAME: init ELAPSED(ticks): 0 CPU: 0 Algorithm: RR
PID: 2 STATUS: SLEEPING  PARENT: 1 NAME: sh ELAPSED(ticks): 570 CPU: 0 Algorithm: RR
PID: 3 STATUS: RUNNING   PARENT: 2 NAME: 4_6_test ELAPSED(ticks): 20 CPU: 0 Algorithm: RR
PID: 4 STATUS: RUNNABLE  PARENT: 3 NAME: 4_6_test ELAPSED(ticks): 9 CPU: 0 Algorithm: RR
PID: 5 STATUS: RUNNABLE  PARENT: 3 NAME: 4_6_test ELAPSED(ticks): 8 CPU: 0 Algorithm: RR
--------------------------------------
Tick: PID 3,cpu : Even ,ticks_run 3, ticks =615 
YIELD: PID 3,
Tick: PID 4,cpu : Even ,ticks_run 1, ticks =617 
Tick: PID 4,cpu : Even ,ticks_run 2, ticks =618 
Tick: PID 4,cpu : Even ,ticks_run 3, ticks =619 
YIELD: PID 4,
process with ID: 5, migrated to CPU: 1

Tick: PID 3,cpu : Even ,ticks_run 1, ticks =621 
Tick: PID 3,cpu : Even ,ticks_run 2, ticks =622 
Tick: PID 5,cpu : Odd ,ticks_run 1, ticks =622 
Tick: PID 5,cpu : Odd ,ticks_run 2, ticks =622 
Tick: PID 4,cpu : Even ,ticks_run 1, ticks =623 
Tick: PID 4,cpu : Even ,ticks_run 2, ticks =624 
Tick: PID 5,cpu : Odd ,ticks_run 3, ticks =624 
Tick: PID 4,cpu : Even ,ticks_run 3, ticks =625 
YIELD: PID 4,
Tick: PID 5,cpu : Odd ,ticks_run 4, ticks =626 
Tick: PID 6,cpu : Even ,ticks_run 1, ticks =627 
Tick: PID 5,cpu : Odd ,ticks_run 5, ticks =627 
Tick: PID 6,cpu : Even ,ticks_run 2, ticks =628 
Tick: PID 6,cpu : Even ,ticks_run 3, ticks =629 
YIELD: PID 6,
Tick: PID 5,cpu : Odd ,ticks_run 6, ticks =628 
Tick: PID 5,cpu : Odd ,ticks_run 7, ticks =629 
Tick: PID 4,cpu : Even ,ticks_run 1, ticks =630 
Tick: PID 5,cpu : Odd ,ticks_run 8, ticks =630 
Tick: PID 4,cpu : Even ,ticks_run 2, ticks =631 
Tick: PID 4,cpu : Even ,ticks_run 3, ticks =632 
YIELD: PID 4,
Tick: PID 5,cpu : Odd ,ticks_run 9, ticks =632 
Tick: PID 6,cpu : Even ,ticks_run 1, ticks =633 
Tick: PID 5,cpu : Odd ,ticks_run 10, ticks =633 
Tick: PID 6,cpu : Even ,ticks_run 2, ticks =634 
Tick: PID 5,cpu : Odd ,ticks_run 11, ticks =634 
Tick: PID 5,cpu : Odd ,ticks_run 12, ticks =634 
Tick: PID 5,cpu : Odd ,ticks_run 13, ticks =634 
Tick: PID 6,cpu : Even ,ticks_run 3, ticks =635 
Tick: PID 5,cpu : Odd ,ticks_run 14, ticks =635 
YIELD: PID 6,
Tick: PID 5,cpu : Odd ,ticks_run 15, ticks =635 
Tick: PID 5,cpu : Odd ,ticks_run 16, ticks =636 
Tick: PID 4,cpu : Even ,ticks_run 1, ticks =637 
Tick: PID 5,cpu : Odd ,ticks_run 17, ticks =637 
Tick: PID 4,cpu : Even ,ticks_run 2, ticks =638 
Tick: PID 5,cpu : Odd ,ticks_run 18, ticks =638 
Tick: PID 6,cpu : Even ,ticks_run 1, ticks =639 
Tick: PID 6,cpu : Even ,ticks_run 2, ticks =640 
Tick: PID 6,cpu : Even ,ticks_run 3, ticks =641 
YIELD: PID 6,
Tick: PID 3,cpu : Even ,ticks_run 3, ticks =642 
YIELD: PID 3,
Tick: PID 6,cpu : Even ,ticks_run 1, ticks =644 
Tick: PID 6,cpu : Even ,ticks_run 2, ticks =645 
Tick: PID 6,cpu : Even ,ticks_run 3, ticks =646 
YIELD: PID 6,
Tick: PID 6,cpu : Even ,ticks_run 1, ticks =647 
Tick: PID 6,cpu : Even ,ticks_run 2, ticks =648 
Tick: PID 6,cpu : Even ,ticks_run 3, ticks =649 
YIELD: PID 6,
Tick: PID 6,cpu : Even ,ticks_run 1, ticks =651 
Tick: PID 6,cpu : Even ,ticks_run 2, ticks =652 
Tick: PID 6,cpu : Even ,ticks_run 3, ticks =653 
YIELD: PID 6,
--- Throughput Measurement ---
Tick: PID 3,cpu : Even ,ticks_run 1, ticks =655 
Tick: PID 3,cpu : Even ,ticks_run 2, ticks =656 
Total Ticks: 53
Completed Processes: 3
Throughput (Completed/Tick): 3 / 53
