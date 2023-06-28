#include QMK_KEYBOARD_H
#include <stdio.h>
#ifdef CONSOLE_ENABLE
  #include <print.h>
#endif
#include <string.h>
#include "keymap_jp.h" // qmk_firmware_bmp/quantum/keymap_extras/keymap_jp.h
#include "bmp.h"
#include "bmp_custom_keycode.h"
#include "keycode_str_converter.h"
#include "process_combo.h"

extern uint8_t is_master;

#define TAP_WITHOUT_SHIFT(CODE) (shift_pressed ? SEND_STRING(SS_UP(X_LSHIFT) SS_TAP(X_##CODE) SS_DOWN(X_LSHIFT)) : SEND_STRING(SS_TAP(X_##CODE)))
#define CASE_AUTO_HANKAKU(CODE) \
case CODE:                                                    \
  if (record->event.pressed && is_kana_internal == true) {    \
    TAP_WITHOUT_SHIFT(LANG2);                         \
    is_kana_internal = false;                                 \
  }                                                           \
  return true;

// #define OLED_TIMEOUT 0
#define WINDOWS_MODE true
#define MAC_MODE false

void show_info_oled(void);

enum layer_number {
  _QWERTY_WIN = 0,
  _QWERTY,
  _LOWER,
  _RAISE,
  _ADJUST,
};

enum custom_keycodes {
  QWERTY = BMP_SAFE_RANGE,
  QWERTY_WIN,
  UBUNTU,
  MAC,
  SHIFT,
  LOWER,
  RAISE,
  ADJUST,
  BT_ID0,
  BT_ID1,
  BT_ID2,
  BT_ID3,
  BT_ID4,
  BT_ID5,
  BT_ID6,
  BT_ID7,
  INFO,
  MY_EISU,
  MY_KANA,
};

const key_string_map_t custom_keys_user = {
  .start_kc = QWERTY,
  .end_kc = MY_KANA,
  .key_strings =
    "QWERTY\0"
    "QWERTY_WIN\0"
    "UBUNTU\0"
    "MAC\0"
    "SHIFT\0"
    "LOWER\0"
    "RAISE\0"
    "ADJUST\0"
    "BT_ID0\0"
    "BT_ID1\0"
    "BT_ID2\0"
    "BT_ID3\0"
    "BT_ID4\0"
    "BT_ID5\0"
    "BT_ID6\0"
    "BT_ID7\0"
    "INFO\0"
    "MY_EISU\0"
    "MY_KANA\0"
};

const key_override_t rprn_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_0, JP_RPRN);
const key_override_t at_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_2, JP_AT);
const key_override_t circ_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_6, JP_CIRC);
const key_override_t ampr_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_7, JP_AMPR);
const key_override_t astr_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_8, JP_ASTR);
const key_override_t lprn_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_9, JP_LPRN);
const key_override_t unds_key_override = ko_make_basic(MOD_MASK_SHIFT, KC_MINS, JP_UNDS);
const key_override_t plus_key_override = ko_make_basic(MOD_MASK_SHIFT, JP_EQL, JP_PLUS);
const key_override_t tild_key_override = ko_make_basic(MOD_MASK_SHIFT, JP_GRV, JP_TILD);
const key_override_t pipe_key_override = ko_make_basic(MOD_MASK_SHIFT, JP_YEN, JP_PIPE);
const key_override_t coln_key_override = ko_make_basic(MOD_MASK_SHIFT, JP_SCLN, JP_COLN);
const key_override_t dquo_key_override = ko_make_basic(MOD_MASK_SHIFT, JP_QUOT, JP_DQUO);

const key_override_t **key_overrides = (const key_override_t *[]){
  &rprn_key_override,
  &at_key_override,
  &circ_key_override,
  &ampr_key_override,
  &astr_key_override,
  &lprn_key_override,
  &unds_key_override,
  &plus_key_override,
  &tild_key_override,
  &pipe_key_override,
  &coln_key_override,
  &dquo_key_override,
  NULL
};

bool shift_pressed = false;
bool os_mode = false;
// It means which key user pushed, xKANA or xEISU. User assumes IME is in the mode he chose.
bool is_kana_user = false;
// It indicates the actual mode, whether the PC is in EISU mode or KANA mode.
bool is_kana_internal = false;

// keymaps[] has no meaning but is necessary to build correctly.
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {};

// Defining this array is required to avoid build error due to commit: 55456cd
combo_t key_combos[] = {};

int layer_state_to_num(layer_state_t state) {
  int n = 0;
  for (; n < 16; ++n) {
    if (((state>>n) & 0x1) == 1) break;
  }
  return n;
}

// Called when a layer changed.
layer_state_t layer_state_set_user(layer_state_t state) {
  // Call _ADJUST
  state = update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);

#ifdef CONSOLE_ENABLE
  uprintf("%d layer_state_set_user\n", state);
  uprintf("%d eeconfig_read_default_layer\n", eeconfig_read_default_layer());
  uprintf("%d layer_state_to_num\n", layer_state_to_num(eeconfig_read_default_layer()));
#endif
  return state;
}

/*
 * Called after powering on.
 * I think this is like init() in Arduino.
 */
void keyboard_post_init_user(void) {
  show_info_oled();
}

// Display information on OLEDs.
void show_info_oled() {
#ifdef OLED_DRIVER_ENABLE
#ifdef CONSOLE_ENABLE
  uprintf("show_info_oled\n");
#endif // CONSOLE_ENABLE
  if (is_keyboard_master()) {
    oled_on();
    char os[24] = "OS: ";
    strcat(os, os_mode ? "WINDOWS" : "MAC");
    char batt[24];
    // The code is based on tmk_core/protocol/nrf/bmp.c
    sprintf(batt, "BATTERY: %4dmV", BMPAPI->app.get_vcc_mv());
    oled_clear();
    oled_write_ln(os, false);
    oled_write_ln(batt, false);
  }
#endif // OLED_DRIVER_ENABLE
}

// Called when a key pressed/released.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool continue_process = process_record_user_bmp(keycode, record);
  if (continue_process == false) {
      return false;
  }

  switch (keycode) {
    case QWERTY:
    case QWERTY_WIN:
      ;
      break;
    default:
      ;
#ifdef OLED_DRIVER_ENABLE
      oled_off();
#endif // OLED_DRIVER_ENABLE
  }
  switch (keycode) {
    CASE_AUTO_HANKAKU(KC_0);
    CASE_AUTO_HANKAKU(KC_1);
    CASE_AUTO_HANKAKU(KC_2);
    CASE_AUTO_HANKAKU(KC_3);
    CASE_AUTO_HANKAKU(KC_4);
    CASE_AUTO_HANKAKU(KC_5);
    CASE_AUTO_HANKAKU(KC_6);
    CASE_AUTO_HANKAKU(KC_7);
    CASE_AUTO_HANKAKU(KC_8);
    CASE_AUTO_HANKAKU(KC_9);
    CASE_AUTO_HANKAKU(JP_EXLM);
    CASE_AUTO_HANKAKU(JP_AT);
    CASE_AUTO_HANKAKU(JP_HASH);
    CASE_AUTO_HANKAKU(JP_DLR);
    CASE_AUTO_HANKAKU(JP_PERC);
    CASE_AUTO_HANKAKU(JP_CIRC);
    CASE_AUTO_HANKAKU(JP_AMPR);
    CASE_AUTO_HANKAKU(JP_ASTR);
    CASE_AUTO_HANKAKU(JP_LPRN);
    CASE_AUTO_HANKAKU(JP_RPRN);
    CASE_AUTO_HANKAKU(JP_LCBR);
    CASE_AUTO_HANKAKU(JP_RCBR);
    CASE_AUTO_HANKAKU(JP_GRV);
    CASE_AUTO_HANKAKU(JP_YEN);
    CASE_AUTO_HANKAKU(JP_PIPE);
    CASE_AUTO_HANKAKU(JP_UNDS);
    CASE_AUTO_HANKAKU(JP_EQL);
    CASE_AUTO_HANKAKU(JP_PLUS);
    CASE_AUTO_HANKAKU(JP_SCLN);
    CASE_AUTO_HANKAKU(JP_COLN);
    CASE_AUTO_HANKAKU(JP_QUOT);
    case BT_ID0 ... BT_ID7:
      // The code is based on tmk_core/protocol/nrf/bmp.c
      if (record->event.pressed) {
        if (shift_pressed) {
#ifdef CONSOLE_ENABLE
          uprintf("[lilyble] Delete BT_ID:%d\n", keycode - BT_ID0);
#endif // CONSOLE_ENABLE
          BMPAPI->ble.delete_bond(keycode - BT_ID0);
        } else {
#ifdef CONSOLE_ENABLE
          uprintf("[lilyble] Advertise BT_ID:%d\n", keycode - BT_ID0);
#endif // CONSOLE_ENABLE
          BMPAPI->ble.advertise(keycode - BT_ID0);
        }
      }
      return false;
    case INFO:
      if (record->event.pressed) {
        show_info_oled();
      }
      return false;
    case QWERTY:
      if (record->event.pressed) {
        os_mode = MAC_MODE;
        set_single_persistent_default_layer(_QWERTY);
        show_info_oled();
      }
      return false;
    case QWERTY_WIN:
      if (record->event.pressed) {
        os_mode = WINDOWS_MODE;
        set_single_persistent_default_layer(_QWERTY_WIN);
        show_info_oled();
      }
      return false;
    case SHIFT:
      if (record->event.pressed) {
        SEND_STRING(SS_DOWN(X_LSHIFT));
        shift_pressed = true;
      } else {
        SEND_STRING(SS_UP(X_LSHIFT));
        shift_pressed = false;
      }
      return false;
    case MY_EISU:
      if (record->event.pressed) {
        tap_code(KC_LANG2);
        is_kana_user = false;
        is_kana_internal = false;
      }
      return false;
    case MY_KANA:
      if (record->event.pressed) {
        TAP_WITHOUT_SHIFT(LANG1);
        is_kana_user = true;
        is_kana_internal = true;
      }
      return false;
    case KC_A ... KC_Z:
      if (record->event.pressed) {
        if (is_kana_user != is_kana_internal) {
          if (is_kana_user == true) {
            tap_code(KC_LANG1);
            is_kana_internal = true;
          } else {
            tap_code(KC_LANG2);
            is_kana_internal = false;
          }
        }
      }
      return true;
  }
  return true;
}