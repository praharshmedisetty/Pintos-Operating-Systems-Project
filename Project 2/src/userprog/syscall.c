#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

// Phase 2 Addition: Included vaddr header file to use PHYS_BASE variable
#include "threads/vaddr.h"

static void syscall_handler(struct intr_frame *);

// Phase 2 Addition: function that reads a byte at user virtual address UADDR and checks if it is less than PHYS_BASE.
// If successful it returns the byte value and if its not, then it returns -1 indicating a segfault occurrance
// CODE REFERENCE: Pintos Documentation : 3.1.5 Accessing User Memory
static int get_val_uaddr(const uint8_t *uaddr);

// Phase 2 Addition: function that returns the number of bytes actually written
static int copy_data(void *a, void *b, size_t data_bytes);

// Phase 2 Addition: used for skeleton implementations of other syscall functions
typedef int pid_t;

// Phase 2 Addition: implementation of system calls
void sys_exit(int status);
int sys_write(int fd, const void *buffer, unsigned size);
void sys_halt (void);
pid_t sys_exec (const char *cmd_line);
int sys_wait (pid_t pid);
bool sys_create (const char *file, unsigned initial_size);
bool sys_remove (const char *file);
int sys_open (const char *file);
int sys_filesize (int fd);
int sys_read (int fd, void *buffer, unsigned size);
void sys_seek (int fd, unsigned position);
unsigned sys_tell (int fd);
void sys_close (int fd);

static int get_val_uaddr(const uint8_t *uaddr)
{
	if ((void *)uaddr > PHYS_BASE)
		return -1;
	if (uaddr == NULL)
		return -1;
	if (pagedir_get_page(thread_current()->pagedir, uaddr) == NULL)
		return -1;

	int retVal;
	asm("movl $1f, %0; movzbl %1, %0; 1:"
		: "=&a"(retVal) : "m"(*uaddr));

	return retVal;
}

static int copy_data(void *src, void *dest, size_t data_bytes)
{
	int addr_val;
	for (int i = 0; i < data_bytes; i++)
	{
		addr_val = get_val_uaddr((uint8_t *)(src + i));

		if (addr_val == -1)
			sys_exit(-1);

		*((uint8_t *)(dest + i)) = (uint8_t)addr_val;
	}

	return (int)data_bytes;
}

void sys_exit(int status)
{
	printf("%s: exit(%d)\n", thread_current()->name, status);
	thread_exit();
}

int sys_write(int file_descriptor, const void *data_buffer, unsigned size)
{
	int val = get_val_uaddr(data_buffer);
	if (val == -1)
		sys_exit(-1);

	int val_buffer_last = get_val_uaddr(data_buffer + size - 1);
	if (val_buffer_last == -1)
		sys_exit(-1);

	if (file_descriptor == 1)
	{
		putbuf(data_buffer, size);
		return size;
	}

	return -1;
}

void syscall_init(void)
{
	intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler(struct intr_frame *f UNUSED)
{

	int sys_call_nr;
	copy_data(f->esp, &sys_call_nr, sizeof(sys_call_nr));

	switch (sys_call_nr)
	{
	case SYS_HALT:
		break;

	case SYS_EXIT:
	{
		int exit_code;
		copy_data(f->esp + 4, &exit_code, sizeof(exit_code));
		sys_exit(exit_code);
		NOT_REACHED();
		break;
	}

	case SYS_EXEC:
		break;

	case SYS_WAIT:
		break;

	case SYS_CREATE:
		break;

	case SYS_REMOVE:
		break;

	case SYS_OPEN:
		break;

	case SYS_FILESIZE:
		break;

	case SYS_READ:
		break;

	case SYS_WRITE:
	{

		int file_descriptor = *((int *)(f->esp + 4));
		const void *data_buffer = *((const void **)(f->esp + 8));
		unsigned file_size = *((unsigned *)(f->esp + 12));
		f->eax = (uint32_t)sys_write(file_descriptor, data_buffer, file_size);

		break;
	}

	case SYS_SEEK:
		break;

	case SYS_TELL:
		break;

	case SYS_CLOSE:
		break;
  
  default:
  {
    sys_exit(-1);
    break;
  }
	}
}