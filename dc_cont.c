#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

typedef unsigned char uint8_t;

#define _BV(bit) (1 << (bit))


/*      Shield      Pi */

#define MOTORLATCH  21
#define MOTORCLK    20
#define MOTORENABLE 12
#define MOTORDATA   16

#define MOTOR_1_PWM  5
#define MOTOR_2_PWM  6

/* 
#define MOTORLATCH  0
#define MOTORCLK    1
#define MOTORENABLE 2
#define MOTORDATA   3
#define MOTOR_1_PWM 4
#define MOTOR_2_PWM 5 */

#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR4_A 0
#define MOTOR4_B 6
#define MOTOR3_A 5
#define MOTOR3_B 7

#define FORWARD  1
#define BACKWARD 2
#define BRAKE    3
#define RELEASE  4

static unsigned char latch_state;

void latch_tx(void)
{
   unsigned char i;

   digitalWrite(MOTORLATCH, LOW);

   digitalWrite(MOTORDATA, LOW);

   for (i=0; i<8; i++)
   {
      digitalWrite(MOTORCLK, LOW);

      if (latch_state & _BV(7-i))
      {
         digitalWrite(MOTORDATA, HIGH);
      }
      else
      {
         digitalWrite(MOTORDATA, LOW);
      }
      digitalWrite(MOTORCLK, HIGH);
   }
   digitalWrite(MOTORLATCH, HIGH);
}


void enable(void)
{
   pinMode(MOTORLATCH,  OUTPUT);
   pinMode(MOTORENABLE, OUTPUT);
   pinMode(MOTORDATA,   OUTPUT);
   pinMode(MOTORCLK,    OUTPUT);

   latch_state = 0;

   latch_tx();

   digitalWrite(MOTORENABLE, LOW);
}

void DCMotorInit(uint8_t num)
{
   switch (num)
   {
      case 1:
         latch_state &= ~_BV(MOTOR1_A) & ~_BV(MOTOR1_B);
         latch_tx();
         break;
      case 2:
         latch_state &= ~_BV(MOTOR2_A) & ~_BV(MOTOR2_B);
         latch_tx();
         break;
      case 3:
         latch_state &= ~_BV(MOTOR3_A) & ~_BV(MOTOR3_B);
         latch_tx();
         break;
      case 4:
         latch_state &= ~_BV(MOTOR4_A) & ~_BV(MOTOR4_B);
         latch_tx();
         break;
   }
}

void DCMotorRun(uint8_t motornum, uint8_t cmd)
{
   uint8_t a, b;

   switch (motornum)
   {
      case 1:
         a = MOTOR1_A; b = MOTOR1_B;
         break;
      case 2:
         a = MOTOR2_A; b = MOTOR2_B;
         break;
      case 3:
         a = MOTOR3_A; b = MOTOR3_B;
         break;
      case 4:
         a = MOTOR4_A; b = MOTOR4_B;
         break;
      default:
         return;
   }
 
   switch (cmd)
   {
      case FORWARD:
         latch_state |= _BV(a);
         latch_state &= ~_BV(b);
         latch_tx();
         break;
      case BACKWARD:
         latch_state &= ~_BV(a);
         latch_state |= _BV(b);
         latch_tx();
         break;
      case RELEASE:
         latch_state &= ~_BV(a);
         latch_state &= ~_BV(b);
         latch_tx();
       break;
   }
}

int main ()
{
   int i, j ;

   if (wiringPiSetup () == -1)
   {
      fprintf (stdout, "oops: %s\n", strerror (errno)) ;
      return 1 ;
   }

   pinMode(MOTOR_1_PWM, OUTPUT);
   pinMode(MOTOR_2_PWM, OUTPUT);

   digitalWrite(MOTOR_1_PWM, 0);
   digitalWrite(MOTOR_2_PWM, 0);

   enable();

   DCMotorInit(1);
   DCMotorInit(2);

   DCMotorRun(1, FORWARD);
   digitalWrite(MOTOR_1_PWM, 1);
   sleep(1);

   DCMotorRun(1, RELEASE);
   sleep(1);

   DCMotorRun(1, BACKWARD);
   sleep(1);

   DCMotorRun(1, RELEASE);
   sleep(1);
   
   DCMotorRun(2, FORWARD);
   digitalWrite(MOTOR_2_PWM, 1);
   sleep(1);

   DCMotorRun(2, RELEASE);
   sleep(1);

   DCMotorRun(2, BACKWARD);
   sleep(1);

   DCMotorRun(2, RELEASE);
   sleep(1);
}
