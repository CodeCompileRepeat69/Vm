#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Vm.h"
#include "defs_utils.h"
#include "lexer.h"

/* temporary development macro*/
#define STACK_SIZE 1024

void printCode(FILE *fp, Inst *code, size_t size) {
  for (size_t i = 0; i < size; i++) {
    fprintf(fp, "0x%zx\t%s, %ld\n", i, op_names(code[i].op),
            code[i].arg.as_i64);
  }
}

static Inst code[] = {
    {.op = OP_PUSH, .arg = {.as_i64 = 1}},
#ifdef DEBUG
    {.op = OP_DPRINT},
#endif
    {.op = OP_PUSH, .arg = {.as_i64 = 1}},
    {.op = OP_ADD},
    {.op = OP_DUP},
    {.op = OP_PUSH, .arg = {.as_i64 = 8000}},
    {.op = OP_CMP},
    {.op = OP_JEQ, .arg = {.as_i64 = 211212}},
    {.op = OP_JMP, .arg = {.as_i64 = 1}},
};

void save_current_vm_state(FILE *fp, Vm *vm) {
  int line_len = 0;
  fprintf(stdout, "|ip%*s| op name%*s| arg%*s | stack%*s|\n", 7, " ", 4, " ", 5,
	    " ", 6, " ");
  int padd = fprintf(fp, "|%8ld | %-10s | %8ld |", vm->ip,
                     op_names(VM_INST_AT(vm, vm->ip).op),
                     VM_INST_AT(vm, vm->ip).arg.as_i64);

  line_len = fprintf(fp, "%zx %10ld|\n", vm->sp, vm->stack[0].as_i64);
  line_len += padd;
  for (size_t i = 1; i < vm->sp; i++) {
    line_len = fprintf(fp, "|%*s|%zx %10ld|\n", padd - 2, " ", vm->sp,
                       vm->stack[i].as_i64);
  }
  line_len -= 2;
  fputc('|', fp);
  while ((line_len -= 1) > 0) {
    fputc('-', fp);
  }
  fputc('|', fp);
  fputc('\n', fp);
}

int main(int argc, char **argv) {
  FILE *out = fopen("state_output.txt", "w+");
  if (out == NULL) {
    printf("Error: %s", strerror(errno));
    exit(-1);
  }
  Word stack[STACK_SIZE];
  Vm vm = make_vm(code, ARRAY_SIZE(code, Inst), stack, ARRAY_SIZE(stack, Word));

  int status = 0;
  printf("isize = %ld\n", vm.isize);
  printCode(stdout, vm.instructions, vm.isize);
  for (; vm.ip < vm.isize; vm.ip++) {
    save_current_vm_state(stdout, &vm);
    ErrorCode err = vm_run_inst(&vm, vm.ip);
    stop_on_err(err);
  }
  fclose(out);
  return status;
}
