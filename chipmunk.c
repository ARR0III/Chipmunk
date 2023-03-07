/*
  This is translator for programming language "Chipmonk";

  COMMANDS:
    u - save data from register
    o - load data from register
    > - come to right register
    < - come to lelt register
    l - load data to register
    a - addition register and data
    s - substruction register and data
    f - analog "counter = ??? while(counter) {operations}
    [ - start while block
    ] - end while block
    ( - start numbers block " l(5*3)a(7/2)"
    ) - end numbers block
    e - exit program
    , - using old command

  BEFORE: "u>u>u>u>l(10-9)>l100>l1000>l10000>a1f(10*10)[a2,3f10[a(5*9/32-6),2000]s5,6]<o<o<o<oe"

  AFTER:
  push eax
  push ebx
  push ecx
  push edx
  mov eax, (10-9)
  mov ebx, 100
  mov ecx, 1000
  mov edx, 10000
  add eax, 1
  mov ecx, (10*10)
label @00000000:
  add eax, 2
  add eax, 3
  push ecx
  mov ecx, 10
label @00000001:
  add eax, (5*9/32-6)
  add eax, 2000
loop @00000001
  pop ecx
  sub eax, 5
  sub eax, 6
loop @00000000
  pop edx
  pop ecx
  pop ebx
  pop eax
  ret
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARG_FOR      'f' /* for */

#define ARG_LOAD     'l' /* mov register, 0 */

#define ARG_MALLOC   'm' /* malloc array */
#define ARG_ADD      'a' /* addition */
#define ARG_SUB      's' /* substruction */
#define ARG_EXIT     'e' /* exit program */

#define ARG_LEFT     '<' /* left  register */
#define ARG_RIGHT    '>' /* right register */

#define ARG_PUSH     'u' /* write data */
#define ARG_POP      'o' /* read data */

#define ARG_BEGIN    '('
#define ARG_END      ')'

#define ARG_START    '['
#define ARG_FINISH   ']'

#define ARG_CONTINUE ',' /* using old command */

#define REGISTER_COUNTER 4

typedef enum {
  REG_EAX, REG_EBX, REG_ECX, REG_EDX
} REG;

typedef enum {
  OPERATION, REGISTER, DATA
} USI;

typedef struct {
  int using;       /* OPERATION, REGISTER or DATA */
  
  int regist;      /* using eax, ebx, ecx, edx */
  int old_regist;

  int command;     /* using command */
  int loop;        /* index loop metka */
  int error;       /* correct input data, or not */
} chip;

const char * __command[] = {
  "add",   /* 0 */
  "sub",   /* 1 */
  "mov",   /* 2 */
  "push",  /* 3 */
  "pop",   /* 4 */

  "label", /* 5 */
  "loop",  /* 6 */
  "ret\n"  /* 7 */
};

const char * __register[] = {
  "eax", "ebx", "ecx", "edx"
};

void __assembler(chip * ctx, char data) {
  if (REGISTER == ctx->using) {
    return;
  }
  else
  if (OPERATION == ctx->using) {
    if (5 == ctx->command) {
      printf("\n%s @%.8d:", __command[ctx->command], ctx->loop);
      ctx->loop = ctx->loop + 1;
    }
    else 
    if (6 == ctx->command) {
      printf("\n%s @%.8d", __command[ctx->command], ctx->loop);
    }
    else
    if (3 == ctx->command || 4 == ctx->command)
      printf("\n  %s %s", __command[ctx->command],
                          __register[ctx->regist]);
    else
    if (7 == ctx->command)
      printf("\n  %s", __command[ctx->command]);
    else
      printf("\n  %s %s, ", __command[ctx->command],
                            __register[ctx->regist]);
  }
  else
  if (DATA == ctx->using) {
    putc(data, stdout);
  }
}

void __parser(chip * ctx, char data) {

/***************************************************************/
  ctx->using = DATA;

  if (data >= '0' && data <= '9') {
    return;
  }

  if (data == ARG_BEGIN || data == ARG_END ||
      data == '+' || data == '-' || data == '*' || data == '/') {
    return;
  }

/***************************************************************/
  ctx->using = REGISTER;

  if (ARG_LEFT == data) {
    ctx->regist = ctx->regist - 1;
    
    if (ctx->regist < 0) {
      ctx->regist = ctx->regist + REGISTER_COUNTER;
    }

    return;
  }

  if (ARG_RIGHT == data) {
    ctx->regist = ctx->regist + 1;
    
    if (REGISTER_COUNTER == ctx->regist) {
      ctx->regist = ctx->regist - REGISTER_COUNTER;
    }    

    return;
  }

/***************************************************************/
  ctx->using = OPERATION;

  if (ARG_FOR == data) {
    ctx->old_regist = ctx->regist;
    ctx->regist = REG_ECX;

    if (ctx->loop > 0) { /* if FOR() using */
      ctx->command = 3;
      __assembler(ctx, data);
    }

    ctx->command = 2;
    return;
  }

  if (6 == ctx->command) {
    if (ctx->loop > 0) {
      ctx->old_regist = ctx->regist;
      ctx->regist = REG_ECX;
      ctx->command = 4;

      __assembler(ctx, data);

      ctx->regist = ctx->old_regist;
    }
  }

  if (ARG_FINISH == data) {
    ctx->loop = ctx->loop - 1;
    ctx->loop = (ctx->loop < 0) ? 0 : ctx->loop;
    ctx->command = 6;
    return;
  }

  switch(data) {
    case ARG_ADD:    ctx->command = 0; break;
    case ARG_SUB:    ctx->command = 1; break;
    case ARG_LOAD:   ctx->command = 2; break;
    case ARG_PUSH:   ctx->command = 3; break;
    case ARG_POP:    ctx->command = 4; break;

    case ARG_EXIT:   ctx->command = 7; break;
    
    case ARG_START:  ctx->command = 5;
                     ctx->regist = ctx->old_regist;
                     break;
  }
}

int __corrector(char * data, int size) {
  int res = 0;
  int i;

  for (i = 0; (i < size) && data[i]; i++) {
    if (data[i] >= '0' && data[i] <= '9') {
      continue;
    }

    if (data[i] == '+' || data[i] == '-' || data[i] == '*' || data[i] == '/') {
      continue;
    }

    if (ARG_FOR    != data[i] && ARG_MALLOC != data[i] &&
        ARG_ADD    != data[i] && ARG_SUB    != data[i] &&
        ARG_LEFT   != data[i] && ARG_RIGHT  != data[i] &&
        ARG_PUSH   != data[i] && ARG_POP    != data[i] &&
        ARG_EXIT   != data[i] && ARG_START  != data[i] &&
        ARG_FINISH != data[i] && ARG_LOAD   != data[i] &&
        ARG_BEGIN  != data[i] && ARG_END    != data[i] && ARG_CONTINUE != data[i]) {

      res = i;
      break;
    }
  }

  return res;
}

int main(int argc, char * argv[]) {

  char * string = NULL;

  int result, i, pos = 0;

  chip * ctx = (chip *)malloc(sizeof(chip));

  if (ctx == NULL) return -1;

  ctx->regist     = REG_EAX;   /* first register */
  ctx->using      = OPERATION; /* OPERATION, REGISTER or DATA */
  ctx->old_regist = REG_EAX;   /* old using register */
  ctx->command    = 0;         /* using command */
  ctx->loop       = 0;         /* index loop metka */
  ctx->error      = 0;         /* correct input data, or not */

  if (argc == 2 && argv[1]) {
    string = argv[1];
  }
  else
    string = "u>u>u>u>l(10-9)>l100>l1000>l10000>a1f(10*10)[a2,3f10[a(5*9/32-6),2000]s5,6]<o<o<o<oe";

  result = __corrector(string, strlen(string));

  if (result) {
    printf("Error symbol position = %d, symbol = \'%c\'\n", result, string[result]);
    printf("data:%s\n", string);
   
    for (i = 0; i < (result + 5); i++)
      putc(' ', stdout);

    putc('^', stdout);
    putc('\n', stdout);
    
    return -1;
  }

  while (string[pos]) {
    __parser(ctx, string[pos]);
    __assembler(ctx, string[pos]);
    pos++;
  }

  free(ctx);

  return 0;
}












