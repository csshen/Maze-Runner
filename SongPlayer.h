#pragma once
#include "mbed.h"
// new class to play a note on Speaker based on PwmOut class
// SLIGHT MODIFICATIONS by Chris Shen
class SongPlayer
{
public:
    SongPlayer(PinName pin) : _pin(pin), music(false) {}
// class method to play a note based on PwmOut class
    void PlaySong(int s, bool oo, float frequency[], float duration[], float volume=0.05) {
        vol = volume;
        notecount = 0;
        _pin.period(1.0/frequency[notecount]);
        _pin = volume/2.0;
        noteduration.attach(this,&SongPlayer::nextnote, duration[notecount]);
        // setup timer to interrupt for next note to play
        frequencyptr = frequency;
        durationptr = duration;
        //returns after first note starts to play
        size = s;
        loop = oo;
    }
    //Interrupt Routine to play next note
    void nextnote() {
        _pin = 0.0;
        if (loop) {
            notecount = (notecount+1)%size; //setup next note in song
        } else {
            if (++notecount >= size) {
                _pin = 0.0;
                return;    
            }
        }
        if (music) {
            _pin.period(1.0/frequencyptr[notecount]);
            noteduration.attach(this,&SongPlayer::nextnote, durationptr[notecount]);
            _pin = vol/2.0;
        } else {
            _pin = 0.0; //turn off on last note
        }
    }
    void turnOff() {
        music = false;
    }
    void turnOn() {
        music = true;
        _pin = p21;
    }
private:
    Timeout noteduration;
    PwmOut _pin;
    int notecount, size;
    float vol;
    float * frequencyptr;
    float * durationptr;
    bool music;
    bool loop;
};