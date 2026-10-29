# Operating Systems Laboratory

Five lab projects based on **xv6**, completed for the **Operating Systems Laboratory** course at the **University of Tehran**.

The projects explore operating-system internals through changes to an educational UNIX-like kernel: adding system calls, experimenting with scheduling, implementing synchronization primitives, and comparing page-replacement policies. Each lab directory contains its own xv6 source tree and Makefile.

## Labs

| Lab | Focus | Selected files |
| --- | --- | --- |
| [Lab 1](OS_Lab_01/) | Exploring the xv6 source tree, build process, and operating-system structure | [Lab README](OS_Lab_01/README.md), [report](OS_Lab_01/report.pdf) |
| [Lab 2](OS_Lab_02/) | Custom system calls for arithmetic, process duplication, process-family inspection, text searching, and process priority | [`syscall.h`](OS_Lab_02/syscall.h), [`sys_arithmetic_test.c`](OS_Lab_02/sys_arithmetic_test.c), [`test_family.c`](OS_Lab_02/test_family.c), [`priority_test.c`](OS_Lab_02/priority_test.c) |
| [Lab 3](OS_Lab_03/) | Scheduling and load-balancing experiments | [`test_fcfs.c`](OS_Lab_03/test_fcfs.c), [`test_lb.c`](OS_Lab_03/test_lb.c), [report](OS_Lab_03/report.pdf) |
| [Lab 4](OS_Lab_04/) | Synchronization experiments involving sleep locks, reader–writer locks, and priority locks | [`test_sleeplock.c`](OS_Lab_04/test_sleeplock.c), [`rwlock_test.c`](OS_Lab_04/rwlock_test.c), [`plock_test.c`](OS_Lab_04/plock_test.c), [document](OS_Lab_04/OS-LAB4.pdf) |
| [Lab 5](OS_Lab_05/) | Paging experiments comparing FIFO, LRU, LFU, and Clock policies under different access patterns | [`test1.c`](OS_Lab_05/test1.c), [`test2.c`](OS_Lab_05/test2.c), [`test3.c`](OS_Lab_05/test3.c), [`test4.c`](OS_Lab_05/test4.c), [document](OS_Lab_05/OS_Lab_05.pdf) |

## Platform and requirements

These projects use the **32-bit x86 version of xv6**, rather than xv6-riscv.

A Linux environment or Linux virtual machine is a convenient setup. You will need:

- GNU Make.
- GCC and GNU binutils capable of building 32-bit x86 code, or a compatible cross-compilation toolchain.
- QEMU with x86 system emulation.
- Perl, used by build-time helper scripts.
- GDB — optional, for kernel debugging.

The Makefiles contain toolchain and emulator detection. If detection fails, configure `TOOLPREFIX` and `QEMU` for the tools installed on your machine. An ARM host needs an x86 cross-compilation setup; its native compiler alone is insufficient.

## Build and boot

Clone the repository and enter one lab directory:

```bash
git clone https://github.com/mhieir/OS-Lab.git
cd OS-Lab/OS_Lab_02
make clean
make qemu-nox
```

`qemu-nox` boots xv6 in the terminal. Use `make qemu` for the graphical emulator target, or `make qemu-gdb` to start a debugging session.

Build each lab from its own directory. The folders are separate kernel snapshots, so a command added in one lab may not exist in another.

## Running the experiments

After xv6 boots, enter the test program's name at its shell prompt. These commands run **inside xv6**, not in the host terminal.

### Lab 2: system calls

```text
sys_arithmetic_test
test_family
grep_test
priority_test
```

The corresponding source files show how the user programs invoke the added system calls. Check each program's source or usage output for any input it expects.

### Lab 3: scheduling

```text
test_fcfs
test_lb
```

The load-balancing test forks multiple CPU-bound child processes and waits for them to complete. Additional experiments are included in `4_1_test.c`, `4_5_test.c`, and `4_6_test.c`.

### Lab 4: synchronization

```text
test_sleeplock
rwlock_test
plock_test
```

These exercises examine lock behavior when multiple processes compete for shared resources.

### Lab 5: paging

The paging tests accept a policy number:

| Argument | Policy |
| --- | --- |
| `0` | FIFO |
| `1` | LRU |
| `2` | LFU |
| `3` | Clock |

For example:

```text
test1 0
test1 1
test2 2
test3 3
test4 0
```

The four programs exercise page pressure, cyclic access, hot/cold pages, and a repeated access sequence. They call the lab's paging interfaces and print paging metrics and elapsed ticks. These are coursework experiments, rather than benchmark results for a production operating system.

## Reading the code

Useful entry points in each source tree include:

- `syscall.h`, `syscall.c`, and `sysproc.c`: system-call numbers, dispatch, and handlers.
- `user.h` and `usys.S`: user-space system-call interfaces.
- `proc.c` and `proc.h`: process management and scheduling.
- `vm.c` and `trap.c`: virtual memory and trap handling.
- `Makefile`: kernel build rules and the `UPROGS` list of programs included in the xv6 filesystem.

The PDFs provide additional explanations and recorded results; written material is primarily in Persian.

## Attribution

The kernel foundation is xv6, developed at MIT for teaching operating systems. The repository includes the upstream `README` and `LICENSE` files in its lab source trees. Course-specific changes and experiments are organized by lab.

**Mahdis Mirzaei** · Computer Engineering, University of Tehran
