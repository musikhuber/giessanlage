#ifndef SETTINGS_H
#define SETTINGS_H


#define __USE_POSIX2

#define PWM_PIN 16
#define INT_PIN 15
#define MOTOR   0
#define CH1     1   // Tomatenhaus
#define CH2     2   // Terasse
#define CH3     3   // Erdbeeren
#define CH4     4   // Hof
#define CH5     5   // Sprenkler
#define CH6     6   // nc

#define CHMIN   1
#define CHMAX   6

#define FLOW1 150
#define FLOW2 250
#define FLOW3 140
#define FLOW4 180
#define FLOW5 500 
#define FLOW6 500

#define FLOWMIN 80
#define FLOWMAX 500

#define WATER_MIN 0.01
#define WATER_MAX 20.0

#define MINFLOW 0.005

#define TRUE 1
#define FALSE 0

#define BUF   255

#define DEBUG 1

static volatile int counter=0;


#endif
