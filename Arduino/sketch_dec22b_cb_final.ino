#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Bounce.h>

#include <play_sd_mp3.h>

AudioPlaySdMp3           playMp31;
AudioMixer4              mixer1;
AudioOutputAnalog        dac1;

AudioConnection          patchCord1(playMp31, 0, mixer1, 0);
AudioConnection          patchCord2(mixer1, dac1);

Bounce nextButton = Bounce(3, 150);

String albums[7] = {"A", "B", "C", "D", "E", "F", "G"};
int numberTracks[7] = {36, 39, 9, 15, 21, 18, 24};

int albumKnob = 0;
int currentAlbum = 0;
int currentTrack = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(3, INPUT_PULLUP);
  
  AudioMemory(25);

  SPI.setMOSI(7);
  SPI.setSCK(14);

  if (!(SD.begin(10))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  playTrack();
}

void playTrack() {
  char albumName[2];
  albums[currentAlbum].toCharArray(albumName, 2);
  
  char filepath[9];
  sprintf(filepath, "%s/%02d.MP3", albumName, currentTrack + 1);
  
  playMp31.play(filepath);
  Serial.println(filepath);
}

void loop() {
  if (nextButton.update()) {
    if (nextButton.fallingEdge()) {
      nextTrack();
      playTrack();
    }
  } else {
    int albumWas = currentAlbum;
    albumKnob = analogRead(1);
    currentAlbum = albumKnob / 170;
  
    if (albumWas != currentAlbum) {
      currentTrack = 0;
      playTrack();
      delay(1200);
    }
  }

  if (!playMp31.isPlaying()) {
    nextTrack();
    playTrack();
  }
}

void nextTrack() {
  int total = numberTracks[currentAlbum];
  currentTrack = (currentTrack + 1) % total;
}

