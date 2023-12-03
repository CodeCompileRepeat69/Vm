#ifndef __VM_H

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include "vector.h"

/*-------------------------| Type defs |-------------------------*/

typedef struct Vm Vm;
typedef struct Frame Frame;
typedef Vec_t(Frame) Frame_vec;
typedef struct Inst Inst;

typedef enum  {
  LOG_INFO,
  LOG_DEBUG,
  LOG_ERROR
} LogType;

typedef enum {
  OP_PUSH,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_POP,
  OP_JMP,
  OP_CMP,
  OP_JLT,
  OP_JGT,
  OP_JEQ,
  OP_JNQ,
  OP_DUP,
#ifdef DEBUG
  OP_DPRINT,
#endif
  OP_LOAD_FAST,
  OP_STORE_FAST,
  OP_LOAD_CONST,
  OP_ERR
}  __attribute__((__packed__)) OpCode;

/*
  desc: description of the instruction
  stack_args:  number of objects the instruction removes from the stack
  args: number of non stack arguments
*/
typedef struct {
  const char *desc;
  int stack_args; 
  int args;
} InstInfo;

typedef enum {
  ERR_SUCCESS, /* default value is safe (failed successfully XD) */
  ERR_UNDERFLOW,
  ERR_OVERFLOW,
  ERR_INVALID_OPCODE,
  ERR_DIV_BY_ZERO,
  ERR_JMP_OVERFLOW,
  ERR_JMP_UNDERFLOW,
  ERR_NO_ARG,
  ERR_NO_SUCH_CONSTANT,
  ERR_UNDER_FRAME_STACK_ACCESS,
  ERR_OVER_FRAME_STACK_ACCESS,
} ErrorCode;

typedef union {
  int64_t as_i64;
  uint64_t as_u64;
  double ad_f64;
  float as_f32;
  char as_i8;
  unsigned char as_u8;
} Word;

struct Inst {
  OpCode op;
  Word arg;
};

struct Vm {
  Inst *instructions;
  size_t ip;
  size_t isize;
  Word *stack;
  size_t sp;
  size_t ssize;
  size_t current_frame;
  Frame_vec frames;
};

struct Frame {
  size_t return_addr;
  size_t stack_start;
};


/*-------------------------| Functions |-------------------------*/

char *err_code_to_cstr(ErrorCode err);
OpCode get_current_inst_op(Vm *vm);
Word get_current_inst_arg(Vm *vm);
ErrorCode vm_push(Vm *vm, Word data);
ErrorCode vm_run_push(Vm *vm);
ErrorCode vm_run_add(Vm *vm);
ErrorCode vm_run_sub(Vm *vm);
ErrorCode vm_run_mul(Vm *vm);
ErrorCode vm_run_pop(Vm *vm, Word *ret);
ErrorCode vm_run_jmp(Vm *vm);
ErrorCode vm_run_cmp(Vm *vm);
ErrorCode vm_run_jlt(Vm *vm);
ErrorCode vm_run_jgt(Vm *vm);
ErrorCode vm_run_jeq(Vm *vm);
ErrorCode vm_run_jnq(Vm *vm);
ErrorCode vm_run_inst(Vm *vm, size_t ip);
ErrorCode vm_run_load_fast(Vm *vm);
ErrorCode vm_run_store_fast(Vm *vm);
ErrorCode vm_run_load_const(Vm *vm);

void stop_on_err(ErrorCode ec);
int vm_exec(Vm *vm);

Vm make_vm(Inst *instructions,
	   size_t n_insts,
	   Word *stack,
	   size_t stack_size);

char *op_names(OpCode);

/*-------------------------| Macros |-------------------------*/
#define OpToNme(op) #op
#define VM_INST_AT(vm, pos) (vm->instructions[pos])

#ifdef DEBUG
#define STACK_PRINT(vm)				\
  printf("[STACK]:\n");				\
  for (size_t i = 0; i <= vm->sp; i++) {	\
    printf("\t%ld %ld\n", i, vm->stack[i]);	\
  }
#else
#define STACK_PRINT(vm) exit(-1)
#endif

#define INVALID_OP(op)					\
  if (op >= OP_ERR) {					\
    Log(LOG_ERROR, "Non Valid Operator%c", '\n');	\
  }							\

/*-------------------------| Globals  |-------------------------*/

static const InstInfo inst_infos[] = {
  [OP_PUSH] = {
    .desc = "Push a value on the stack",
    .stack_args = 0,
    .args = 1
  },
  [OP_ADD] = {
    .desc = "Sums two values on top of the stack",
    .stack_args = 2,
    .args = 0
  },
  [OP_SUB] = {
    .desc = "Performs a - b with 'a' being top of the stack and 'b' the value before 'a' ",
    .stack_args = 2,
    .args = 0
  },
  [OP_MUL] = {
    .desc = "Multiplies two values on top of the stack",
    .stack_args = 2,
    .args = 0
  },
  [OP_POP] = {
    .desc = "Pops top of stack",
    .stack_args = 1,
    .args = 0
  },
  [OP_JMP] = {
    .desc = "Unconditional jump to instrucition passed as argument",
    .stack_args = 0,
    .args = 1
  },
  [OP_CMP] = {
    .desc =
        "Compares 'a' and 'b' with 'b' being the TOS value and 'a' the value before 'b', if a > b -> push 1; if a = b -> push 0; if a < b -> push -1",
    .stack_args = 2,
    .args = 0
  },
  [OP_JLT] = {
    .desc = "Conditional Jump, performs jump if TOS is -1",
    .stack_args = 1,
    .args = 1
  },
  [OP_JGT] = {
    .desc = "Conditional Jump, performs jump if TOS is 1",
    .stack_args = 1,
    .args = 1
  },
  [OP_JEQ] = {
    .desc = "Conditional Jump, performs jump if TOS is 0",
    .stack_args = 0,
    .args = 1
  },
  [OP_JNQ] = {
    .desc = "Conditional Jump, performs jump if TOS is not 0",
    .stack_args = 1,
    .args = 1
  },
  [OP_DUP] = {
    .desc = "Duplicates valutt at TOS",
    .stack_args = 0,
    .args = 0
  },
#ifdef DEBUG
  [OP_DPRINT] = {
    .desc = "Debug only print, prints to stdout value at TOS",
    .stack_args = 0,
    .args = 0
  },
#endif
  [OP_ERR] = {
    .desc = "",
    .stack_args = 0,
    .args = 0
  }
};


#define __VM_H
#endif
