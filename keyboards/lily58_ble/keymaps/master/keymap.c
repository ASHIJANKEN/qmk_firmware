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

extern uint8_t is_master;

// https://qiita.com/m47ch4n/items/1a546f7b1295e0e24a58
#define SHIFT_RESTORE(STR) (shift_pressed ? SEND_STRING(STR SS_DOWN(X_LSHIFT)) : SEND_STRING(STR SS_UP(X_LSHIFT)))
#define KEY(CODE) (record->event.pressed ? SEND_STRING(SS_DOWN(X_##CODE)) : SHIFT_RESTORE(SS_UP(X_##CODE)))
#define KEY_SHIFT(CODE) (record->event.pressed ? SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_##CODE)) : SHIFT_RESTORE(SS_UP(X_##CODE)))
#define KEY_UPSHIFT(CODE) (record->event.pressed ? SEND_STRING(SS_UP(X_LSHIFT) SS_DOWN(X_##CODE)) : SHIFT_RESTORE(SS_UP(X_##CODE)))
#define SHIFT_DU(CODE_DOWN, CODE_UP) (shift_pressed ? CODE_DOWN : CODE_UP)
#define CASE_US(CODE, US, JIS)                   \
    case CODE:                                  \
        uprintf("CASE_US\n"); \
        (kb_layout == JIS_LAYOUT ? JIS : US); \
        return false;

// #define OLED_TIMEOUT 0
#define JIS_LAYOUT true
#define US_LAYOUT false
#define WINDOWS_MODE true
#define MAC_MODE false

void show_info_oled(void);

enum layer_number {
  _QWERTY = 0,
  _LOWER,
  _RAISE,
  _QWERTY_WIN,
  _LOWER_WIN,
  _RAISE_WIN,
  _ADJUST,
};

enum custom_keycodes {
  QWERTY = BMP_SAFE_RANGE,
  QWERTY_WIN,
  JIS,
  US,
  UBUNTU,
  MAC,
  SHIFT,
  LOWER,
  RAISE,
  ADJUST,
  CS_0,
  CS_1,
  CS_2,
  CS_3,
  CS_4,
  CS_5,
  CS_6,
  CS_7,
  CS_8,
  CS_9,
  BT_ID0,
  BT_ID1,
  BT_ID2,
  BT_ID3,
  BT_ID4,
  BT_ID5,
  BT_ID6,
  BT_ID7,
  INFO,
  MY_CIRC,
  MY_AT,
  MY_LBRC,
  MY_RBRC,
  MY_BSLS,
  MY_AMPR,
  MY_QUOT,
  MY_LPRN,
  MY_RPRN,
  MY_EQL,
  MY_TILD,
  MY_PIPE,
  MY_GRV,
  MY_LCBR,
  MY_PLUS,
  MY_ASTR,
  MY_RCBR,
  MY_UNDS,
  MY_MINS,
  MY_SCLN,
  MY_BKKR, // Name of "EXLM" causes a bug of conflicting to EX().
  MY_HASH,
  MY_DLR,
  MY_PERC,
  MY_DEL,
};

const key_string_map_t custom_keys_user = {
  .start_kc = QWERTY,
  .end_kc = MY_DEL,
  .key_strings =
    "QWERTY\0"
    "QWERTY_WIN\0"
    "JIS\0"
    "US\0"
    "UBUNTU\0"
    "MAC\0"
    "SHIFT\0"
    "LOWER\0"
    "RAISE\0"
    "ADJUST\0"
    "CS_0\0"
    "CS_1\0"
    "CS_2\0"
    "CS_3\0"
    "CS_4\0"
    "CS_5\0"
    "CS_6\0"
    "CS_7\0"
    "CS_8\0"
    "CS_9\0"
    "BT_ID0\0"
    "BT_ID1\0"
    "BT_ID2\0"
    "BT_ID3\0"
    "BT_ID4\0"
    "BT_ID5\0"
    "BT_ID6\0"
    "BT_ID7\0"
    "INFO\0"
    "MY_CIRC\0"
    "MY_AT\0"
    "MY_LBRC\0"
    "MY_RBRC\0"
    "MY_BSLS\0"
    "MY_AMPR\0"
    "MY_QUOT\0"
    "MY_LPRN\0"
    "MY_RPRN\0"
    "MY_EQL\0"
    "MY_TILD\0"
    "MY_PIPE\0"
    "MY_GRV\0"
    "MY_LCBR\0"
    "MY_PLUS\0"
    "MY_ASTR\0"
    "MY_RCBR\0"
    "MY_UNDS\0"
    "MY_MINS\0"
    "MY_SCLN\0"
    "MY_BKKR\0"
    "MY_HASH\0"
    "MY_DLR\0"
    "MY_PERC\0"
    "MY_DEL\0"
};

typedef union {
  uint8_t raw;
  struct {
    bool layout:1;
  };
} kb_config_t;

kb_config_t kb_config;

bool kb_layout = US_LAYOUT;
bool shift_pressed = false;
bool os_mode = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* QWERTY
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |BackSP|
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | LCTRL|   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '   |
 * |------+------+------+------+------+------|   (   |    |    )  |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LOWER| LGUI | /Space  /       \Enter \  | RGUI | RAISE| RAlt |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
 [_QWERTY] = LAYOUT( \
   KC_ESC, CS_1, CS_2, CS_3, CS_4, CS_5,                   CS_6, CS_7,    CS_8,    CS_9,    CS_0,    KC_BSPC, \
   KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T,                   KC_Y, KC_U,    KC_I,    KC_O,    KC_P,    MY_MINS, \
  KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G,                   KC_H, KC_J,    KC_K,    KC_L, MY_SCLN,    MY_QUOT, \
    SHIFT, KC_Z, KC_X, KC_C, KC_V, KC_B, MY_LPRN, MY_RPRN, KC_N, KC_M, KC_COMM,  KC_DOT, KC_SLSH,      SHIFT, \
            KC_LALT, KC_LGUI, MO(_LOWER), KC_SPC, KC_ENT, MO(_RAISE), KC_RGUI, KC_RALT \
),

/* LOWER
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |                    |  F7  |  F8  |  F9  | F10  | F11  | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |   ~  |   !  |   @  |   #  |   $  |   %  |-------.    ,-------|   ^  |   &  |   *  |   (  |   )  |   -  |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------|    |-------|      |   _  |   +  |   {  |   }  |   |  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LOWER| LGUI | /Space  /       \Enter \  | RGUI | RAISE| RAlt |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_LOWER] = LAYOUT( \
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, \
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, \
  MY_TILD, MY_BKKR,   MY_AT, MY_HASH,  MY_DLR, MY_PERC,                   MY_CIRC, MY_AMPR, MY_ASTR, MY_LPRN, MY_RPRN, MY_TILD, \
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MY_LBRC, MY_RBRC, XXXXXXX, MY_UNDS, MY_PLUS, MY_LCBR, MY_RCBR, MY_PIPE, \
                             _______, _______, _______, _______, _______, _______, _______, _______\
),

/* RAISE
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |DELETE|
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |   `  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |-------.    ,-------| Left | Down |  Up  |Right |      |      |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |  F7  |  F8  |  F9  | F10  | F11  | F12  |-------|    |-------|   +  |   -  |   =  |   [  |   ]  |   \  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LOWER| LGUI | /Space  /       \Enter \  | RGUI | RAISE| RAlt |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_RAISE] = LAYOUT( \
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  KC_DEL, \
   MY_GRV,    CS_1,    CS_2,    CS_3,    CS_4,    CS_5,                      CS_6,    CS_7,    CS_8,    CS_9,    CS_0, _______, \
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                   KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, XXXXXXX, \
  _______,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, MY_LBRC, MY_RBRC, MY_PLUS, MY_MINS,  MY_EQL, MY_LBRC, MY_RBRC, MY_BSLS, \
                             _______, _______, _______, _______, _______, _______, _______, _______ \
),

/* QWERTY_WIN
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * | ESC  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |BackSP|
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |                    |   Y  |   U  |   I  |   O  |   P  |  -   |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |LCTRL |   A  |   S  |   D  |   F  |   G  |-------.    ,-------|   H  |   J  |   K  |   L  |   ;  |  '   |
 * |------+------+------+------+------+------| MUHEN |    |HENKAN |------+------+------+------+------+------|
 * |LShift|   Z  |   X  |   C  |   V  |   B  |-------|    |-------|   N  |   M  |   ,  |   .  |   /  |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LOWER|LOWER | /Space  /       \Enter \  |RAISE |RAISE | RAlt |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
 [_QWERTY_WIN] = LAYOUT( \
   KC_ESC,    CS_1,   CS_2,   CS_3,   CS_4,   CS_5,                   CS_6,   CS_7,      CS_8,     CS_9,      CS_0,   KC_BSPC, \
   KC_TAB,    JP_Q,   JP_W,   JP_E,   JP_R,   JP_T,                   JP_Y,   JP_U,      JP_I,     JP_O,      JP_P,   MY_MINS, \
  KC_LCTL,    JP_A,   JP_S,   JP_D,   JP_F,   JP_G,                   JP_H,   JP_J,      JP_K,     JP_L,   MY_SCLN,   MY_QUOT, \
    SHIFT,    JP_Z,   JP_X,   JP_C,   JP_V,   JP_B, KC_MHEN, KC_HENK, JP_N,   JP_M,   KC_COMM,   KC_DOT,   KC_SLSH,     SHIFT, \
  KC_LALT, KC_LGUI, LT(_LOWER_WIN, KC_MHEN), KC_SPC, KC_ENT, LT(_RAISE_WIN, KC_HENK), MO(_RAISE_WIN), KC_RALT \
),

/* LOWER_WIN
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |                    |  F7  |  F8  |  F9  | F10  | F11  | F12  |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |   `  |   !  |   @  |   #  |   $  |   %  |-------.    ,-------| Left | Down |  Up  |Right |      |   ~  |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |LShift|   ^  |   &  |   *  |   (  |   )  |-------|    |-------|      |   _  |   +  |   {  |   }  |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /Space  /       \Enter \  |      |      |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_LOWER_WIN] = LAYOUT( \
  _______,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, \
    KC_F1,      KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                     KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, \
   MY_GRV, MY_BKKR,   MY_AT, MY_HASH,  MY_DLR, MY_PERC,                   KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, MY_TILD, \
  _______,    MY_CIRC, MY_AMPR, MY_ASTR, MY_LPRN, MY_RPRN, MY_LBRC, MY_RBRC, XXXXXXX, MY_UNDS, MY_PLUS, MY_LCBR, MY_RCBR, _______, \
                                _______, _______, _______, _______, _______, _______, _______, _______\
),

/* RAISE_WIN
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |DELETE|
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |   `  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------.    ,-------| Left | Down |  Up  |Right |      |      |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |LShift|      |      |      |      |      |-------|    |-------|   +  |   -  |   =  |   [  |   \  |RShift|
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /Space  /       \Enter \  |      |      |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_RAISE_WIN] = LAYOUT( \
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  KC_DEL, \
   MY_GRV,    CS_1,    CS_2,    CS_3,    CS_4,    CS_5,                      CS_6,    CS_7,    CS_8,    CS_9,    CS_0, XXXXXXX, \
  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, XXXXXXX, \
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, MY_LBRC, MY_RBRC, MY_PLUS, MY_MINS,  MY_EQL, MY_LBRC, MY_BSLS, _______, \
                            _______, _______, _______,  _______, _______,  _______, _______, _______ \
),

/* ADJUST
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |BT_ID0|BT_ID1|BT_ID2|BT_ID3|BT_ID4|                    |BT_ID5|BT_ID6|      |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      | WIN  |      |      |      |                    |      |  US  | INFO |      |      |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |      |      |      |      |      |      |-------.    ,-------|      | JIS  |      |      |      |      |
 * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
 * |     |AD_WO_L|SEL_BLE|SEL_USB|    |      |-------|    |-------|      | MAC  |      |      |      |      |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   |      |      |      | /Space  /       \Enter \  |      |      |      |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_ADJUST] = LAYOUT( \
  XXXXXXX,  BT_ID1,     BT_ID2,  BT_ID3,  BT_ID4,  BT_ID5,                    BT_ID6,  BT_ID7, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX, QWERTY_WIN, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX,      US,    INFO, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX,     JIS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  _______, AD_WO_L,    SEL_BLE, SEL_USB, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  QWERTY, XXXXXXX, XXXXXXX, XXXXXXX, _______, \
                                _______, _______, _______, _______, _______,  _______, _______, _______ \
)
};

int layer_state_to_num(layer_state_t state) {
  int n = 0;
  for (; n < 16; ++n) {
    if (((state>>n) & 0x1) == 1) break;
  }
  return n;
}

// Called when a layer changed.
layer_state_t layer_state_set_user(layer_state_t state) {
  layer_state_t s = state;

  // Call _ADJUST from MAC_MODE
  state = update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
  if (s != state) return state;

  // Call _ADJUST from WIN_MODE
  state = update_tri_layer_state(state, _LOWER_WIN, _RAISE_WIN, _ADJUST);

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
  kb_config.raw = eeconfig_read_user();
  kb_layout = kb_config.layout;
  os_mode = layer_state_to_num(eeconfig_read_default_layer()) == _QWERTY ? MAC_MODE : WINDOWS_MODE;
  show_info_oled();
}

// Set keyboard layout information to eeprom.
void set_default_kb_layout(bool layout) {
  kb_config.layout = layout;
  eeconfig_update_user(kb_config.raw);
}

#ifdef OLED_DRIVER_ENABLE
// Display information on OLEDs.
void show_info_oled() {
#ifdef CONSOLE_ENABLE
  uprintf("show_info_oled\n");
#endif // CONSOLE_ENABLE
  if (is_keyboard_master()) {
    oled_on();
    char os[24] = "OS: ";
    strcat(os, os_mode ? "WINDOWS" : "MAC");
    char layout[24] = "LAYOUT: ";
    strcat(layout, kb_layout ? "JIS" : "US");
    char batt[24];
    // The code is based on tmk_core/protocol/nrf/bmp.c
    sprintf(batt, "BATTERY: %4dmV", BMPAPI->app.get_vcc_mv());
    oled_clear();
    oled_write_ln(os, false);
    oled_write_ln(layout, false);
    oled_write_ln(batt, false);
  }
}
#endif // OLED_DRIVER_ENABLE

// Called when a key pressed/released.
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case JIS:
    case US:
    case QWERTY:
    case QWERTY_WIN:
      ;
      break;
    default:
#ifdef OLED_DRIVER_ENABLE
      oled_off();
#endif // OLED_DRIVER_ENABLE
  }

  switch (keycode) {
    CASE_US(CS_0, KEY(0), SHIFT_DU(KEY_SHIFT(9), KEY(0)));
    CASE_US(CS_1, KEY(1), KEY(1));
    CASE_US(CS_2, KEY(2), SHIFT_DU(KEY_UPSHIFT(LBRACKET), KEY(2)));
    CASE_US(CS_3, KEY(3), KEY(3));
    CASE_US(CS_4, KEY(4), KEY(4));
    CASE_US(CS_5, KEY(5), KEY(5));
    CASE_US(CS_6, KEY(6), SHIFT_DU(KEY_UPSHIFT(EQUAL), KEY(6)));
    CASE_US(CS_7, KEY(7), SHIFT_DU(KEY_SHIFT(6), KEY(7)));
    CASE_US(CS_8, KEY(8), SHIFT_DU(KEY_SHIFT(QUOTE), KEY(8)));
    CASE_US(CS_9, KEY(9), SHIFT_DU(KEY_SHIFT(8), KEY(9)));
    CASE_US(MY_DEL, KEY(DELETE), KEY_UPSHIFT(BSPACE));
    CASE_US(MY_TILD, KEY_SHIFT(GRAVE), KEY_SHIFT(EQUAL));
    CASE_US(MY_BKKR, KEY_SHIFT(1), KEY_SHIFT(1));
    CASE_US(MY_AT, KEY_SHIFT(2), KEY(LBRACKET));
    CASE_US(MY_HASH, KEY_SHIFT(3), KEY_SHIFT(3));
    CASE_US(MY_DLR, KEY_SHIFT(4), KEY_SHIFT(4));
    CASE_US(MY_PERC, KEY_SHIFT(5), KEY_SHIFT(5));
    CASE_US(MY_CIRC, KEY_SHIFT(6), KEY(EQUAL));
    CASE_US(MY_AMPR, KEY_SHIFT(7), KEY_SHIFT(6));
    CASE_US(MY_ASTR, KEY_SHIFT(8), KEY_SHIFT(QUOTE));
    CASE_US(MY_LPRN, KEY_SHIFT(9), KEY_SHIFT(8));
    CASE_US(MY_RPRN, KEY_SHIFT(0), KEY_SHIFT(9));
    CASE_US(MY_LBRC, KEY(LBRACKET), SHIFT_DU(KEY_SHIFT(RBRACKET), KEY(RBRACKET)));
    CASE_US(MY_RBRC, KEY(RBRACKET), SHIFT_DU(KEY_SHIFT(NONUS_HASH), KEY(NONUS_HASH)));
    CASE_US(MY_LCBR, KEY_SHIFT(LBRACKET), KEY_SHIFT(RBRACKET));
    CASE_US(MY_RCBR, KEY_SHIFT(RBRACKET), KEY_SHIFT(NONUS_HASH));
    CASE_US(MY_GRV, KEY(GRAVE), SHIFT_DU(KEY_SHIFT(EQUAL), KEY_SHIFT(LBRACKET)));
    CASE_US(MY_BSLS, KEY(BSLASH), SHIFT_DU(KEY_SHIFT(INT3), KEY(INT3)));
    CASE_US(MY_PIPE, KEY_SHIFT(BSLASH), KEY_SHIFT(INT3));
    CASE_US(MY_MINS, KEY(MINUS), SHIFT_DU(KEY_SHIFT(INT1), KEY(MINUS)));
    CASE_US(MY_UNDS, KEY_SHIFT(MINUS), KEY_SHIFT(INT1));
    CASE_US(MY_EQL, KEY(EQUAL), SHIFT_DU(KEY_SHIFT(SCOLON), KEY_SHIFT(MINUS)));
    CASE_US(MY_PLUS, KEY_SHIFT(EQUAL), KEY_SHIFT(SCOLON));
    CASE_US(MY_SCLN, KEY(SCOLON), SHIFT_DU(KEY_UPSHIFT(QUOTE), KEY(SCOLON)));
    CASE_US(MY_QUOT, KEY(QUOTE), SHIFT_DU(KEY_SHIFT(2), KEY_SHIFT(7)));
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
    case SEL_BLE:
      if (record->event.pressed) {
#ifdef CONSOLE_ENABLE
        uprintf("[lilyble] SEL_BLE\n");
#endif // CONSOLE_ENABLE
        select_ble();
      }
      return false;
    case SEL_USB:
      if (record->event.pressed) {
#ifdef CONSOLE_ENABLE
        uprintf("[lilyble] SEL_USB\n");
#endif // CONSOLE_ENABLE
        select_usb();
      }
      return false;
    case AD_WO_L:
      if (record->event.pressed) {
#ifdef CONSOLE_ENABLE
        uprintf("[lilyble] AD_WO_L\n");
#endif // CONSOLE_ENABLE
        BMPAPI->ble.advertise(255);
      }
      return false;
    case INFO:
      if (record->event.pressed) {
        show_info_oled();
      }
      return false;
    case JIS:
      if (record->event.pressed) {
        kb_layout = JIS_LAYOUT;
        set_default_kb_layout(kb_layout);
        show_info_oled();
      }
      return false;
    case US:
      if (record->event.pressed) {
        kb_layout = US_LAYOUT;
        set_default_kb_layout(kb_layout);
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
      break;
  }
  return true;
}