/*
  This is translator for programming language "Chipmunk";

  COMMANDS:
    u - save data from register
    o - load data from register
    > - come to right register
    < - come to lelt register
    l - load data to register
    a - addition register and data
    s - substruction register and data
    f - analog "counter = ??? while(counter--) {operations}
    n - not operation
    [ - start while block
    ] - end while block
    ( - start numbers block " l(5*3)a(7/2)"
    ) - end numbers block
    e - exit program
    , - using old command

  BEFORE: "u>u>u>u>>>>>l0>l60>l0001>lFFFFFFFF<<<a10f(10*10)[a10,0BADf20[a20,20ns00C0FFEE,20]s10,10]l0>f30[a30,300,3000n]>>o<o<o<oe";
  AFTER:
	push eax
	push ebx
	push ecx
	push edx
	mov eax, 0
	mov ebx, 60
	mov ecx, 0001
	mov edx, FFFFFFFF
	add eax, 10
	mov ecx, (10*10)
.L2:
	add eax, 10
	add eax, 0BAD
	push ecx
	mov ecx, 20
.L4:
	add eax, 20
	add eax, 20
	nop
	sub eax, 00C0FFEE
	sub eax, 20
	loop .L4
	pop ecx
	sub eax, 10
	sub eax, 10
	loop .L2
	mov eax, 0
	mov ecx, 30
.L5:
	add ebx, 30
	add ebx, 300
	add ebx, 3000
	nop
	loop .L5
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
#define ARG_NOP      'n' /* not operation */
#define ARG_LEFT     '<' /* left  register */
#define ARG_RIGHT    '>' /* right register */
#define ARG_PUSH     'u' /* write data */
#define ARG_POP      'o' /* read data */
#define ARG_BEGIN    '('
#define ARG_END      ')'
#define ARG_START    '['
#define ARG_FINISH   ']'
#define ARG_CONTINUE ',' /* using old command */

#define REGISTER_QUANTITY 8

typedef enum {
  REG_EAX, REG_EBX, REG_ECX, REG_EDX, REG_ESP, REG_EBP, REG_ESI, REG_EDI,
} REG;

typedef enum {
/*  0    1    2     3    4      5     6    7    8 */
  ADD, SUB, MOV, PUSH, POP, LABEL, LOOP, RET, NOP
} COM;

typedef enum {
  OPERATION, REGISTER, DATA
} USI;

typedef struct {
  int using;        /* OPERATION, REGISTER or DATA */

  int regist;       /* using eax, ebx, ecx, edx */
  int old_regist;

  int command;      /* using command */

  int loop;         /* index loop metka */
  int loop_counter;
  int loop_corrector;

  int error;        /* correct input data, or not */
} chip;

const char * __command[] = {
  "add", "sub", "mov", "push", "pop",

  "label", "loop", "ret\n", "nop"
};

const char * __register[] = {
  "eax", "ebx", "ecx", "edx",
  "esp", "ebp", "esi", "edi"
};

/* DO NOT CHANGE THIS CODE IF YOU ARE WOODPECKER!!! */
void __assembler(chip * ctx, char data) {
  if (REGISTER == ctx->using) {
    return;
  }
  else
  if (OPERATION == ctx->using) {
    if (LABEL == ctx->command) {
      ctx->loop++;
      printf("\n.L%d:", (ctx->loop > 0) ? (ctx->loop_counter + ctx->loop) : ctx->loop_counter);
    }
    else
    if (LOOP == ctx->command) {
      printf("\n\t%s .L%d", __command[ctx->command],
                           (ctx->loop > 0) ? (ctx->loop_counter + ctx->loop) - ctx->loop_corrector : ctx->loop_counter);
      ctx->loop_corrector++;
      ctx->loop--;
      if (ctx->loop < 0) ctx->loop = 0;
    }
    else
    if (PUSH == ctx->command || POP == ctx->command)
      printf("\n\t%s %s", __command[ctx->command],
                          __register[ctx->regist]);
    else
    if (RET == ctx->command || NOP == ctx->command)
      printf("\n\t%s", __command[ctx->command]);
    else
      printf("\n\t%s %s, ", __command[ctx->command],
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

  if (data == ' ' || data == '\n' || data == '\t') {
    ctx->using = REGISTER;
    return;
  }

  if ((data >= '0' && data <= '9') ||
      (data >= 'A' && data <= 'F')) {

    return;
  }

  if (data == ARG_BEGIN || data == ARG_END ||
      data == '+' || data == '-' || data == '*' || data == '/') {
    return;
  }

/***************************************************************/
  ctx->using = REGISTER;

  if (ARG_LEFT == data) {
    ctx->regist--;

    if (ctx->regist < 0) {
      ctx->regist = ctx->regist + REGISTER_QUANTITY;
    }

    return;
  }

  if (ARG_RIGHT == data) {
    ctx->regist++;

    if (REGISTER_QUANTITY == ctx->regist) {
      ctx->regist = ctx->regist - REGISTER_QUANTITY;
    }    

    return;
  }

/***************************************************************/
  ctx->using = OPERATION;

  if (ARG_FOR == data) {
    ctx->old_regist = ctx->regist;
    ctx->regist = REG_ECX;

    if (ctx->loop > 0) {      /* if FOR() using */
      ctx->command = PUSH;
      __assembler(ctx, data);
    }

    if (ctx->loop_corrector > 0) {
      ctx->loop_corrector = 0;
      ctx->loop_counter++;
    }

    ctx->loop_counter++;

    ctx->command = MOV;
    return;
  }

  /* if OLD command == LOOP */
  if (LOOP == ctx->command) {
    if (ctx->loop > 0) {
      ctx->old_regist = ctx->regist;
      ctx->regist = REG_ECX;
      ctx->command = POP;

      __assembler(ctx, data);

      ctx->regist = ctx->old_regist;
    }
  }

  if (ARG_START == data) {
    ctx->regist = ctx->old_regist;
    ctx->command = LABEL;
    return;
  }

  if (ARG_FINISH == data) {
    ctx->command = LOOP;
    return;
  }

  switch(data) {
    case ARG_ADD:   ctx->command = ADD;  break;
    case ARG_SUB:   ctx->command = SUB;  break;
    case ARG_LOAD:  ctx->command = MOV;  break;
    case ARG_PUSH:  ctx->command = PUSH; break;
    case ARG_POP:   ctx->command = POP;  break;
    case ARG_NOP:   ctx->command = NOP;  break;
    case ARG_EXIT:  ctx->command = RET;  break;
  }
}

int __corrector(char * data, int size) {
  int res = 0;
  int i;

  for (i = 0; (i < size) && data[i]; i++) {
    if ((data[i] >= '0' && data[i] <= '9') ||
        (data[i] >= 'A' && data[i] <= 'F')) {

      continue;
    }

    if (data[i] == '+' || data[i] == '-' || data[i] == '*'  ||
        data[i] == '/' || data[i] == ' ' || data[i] == '\n' || data[i] == '\t') {
      continue;
    }

    if (ARG_FOR      != data[i] && ARG_MALLOC != data[i] &&
        ARG_ADD      != data[i] && ARG_SUB    != data[i] &&
        ARG_LEFT     != data[i] && ARG_RIGHT  != data[i] &&
        ARG_PUSH     != data[i] && ARG_POP    != data[i] &&
        ARG_EXIT     != data[i] && ARG_START  != data[i] &&
        ARG_FINISH   != data[i] && ARG_LOAD   != data[i] &&
        ARG_BEGIN    != data[i] && ARG_END    != data[i] &&
        ARG_CONTINUE != data[i] && ARG_NOP    != data[i]) {

      res = i;
      break;
    }
  }

  return res;
}

void __chip_init(chip * ctx) {
  ctx->regist         = REG_EAX;   /* first register */
  ctx->using          = OPERATION; /* OPERATION, REGISTER or DATA */
  ctx->old_regist     = REG_EAX;   /* old using register */
  ctx->command        = RET;       /* using command */
  ctx->loop           = 0;         /* index loop metka */
  ctx->loop_counter   = 0;         /* counter 'f' metks */
  ctx->loop_corrector = 0;         /*  */
  ctx->error          = 0;         /* correct input data, or not */
}

int main(int argc, char * argv[]) {

  char * string = NULL;

  int result, i, pos = 0;

  chip * ctx = (chip *)malloc(sizeof(chip));

  if (ctx == NULL) return -1;

  __chip_init(ctx);

  if (argc == 2 && argv[1]) {
    string = argv[1];
  }
  else
    string = "u>u>u>u>>>>>l0>l60>l0001>lFFFFFFFF<<<a10f(10*10)[a10,0BADf20[a20,20ns00C0FFEE,20]s10,10]l0>f30[a30,300,3000n]>>o<o<o<oe";

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

