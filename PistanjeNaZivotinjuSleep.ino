/*
  Opis: pistanje kad zec predje na zabranjeni deo kauca, kod nekih zica i sl.
  Arduino: ATtiny85
  Napajanje: 18650
  Senzori:
    - PIR - detekcija zivotinje
  Aktuatori:
    - Buzzer
  Napomene:
    - Mozda bi trebalo ubaciti V boost zbog stabilnog 5V napona.
*/

#include <avr/sleep.h>
#include <avr/interrupt.h>

const int pinPIR = 2;          // PIR senzor; ako se promeni ova vrednost, obavezno izmeniti PCINT? u sleep() i setup()
const int pinBuzz = 0;         // buzzer/zvucnik
const int itvBuzzOn = 350;     // trajanje u msec piska
const int itvBuzzOff = 700;    // trajanje u msec vremena izmedju 2 piska
const int buzzLevel = 4;       // jacina zvuka na buzzer-u
volatile long msPirSignal = 0; // vreme (poslednjeg) PIR signala
bool firstPirSignal = true;    // prvi PIR signal se javlja odmah po paljenju aparata i smatra se laznim

void setup()
{
  pinMode(pinPIR, INPUT);

  pinMode(pinBuzz, OUTPUT);
  digitalWrite(pinBuzz, false);

  GIMSK |= _BV(PCIE);   // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT2); // Use PBX as interrupt pin
  sei();                // Enable interrupts
}

void sleep()
{
  GIMSK |= _BV(PCIE);                  // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT2);                // Use PBX as interrupt pin
  ADCSRA &= ~_BV(ADEN);                // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // replaces above statement
  sleep_enable();                      // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                               // Enable interrupts
  sleep_cpu();                         // sleep
}

ISR(PCINT0_vect)
{
  msPirSignal = millis();
}

void loop()
{
  if (digitalRead(pinPIR) && !firstPirSignal)
  {
    int itv = (millis() - msPirSignal) % (itvBuzzOn + itvBuzzOff);
    analogWrite(pinBuzz, itv < itvBuzzOn ? buzzLevel : 0);
  }
  else
  {
    firstPirSignal = false;
    digitalWrite(pinBuzz, false);
    sleep();
  }
}
