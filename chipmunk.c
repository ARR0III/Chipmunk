/*
  This is translator for programming language "Chipmunk";

  COMMANDS:
    u - save data from register in stack
    o - load data from stack in register

    > - come to right register
    < - come to lelt register

    n - not operation
    r - move in old register data from active register
    l - load data to register
    a - addition register and data
    s - substruction register and data
    e - exit program
    , - using old command

    f - analog "ecx = ??? while(ecx--){operations}"
    [ - start while block
    ] - end while block

    ( - start data block "l(5*3)>a(7/2)<"
    ) - end data block
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARG_REGISTER 'r' /* move in old register data from active register */
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
#define ARG_CONTINUE ',' /* using old command */
#define ARG_BEGIN    '('
#define ARG_END      ')'
#define ARG_START    '['
#define ARG_FINISH   ']'

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

typedef enum {
  EXP_COMMAND, EXP_OPERAND, EXP_UNKNOWN
} EXP;

typedef struct {
  USI   using;          /* OPERATION, REGISTER or DATA */
  COM   command;        /* using command */

  REG   regist;         /* using eax, ebx, ecx, edx */
  REG   old_regist;     /* old using register */

  EXP   expect;         /* expect data or this alono command */

  int   loop;           /* index loop metka */
  int   loop_counter;
  int   loop_corrector;
} chip;

const char * __command[] = {
  "add", "sub", "mov", "push", "pop",
  "label", "loop", "ret", "nop"
};

const char * __register[] = {
  "eax", "ebx", "ecx", "edx",
  "esp", "ebp", "esi", "edi"
};

/* DO NOT CHANGE THIS CODE IF YOU ARE WOODPECKER!!! */
void __assembler(chip * ctx, char data) {
  if (REGISTER == ctx->using && ARG_REGISTER == data) {
    printf("%s", __register[ctx->regist]);
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
    if (data == ' ' || data == '\n' || data == '\t') {
      return;
    }
    putc(data, stdout);
  }
}

void __parser(chip * ctx, char data) {
/***************************************************************/
  ctx->using = DATA;

  if (data == ' ' || data == '\n' || data == '\t') {
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

  if (ARG_REGISTER == data) {
    return;
  }

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
      ctx->loop_counter += ctx->loop_counter;
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
    case ARG_ADD:      ctx->command = ADD;  break;
    case ARG_SUB:      ctx->command = SUB;  break;
    case ARG_LOAD:     ctx->command = MOV;  break;
    case ARG_PUSH:     ctx->command = PUSH; break;
    case ARG_EXIT:     ctx->command = RET;  break;
    case ARG_POP:      ctx->command = POP;  break;
    case ARG_NOP:      ctx->command = NOP;  break;
    case ARG_CONTINUE: break;
  }
/***************************************************************/
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
        ARG_CONTINUE != data[i] && ARG_NOP    != data[i] && ARG_REGISTER != data[i]) {

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
  ctx->loop_corrector = 0;         /* ---долго объяснять что это за хренатень--- */
  ctx->expect         = EXP_COMMAND;      /* what we are expect */
}

int main(int argc, char * argv[]) {

  char * string = NULL;
  FILE * f      = NULL;

  int result, i, pos = 0;
  int c = 0;

  chip * ctx = (chip *)malloc(sizeof(chip));

  if (ctx == NULL) return -1;

  __chip_init(ctx);

  if (argc == 2 && argv[1]) {
    f = fopen(argv[1], "rb");
    
    if (NULL != f) {
      printf("File open: \"%s\"\n" ,argv[1]);
    }
    else {
      string = argv[1];
    }
  }
  else {
    string = "u>u>u>u<<<l0>l0>l10>lFFf>r[<<<a1,10,100f20[s2,10,100f(3*10)[a30,300s30,300]ulFFa>r<no]]>>l0BAD<<fr[n,n,]>>>o<o<o<o<e";
  }

  if (NULL == f) {
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
  }

  if (f) {
    while ((c = fgetc(f)) != EOF) {
      __parser(ctx, (char)c);
      __assembler(ctx, (char)c);
    }

    putc('\n', stdout);
  }
  else {
    while (string[pos]) {
      __parser(ctx, string[pos]);
      __assembler(ctx, string[pos]);
      pos++;
    }

    putc('\n', stdout);
  }

  free(ctx);

  return 0;
}

