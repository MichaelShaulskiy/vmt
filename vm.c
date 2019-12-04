#include "includes/vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_OPCODES 40
#define MAX_LEVELS 5

#ifdef DEBUG
#define PRINTREGS(vm) _printRegs(vm)
#else /* DEBUG */
#define PRINTREGS
#endif /* !DEBUG */

#define DELTA(v, a) (a - v)

struct type_s {
	char bletter;
	uint8_t elements;
	uint8_t opcode;
	struct type_s *next;
};

static vm_t *vmPool;
static uint8_t vmPoolInit = 0;
static uint16_t vmCount;
static OPCODE opcodes[NUM_OPCODES];
static char *opNames[NUM_OPCODES];

static struct type_s *fTypes;
static uint8_t typeCount;

static uint64_t _findVM(vm_t *);
static uint32_t _fetch(vm_t *);
static void _decode(vm_t *, uint32_t);
static void _eval(vm_t *);

static void _append(struct type_s *, char);

static int _textToHex(const char *);

static int _textToHex(const char *txt) {
	int val;
	int c;

	val = 0;
	while ((c = *txt++) != 0) {
		if (c >= '0' && c <= '9') {
			val = val * 16 + c - '0';
			continue;
		}

		if (c >= 'a' && c <= 'f') {
			val = val * 16 + 10 + c - 'a';
			continue;
		}
		if (c >= 'A' && c <= 'F') {
			val = val * 16 + 10 + c - 'A';
			continue;
		}
	}

	return val;
}

#ifdef DEBUG
static void _printRegs(vm_t *);
#endif

static void _append(struct type_s *head, char b, char t) {
	struct type_s *temp;
	struct type_s *current;
	static uint8_t i;
	static uint8_t j;

	temp = (struct type_s *) malloc(sizeof(struct type_s));
	temp->bletter = b;
	temp->opcode = head->opcode;
jmpMark:
	for(i = 0; i < 25; i++){
		for(j = 0; j < NUM_OPCODES; j++){
			if(opcodes[j].name[1] == b){
				
			}
		}
	}
	temp->next = ((void *) 0x0);

	current = head;
	while (current->next != ((void *) 0x0))
		current = current->next;
	current->next = temp;

}

static uint64_t _findVM(vm_t *VM) {
	uint64_t vmPos;
	for (vmPos = 0; vmPos < vmCount; vmPos++) {
		if (&vmPool[vmPos] == VM)
			break;
	}

	return vmPos;
}

/* fetches an instruction from an VM */
static uint32_t _fetch(vm_t *VM) {
	return (uint32_t) VM->program[(VM->pc)++];
}

static void _decode(vm_t *VM, uint32_t instruction) {
	VM->opcode = (uint8_t) (instruction & 0xF000000) >> 24;
	VM->reg1 = (uint8_t) (instruction & 0xF00000) >> 20;
	VM->reg2 = (uint8_t) (instruction & 0xF0000) >> 16;
	VM->reg3 = (uint8_t) (instruction & 0xF000) >> 12;
	VM->imm = (uint8_t) (instruction & 0xFF);
}

static void _eval(vm_t *VM) {
	uint8_t cmpReg;
	switch (VM->opcode) {
	case 0:
		/* halt */
#ifdef DEBUG
		printf("halt\n");
#endif /* DEBUG */
		VM->running = 0;
		break;

	case 1:
		/* load */
		/* load <dst> <imm> */
#ifdef DEBUG
		printf("load r%d, #%d\n", VM->reg1, VM->imm);
#endif /* DEBUG */
		VM->registers[VM->reg1] = VM->imm;
		break;

	case 2:
		/* mov */
		/* mov <dst> <src> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("mov r%d, r%d\n", VM->reg1, VM->reg2);
#endif
		VM->registers[VM->reg1] = VM->registers[VM->reg2];
		VM->registers[VM->reg2] ^= VM->registers[VM->reg2];
		break;

	case 3:
		/* push */
		/* push <src> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("push r%d\n", VM->reg1);
#endif /* DEBUG */
		VM->stack[VM->sp] = VM->registers[VM->reg1];
		VM->registers[VM->reg1] ^= VM->registers[VM->reg1];
		(VM->sp)--;
		break;

	case 4:
		/* pop */
		/* pop <dst> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("pop r%d\n", VM->reg1);
#endif /* DEBUG */
		VM->registers[VM->reg1] = VM->stack[VM->reg1];
		VM->stack[VM->sp] ^= VM->registers[VM->sp];
		(VM->sp)++;
		break;

	case 5:
		/* inc */
		/* inc <dst> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("inc r%d\n", VM->reg1);
#endif /* DEBUG */
		(VM->registers[VM->reg1])++;
		break;

	case 6:
		/* dec */
		/* dec <dst> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("dec r%d\n", VM->reg1);
#endif /* DEBUG */
		(VM->registers[VM->reg1])--;
		break;

	case 7:
		/* add */
		/* add <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("add r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		VM->registers[VM->reg1] = (uint8_t) (VM->registers[VM->reg2] + VM->registers[VM->reg3]);
		break;

	case 8:
		/* sub */
		/* sub <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("sub r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		VM->registers[VM->reg1] = (uint8_t) (VM->registers[VM->reg2] - VM->registers[VM->reg3]);
		break;

	case 9:
		/* mul */
		/* mul <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("mul r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		VM->registers[VM->reg1] = (uint8_t) (VM->registers[VM->reg2] * VM->registers[VM->reg3]);
		break;

	case 10:
		/* div */
		/* div <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("div r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		VM->registers[VM->reg1] = VM->registers[VM->reg2] / VM->registers[VM->reg3];
		break;

	case 11:
		/* cmp */
		/* cmp <dst> <src1> <src2> */
		/* ANNO: @jit */
		/* to speed up VM by not having to dereference reg3 twice */
		cmpReg = VM->registers[VM->reg3];
#ifdef DEBUG
		printf("cmp r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		if ((VM->registers[VM->reg2] & cmpReg) == cmpReg)
			VM->registers[VM->reg1] = 0x01;
		else
			VM->registers[VM->reg1] = 0x00;
		break;

	case 12:
		/* jmp */
		/* jmp @<dst> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("jmp @r%d\n", VM->reg1);
#endif /* DEBUG */
		VM->pc = VM->pc + VM->registers[VM->reg1];
		break;

	case 13:
		/* rjmp */
		/* rjmp @<dst> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("rjmp @r%d\n", VM->reg1);
#endif /* DEBUG */
		VM->pc = VM->pc - VM->registers[VM->reg1];
		break;

	case 14:
		/* je */
		/* je @<dst> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("je @r%d, r%d\n", VM->reg1, VM->reg2);
#endif /* DEBUG */
		if (VM->registers[VM->reg2])
			VM->pc = VM->pc + VM->registers[VM->reg1];
		break;

	case 15:
		/* rje */
		/* rje @<dst> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("rje @r%d, r%d\n", VM->reg1, VM->reg2);
#endif /* DEBUG */
		if (VM->registers[VM->reg2])
			VM->pc = VM->pc - VM->registers[VM->reg1];
		break;

	case 16:
		/* jne */
		/* jne @<dst> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("jne @r%d, r%d\n", VM->reg1, VM->reg2);
#endif /* DEBUG */
		if (!VM->registers[VM->reg2])
			VM->pc = VM->pc + VM->registers[VM->reg1];
		break;

	case 17:
		/* rjne */
		/* rjne @<dst> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("jne @r%d, r%d\n", VM->reg1, VM->reg2);
#endif /* DEBUG */
		if (!VM->registers[VM->reg2])
			VM->pc = VM->pc - VM->registers[VM->reg1];
		break;

	case 18:
		/* jb */
		/* jb @<dst> <src> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("jb @r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		if (VM->registers[VM->reg2] < VM->registers[VM->reg3])
			VM->pc = VM->pc + VM->registers[VM->reg1];
		break;

	case 19:
		/* rjb */
		/* rjb @<dst> <src> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("rjb @r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		if (VM->registers[VM->reg2] < VM->registers[VM->reg3])
			VM->pc = VM->pc - VM->registers[VM->reg1];
		break;

	case 20:
		/* jnae */
		/* jnae @<dst> <src> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("jnae @r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		if (VM->registers[VM->reg2] < VM->registers[VM->reg3] && VM->registers[VM->reg2] != VM->registers[VM->reg3])
			VM->pc = VM->pc + VM->registers[VM->reg1];
		break;

	case 21:
		/* rjnae */
		/* rjnae @<dst> <src> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("rjnae @r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		if (VM->registers[VM->reg2] < VM->registers[VM->reg3] && VM->registers[VM->reg2] != VM->registers[VM->reg3])
			VM->pc = VM->pc - VM->registers[VM->reg1];
		break;

	case 22:
		/* jnb */
		/* jnb @<dst> <src> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("jnb @r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif
		if (VM->registers[VM->reg2] > VM->registers[VM->reg3])
			VM->pc = VM->pc + VM->registers[VM->reg1];
		break;

	case 23:
		/* rjnb */
		/* rjnb @<dst> <src> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("rjnb @r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif
		if (VM->registers[VM->reg2] > VM->registers[VM->reg3])
			VM->pc = VM->pc - VM->registers[VM->reg1];
		break;

	case 24:
		/* jl */
		/* jl @<dst> <src> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("jl @r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		if (VM->registers[VM->reg2] < VM->registers[VM->reg3])
			VM->pc = VM->pc + VM->registers[VM->reg1];
		break;

	case 25:
		/* rjl */
		/* rjl @<dst> <src> <cmp> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("rjl @r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		if (VM->registers[VM->reg2] < VM->registers[VM->reg3])
			VM->pc = VM->pc - VM->registers[VM->reg1];
		break;

	case 34:
		/* and */
		/* and <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("and r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		VM->registers[VM->reg1] = (VM->registers[VM->reg2] & VM->registers[VM->reg3]);
		break;

	case 35:
		/* or */
		/* or <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("or r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		VM->registers[VM->reg1] = (VM->registers[VM->reg2] | VM->registers[VM->reg3]);
		break;

	case 36:
		/* xor */
		/* xor <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("xor r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif /* DEBUG */
		VM->registers[VM->reg1] = (VM->registers[VM->reg2] ^ VM->registers[VM->reg3]);
		break;

	case 37:
		/* not */
		/* not <src> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("not r%d\n", VM->reg1);
#endif /* DEBUG */
		VM->registers[VM->reg1] = (uint8_t) ~VM->registers[VM->reg1];
		break;

	case 38:
		/* shl */
		/* shl <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("shl r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif
		VM->registers[VM->reg1] = (uint8_t) (VM->registers[VM->reg2] << VM->registers[VM->reg3]);
		break;

	case 39:
		/* shr */
		/* shr <dst> <src1> <src2> */
		/* ANNO: @jit */
#ifdef DEBUG
		printf("shr r%d, r%d, r%d\n", VM->reg1, VM->reg2, VM->reg3);
#endif
		VM->registers[VM->reg1] = (uint8_t) (VM->registers[VM->reg2] >> VM->registers[VM->reg3]);
		break;

	default:
		/* unknown opcode */
		printf("ERROR: rvm stumbled upon an unknown (or yet not implemented) opcode\n");
		break;
	}
}

#ifdef DEBUG
static void _printRegs(vm_t *VM) {
	uint8_t i;
	printf("regs = ");
	for (i = 0; i < 16; i++)
		printf("%02X ", VM->registers[i]);
	printf("\n");

	printf("stack = ");
	for (i = 32; i > 0; i--)
		printf("%02X ", VM->stack[i]);

	printf("\n");

}
#endif /* DEBUG */

vm_t *new_vm(void) {
	static uint8_t i;
	static uint8_t j;

	if (!vmPoolInit) {
		vmPool = (vm_t *) calloc(1, sizeof(vm_t));

		if (vmPool == ((void *) 0x0)) {
			fprintf(stderr, "RVM: ERROR [new_vm]: not enough free space!\n");
			return ((void *) 0x0);
		}
		opNames[0] = "halt";
		opNames[1] = "load";
		opNames[2] = "mov";
		opNames[3] = "push";
		opNames[4] = "pop";
		opNames[5] = "inc";
		opNames[6] = "dec";
		opNames[7] = "add";
		opNames[8] = "sub";
		opNames[9] = "mul";
		opNames[10] = "div";
		opNames[11] = "cmp";
		opNames[12] = "jmp";
		opNames[13] = "rjmp";
		opNames[14] = "je";
		opNames[15] = "rje";
		opNames[16] = "jne";
		opNames[17] = "rjne";
		opNames[18] = "jb";
		opNames[19] = "rjb";
		opNames[20] = "jnae";
		opNames[21] = "rjnae";
		opNames[22] = "jnb";
		opNames[23] = "rjnb";
		opNames[24] = "jl";
		opNames[25] = "rjl";
		opNames[26] = "jg";
		opNames[27] = "rjg";
		opNames[28] = "jnge";
		opNames[29] = "rjnge";
		opNames[30] = "jz";
		opNames[31] = "rjz";
		opNames[32] = "jnz";
		opNames[33] = "rjnz";
		opNames[34] = "and";
		opNames[35] = "or";
		opNames[36] = "xor";
		opNames[37] = "not";
		opNames[38] = "shl";
		opNames[39] = "shr";

		for (i = 0; i < NUM_OPCODES; i++) {
			opcodes[i].code = i;
			opcodes[i].name = opNames[i];
		}

		/* NOTE: this only works for ASCII */
		typeCount = 122 - 97;
		fTypes = (struct type_s *) calloc(typeCount, sizeof(struct type_s));

		for (i = 0; i <= typeCount; i++) {
			fTypes[i].bletter = (char) ('a' + i);
			for (j = 0; j < NUM_OPCODES; j++) {
				if (opcodes[j].name[0] == (char) ('a' + i)) {
					fTypes[i].opcode = opcodes[j].code;
					(fTypes[i].elements)++;
					_append(fTypes, opcodes[j].name[1], opcodes[j].name[0]);
				}

			}
			printf("fTypes: %c\n", fTypes[i].bletter);
			printf("contains: %d\n", fTypes[i].elements);
		}

		vmCount = 0;
		vmPoolInit = 1;
	} else {
		vmPool = (vm_t *) realloc(vmPool, sizeof(vm_t) * (size_t) (vmCount + 1));
		vmCount++;
	}

	return &vmPool[vmCount];

}

void destroy_vm(vm_t *VM) {
	uint64_t vmNum;

	vmNum = _findVM(VM);
	free(&vmPool[vmNum]);
}

void vm_program(vm_t *VM, uint64_t *prog_buffer, uint64_t prog_size) {
	uint64_t vmPos;
	uint64_t i;
	if (VM == ((void *) 0x0) || prog_buffer == NULL || prog_size == 0)
		return;

	vmPos = _findVM(VM);
	vmPool[vmPos].program = (uint64_t *) calloc(prog_size, sizeof(uint64_t));

	for (i = 0; i < prog_size; i++)
		memcpy(&vmPool[vmPos].program[i], &prog_buffer[i], 4);

	vmPool[vmPos].program_size = prog_size;

#ifdef DEBUG
	for (i = 0; i < vmPool[vmPos].program_size; i++)
		printf("program is: %0X\n", (unsigned int) vmPool[vmPos].program[i]);
#endif /* DEBUG */

	free(prog_buffer);
}

void vm_run(vm_t *VM) {
	VM->pc = 0;
	VM->sp = 31;
	VM->running = 1;
	while (VM->running) {
		PRINTREGS(VM);
		_decode(VM, _fetch(VM));
		_eval(VM);
		PRINTREGS(VM);
	}
}

/* NOTE: assembler only supports ASCII right now (for speed reasons)  */
uint64_t *vm_assemble(char *cnt) {
	struct position_s {
		uint64_t begin;
		uint64_t end;
	};

	struct instruction_s {
		uint8_t opcode;
		uint8_t op1;
		uint8_t op2;
		uint8_t op3;
		uint8_t imm;
	};

	static uint64_t i;
	static uint8_t lock;

	static struct position_s *pos;
	static struct position_s *tmpPos;
	static uint64_t posCount;

	static char *tmpBuf;

	static uint64_t j;
	static uint64_t l;
	static uint64_t range;
	static uint8_t level;

	static struct instruction_s *insTree;
	static uint64_t instructions;

	posCount = 0;
	lock = 0;
	tmpPos = ((void *) 0x0);
	instructions = 0;

	pos = (struct position_s *) calloc(1, sizeof(struct position_s));
	tmpBuf = (char *) calloc(1, sizeof(char));
	insTree = (struct instruction_s *) calloc(1, sizeof(struct instruction_s));
	level = 0;
	for (i = 0; i < strlen(cnt); i++) {
		if (cnt[i] != ' ' && lock == 0) {
			if (posCount > 0) {
				if (posCount == 1)
					tmpPos = (struct position_s *) realloc(pos, sizeof(struct position_s) * (posCount + 1));
				else
					tmpPos = (struct position_s *) realloc(pos, sizeof(struct position_s) * posCount);

				if (tmpPos == ((void *) 0x0))
					fprintf(stderr, "pos realloc failed\n");
				else
					pos = tmpPos;
			}

			pos[posCount].begin = i;
			printf("found begin: %u\n", (unsigned int) pos[posCount].begin);

			lock = 1;
		} else if (cnt[i] == ' ' && lock == 1) {
			pos[posCount].end = i;

			range = DELTA(pos[posCount].begin, pos[posCount].end);
			tmpBuf = (char *) realloc(tmpBuf, sizeof(char) * range);

			if (instructions > 0) {
				if (instructions == 1)
					insTree = (struct instruction_s *) realloc(insTree, sizeof(struct instruction_s) * (instructions + 1));
				else
					insTree = (struct instruction_s *) realloc(insTree, sizeof(struct instruction_s) * instructions);
			}

			for (j = pos[posCount].begin, l = 0; l < range; j++, l++)
				tmpBuf[l] = cnt[j];

			tmpBuf[range] = '\0';

			for (j = 0; j < 25; j++) {
				if (fTypes[j].elements > 0) {
					for (l = 0; l < MAX_LEVELS; l++) {
						if (tmpBuf[level] == fTypes[j].bletter) {
							if (fTypes[j].elements == 1) {
								/* we found the mnemonic */
								break;
							}else{
								/* try again */
								fTypes[j] = fTypes[j].next;
								level++;
								continue;
							}
						}
					}
				}
			}
			printf("found end: %u\n", (unsigned int) pos[posCount].end);
			posCount++;
			lock = 0;
		}
	}

	free(pos);
	free(tmpBuf);
	free(fTypes);

	return (uint64_t *) 0x0;
}
