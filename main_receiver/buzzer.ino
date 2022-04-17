#include "pitches.h"

int notePlaying;
int tempo = 1000;
const uint8_t pauseBetweenNotes = 50;
uint8_t currentNote;
int currentNoteDuration;
long lastNotePlayed = 0;
boolean loopMelody = false;
boolean melodyPlaying = false;

/* MELODY LIST */
int amogusMelody[] = {
  NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_FS4, NOTE_F4, NOTE_DS4, NOTE_C4, 0, NOTE_AS3, NOTE_DS4, NOTE_C4, 0, NOTE_G2, NOTE_C3
};
uint8_t amogusNoteDurations[] = {
  4, 4, 4, 4, 4, 4, 4, 2, 8, 8, 4, 2, 4, 4
};
int amogusMelodyLength = sizeof(amogusMelody) / sizeof(amogusMelody[0]);

int curbMelody[] = {
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
uint8_t curbNoteDurations[] = {
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
int curbMelodyLength = sizeof(curbMelody) / sizeof(curbMelody[0]);

int sansMelody[] = {
  NOTE_D4, NOTE_D4, NOTE_D5, NOTE_A4, 0, NOTE_GS4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4,
};
uint8_t sansNoteDurations[] = {
  8, 8, 4, 3, 16, 4, 4, 4, 8, 8, 8
};
int sansMelodyLength = sizeof(sansMelody) / sizeof(sansMelody[0]);

int backwardsMelody[] = {1000 , 0};
uint8_t backwardsNoteDurations[] = {2, 2};
int backwardsMelodyLength = sizeof(backwardsMelody) / sizeof(backwardsMelody[0]);

int spaceMelody[] = {NOTE_C3, NOTE_G3, NOTE_C4, 0, NOTE_E4, NOTE_DS4};
uint8_t spaceNoteDurations[] = {2, 2, 2, 8, 8, 2};
int spaceMelodyLength = sizeof(spaceMelody) / sizeof(spaceMelody[0]);

int denyMelody[] = {0, NOTE_C3, NOTE_G3};
uint8_t denyNoteDurations[] = {4, 8, 8};
int denyMelodyLength = sizeof(denyMelody) / sizeof(denyMelody[0]);

int acceptMelody[] = {0, NOTE_G4, NOTE_C4};
uint8_t acceptNoteDurations[] = {4, 8, 8};
int acceptMelodyLength = sizeof(acceptMelody) / sizeof(acceptMelody[0]);

/* MELODY LIST */
enum melodyType {
  PASS,
  AMOGUS,
  CURB,
  SANS,
  BACKWARDS,
  SPACE,
  DENY,
  ACCEPT,
  NOSOUND
};

melodyType currentMelody = NOSOUND;

/* MELODY STARTERS */
void startAmogusMelody(int melody_tempo, boolean loop) {
  if (!melodyPlaying) {
    tempo = melody_tempo;
    loopMelody = loop;
    currentNote = 0;
    currentNoteDuration = tempo / amogusNoteDurations[0];
    melodyPlaying = true;
    currentMelody = AMOGUS;
  }
}

void startCurbMelody(int melody_tempo, boolean loop) {
  if (!melodyPlaying) {
    tempo = melody_tempo;
    loopMelody = loop;
    currentNote = 0;
    currentNoteDuration = tempo / curbNoteDurations[0];
    melodyPlaying = true;
    currentMelody = CURB;
  }
}

void startSansMelody(int melody_tempo, boolean loop) {
  if (!melodyPlaying) {
    tempo = melody_tempo;
    loopMelody = loop;
    currentNote = 0;
    currentNoteDuration = tempo / sansNoteDurations[0];
    melodyPlaying = true;
    currentMelody = SANS;
  }
}

void startBackwardsMelody() {
  if (!melodyPlaying) {
    tempo = 1000;
    loopMelody = true;
    currentNote = 0;
    currentNoteDuration = tempo / backwardsNoteDurations[0];
    melodyPlaying = true;
    currentMelody = BACKWARDS;
  }
}

void startSpaceMelody(int melody_tempo, boolean loop) {
  if (!melodyPlaying) {
    tempo = melody_tempo;
    loopMelody = loop;
    currentNote = 0;
    currentNoteDuration = tempo / spaceNoteDurations[0];
    melodyPlaying = true;
    currentMelody = SPACE;
  }
}

void startDenyMelody(int melody_tempo, boolean loop) {
  if (!melodyPlaying) {
    tempo = melody_tempo;
    loopMelody = loop;
    currentNote = 0;
    currentNoteDuration = tempo / denyNoteDurations[0];
    melodyPlaying = true;
    currentMelody = DENY;
  }
}

void startAcceptMelody(int melody_tempo, boolean loop) {
  if (!melodyPlaying) {
    tempo = melody_tempo;
    loopMelody = loop;
    currentNote = 0;
    currentNoteDuration = tempo / acceptNoteDurations[0];
    melodyPlaying = true;
    currentMelody = ACCEPT;
  }
}

void stopMelody() {
  melodyPlaying = false;
  noTone(buzzerPin);
  currentMelody = NOSOUND;
}

/* MELODY UPDATER */
void updateMelody() {
  if (melodyPlaying) {
    if (millis() - lastNotePlayed > currentNoteDuration) {
      switch (currentMelody) {
        case AMOGUS:
          if (currentNote == amogusMelodyLength && loopMelody) {
            currentNote = 0;
          } else if (currentNote == amogusMelodyLength && !loopMelody) {
            melodyPlaying = false;
            noTone(buzzerPin);
          }
          currentNoteDuration = tempo / amogusNoteDurations[currentNote];
          tone(buzzerPin, amogusMelody[currentNote], currentNoteDuration - pauseBetweenNotes);
          lastNotePlayed = millis();
          break;

        case CURB:
          if (currentNote == curbMelodyLength && loopMelody) {
            currentNote = 0;
          } else if (currentNote == curbMelodyLength && !loopMelody) {
            melodyPlaying = false;
            noTone(buzzerPin);
          }
          currentNoteDuration = tempo / curbNoteDurations[currentNote];
          tone(buzzerPin, curbMelody[currentNote], currentNoteDuration - pauseBetweenNotes);
          lastNotePlayed = millis();
          break;

        case SANS:
          if (currentNote == sansMelodyLength && loopMelody) {
            currentNote = 0;
          } else if (currentNote == sansMelodyLength && !loopMelody) {
            melodyPlaying = false;
            noTone(buzzerPin);
          }
          currentNoteDuration = tempo / sansNoteDurations[currentNote];
          tone(buzzerPin, sansMelody[currentNote], currentNoteDuration - pauseBetweenNotes);
          lastNotePlayed = millis();
          break;

        case BACKWARDS:
          if (currentNote == backwardsMelodyLength && loopMelody) {
            currentNote = 0;
          } else if (currentNote == backwardsMelodyLength && !loopMelody) {
            melodyPlaying = false;
            noTone(buzzerPin);
          }
          currentNoteDuration = tempo / backwardsNoteDurations[currentNote];
          tone(buzzerPin, backwardsMelody[currentNote], currentNoteDuration - pauseBetweenNotes);
          lastNotePlayed = millis();
          break;

        case SPACE:
          if (currentNote == spaceMelodyLength && loopMelody) {
            currentNote = 0;
          } else if (currentNote == spaceMelodyLength && !loopMelody) {
            melodyPlaying = false;
            noTone(buzzerPin);
          }
          currentNoteDuration = tempo / spaceNoteDurations[currentNote];
          tone(buzzerPin, spaceMelody[currentNote], currentNoteDuration - pauseBetweenNotes);
          lastNotePlayed = millis();
          break;

        case DENY:
          if (currentNote == denyMelodyLength && loopMelody) {
            currentNote = 0;
          } else if (currentNote == denyMelodyLength && !loopMelody) {
            melodyPlaying = false;
            noTone(buzzerPin);
          }
          currentNoteDuration = tempo / denyNoteDurations[currentNote];
          tone(buzzerPin, denyMelody[currentNote], currentNoteDuration - pauseBetweenNotes);
          lastNotePlayed = millis();
          break;

        case ACCEPT:
          if (currentNote == acceptMelodyLength && loopMelody) {
            currentNote = 0;
          } else if (currentNote == acceptMelodyLength && !loopMelody) {
            melodyPlaying = false;
            noTone(buzzerPin);
          }
          currentNoteDuration = tempo / acceptNoteDurations[currentNote];
          tone(buzzerPin, acceptMelody[currentNote], currentNoteDuration - pauseBetweenNotes);
          lastNotePlayed = millis();
          break;
      }
      currentNote++;
    }
  } else {
    noTone(buzzerPin);
  }
}
