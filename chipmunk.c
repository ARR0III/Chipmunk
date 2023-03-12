/*
  This is translator for programming language "Chipmunk";

  BEFORE: "<<<ul<rn>>>>l10f<<r[>l20<a>rn]<<<<oe"
  
  AFTER:
	push ebp
	mov ebp, esp
	nop
	mov eax, 10
	mov ecx, eax
.L2:
	mov ebx, 20
	add eax, ebx
	nop
	dec ecx
	cmp ecx, 0
	jne .L2
	pop ebp
	ret
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef OS_WINDOWS
#define OS_WINDOWS 1
#endif

#define ARG_DEC      'd' /* decrement register */
#define ARG_INC      'i' /* increment register */
#define ARG_REGISTER 'r' /* using active register */
#define ARG_LOAD     'l' /* mov data in active register */
#define ARG_MALLOC   'm' /* malloc array */
#define ARG_ADD      'a' /* addition active register and (data/active register) */
#define ARG_SUB      's' /* substruction active register and (data/active register) */
#define ARG_EXIT     'e' /* exit program */
#define ARG_NOP      'n' /* not operation */
#define ARG_LEFT     '<' /* left register */
#define ARG_RIGHT    '>' /* right register */
#define ARG_PUSH     'u' /* write data in stack */
#define ARG_POP      'o' /* read data from stack */
#define ARG_CONTINUE ',' /* using old command */

#define ARG_FOR      'f' /* while cycle */
#define ARG_START    '[' /* start for cycle */
#define ARG_FINISH   ']' /* finish for cycle */
#define ARG_BEGIN    '(' /* begin block data */
#define ARG_END      ')' /* end block data */

typedef size_t DATA_T;

typedef struct stack {
  DATA_T         data;
  struct stack * prev;
} node_t, stack_t;

typedef enum {
  REG_EAX, REG_EBX, REG_ECX, REG_EDX,
  REG_ESP, REG_EBP, REG_ESI, REG_EDI

} REG;

typedef enum {
/*  0    1    2     3    4      5     6    7    8    9   10 */
  ADD, SUB, MOV, PUSH, POP, LABEL, LOOP, RET, NOP, INC, DEC
} COM;

typedef enum {
  OPERATION, REGISTER, DATA
} USI;

typedef enum {
  EXP_COMMAND, EXP_OPERAND, EXP_NEXT
} EXP;

typedef struct {
  USI   using;        /* OPERATION, REGISTER or DATA */
  COM   command;      /* using command */

  REG   regist;       /* using eax, ebx, ecx, edx */
  REG   old_regist;   /* old using register */

  EXP   expect;       /* expect data or this alono command */

  int   loop;
  int   loop_counter; /* counter "for" cycles */
} chip;

const char * __command[] = {
  "add", "sub", "mov", "push", "pop",
  "label", 

  "\n\tdec ecx\n\tcmp ecx, 0\n\tjne",

  "ret", "nop", "inc", "dec"
};

const char * __register[] = {
  "eax", "ebx", "ecx", "edx",
  "esp", "ebp", "esi", "edi"
};

void stack_push(stack_t ** s, int loop) {
  node_t * tmp = NULL;
           tmp = (node_t *)malloc(sizeof(node_t));

  if (NULL == tmp) {
    printf("[!] ERROR: cannot allocate memory!\n");
    exit(-1);
  }

  tmp->data = loop;
  tmp->prev = *s;
  *s = tmp;
}

void stack_pop(stack_t ** s, int * loop) {
  node_t * tmp;

  if (NULL == *s) {
    return;
  }

  *loop = (*s)->data;

  tmp = *s;
  *s = (*s)->prev;
  free(tmp);
}

/* if stack == NULL then return code 1 */
int stack_empty(stack_t ** s) {
  return (NULL == *s);
}

void stack_burn(stack_t ** s) {
  node_t * tmp;

  while(!stack_empty(s)) {
    tmp = (*s)->prev;
    (*s)->data = 0; /* for security */
    free(*s);
    *s = tmp;
  }
}

/* DO NOT CHANGE THIS CODE IF YOU ARE WOODPECKER!!! */
void __assembler(stack_t ** s, chip * ctx, char data) {
  int loop_metka = 0;

  if (OPERATION == ctx->using) {
    if (LABEL == ctx->command) {
      stack_push(s, ctx->loop_counter);
      printf("\n.L%d:", ctx->loop_counter);
    }
    else
    if (LOOP == ctx->command) {
      stack_pop(s, &loop_metka);
      printf("%s .L%d", __command[ctx->command], loop_metka);
    }
    else
    if (INC  == ctx->command || DEC == ctx->command ||
        PUSH == ctx->command || POP == ctx->command)
		
      printf("\n\t%s %s", __command[ctx->command], __register[ctx->regist]);
    else
    if (RET == ctx->command || NOP == ctx->command)
      printf("\n\t%s", __command[ctx->command]);
    else
      printf("\n\t%s %s, ", __command[ctx->command], __register[ctx->regist]);
  }
  else
  if (REGISTER == ctx->using && ARG_REGISTER == data) {
    printf("%s", __register[ctx->regist]);
  }
  else
  if (DATA == ctx->using) {
    if (data == ' ' || data == '\n' || data == '\t') {
      return;
    }
    putc(data, stdout);
  }
}

void __parser(stack_t ** s, chip * ctx, char data) {
/***************************************************************/
  ctx->using  = DATA;

#ifdef OS_WINDOWS /* перевод строки в Windows системах */
  if (data == 0x0D) return;
#endif

  if (data == ' ' || data == 0x0A || data == '\t') {
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
  ctx->using  = REGISTER;

  if (ARG_REGISTER == data) {
    return;
  }

  if (ARG_LEFT == data) {
    if (REG_EAX == ctx->regist) {
      ctx->regist = REG_EDI;
    }
    else {
      ctx->regist--;
    }

    return;
  }

  if (ARG_RIGHT == data) {
    if (REG_EDI == ctx->regist) {
      ctx->regist = REG_EAX;
    }
    else {
      ctx->regist++;
    } 

    return;
  }
/***************************************************************/
  ctx->using = OPERATION;

  /* if OLD command == LOOP */
  if (LOOP == ctx->command) {
    if (ctx->loop > 0) {
      ctx->old_regist = ctx->regist;
      ctx->regist = REG_ECX;
      ctx->command = POP;

      __assembler(s, ctx, data);

      ctx->regist = ctx->old_regist;
    }
  }

  if (ARG_FOR == data) {
    ctx->old_regist = ctx->regist;
    ctx->regist = REG_ECX;

    if (ctx->loop > 0) {      /* if FOR() using */
      ctx->command = PUSH;
      __assembler(s, ctx, data);
    }

    ctx->loop_counter++;
    ctx->command = MOV;
    return;
  }

  if (ARG_START == data) {
    ctx->regist = ctx->old_regist;
    ctx->command = LABEL;
    ctx->loop++;
    return;
  }

  if (ARG_FINISH == data) {
    ctx->command = LOOP;
    ctx->loop--;
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
    case ARG_INC:      ctx->command = INC;  break;
    case ARG_DEC:      ctx->command = DEC;  break;
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
        ARG_CONTINUE != data[i] && ARG_NOP    != data[i] &&
        ARG_REGISTER != data[i] && ARG_DEC    != data[i] && ARG_INC != data[i]) {

      res = i;
      break;
    }
  }

  return res;
}

void __chip_init(chip * ctx) {
  ctx->using        = OPERATION; /* OPERATION, REGISTER or DATA */
  ctx->command      = NOP;       /* using command */
  ctx->regist       = REG_EAX;   /* using eax, ebx, ecx, edx */
  ctx->old_regist   = REG_EAX;   /* old using register */

  ctx->expect       = 0;         /* expect data or this alone command */

  ctx->loop         = 0;         /* translate("счетчик вложенных циклов"); */
  ctx->loop_counter = 0;         /* counter "for" cycles */
}

int main(int argc, char * argv[]) {

  char * string = NULL;
  FILE * f      = NULL;

  int result, i;

  int pos = 0;
  int c = 0;

  stack_t * stack = NULL; /* dynamic list loop metks */

/*****************************************************************************/

  chip * ctx = (chip *)malloc(sizeof(chip));

  if (NULL == ctx) return -1;

  __chip_init(ctx);

/*****************************************************************************/

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
      __parser(&stack, ctx, (char)c);
      __assembler(&stack, ctx, (char)c);
    }

    putc('\n', stdout);
  }
  else {
    while (string[pos]) {
      __parser(&stack, ctx, string[pos]);
      __assembler(&stack, ctx, string[pos]);
      pos++;
    }

    putc('\n', stdout);
  }

/*****************************************************************************/

  stack_burn(&stack);

  free(ctx);

/*****************************************************************************/

  return 0;
}

