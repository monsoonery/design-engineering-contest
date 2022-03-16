/* AMONG US */
#include "pitches.h"

const uint8_t buzzerPin = 8;

void setup() {
  playCurb();
}

void loop() {

}

void playAmogus() {
  // notes in the melody
  int melody[] = {
    NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_FS4, NOTE_F4, NOTE_DS4, NOTE_C4, 0, NOTE_AS3, NOTE_DS4, NOTE_C4, 0, NOTE_G2, NOTE_C3
  };
  // note durations: 4 = quarter note, 8 = eighth note, etc.
  uint8_t noteDurations[] = {
    4, 4, 4, 4, 4, 4, 4, 2, 8, 8, 4, 2, 4, 4
  };
  // iterate over the notes of the melody
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  for (int note = 0; note < melodyLength; note++) {
    // to calculate the note duration, take one second divided by the note type
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc
    int noteDuration = 1000 / noteDurations[note];
    tone(buzzerPin, melody[note], noteDuration);
    // to distinguish the notes, set a minimum time between them
    // the note's duration + 30% seems to work well
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing
    noTone(buzzerPin);
  }
}

void playCurb() {
    // notes in the melody
  int melody[] = {
    NOTE_G2, 0, NOTE_A3, 0, NOTE_B3, 0, 
    
    NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, 0, 
    NOTE_AS4, 
    NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4, 0, 
    NOTE_AS4, NOTE_B4, 0, NOTE_GS3, 
    NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4, 0,
    
    NOTE_GS3, NOTE_A4, 0, NOTE_AS4,
    NOTE_B4, NOTE_B4, NOTE_B4, NOTE_B4, 0, 
    NOTE_AS4, 
    NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4, 0,
    NOTE_AS4, NOTE_B4, 0,

    NOTE_B4, NOTE_DS4, NOTE_FS4, NOTE_A5, NOTE_B5, 0,
    NOTE_B2, 0,


    NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4, 0, 
    NOTE_GS3, 
    NOTE_G3, NOTE_G3, NOTE_G3, NOTE_G3, 0, 
    NOTE_FS3, NOTE_A4, 0, NOTE_FS3, 
    NOTE_G3, NOTE_G3, NOTE_G3, NOTE_G3, 0,

    NOTE_FS3, NOTE_G3, 0, NOTE_GS3,
    NOTE_A4, NOTE_A4, NOTE_A4, NOTE_A4, 0, 
    NOTE_GS3, 
    NOTE_G3, NOTE_G3, NOTE_G3, NOTE_G3, 0,
    NOTE_GS3, NOTE_A4, 0,
    
    NOTE_CS2, NOTE_E2,  NOTE_A3
  };
  // note durations: 4 = quarter note, 8 = eighth note, etc.
  uint8_t noteDurations[] = {
    4, 8, 4, 8, 4, 8, 
    
    8, 8, 8, 8, 8, 
    8, 
    8, 8, 8, 8, 8,
    8, 8, 8, 8,
    8, 8, 8, 8, 8,

    8, 8, 8, 8,
    8, 8, 8, 8, 8,
    8,
    8, 8, 8, 8, 8,
    8, 8, 8,
    
    32, 32, 32, 32, 4, 8,
    4, 2,

    8, 8, 8, 8, 8, 
    8, 
    8, 8, 8, 8, 8,
    8, 8, 8, 8,
    8, 8, 8, 8, 8,

    8, 8, 8, 8,
    8, 8, 8, 8, 8,
    8,
    8, 8, 8, 8, 8,
    8, 8, 4,

    6, 6, 2
  };
  // iterate over the notes of the melody
  int melodyLength = sizeof(melody) / sizeof(melody[0]);
  for (int note = 0; note < melodyLength; note++) {
    // to calculate the note duration, take one second divided by the note type
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc
    int noteDuration = 1000 / noteDurations[note];
    tone(buzzerPin, melody[note], noteDuration);
    // to distinguish the notes, set a minimum time between them
    // the note's duration + 30% seems to work well
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing
    noTone(buzzerPin);
  }
}
