#include "settings.h"
#include "functions.h"
#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>

/* Funktion um alle Relais auszuschalten */
void aufraeumen()
{
  digitalWrite(MOTOR, LOW);
  for(int i=0; i<8; i++)
  {
    digitalWrite(i, LOW);
  }
  exit(0);
}


/* Signalroutine zum Programmende */
void sigfunc(int sig)
{
  if (sig != SIGINT) return;

  #ifdef DEBUG
  printf("Programmabbruch\n");
  #endif
  aufraeumen();
}


/*ISR*/
void interrupt_0(void)
{
   ++counter;
}


double getWassermenge()
{
   return counter/450.0;
}

void show_help(char* programmname)
{
	printf("\nProgrammaufruf mit %s -c <Kanal> -m <Wassermenge> | -t <Zeit> [-f <Fluss>]\n", programmname);
	exit(1);
}
