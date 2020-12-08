#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define SEND_A 0
#define REC_A 3

#define SEND_B 1
#define REC_B 4

#define SEND_C 2
#define REC_C 5

int canal[6] = {-1, -1, -1, -1, -1, -1};

void receive(int *value, int index)
{
  while (canal[index] == -1)
    ;
  *value = canal[index];
  canal[index] = -1;
  return;
}

void sendAsync(int *value, int index)
{
  canal[index] = *value;
  return;
}

int getMajorityVote(int votos[], int *versao_erro)
{
  if ((votos[0] == votos[1]) && (votos[1] == votos[2]))
  {
    *versao_erro = 0;
    return votos[0];
  }
  if ((votos[0] != votos[1]) && (votos[1] == votos[2]))
  {
    *versao_erro = 1;
    return votos[1];
  }
  if ((votos[0] == votos[2]) && (votos[1] != votos[2]))
  {
    *versao_erro = 2;
    return votos[0];
  }
  if ((votos[0] == votos[1]) && (votos[1] != votos[2]))
  {
    *versao_erro = 3;
    return votos[0];
  }
  return 0;
}

void *thread_code_a(void *threadno)
{
  int status;
  int voto = 7 + 3;

  sendAsync(&voto, SEND_A);
  receive(&status, REC_A);

  if (status == 0)
  {
    printf("\nThread A continua.");
    while (1)
      ;
  }
  else
  {
    printf("\nThread A gerou erro.");
    return NULL;
  }
}

void *thread_code_b(void *threadno)
{
  int status;
  int voto = 3 * 5;

  sendAsync(&voto, SEND_B);
  receive(&status, REC_B);

  if (status == 0)
  {
    printf("\nThread B continua.");
    while (1)
      ;
  }
  else
  {
    printf("\nThread B gerou erro.");
    return NULL;
  }
}

void *thread_code_c(void *threadno)
{
  int status;
  int voto = 30 / 2;

  sendAsync(&voto, SEND_C);
  receive(&status, REC_C);

  if (status == 0)
  {
    printf("\nThread C continua.");
    while (1)
      ;
  }
  else
  {
    printf("\nThread C gerou erro.");
    return NULL;
  }
}

void *driver(void *threadno)
{
  int votos[3];
  int majority;
  int error_thread;
  int ok_code = 0;
  int error_code = 1;

  pthread_t TA;
  pthread_t TB;
  pthread_t TC;

  pthread_create(&TA, NULL, thread_code_a, NULL);
  pthread_create(&TB, NULL, thread_code_b, NULL);
  pthread_create(&TC, NULL, thread_code_c, NULL);

  receive(&votos[0], SEND_A);
  receive(&votos[1], SEND_B);
  receive(&votos[2], SEND_C);

  majority = getMajorityVote(votos, &error_thread);

  printf("\nVoto majoritario: %d", majority);

  if (error_thread == 0)
  {
    sendAsync(&ok_code, REC_A);
    sendAsync(&ok_code, REC_B);
    sendAsync(&ok_code, REC_C);
  }

  else if (error_thread == 1)
  {
    sendAsync(&error_code, REC_A);
    sendAsync(&ok_code, REC_B);
    sendAsync(&ok_code, REC_C);
  }

  else if (error_thread == 2)
  {
    sendAsync(&ok_code, REC_A);
    sendAsync(&error_code, REC_B);
    sendAsync(&ok_code, REC_C);
  }

  else if (error_thread == 3)
  {
    sendAsync(&ok_code, REC_A);
    sendAsync(&ok_code, REC_B);
    sendAsync(&error_code, REC_C);
  }

  return NULL;
}

int main(void)
{
  pthread_t TD;
  pthread_create(&TD, NULL, driver, NULL);

  system("sleep 1");
  printf("\n");

  return 0;
}