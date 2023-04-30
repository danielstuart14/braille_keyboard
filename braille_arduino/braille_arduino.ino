#include <bluefruit.h>
#include "nrf.h"

#define SERIAL_ENABLED

#define POOLING_DELAY 10 // ms -> 1000/Hz
#define CONNECTION_DELAY 100 // ms -> 1000/Hz
#define BAT_DELAY 10000 // ms -> 1000/Hz
#define BUZZER_DELAY 200 // ms -> 1000/Hz

#define BAT_MIN 3.05 // minimum voltage

#define BTN_3 17 // P0_17
#define BTN_2 20 // P0_20
#define BTN_1 22 // P0_22
#define BTN_4 24 // P0_24
#define BTN_5 32 // P1_00
#define BTN_6 9 // P0_09
#define BTN_SPACE 10 // P0_10
#define BTN_BACK 13 // P0_13
#define BTN_ENTER 15 // P0_15

#define BUZZER_PIN 45 // P1_13

#define NOMOD 0
#define SHIFT KEYBOARD_MODIFIER_LEFTSHIFT
#define ALTGR KEYBOARD_MODIFIER_RIGHTALT

#define IS_PRESSED(key) (digitalRead(key) ? 0 : 1)

enum KeyState {
  DEFAULT = 0,
  UPPER = 1,
  HOLDUPPER = 2,
  NUMBER = 3,
};

struct KeyCode {
  uint8_t modifier;
  uint8_t key;
};

struct KeySequence {
  struct KeyCode first;
  struct KeyCode second;
  enum KeyState state;
};

BLEDis bledis;
BLEBas blebas;
BLEHidAdafruit blehid;

const uint8_t braille_btns[] = {BTN_6, BTN_3, BTN_5, BTN_2, BTN_4, BTN_1};

const struct KeySequence default_map[] = {
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, // 
  {{NOMOD,0x35},{NOMOD,0x00},DEFAULT}, // '
  {{NOMOD,0x37},{NOMOD,0x00},DEFAULT}, // .
  {{NOMOD,0x2d},{NOMOD,0x00},DEFAULT}, // -
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, // 
  {{SHIFT,0x21},{NOMOD,0x00},DEFAULT}, // $
  {{SHIFT,0x25},{NOMOD,0x00},DEFAULT}, // *
  {{ALTGR,0x08},{NOMOD,0x00},DEFAULT}, // °
  {{NOMOD,0x36},{NOMOD,0x00},DEFAULT}, // ,
  {{ALTGR,0x1a},{NOMOD,0x00},DEFAULT}, // ?
  {{NOMOD,0x38},{NOMOD,0x00},DEFAULT}, // ;
  {{SHIFT,0x35},{NOMOD,0x00},DEFAULT}, // "
  {{SHIFT,0x38},{NOMOD,0x00},DEFAULT}, // :
  {{ALTGR,0x14},{NOMOD,0x00},DEFAULT}, // /
  {{SHIFT,0x1e},{NOMOD,0x00},DEFAULT}, // !
  {{NOMOD,0x2e},{NOMOD,0x00},DEFAULT}, // =
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},UPPER}, //
  {{NOMOD,0x2f},{NOMOD,0x0c},DEFAULT}, // í
  {{NOMOD,0x2f},{NOMOD,0x12},DEFAULT}, // ó
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{SHIFT,0x64},{NOMOD,0x00},DEFAULT}, // |
  {{NOMOD,0x34},{NOMOD,0x04},DEFAULT}, // ã
  {{NOMOD,0x00},{NOMOD,0x00},NUMBER}, //
  {{NOMOD,0x0c},{NOMOD,0x00},DEFAULT}, // i
  {{NOMOD,0x34},{NOMOD,0x12},DEFAULT}, // õ
  {{NOMOD,0x16},{NOMOD,0x00},DEFAULT}, // s
  {{SHIFT,0x2f},{NOMOD,0x08},DEFAULT}, // è
  {{NOMOD,0x0d},{NOMOD,0x00},DEFAULT}, // j
  {{NOMOD,0x1a},{NOMOD,0x00},DEFAULT}, // w
  {{NOMOD,0x17},{NOMOD,0x00},DEFAULT}, // t
  {{NOMOD,0x2f},{NOMOD,0x18},DEFAULT}, // ú
  {{NOMOD,0x04},{NOMOD,0x00},DEFAULT}, // a
  {{SHIFT,0x34},{NOMOD,0x04},DEFAULT}, // â
  {{NOMOD,0x0e},{NOMOD,0x00},DEFAULT}, // k
  {{NOMOD,0x18},{NOMOD,0x00},DEFAULT}, // u
  {{NOMOD,0x08},{NOMOD,0x00},DEFAULT}, // e
  {{SHIFT,0x1f},{NOMOD,0x00},DEFAULT}, // @
  {{NOMOD,0x12},{NOMOD,0x00},DEFAULT}, // o
  {{NOMOD,0x1d},{NOMOD,0x00},DEFAULT}, // z
  {{NOMOD,0x05},{NOMOD,0x00},DEFAULT}, // b
  {{SHIFT,0x34},{NOMOD,0x08},DEFAULT}, // ê
  {{NOMOD,0x0f},{NOMOD,0x00},DEFAULT}, // l
  {{NOMOD,0x19},{NOMOD,0x00},DEFAULT}, // v
  {{NOMOD,0x0b},{NOMOD,0x00},DEFAULT}, // h
  {{SHIFT,0x23},{NOMOD,0x18},DEFAULT}, // ü
  {{NOMOD,0x15},{NOMOD,0x00},DEFAULT}, // r
  {{NOMOD,0x2f},{NOMOD,0x04},DEFAULT}, // á
  {{NOMOD,0x06},{NOMOD,0x00},DEFAULT}, // c
  {{SHIFT,0x2f},{NOMOD,0x0c},DEFAULT}, // ì
  {{NOMOD,0x10},{NOMOD,0x00},DEFAULT}, // m
  {{NOMOD,0x1b},{NOMOD,0x00},DEFAULT}, // x
  {{NOMOD,0x07},{NOMOD,0x00},DEFAULT}, // d
  {{SHIFT,0x34},{NOMOD,0x12},DEFAULT}, // ô
  {{NOMOD,0x11},{NOMOD,0x00},DEFAULT}, // n
  {{NOMOD,0x1c},{NOMOD,0x00},DEFAULT}, // y
  {{NOMOD,0x09},{NOMOD,0x00},DEFAULT}, // f
  {{SHIFT,0x2f},{NOMOD,0x04},DEFAULT}, // à
  {{NOMOD,0x13},{NOMOD,0x00},DEFAULT}, // p
  {{NOMOD,0x33},{NOMOD,0x00},DEFAULT}, // ç
  {{NOMOD,0x0a},{NOMOD,0x00},DEFAULT}, // g
  {{SHIFT,0x23},{NOMOD,0x0c},DEFAULT}, // ï
  {{NOMOD,0x14},{NOMOD,0x00},DEFAULT}, // q
  {{NOMOD,0x2f},{NOMOD,0x08},DEFAULT}, // é
};
const struct KeySequence upper_map[] = {
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},HOLDUPPER}, //
  {{NOMOD,0x2f},{SHIFT,0x0c},DEFAULT}, // Í
  {{NOMOD,0x2f},{SHIFT,0x12},DEFAULT}, // Ó
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x34},{SHIFT,0x04},DEFAULT}, // Ã
  {{NOMOD,0x00},{NOMOD,0x00},NUMBER}, //
  {{SHIFT,0x0c},{NOMOD,0x00},DEFAULT}, // I
  {{NOMOD,0x34},{SHIFT,0x12},DEFAULT}, // Õ
  {{SHIFT,0x16},{NOMOD,0x00},DEFAULT}, // S
  {{SHIFT,0x2f},{SHIFT,0x08},DEFAULT}, // È
  {{SHIFT,0x0d},{NOMOD,0x00},DEFAULT}, // J
  {{SHIFT,0x1a},{NOMOD,0x00},DEFAULT}, // W
  {{SHIFT,0x17},{NOMOD,0x00},DEFAULT}, // T
  {{NOMOD,0x2f},{SHIFT,0x18},DEFAULT}, // Ú
  {{SHIFT,0x04},{NOMOD,0x00},DEFAULT}, // A
  {{SHIFT,0x34},{SHIFT,0x04},DEFAULT}, // Â
  {{SHIFT,0x0e},{NOMOD,0x00},DEFAULT}, // K
  {{SHIFT,0x18},{NOMOD,0x00},DEFAULT}, // U
  {{SHIFT,0x08},{NOMOD,0x00},DEFAULT}, // E
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{SHIFT,0x12},{NOMOD,0x00},DEFAULT}, // O
  {{SHIFT,0x1d},{NOMOD,0x00},DEFAULT}, // Z
  {{SHIFT,0x05},{NOMOD,0x00},DEFAULT}, // B
  {{SHIFT,0x34},{SHIFT,0x08},DEFAULT}, // Ê
  {{SHIFT,0x0f},{NOMOD,0x00},DEFAULT}, // L
  {{SHIFT,0x19},{NOMOD,0x00},DEFAULT}, // V
  {{SHIFT,0x0b},{NOMOD,0x00},DEFAULT}, // H
  {{SHIFT,0x23},{SHIFT,0x18},DEFAULT}, // Ü
  {{SHIFT,0x15},{NOMOD,0x00},DEFAULT}, // R
  {{NOMOD,0x2f},{SHIFT,0x04},DEFAULT}, // Á
  {{SHIFT,0x06},{NOMOD,0x00},DEFAULT}, // C
  {{SHIFT,0x2f},{SHIFT,0x0c},DEFAULT}, // Ì
  {{SHIFT,0x10},{NOMOD,0x00},DEFAULT}, // M
  {{SHIFT,0x1b},{NOMOD,0x00},DEFAULT}, // X
  {{SHIFT,0x07},{NOMOD,0x00},DEFAULT}, // D
  {{SHIFT,0x34},{SHIFT,0x12},DEFAULT}, // Ô
  {{SHIFT,0x11},{NOMOD,0x00},DEFAULT}, // N
  {{SHIFT,0x1c},{NOMOD,0x00},DEFAULT}, // Y
  {{SHIFT,0x09},{NOMOD,0x00},DEFAULT}, // F
  {{SHIFT,0x2f},{SHIFT,0x04},DEFAULT}, // À
  {{SHIFT,0x13},{NOMOD,0x00},DEFAULT}, // P
  {{SHIFT,0x33},{NOMOD,0x00},DEFAULT}, // Ç
  {{SHIFT,0x0a},{NOMOD,0x00},DEFAULT}, // G
  {{SHIFT,0x23},{SHIFT,0x0c},DEFAULT}, // Ï
  {{SHIFT,0x14},{NOMOD,0x00},DEFAULT}, // Q
  {{NOMOD,0x2f},{SHIFT,0x08},DEFAULT}, // É
};
const struct KeySequence holdupper_map[] = {
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},UPPER}, //
  {{NOMOD,0x2f},{SHIFT,0x0c},HOLDUPPER}, // Í
  {{NOMOD,0x2f},{SHIFT,0x12},HOLDUPPER}, // Ó
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x34},{SHIFT,0x04},HOLDUPPER}, // Ã
  {{NOMOD,0x00},{NOMOD,0x00},NUMBER}, //
  {{SHIFT,0x0c},{NOMOD,0x00},HOLDUPPER}, // I
  {{NOMOD,0x34},{SHIFT,0x12},HOLDUPPER}, // Õ
  {{SHIFT,0x16},{NOMOD,0x00},HOLDUPPER}, // S
  {{SHIFT,0x2f},{SHIFT,0x08},HOLDUPPER}, // È
  {{SHIFT,0x0d},{NOMOD,0x00},HOLDUPPER}, // J
  {{SHIFT,0x1a},{NOMOD,0x00},HOLDUPPER}, // W
  {{SHIFT,0x17},{NOMOD,0x00},HOLDUPPER}, // T
  {{NOMOD,0x2f},{SHIFT,0x18},HOLDUPPER}, // Ú
  {{SHIFT,0x04},{NOMOD,0x00},HOLDUPPER}, // A
  {{SHIFT,0x34},{SHIFT,0x04},HOLDUPPER}, // Â
  {{SHIFT,0x0e},{NOMOD,0x00},HOLDUPPER}, // K
  {{SHIFT,0x18},{NOMOD,0x00},HOLDUPPER}, // U
  {{SHIFT,0x08},{NOMOD,0x00},HOLDUPPER}, // E
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{SHIFT,0x12},{NOMOD,0x00},HOLDUPPER}, // O
  {{SHIFT,0x1d},{NOMOD,0x00},HOLDUPPER}, // Z
  {{SHIFT,0x05},{NOMOD,0x00},HOLDUPPER}, // B
  {{SHIFT,0x34},{SHIFT,0x08},HOLDUPPER}, // Ê
  {{SHIFT,0x0f},{NOMOD,0x00},HOLDUPPER}, // L
  {{SHIFT,0x19},{NOMOD,0x00},HOLDUPPER}, // V
  {{SHIFT,0x0b},{NOMOD,0x00},HOLDUPPER}, // H
  {{SHIFT,0x23},{SHIFT,0x18},HOLDUPPER}, // Ü
  {{SHIFT,0x15},{NOMOD,0x00},HOLDUPPER}, // R
  {{NOMOD,0x2f},{SHIFT,0x04},HOLDUPPER}, // Á
  {{SHIFT,0x06},{NOMOD,0x00},HOLDUPPER}, // C
  {{SHIFT,0x2f},{SHIFT,0x0c},HOLDUPPER}, // Ì
  {{SHIFT,0x10},{NOMOD,0x00},HOLDUPPER}, // M
  {{SHIFT,0x1b},{NOMOD,0x00},HOLDUPPER}, // X
  {{SHIFT,0x07},{NOMOD,0x00},HOLDUPPER}, // D
  {{SHIFT,0x34},{SHIFT,0x12},HOLDUPPER}, // Ô
  {{SHIFT,0x11},{NOMOD,0x00},HOLDUPPER}, // N
  {{SHIFT,0x1c},{NOMOD,0x00},HOLDUPPER}, // Y
  {{SHIFT,0x09},{NOMOD,0x00},HOLDUPPER}, // F
  {{SHIFT,0x2f},{SHIFT,0x04},HOLDUPPER}, // À
  {{SHIFT,0x13},{NOMOD,0x00},HOLDUPPER}, // P
  {{SHIFT,0x33},{NOMOD,0x00},HOLDUPPER}, // Ç
  {{SHIFT,0x0a},{NOMOD,0x00},HOLDUPPER}, // G
  {{SHIFT,0x23},{SHIFT,0x0c},HOLDUPPER}, // Ï
  {{SHIFT,0x14},{NOMOD,0x00},HOLDUPPER}, // Q
  {{NOMOD,0x2f},{SHIFT,0x08},HOLDUPPER}, // É
};
const struct KeySequence number_map[] = {
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x37},{NOMOD,0x00},NUMBER}, // .
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x36},{NOMOD,0x00},NUMBER}, // ,
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x1b},{NOMOD,0x00},DEFAULT}, // x
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{SHIFT,0x2e},{NOMOD,0x00},DEFAULT}, // +
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x26},{NOMOD,0x00},NUMBER}, // 9
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x27},{NOMOD,0x00},NUMBER}, // 0
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x1e},{NOMOD,0x00},NUMBER}, // 1
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{SHIFT,0x20},{NOMOD,0x00},NUMBER}, // #
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x22},{NOMOD,0x00},NUMBER}, // 5
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x1f},{NOMOD,0x00},NUMBER}, // 2
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x25},{NOMOD,0x00},NUMBER}, // 8
  {{NOMOD,0x35},{NOMOD,0x00},NUMBER}, // '
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x20},{NOMOD,0x00},NUMBER}, // 3
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x21},{NOMOD,0x00},NUMBER}, // 4
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x23},{NOMOD,0x00},NUMBER}, // 6
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x24},{NOMOD,0x00},NUMBER}, // 7
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
  {{NOMOD,0x00},{NOMOD,0x00},DEFAULT}, //
};

const struct KeySequence *states[] = {
  default_map,
  upper_map,
  holdupper_map,
  number_map,
};

uint8_t get_braille_btns() {
  uint8_t value = 0;
  for (uint8_t i = 0; i < sizeof(braille_btns); i++) {
    value |= IS_PRESSED(braille_btns[i]) << i;
  }

  return value;
}

bool send_key(const struct KeySequence *keys) {
  uint8_t keycodes[6] = {}; 

  if (keys->first.key != 0) {
    keycodes[0] = keys->first.key;
    blehid.keyboardReport(keys->first.modifier, keycodes);

    if (keys->second.key != 0) {
      keycodes[0] = keys->second.key;
      blehid.keyboardReport(keys->second.modifier, keycodes);
    }
    return true;
  }

  return false;
}

static enum KeyState cur_state = DEFAULT;

void convert_symbol(const uint8_t symbol) {
  const struct KeySequence *keys = states[cur_state];
  cur_state = keys[symbol].state;
  if (!send_key(&keys[symbol])) {
    keys = states[cur_state];
    send_key(&keys[symbol]);
  }
}

bool process_braille_btns(bool reset) {
  static uint8_t cur_symbol = 0;

  if (reset) {
    cur_symbol = 0;
    return false;
  }

  uint8_t cur_buttons = get_braille_btns();
  if (cur_buttons > 0) {
    cur_symbol |= cur_buttons;
  } else {    
    if (cur_symbol > 0) {
#ifdef SERIAL_ENABLED
      Serial.print("Braille pressed: ");
      Serial.println(cur_symbol, HEX);
#endif

      convert_symbol(cur_symbol);
      cur_symbol = 0;
    } else {
      return false;
    }
  }

  return true;  
}

bool btn_pressed_once(bool *prev_state, uint8_t button) {
  bool btn_state = IS_PRESSED(button);
  if (btn_state != *prev_state) {
    *prev_state = btn_state;
    if (btn_state) {
      return true;
    }
  }
  
  return false;
}

bool process_action_btns(bool reset) {
  static bool space_pressed = false;
  static bool enter_pressed = false;
  static bool back_pressed = false;

  if (reset) {
    space_pressed = false;
    enter_pressed = false;
    back_pressed = false;
    return false;
  }

  uint8_t keycodes[6] = {};

  if (btn_pressed_once(&space_pressed, BTN_SPACE)) {
#ifdef SERIAL_ENABLED
    Serial.println("Space pressed");
#endif

    cur_state = DEFAULT;
    keycodes[0] = HID_KEY_SPACE;

  } else if (btn_pressed_once(&enter_pressed, BTN_ENTER)) {
#ifdef SERIAL_ENABLED
    Serial.println("Enter pressed");
#endif
    cur_state = DEFAULT;
    keycodes[0] = HID_KEY_ENTER;

  } else if (btn_pressed_once(&back_pressed, BTN_BACK)) {
#ifdef SERIAL_ENABLED
    Serial.println("Backspace pressed");
#endif
    keycodes[0] = HID_KEY_BACKSPACE;

  } else {
    
    return false;
  }
  
  blehid.keyboardReport(0, keycodes);

  return true;
}

static uint32_t analogReadVDDH()
{
  volatile int16_t value = 0;
  
  const uint32_t saadcReference = SAADC_CH_CONFIG_REFSEL_Internal;
  const uint32_t saadcGain      = SAADC_CH_CONFIG_GAIN_Gain1_2;
  const uint32_t saadcSampleTime = SAADC_CH_CONFIG_TACQ_10us;
  const uint32_t saadcResolution = SAADC_RESOLUTION_VAL_10bit;
  const uint32_t psel = SAADC_CH_PSELP_PSELP_VDDHDIV5;
  const bool saadcBurst = SAADC_CH_CONFIG_BURST_Disabled;

  NRF_SAADC->RESOLUTION = saadcResolution;

  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Enabled << SAADC_ENABLE_ENABLE_Pos);
  for (int i = 0; i < 8; i++) {
    NRF_SAADC->CH[i].PSELN = SAADC_CH_PSELP_PSELP_NC;
    NRF_SAADC->CH[i].PSELP = SAADC_CH_PSELP_PSELP_NC;
  }
  NRF_SAADC->CH[0].CONFIG = ((SAADC_CH_CONFIG_RESP_Bypass     << SAADC_CH_CONFIG_RESP_Pos)   & SAADC_CH_CONFIG_RESP_Msk)
                            | ((SAADC_CH_CONFIG_RESP_Bypass   << SAADC_CH_CONFIG_RESN_Pos)   & SAADC_CH_CONFIG_RESN_Msk)
                            | ((saadcGain                     << SAADC_CH_CONFIG_GAIN_Pos)   & SAADC_CH_CONFIG_GAIN_Msk)
                            | ((saadcReference                << SAADC_CH_CONFIG_REFSEL_Pos) & SAADC_CH_CONFIG_REFSEL_Msk)
                            | ((saadcSampleTime               << SAADC_CH_CONFIG_TACQ_Pos)   & SAADC_CH_CONFIG_TACQ_Msk)
                            | ((SAADC_CH_CONFIG_MODE_SE       << SAADC_CH_CONFIG_MODE_Pos)   & SAADC_CH_CONFIG_MODE_Msk)
                            | ((saadcBurst                    << SAADC_CH_CONFIG_BURST_Pos)   & SAADC_CH_CONFIG_BURST_Msk);
  NRF_SAADC->CH[0].PSELN = psel;
  NRF_SAADC->CH[0].PSELP = psel;


  NRF_SAADC->RESULT.PTR = (uint32_t)&value;
  NRF_SAADC->RESULT.MAXCNT = 1; // One sample

  NRF_SAADC->TASKS_START = 0x01UL;

  while (!NRF_SAADC->EVENTS_STARTED);
  NRF_SAADC->EVENTS_STARTED = 0x00UL;

  NRF_SAADC->TASKS_SAMPLE = 0x01UL;

  while (!NRF_SAADC->EVENTS_END);
  NRF_SAADC->EVENTS_END = 0x00UL;

  NRF_SAADC->TASKS_STOP = 0x01UL;

  while (!NRF_SAADC->EVENTS_STOPPED);
  NRF_SAADC->EVENTS_STOPPED = 0x00UL;

  if (value < 0) {
    value = 0;
  }

  NRF_SAADC->ENABLE = (SAADC_ENABLE_ENABLE_Disabled << SAADC_ENABLE_ENABLE_Pos);

  return value;
}

void play_buzzer(uint8_t beeps) {
  for (;beeps > 1; beeps--) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZER_DELAY);
    digitalWrite(BUZZER_PIN, LOW);
    delay(BUZZER_DELAY);
  }
  
  if (beeps == 1) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZER_DELAY);
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void process_battery(bool reset) {
  static uint32_t prev_millis = (uint32_t) (-2*BAT_DELAY);
  static uint8_t prev_bat = 255;

  if (reset) {
    prev_bat = 255;
  }

  uint32_t cur_millis = millis();
  if (cur_millis - prev_millis < BAT_DELAY) {
    return;
  }
  prev_millis = cur_millis;
  
  float bat_volt = (5.0 * map(analogReadVDDH(), 0, 1023, 0, 1200)) / 1000.0;

#ifdef SERIAL_ENABLED
  Serial.print("Battery voltage: ");
  Serial.println(bat_volt);
#endif

  //TODO: Go to sleep if battery is low

  if (bat_volt <= BAT_MIN) {
#ifdef SERIAL_ENABLED
    Serial.println("Battery low! Powering off...");
#endif

    play_buzzer(1);
    sd_power_system_off();
  }

  float bat_percent = 50.0 + 50.0 * ((bat_volt - 3.65) / 0.5);

  int16_t bat_int;
  bat_int = (uint8_t) bat_percent + 1;
  if (bat_int > 100) {
    bat_int = 100;
  } else if (bat_int < 0) {
    bat_int = 0;
  }

  if (bat_int < prev_bat) {  
    blebas.write((uint8_t) bat_int);
    blebas.notify((uint8_t) bat_int);
    prev_bat = (uint8_t) bat_int;
  }

#ifdef SERIAL_ENABLED
  Serial.print("Battery percentage: ");
  Serial.println(bat_int);
#endif
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addService(blebas);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
  
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 338);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void check_dfu() {
  if (IS_PRESSED(BTN_BACK) && IS_PRESSED(BTN_ENTER)) {
#ifdef SERIAL_ENABLED
    Serial.println("Going to DFU mode...");
#endif

    play_buzzer(3);
    enterOTADfu();
  }
}

void setup() 
{
  sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);

#ifdef SERIAL_ENABLED
  Serial.begin(115200);
#endif

  for (uint8_t i = 0; i < sizeof(braille_btns); i++) {
    pinMode(braille_btns[i], INPUT);
  }
  pinMode(BTN_SPACE, INPUT);
  pinMode(BTN_BACK, INPUT);
  pinMode(BTN_ENTER, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Bluefruit.autoConnLed(false); // Disable LED
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX); 
  Bluefruit.begin();
  Bluefruit.setTxPower(0);    // Check bluefruit.h for supported values
  Bluefruit.setName("Braille");

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Braille Keyboard");
  bledis.begin();

  blebas.begin();  
  blehid.begin();

  // Wait for battery voltage to stabilize
  delay(100);
  process_battery(false);

  check_dfu();

  startAdv();

#ifdef SERIAL_ENABLED
  Serial.println("Braille Keyboard started!");
#endif

  play_buzzer(2);
}

void loop()
{
  delay(CONNECTION_DELAY);
  
  if (Bluefruit.connected() == 0) { // If no one is connected
    process_battery(true); // Reset states
    return;
  }

  process_braille_btns(true); // Reset states
  process_action_btns(true); // Reset states
  while (Bluefruit.connected() > 0) { // While someone is connected
    if (!process_braille_btns(false)) { // Process braille buttons and check if they're release
      process_action_btns(false); // Process actions (space, backspace, enter) if braille buttons are released
    }
    blehid.keyRelease();

    process_battery(false);
    delay(POOLING_DELAY);
  }
}