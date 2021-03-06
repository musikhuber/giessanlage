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
#include <unistd.h>
#include <getopt.h>

int main(int argc, char **argv)
{
  int pin;
  double fluss=0;
  int flowErrorCounter=0;
  int duty=0;
//  int input;
  int kanal=-1;
//  int modus=0;
  int mflag=0;
  int fflag=0;
//  char *mvalue=NULL;
//  char *tvalue=NULL;
  int tflag=0;
  double wMenge=-1.0;
  int wZeit=-1.0;
  int wFluss=0;
//  double geflossenesWasser=-1.0;
//  time_t start, jetzt, ende, laufzeit;
//  char buffer[100];
  int dauer=0;
  int c;
  
  /* Signalhandler einrichten */
  signal(SIGINT, sigfunc);

  /* Parameteranzahl prüfen */
  if (argc<3)
  {
    show_help(argv[0]);
    exit(1);
  }

  
  while((c=getopt(argc,argv,"c:m:t:f:"))!=-1)
  {
	  switch(c)
	  {
	  case 'c':
	          kanal=atoi(optarg);
                  break;
	  case 'm':
		  mflag=1;
		  wMenge=atof(optarg);
		  break;
	  case 't':
		  tflag=1;
		  wZeit=atoi(optarg);
		  break;
          case 'f':
                  fflag=1;
                  wFluss=atoi(optarg);
                  break;
	  case '?':
		  if(optopt=='m')
		  {
			  fprintf(stderr, "Die Option -%c benoetigt die Anzahl an Litern.\n", optopt);
		  }
		  else if (isprint(optopt))
		  {
			  fprintf(stderr, "Unbekannte Option -%c.\n", optopt);
		  }
		  else
		  {
			  fprintf(stderr, "Unbekanntes Zeichen.\n");
		  }
		  return 1;
	  default:
		  show_help(argv[0]);
	  }
  }
  
  /* Nun stehen die Parameter in den Variablen kanal, wMenge, wZeit */
  
  /* Flags prüfen -- es darf nur gegossen werden wenn etntweder die Wassermenge oder die Zeit angegeben wurde */
  if(mflag && tflag)
  {
    fprintf(stderr, "Es darf entweder die Wassermenge oder die Zeit angegeben werden");
    show_help(argv[0]);
  }
 

  /* Kanalnummer prüfen */
  if(kanal>=CHMIN && kanal<=CHMAX)
  {
    #ifdef DEBUG
    printf("Kanal: %i\n", kanal);
    #endif
  }
  else
  {
    fprintf(stderr, "Kanalnummer muss zwischen %i und %i liegen\n", CHMIN, CHMAX);
    exit(1);
  }

  /* Wassermenge prüfen */
  if (mflag)
  {
    if(wMenge>=WATER_MIN && wMenge<=WATER_MAX)
    {
      #ifdef DEBUG
      printf("Wassermenge: %.2lf\n", wMenge);
      #endif
    }
    else
    {
      fprintf(stderr, "Wassermenge muss zwischen %.2lf und %.2lf liegen\n", WATER_MIN, WATER_MAX);
      exit(1);
    }
  }
  
  
  if(wiringPiSetup()==-1) 
  {
    fprintf(stderr, "Fehler bei der Initialisierung der wiringPi Schnittstelle\n");
    exit(1);
  }



  /* PINs initialisieren */  
  for(pin=0; pin<8; pin++)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }

  /* PWM initialisieren */  
  softPwmCreate(PWM_PIN, duty, 500);

  /* ISR einrichten */
  pinMode(INT_PIN, INPUT);
  pullUpDnControl(INT_PIN, PUD_UP);
  if(wiringPiISR(INT_PIN, INT_EDGE_RISING, interrupt_0) < 0 )
  {
    fprintf(stderr, "Fehler bei der Einrichtung der Interrupt Service Routine\n");
    exit(1);
  }

  
  /* Motor mit Strom versorgen */
  digitalWrite(MOTOR, HIGH);

  /* Kanal auf */
  digitalWrite(kanal, HIGH);

  /* PWM ein */
    /* Fluss prüfen */
  if(fflag)
  {
    if(wFluss>=FLOWMIN && wFluss<=FLOWMAX)
    {
      #ifdef DEBUG
      printf("Fluss auf %i eingestellt.\n", wFluss);
      #endif
      softPwmWrite(PWM_PIN, wFluss);
      
    }
  }
  else
  {
    switch(kanal)
    {
      case 1: softPwmWrite(PWM_PIN, FLOW1);
              break;
      case 2: softPwmWrite(PWM_PIN, FLOW2);
              break;
      case 3: softPwmWrite(PWM_PIN, FLOW3);
              break;
      case 4: softPwmWrite(PWM_PIN, FLOW4);
              break;
      case 5: softPwmWrite(PWM_PIN, FLOW5);
              break;
      case 6: softPwmWrite(PWM_PIN, FLOW6);
              break;
      default: softPwmWrite(PWM_PIN, 0);
             break;
    }
  }
  

  /* GIESSEN */
  if (mflag)	// Giessen mit Wassermenge
  {
    while (getWassermenge() <=wMenge)
    {
      delay(1000);
      dauer++;
      fluss=getWassermenge()/dauer;
      if (fluss<MINFLOW) flowErrorCounter++;
      if (flowErrorCounter>100) exit(2);
      #ifdef DEBUG
      printf("Kanal: %i noch %.2lf Liter zu giessen. Fluss=%.5lf FEC=%i\n", kanal, wMenge-getWassermenge(), fluss, flowErrorCounter);
      #endif
      
    }
    
  }
  
  if (tflag)    // Giessen nach Zeit
  {
    while (dauer < wZeit)
    {
      delay(1000);
      dauer++;
      fluss=getWassermenge()/dauer;
      if (fluss<MINFLOW) flowErrorCounter++;
      if (flowErrorCounter>100 && kanal==5) exit(2);
      #ifdef DEBUG
      printf("Kanal: %i noch %i Sekunden zu giessen. Fluss=%.5lf FEC=%i\n", kanal, wZeit-dauer, fluss, flowErrorCounter);
      #endif
      
    }
    
  }
  

  /* Aufraeumen */
  digitalWrite(MOTOR, LOW);
  digitalWrite(kanal, LOW);
  
  #ifdef  DEBUG
  printf("FERTIG! Dauer %i Sekunden;-)\n", dauer);
  #endif

  return EXIT_SUCCESS;
}
