#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARG_FOR 'f' /* for */

#define ARG_ZER 'z' /* mov eax, 0 */
#define ARG_MLC 'm' /* malloc array */

#define ARG_ADD 'a' /* addition */
#define ARG_SUB 's' /* substruction */

#define ARG_LFT '<' /* left  move */
#define ARG_RHT '>' /* right move */

#define ARG_PSH 'u' /* write data */
#define ARG_POP 'o' /* read data */

#define ARG_EXT 'e' /* exit program */

#define ARG_SRT '['
#define ARG_FIN ']'

#define ARG_CNT ','

char * command[] = {
  "  add eax, ",
  "  sub eax, ",

  "  push eax\n",
  "  pop eax\n",

  "  ret\n",

  "  mov ecx, ", "label", "loop", "  xor eax, eax\n"
};


int parser(char data) {
  int result = -1;

  if (data >= '0' && data <= '9') {
    return (int)data;
  }

  switch (data) {
    case ARG_ADD: result = 0;
                  break;
    case ARG_SUB: result = 1;
                  break;
    case ARG_PSH: result = 2;
                  break;
    case ARG_POP: result = 3;
                  break;
    case ARG_EXT: result = 4;
                  break;
    case ARG_FOR: result = 5;
                  break;
    case ARG_SRT: result = 6;
                  break;
    case ARG_FIN: result = 7;
                  break;
    case ARG_ZER: result = 8;
                  break;

    case ARG_CNT: break;

    default: break;
  }

  return result;
}

int correct(char * data, int size) {
  int res = 0;
  int i;

  for (i = 0; (i < size) && data[i]; i++) {
    if (data[i] >= '0' && data[i] <= '9') {
      continue;
    }
    
    if (ARG_FOR != data[i] && ARG_MLC != data[i] &&
        ARG_ADD != data[i] && ARG_SUB != data[i] &&
        ARG_LFT != data[i] && ARG_RHT != data[i] &&
        ARG_PSH != data[i] && ARG_POP != data[i] &&
        ARG_EXT != data[i] && ARG_SRT != data[i] &&
        ARG_FIN != data[i] && ARG_CNT != data[i] && ARG_ZER != data[i]) {

      res = i;
      break;
    }
  }

  return res;
}

int main (int argc, char * argv[]) {

  char * string = NULL;
  int result, old_result;

  int pos = 0;

  int i;
  int number = 0;
  int regist = 0;

  int label = 0;
  int old_label = 0;

  if (argc == 2 && argv[1]) {
    string = argv[1];
  }
  else
    string = "uza255s309a96,300,91,559s631,9,800uf32[a64]oe";

  result = correct(string, strlen(string));

  if (result) {
    printf("Error symbol position = %d, symbol = %c;\n", result, string[result]);
    printf("data:%s\n", string);
   
    for (i = 0; i < (result + 5); i++)
      putc(' ', stdout);

    putc('^', stdout);
    putc('\n', stdout);
    
    return -1;
  }

  while (string[pos]) {
    result = parser(string[pos]);

    /* number */
    if (result >= '0' && result <= '9') {
      if (0 == regist) {
        regist = 1;
      }

      printf("%c", result);
    }
    /* command */
    else {
      if (regist > 0) {
        regist = 0;

        putc('\n', stdout);

        if (-1 == result) {
          result = old_result;
        }
      }

      printf("%s", command[result]);
      
      if (ARG_SRT == string[pos]) {
        printf(" %.6dL:\n", label);
        old_label = label;
        label++;
      }

      if (ARG_FIN == string[pos]) {
        printf(" %.6dL\n", old_label);
      }

      old_result = result;
    }

    pos++;
  }

  return 0;
}












