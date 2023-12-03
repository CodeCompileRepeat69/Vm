#include "Vm.h"
#include "defs_utils.h"
#include <assert.h>
#include <stdbool.h>


char *op_names(OpCode op) {
  switch (op) {
  case OP_PUSH:return TO_STR(OP_PUSH);
  case OP_ADD:return TO_STR(OP_ADD);
  case OP_SUB:return TO_STR(OP_SUB);
  case OP_MUL:return TO_STR(OP_MUL);
  case OP_POP:return TO_STR(OP_POP);
  case OP_JMP:return TO_STR(OP_JMP);
  case OP_CMP:return TO_STR(OP_CMP);
  case OP_JLT:return TO_STR(OP_JLT);
  case OP_JGT:return TO_STR(OP_JGT);
  case OP_JEQ:return TO_STR(OP_JEQ);
  case OP_JNQ:return TO_STR(OP_JNQ);
#ifdef DEBUG
  case OP_DPRINT:return TO_STR(OP_DPRINT);
#endif
  case OP_DUP:return TO_STR(OP_DUP);
  case OP_ERR:return TO_STR(OP_ERR);
  case OP_LOAD_FAST:return TO_STR(OP_LOAD_FAST);
  case OP_STORE_FAST:return TO_STR(OP_STORE_FAST);
  case OP_LOAD_CONST:return TO_STR(OP_LOAD_CONST);
  default:
    return "NOT A OPCODE";
  }
}

char *err_code_to_cstr(ErrorCode err) {
  switch (err) {
  case ERR_DIV_BY_ZERO:
    return TO_STR(ERR_DIV_BY_ZERO);
  case ERR_UNDERFLOW:
    return TO_STR(ERR_UNDERFLOW);
  case ERR_OVERFLOW:
    return TO_STR(ERR_OVERFLOW);
  case ERR_INVALID_OPCODE:
    return TO_STR(ERR_INVALID_OPCODE);
  case ERR_NO_ARG:
    return TO_STR(ERR_NO_ARG);
  case ERR_SUCCESS:
    return TO_STR(ERR_SUCCESS);
  case ERR_JMP_OVERFLOW:
    return TO_STR(ERR_JMP_OVERFLOW);
  case ERR_JMP_UNDERFLOW:
    return TO_STR(ERR_JMP_UNDERFLOW);
  case ERR_NO_SUCH_CONSTANT:
    return TO_STR(ERR_NO_SUCH_CONSTANT);
  case ERR_UNDER_FRAME_STACK_ACCESS:
    return TO_STR(ERR_UNDER_FRAME_STACK_ACCESS);
  case ERR_OVER_FRAME_STACK_ACCESS:
    return TO_STR(ERR_OVER_FRAME_STACK_ACCESS);
  }
  assert(0 && "Invalide Error code (Oh the irony XD)");
  return "unreachable";
}

#define CHK_OVERFLOW(vm, n) (vm->sp + (n -1) >= vm->ssize)
#define CHK_UNDERFLOW(vm, n) (vm->sp < 1 + (n - 1))

inline OpCode get_current_inst_op(Vm *vm) {
  return vm->instructions[vm->ip].op;
}
inline Word get_current_inst_arg(Vm *vm) {
  return vm->instructions[vm->ip].arg;
}

inline ErrorCode vm_push(Vm *vm, Word data) {
  if (CHK_OVERFLOW(vm, 1)) {
    return ERR_OVERFLOW;
  }
  vm->stack[vm->sp] = data;
  vm->sp += 1;
  return ERR_SUCCESS;  
}

inline ErrorCode vm_run_pop(Vm *vm, Word *ret) {
  if (CHK_UNDERFLOW(vm, 1)) {
    return ERR_UNDERFLOW;
  }
  if (ret != NULL) {
    *ret = vm->stack[vm->sp];
  }
  vm->sp--;
  return ERR_SUCCESS;
}

inline ErrorCode vm_run_push(Vm *vm) {
  return vm_push(vm, get_current_inst_arg(vm));
}
inline ErrorCode vm_run_add(Vm *vm) {
  ErrorCode err;
  Word a, b, c;
  if ((err = vm_run_pop(vm, &a)) != ERR_SUCCESS) return err;
  if ((err = vm_run_pop(vm, &b)) != ERR_SUCCESS) return err;
  c.as_i64 = a.as_i64 + b.as_i64;
  err = vm_push(vm, c);
  return err;
}
ErrorCode vm_run_sub(Vm *vm) {
  ErrorCode err;
  Word a, b, c;
  if ((err = vm_run_pop(vm, &a)) != ERR_SUCCESS) return err;
  if ((err = vm_run_pop(vm, &b)) != ERR_SUCCESS) return err;
  c.as_i64 = (a.as_i64 - b.as_i64);
  return vm_push(vm, c);
}
ErrorCode vm_run_mul(Vm *vm) {
  ErrorCode err;
  Word a, b, c;
  if ((err = vm_run_pop(vm, &a)) != ERR_SUCCESS) return err;
  if ((err = vm_run_pop(vm, &b)) != ERR_SUCCESS) return err;
  c.as_i64 = a.as_i64 * b.as_i64;
  return vm_push(vm, c);;
}

ErrorCode vm_run_jmp(Vm *vm) {
  Word jmp_val = get_current_inst_arg(vm);
  if (jmp_val.as_u64 > vm->isize) {
    return ERR_JMP_OVERFLOW;
  }
  if (jmp_val.as_i64 <= 0) {
    return ERR_JMP_UNDERFLOW;
  }
  vm->ip = jmp_val.as_i64 - 1;
  return ERR_SUCCESS;
}

/*
  STACK:
  before: ...,
  value1,
  value2
  after: ...,
  value1,
  value2,
  cmp_result
	 
  if value1 > value2 -> cmp_result =  1
  if value1 = value2 -> cmp_result =  0
  if value1 < value2 -> cmp_result = -1
*/
ErrorCode vm_run_cmp(Vm *vm){
  Word a, b, c;
  ErrorCode err;
  if ((err = vm_run_pop(vm, &b)) != ERR_SUCCESS) return err;
  if ((err = vm_run_pop(vm, &a)) != ERR_SUCCESS) return err;
  c.as_i64 = (a.as_i64 > b.as_i64 ? 1 :
	      (a.as_i64 == b.as_i64) ? 0 : -1);
  return vm_push(vm,c); 
}

ErrorCode vm_run_jlt(Vm *vm) {
  ErrorCode err;
  Word val;
  if ((err = vm_run_pop(vm, &val)) != ERR_SUCCESS) return err;
  if (val.as_i64 == -1) {
    vm_run_pop(vm, NULL);
    vm_run_pop(vm, NULL);
    return vm_run_jmp(vm);
  }
  if ((err = vm_run_pop(vm, NULL)) != ERR_SUCCESS) return err;
  if ((err = vm_run_pop(vm, NULL)) != ERR_SUCCESS) return err;
  return ERR_SUCCESS;
}
ErrorCode vm_run_jgt(Vm *vm) {
  ErrorCode err;
  Word val;
  if ((err = vm_run_pop(vm, &val)) != ERR_SUCCESS) return err;
  if (val.as_i64 == 1) {
    vm_run_pop(vm, NULL);
    vm_run_pop(vm, NULL);
    return vm_run_jmp(vm);
  }
  if ((err = vm_run_pop(vm, NULL)) != ERR_SUCCESS) return err;
  if ((err = vm_run_pop(vm, NULL)) != ERR_SUCCESS) return err;
  return ERR_SUCCESS;
}
ErrorCode vm_run_jeq(Vm *vm) {
  ErrorCode err;
  Word val;
  if ((err = vm_run_pop(vm, &val)) != ERR_SUCCESS) return err;
  if (val.as_i64 == 0) {
    vm_run_pop(vm, NULL);
    vm_run_pop(vm, NULL);
    return vm_run_jmp(vm);
  }
  if ((err = vm_run_pop(vm, NULL)) != ERR_SUCCESS) return err;
  if ((err = vm_run_pop(vm, NULL)) != ERR_SUCCESS) return err;

  return ERR_SUCCESS;
}
ErrorCode vm_run_jnq(Vm *vm) {
  ErrorCode err;
  Word val;
  if ((err = vm_run_pop(vm, &val)) != ERR_SUCCESS) return err;
  if (val.as_i64 != 0) {
    vm_run_pop(vm, NULL);
    vm_run_pop(vm, NULL);
    return vm_run_jmp(vm);
  }
  if ((err = vm_run_pop(vm, NULL)) != ERR_SUCCESS) return err;
  if ((err = vm_run_pop(vm, NULL)) != ERR_SUCCESS) return err;
  return ERR_SUCCESS;
}
ErrorCode vm_run_dup(Vm *vm) {
  ErrorCode err;
  Word val;
  if ((err = vm_run_pop(vm, &val)) != ERR_SUCCESS) return err;
  if ((err = vm_push(vm, val)) != ERR_SUCCESS) return err;
  if ((err = vm_push(vm, val)) != ERR_SUCCESS) return err;
  return ERR_SUCCESS;
}
void stop_on_err(ErrorCode ec) {
  if (ec != ERR_SUCCESS) {
    exit(ec);
  }
}
int vm_exec(Vm *vm) {
  ErrorCode err = 0;
  for (; vm->ip <= vm->isize; vm->ip++) {
    err = vm_run_inst(vm, vm->ip);
  }
  return err;
}
ErrorCode vm_run_inst(Vm *vm, size_t ip) {
  ErrorCode err = 0;
  if (ip > vm->isize) {
    return ERR_OVERFLOW;
  }
  switch (VM_INST_AT(vm, ip).op) {
  case OP_PUSH:
    err = vm_run_push(vm);
    break;
  case OP_ADD:
    err = vm_run_add(vm);
    break;
  case OP_SUB:
    err = vm_run_sub(vm);
    break;
  case OP_MUL:
    err = vm_run_mul(vm);
    break;
  case OP_POP:
    err = vm_run_pop(vm, NULL);
    break;
  case OP_JMP:
    err = vm_run_jmp(vm);
    break;
  case OP_CMP:
    err = vm_run_cmp(vm);
    break;
  case OP_JLT:
    err = vm_run_jlt(vm);
    break;
  case OP_JGT:
    err = vm_run_jgt(vm);
    break;
  case OP_JEQ:
    err = vm_run_jeq(vm);
    break;
  case OP_JNQ:
    err = vm_run_jnq(vm);
    break;
#ifdef DEBUG
  case OP_DPRINT:
    printf("%ld\n", vm->stack[(vm->sp - 1)]);
    break;
#endif
  case OP_DUP:
    err = vm_run_dup(vm);
    break;
  case OP_ERR:
    assert(1 && "Invalid stuff");
    break;
  }
  return err;
}      
ErrorCode vm_run_load_fast(Vm *vm) {
  ErrorCode err = ERR_SUCCESS;
  Frame *current_frame = &vm->frames.data[vm->current_frame];
  size_t stack_start = current_frame->stack_start;
  size_t stack_offst = get_current_inst_arg(vm).as_i64;
  
  if ((stack_start + stack_offst) > vm->ssize) {
    err = ERR_OVERFLOW;
    return err;
  }
  Word value = vm->stack[stack_start + stack_offst];
  vm_push(vm, value);
  return err;
}
ErrorCode vm_run_store_fast(Vm *vm) {
  ErrorCode err = ERR_SUCCESS;
  return err;
}
ErrorCode vm_run_load_const(Vm *vm) {
  ErrorCode err = ERR_SUCCESS;
  return err;
}
/*
  constructs and returns a Vm object
*/
Vm make_vm(Inst *instructions,
	   size_t n_insts,
	   Word *stack,
	   size_t stack_size) {
  return (Vm) {
    .instructions = instructions, 
    .ip	 = 0,
    .isize = n_insts,
    .stack = stack,
    .sp	 = 0,
    .ssize = stack_size 
  };
  /* TODO: update this */
}
