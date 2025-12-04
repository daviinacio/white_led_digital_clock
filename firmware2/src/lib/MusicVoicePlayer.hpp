#include <Thread.h>
#include <ThreadController.h>
#include "MusicHelper.hpp"
#include "assets/music/base.h"
#include "drivers/Buzzer.hpp"

#define NOTE_GAP_LOOPS 2
#define MAX_REPEAT_LEVELS 5
#define REST_FREQUENCY 0.1

#ifndef WLDC_MUSIC_VOICE_PLAYER_H
#define WLDC_MUSIC_VOICE_PLAYER_H

class RepeatNotation {
public:
  uint16_t start_position = 0;
  uint8_t count = 0;
  uint8_t cursor = 0;

  void clear(){
    start_position = 0;
    count = 0;
    cursor = 0;
  }
};

class MusicVoicePlayer {
protected:
  MusicHelper* helper;
  // SlurHelper slur_helper;

  uint8_t voice_index = 0;

  uint16_t content_cursor = 0;
  const uint8_t* content_pointer = nullptr;

  uint8_t timing_ticks_count = 0;
  uint8_t timing_ticks_cursor = 0;

  RepeatNotation repeats[MAX_REPEAT_LEVELS];
  uint8_t repeat_level = 0;

  bool has_gap = false;

  double start_frequency = 0;
  double end_frequency = 0;

  double calc_current_frequency(){
    if(start_frequency == 0 || end_frequency == 0) return 0;
    if (timing_ticks_count < 2) return end_frequency;
    double delta = (end_frequency - start_frequency) / (timing_ticks_count - 1);
    return start_frequency + timing_ticks_cursor * delta;
  }

  uint8_t next_content_node(){
    return pgm_read_word(&content_pointer[content_cursor++]);
  }

  void revert_content_cursor(){
    content_cursor--;
  }

  uint8_t next_note_symbol(){
    for(uint16_t i = 0; true; i++){
      uint8_t node = pgm_read_word(&content_pointer[content_cursor + i]);

      if(helper->is_note_symbol(node))
        return node;
      else if(node == MUSIC_END)
        return 0;
    }
  }

  uint8_t next_note_octave(){
    for(uint16_t i = 0; true; i++){
      uint8_t node = pgm_read_word(&content_pointer[content_cursor + i]);

      if(helper->is_note_symbol(node))
        return pgm_read_word(&content_pointer[content_cursor + i + 1]);
      else if(node == MUSIC_END)
        return 0;
    }
  }

  uint8_t next_rest(){
    while(true){
      uint8_t node = next_content_node();
      if(helper->is_rest(node))
        return node;
    }
  }

  uint8_t next_timing(){
    while(true){
      uint8_t node = next_content_node();
      if(helper->is_timing(node))
        return node;
    }
  }

  uint8_t next_value(){
    while(true){
      uint8_t node = next_content_node();
      if(helper->is_valid_value(node))
        return node;
    }
  }

  void on_finished(){
    this->active = false;
    helper->on_player_finished();
  }

public:
  bool active = false;

  MusicVoicePlayer(MusicHelper* _helper, uint8_t _voice_index): repeats {} {
    this->helper = _helper;
    this->voice_index = _voice_index;
  }

  void setup(const uint8_t* _content_pointer){
    this->content_pointer = _content_pointer;
    this->content_cursor = 0;
    this->timing_ticks_count = 0;
    this->timing_ticks_cursor = 0;
    this->active = true;
    this->repeat_level = 0;

    for (size_t i = 0; i < MAX_REPEAT_LEVELS; i++)
      repeats[i].clear();
  }

  void catch_unexpected_music_end(){
    on_finished();
  }

  void run_tick(){
    if(!active) return;

    // Playing
    if(start_frequency != 0 && end_frequency != 0){
      if(timing_ticks_cursor < timing_ticks_count){
        if(
          start_frequency == REST_FREQUENCY ||
          end_frequency == REST_FREQUENCY ||
          (has_gap && (timing_ticks_count - timing_ticks_cursor) < NOTE_GAP_LOOPS)
        )
          buzzer.mute(voice_index);
        else
          buzzer.tone((uint16_t) calc_current_frequency(), voice_index);
        timing_ticks_cursor++;
        return;
      }
      else {
        start_frequency = 0;
        end_frequency = 0;
      }
    }

    uint8_t node = next_content_node();
    RepeatNotation* current_repeat_block = repeat_level <= 0 ? nullptr : &repeats[repeat_level -1];
    
    if(helper->is_note_symbol(node)){
      uint8_t octave = next_content_node();
      if(!helper->is_valid_value(octave))
        return revert_content_cursor();

      double frequency_hz = helper->get_note_frequency(node, octave);

      if(start_frequency != 0 && end_frequency == 0){
        end_frequency = frequency_hz;
        revert_content_cursor();
        revert_content_cursor();
        return run_tick();
      }

      uint8_t timing = next_content_node();
      if(!helper->is_timing(timing))
        return revert_content_cursor();

      uint8_t timing_modifier = next_content_node();
      if(!helper->is_timing_modifier(timing_modifier)){
        revert_content_cursor();
        timing_modifier = 0;
      }

      has_gap = timing_modifier != TIE && timing_modifier != SLUR;
      timing_ticks_cursor = 0;
      timing_ticks_count = helper->get_timing_ticks(timing);

      start_frequency = frequency_hz;
      // Wait for the next note
      end_frequency = timing_modifier == SLUR ? 0 : frequency_hz;

      return run_tick();
    }
    else if(helper->is_rest(node)){
      uint8_t timing = next_content_node();
      if(!helper->is_timing(timing))
        return revert_content_cursor();

      timing_ticks_cursor = 0;
      timing_ticks_count = helper->get_timing_ticks(timing);
      start_frequency = REST_FREQUENCY;
      end_frequency = REST_FREQUENCY;
      return run_tick();
    }
    else if(node == MUSIC_BPM){
      helper->set_bpm(next_content_node());
      run_tick();
    }
    else if(node == MUSIC_BEATS){
      helper->set_beats(next_content_node());
      run_tick();
    }
    else if(node == REPEAT_ENDING){
      // Is not inside a repeat block
      if(current_repeat_block == nullptr) return run_tick();
      uint8_t nested_repeat_level_count;

      // Searching ending flags
      while(true){
        while(true){
          uint8_t node = next_content_node();

          if(node == MUSIC_END)
            return catch_unexpected_music_end();
          else if(helper->is_valid_value(node)){
            // Play ending
            if(node == current_repeat_block->cursor)
              return run_tick();
          }
          else {
            // No flags match
            revert_content_cursor();
            break;
          }
        }

        // Skip to the next ending or current block end
        while(true){
          uint8_t node = next_content_node();

          if(node == MUSIC_END)
            return catch_unexpected_music_end();
          else if(node == REPEAT_START)
            nested_repeat_level_count++;
          else if(node == REPEAT_END){
            if(nested_repeat_level_count > 0)
              nested_repeat_level_count--;
            else {
              // Reached the end of the current block
              revert_content_cursor();
              return run_tick();
            }
          }

          // Found another ending, try check the flags again
          else if(node == REPEAT_ENDING && nested_repeat_level_count == 0){
            break;
          }
        }
      }
    }
    else if(node == REPEAT_START){
      repeats[repeat_level].count = next_content_node();
      repeats[repeat_level].start_position = content_cursor;
      repeats[repeat_level].cursor = 1;
      repeat_level++;
      return run_tick();
    }
    else if(node == REPEAT_END){
      if(current_repeat_block == nullptr) return run_tick();
      
      if(current_repeat_block->cursor < current_repeat_block->count){
        current_repeat_block->cursor++;
        content_cursor = current_repeat_block->start_position;
      }
      else {
        current_repeat_block->clear();
        repeat_level--;
      }
      return run_tick();
    }
    else if(node == MUSIC_END){
      return on_finished();
    }
    else {
      return run_tick();
    }
  }
};

#endif
