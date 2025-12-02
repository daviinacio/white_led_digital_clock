#include <Thread.h>
#include <ThreadController.h>
#include "MusicHelper.hpp"
#include "assets/music/base.h"
#include "drivers/Buzzer.hpp"

#define NOTE_GAP_LOOPS 2
#define MAX_REPEAT_LEVELS 5

#ifndef WLDC_MUSIC_VOICE_PLAYER_H
#define WLDC_MUSIC_VOICE_PLAYER_H

// class MusicVoicePlayerObserver {
//   public:
//     virtual void on_player_finished() = 0;
// };

class RepeatNotation {
public:
  uint16_t start_position = 0;
  uint8_t  count = 0;
};

class MusicVoicePlayer {
protected:
  MusicHelper* helper;

  uint16_t content_cursor = 0;
  const uint8_t* content_pointer = nullptr;
  uint8_t voice_index = 0;
  uint8_t remaining_timing_loops = 0;
  RepeatNotation* repeats[MAX_REPEAT_LEVELS];
  uint8_t repeat_level = 0;
  bool has_gap = false;

  uint8_t next_content_node(){
    return pgm_read_word(&content_pointer[content_cursor++]);
  }

  void revert_content_cursor(){
    content_cursor--;
  }

  void on_finished(){
    this->active = false;
    helper->on_player_finished();
  }

public:
  bool active = false;

  MusicVoicePlayer(MusicHelper* _helper, uint8_t _voice_index){
    this->helper = _helper;
    this->voice_index = _voice_index;
    for (size_t i = 0; i < MAX_REPEAT_LEVELS; i++)
      repeats[i] = new RepeatNotation();
  }

  void setup(const uint8_t* _content_pointer){
    this->content_pointer = _content_pointer;
    this->content_cursor = 0;
    this->remaining_timing_loops = 0;
    this->active = true;
  }

  void run_loop(){
    if(!active) return;
    if(remaining_timing_loops > 1){
      if(has_gap && remaining_timing_loops <= NOTE_GAP_LOOPS)
        buzzer.mute(voice_index);

      remaining_timing_loops--;
      return;
    }

    uint8_t node = next_content_node();
    
    if(helper->is_note_symbol(node)){
      uint8_t octave = next_content_node();
      if(!helper->is_valid_octave(octave))
        return revert_content_cursor();

      uint8_t timing = next_content_node();
      if(!helper->is_timing(timing))
        return revert_content_cursor();

      uint8_t timing_modifier = next_content_node();
      if(!helper->is_timing_modifier(timing_modifier)){
        revert_content_cursor();
        timing_modifier = 0;
      }

      has_gap = timing_modifier != TIE && timing_modifier != SLUR;
      remaining_timing_loops = helper->get_timing_loops(timing);

      double frequency_hz = helper->get_note_frequency(node, octave);

      buzzer.tone((uint16_t) frequency_hz, voice_index);
      return;
    }
    else if(helper->is_rest(node)){
      uint8_t timing = next_content_node();
      if(!helper->is_timing(timing))
        return revert_content_cursor();

      buzzer.mute(voice_index);
      remaining_timing_loops = helper->get_timing_loops(timing);
    }
    else if(node == MUSIC_BPM){
      helper->set_bpm(next_content_node());
      run_loop();
    }
    else if(node == MUSIC_BEATS){
      helper->set_beats(next_content_node());
      run_loop();
    }
    else if(node == REPEAT_START){
      repeats[repeat_level]->count = next_content_node();
      repeats[repeat_level]->start_position = content_cursor;
      repeat_level++;
      run_loop();
    }
    else if(node == REPEAT_END){
      if(repeat_level <= 0) return run_loop();
      repeats[repeat_level -1]->count--;
      if(repeats[repeat_level -1]->count > 0)
        content_cursor = repeats[repeat_level -1]->start_position;
      else
        repeat_level--;
      run_loop();
    }
    else if(node == MUSIC_END){
      return on_finished();
    }
    else {
      run_loop();
    }
  }
};

#endif
