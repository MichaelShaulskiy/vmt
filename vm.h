#ifndef VM_H_
#define VM_H_

#include <stdint.h>

typedef struct OPCODE_S {
	uint8_t code;
	char *name;
} OPCODE;

typedef struct vm_s {
	/* memory */
	uint8_t registers[16];
	uint8_t stack[32];

	/* program */
	uint64_t *program;
	uint64_t program_size;

	/* machine state */
	uint64_t pc;
	uint64_t sp;

	/* instruction */
	uint8_t opcode;
	uint8_t reg1;
	uint8_t reg2;
	uint8_t reg3;
	uint8_t imm;

	/* machine state */
	uint8_t running;
} vm_t;

/* constructor and destructor */
vm_t *new_vm(void);
void destroy_vm(vm_t *);

/* run the given vm */
void vm_run(vm_t *);

/* pause the given vm */
void vm_pause(vm_t *);

/* shuts down the given vm */
void vm_shutdown(vm_t *);

/* assembles a program */
uint64_t *vm_assemble(char *);

/* assigns a program to a vm */
void vm_program(vm_t *, uint64_t *, uint64_t);

#endif /* VM_H_ */
