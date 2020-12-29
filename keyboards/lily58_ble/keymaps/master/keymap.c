#include QMK_KEYBOARD_H
#ifdef CONSOLE_ENABLE
  #include <print.h>
#endif
#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif
#include <string.h>
#include "keymap_jp.h"  // qmk_firmware_bmp/quantum/keymap_extras/keymap_jp.h
#include "bmp.h"
#include "bmp_custom_keycode.h"
#include "keycode_str_converter.h"

extern uint8_t is_master;

#define SHIFT_RESTORE(STR) (shift_pressed ? SEND_STRING(STR SS_DOWN(X_LSHIFT)) : SEND_STRING(STR SS_UP(X_LSHIFT)))
#define KEY(CODE) (record->event.pressed ? SEND_STRING(SS_DOWN(X_##CODE)) : SHIFT_RESTORE(SS_UP(X_##CODE)))
#define KEY_SHIFT(CODE) (record->event.pressed ? SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_##CODE)) : SHIFT_RESTORE(SS_UP(X_##CODE)))
#define KEY_UPSHIFT(CODE) (record->event.pressed ? SEND_STRING(SS_UP(X_LSHIFT) SS_DOWN(X_##CODE)) : SHIFT_RESTORE(SS_UP(X_##CODE)))
#define SHIFT_DU(CODE_DOWN, CODE_UP) (shift_pressed ? CODE_DOWN : CODE_UP)
#define CASE_US(CODE, US, JIS)                   \
    case CODE:                                  \
        (kb_layout == JIS_LAYOUT ? JIS : US); \
        return false;

// #define OLED_TIMEOUT 0
#define JIS_LAYOUT true
#define US_LAYOUT false
#define WINDOWS_MODE true
#define MAC_MODE false

void show_mode(void);

enum layer_number {
  _QWERTY = 0,
  _LOWER,
  _RAISE,
  _QWERTY_WIN,
  _LOWER_WIN,
  _RAISE_WIN,
  _ADJUST,
};

// char layer_name[7][12] = {
//   "QWERTY",
//   "LOWER",
//   "RAISE",
//   "QWERTY_WIN",
//   "LOWER_WIN",
//   "RAISE_WIN",
//   "ADJUST",
// };

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
  CIRC,
  AT,
  LBRC,
  RBRC,
  BSLS,
  AMPR,
  QUOT,
  LPRN,
  RPRN,
  EQL,
  TILD,
  PIPE,
  GRV,
  LCBR,
  PLUS,
  ASTR,
  RCBR,
  UNDS,
  MINS,
  SCLN,
  EXLM,
  HASH,
  DLR,
  PERC,
  DEL,
};

const key_string_map_t custom_keys_user = {
    .start_kc = QWERTY,
    .end_kc = DEL,
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
      "CIRC\0"
      "AT\0"
      "LBRC\0"
      "RBRC\0"
      "BSLS\0"
      "AMPR\0"
      "QUOT\0"
      "LPRN\0"
      "RPRN\0"
      "EQL\0"
      "TILD\0"
      "PIPE\0"
      "GRV\0"
      "LCBR\0"
      "PLUS\0"
      "ASTR\0"
      "RCBR\0"
      "UNDS\0"
      "MINS\0"
      "SCLN\0"
      "EXLM\0"
      "HASH\0"
      "DLR\0"
      "PERC\0"
      "DEL\0"
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
    KC_ESC,   CS_1,    CS_2,    CS_3,    CS_4,    CS_5,                CS_6,    CS_7,    CS_8,    CS_9,    CS_0, KC_BSPC, \
    KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    MINS, \
  KC_LCTRL,   KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                KC_H,    KC_J,    KC_K,    KC_L,    SCLN,    QUOT, \
     SHIFT,   KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,  LPRN,  RPRN,  KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,   SHIFT, \
                         KC_LALT, KC_LGUI, MO(_LOWER), KC_SPC,  KC_ENT, MO(_RAISE), KC_RGUI, KC_RALT \
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
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, \
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,               KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, \
     TILD,    EXLM,      AT,    HASH,     DLR,    PERC,                CIRC,    AMPR,    ASTR,    LPRN,    RPRN,    TILD, \
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, LBRC, RBRC, XXXXXXX, UNDS, PLUS, LCBR, RCBR, PIPE, \
                          _______, _______, _______, _______, _______,  _______, _______, _______\
),

/* RAISE
 * ,-----------------------------------------.                    ,-----------------------------------------.
 * |      |      |      |      |      |      |                    |      |      |      |      |      |DELETE|
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |   `  |   1  |   2  |   3  |   4  |   5  |                    |   6  |   7  |   8  |   9  |   0  |      |
 * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
 * |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |-------.    ,-------|      | Left | Down |  Up  |Right |      |
 * |------+------+------+------+------+------|   [   |    |    ]  |------+------+------+------+------+------|
 * |  F7  |  F8  |  F9  | F10  | F11  | F12  |-------|    |-------|   +  |   -  |   =  |   [  |   ]  |   \  |
 * `-----------------------------------------/       /     \      \-----------------------------------------'
 *                   | LAlt | LOWER| LGUI | /Space  /       \Enter \  | RGUI | RAISE| RAlt |
 *                   |      |      |      |/       /         \      \ |      |      |      |
 *                   `----------------------------'           '------''--------------------'
 */
[_RAISE] = LAYOUT( \
  _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  KC_DEL, \
      GRV,    CS_1,    CS_2,    CS_3,    CS_4,    CS_5,                CS_6,    CS_7,    CS_8,    CS_9,    CS_0, _______, \
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,             XXXXXXX, KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, \
  _______,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, LBRC, RBRC,    PLUS,    MINS,     EQL,    LBRC,    RBRC,    BSLS, \
                         _______, _______, _______,  _______, _______,  _______, _______, _______ \
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
    KC_ESC,   CS_1,    CS_2,    CS_3,    CS_4,    CS_5,                      CS_6,    CS_7,    CS_8,    CS_9,    CS_0, KC_BSPC, \
    KC_TAB,   JP_Q,    JP_W,    JP_E,    JP_R,    JP_T,                      JP_Y,    JP_U,    JP_I,    JP_O,    JP_P,    MINS, \
  KC_LCTRL,   JP_A,    JP_S,    JP_D,    JP_F,    JP_G,                      JP_H,    JP_J,    JP_K,    JP_L,    SCLN,    QUOT, \
     SHIFT,   JP_Z,    JP_X,    JP_C,    JP_V,    JP_B, KC_MHEN, KC_HENK,    JP_N,    JP_M, KC_COMM,  KC_DOT, KC_SLSH,   SHIFT, \
       KC_LALT, MO(_LOWER_WIN), LT(_LOWER_WIN, KC_MHEN), KC_SPC, KC_ENT, LT(_RAISE_WIN, KC_HENK), MO(_RAISE_WIN), KC_RALT \
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
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,             XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______, \
      KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,               KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, \
        GRV,    EXLM,      AT,    HASH,     DLR,    PERC,             KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX,    TILD, \
    _______,    CIRC,    AMPR,    ASTR,    LPRN,    RPRN, LBRC, RBRC, XXXXXXX,    UNDS,    PLUS,    LCBR,    RCBR, _______, \
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
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,              XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  KC_DEL, \
        GRV,    CS_1,    CS_2,    CS_3,    CS_4,    CS_5,                 CS_6,    CS_7,    CS_8,    CS_9,    CS_0, XXXXXXX, \
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,              KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, XXXXXXX, \
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  LBRC, RBRC,    PLUS,    MINS,     EQL,    LBRC,    BSLS, _______, \
                            _______, _______, _______,  _______, _______,  _______, _______, _______ \
  ),

  /* ADJUST
   * ,-----------------------------------------.                    ,-----------------------------------------.
   * |      |      |      |      |      |      |                    |      |      |      |      |      |      |
   * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
   * |      |      | WIN  |      |      |      |                    |      |  US  |      |      |      |      |
   * |------+------+------+------+------+------|                    |------+------+------+------+------+------|
   * |      |      |      |      |      |      |-------.    ,-------|      | JIS  |      |      |      |      |
   * |------+------+------+------+------+------|       |    |       |------+------+------+------+------+------|
   * |      |      |      |      |      |      |-------|    |-------|      | MAC  |      |      |      |      |
   * `-----------------------------------------/       /     \      \-----------------------------------------'
   *                   |      |      |      | /Space  /       \Enter \  |      |      |      |
   *                   |      |      |      |/       /         \      \ |      |      |      |
   *                   `----------------------------'           '------''--------------------'
   */
  [_ADJUST] = LAYOUT( \
  XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX, QWERTY_WIN, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX,      US, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                   XXXXXXX,     JIS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
  XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  QWERTY, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, \
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
  show_mode();
}

// Set keyboard layout information to eeprom.
void set_default_kb_layout(bool layout) {
  kb_config.layout = layout;
  eeconfig_update_user(kb_config.raw);
}

// Display information on OLEDs.
void show_mode() {
#ifdef CONSOLE_ENABLE
  print("show_mode\n");
#endif
  if (is_keyboard_master()) {
#ifdef OLED_DRIVER_ENABLE
    oled_on();
    char os[24] = "OS: ";
    strcat(os, os_mode ? "WINDOWS\n" : "MAC\n");
    char layout[24] = "LAYOUT: ";
    strcat(layout, kb_layout ? "JIS\n" : "US\n");
    oled_clear();
    oled_write_ln(os, false);
    oled_write_ln(layout, false);
#endif // OLED_DRIVER_ENABLE
  }
}

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
      ;
#ifdef OLED_DRIVER_ENABLE
      oled_clear();
      oled_off();
#endif // OLED_DRIVER_ENABLE
  }

  bool change_keyboard_mode = false;
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
    CASE_US(DEL, KEY(DELETE), KEY_UPSHIFT(BSPACE));
    CASE_US(TILD, KEY_SHIFT(GRAVE), KEY_SHIFT(EQUAL));
    CASE_US(EXLM, KEY_SHIFT(1), KEY_SHIFT(1));
    CASE_US(AT, KEY_SHIFT(2), KEY(LBRACKET));
    CASE_US(HASH, KEY_SHIFT(3), KEY_SHIFT(3));
    CASE_US(DLR, KEY_SHIFT(4), KEY_SHIFT(4));
    CASE_US(PERC, KEY_SHIFT(5), KEY_SHIFT(5));
    CASE_US(CIRC, KEY_SHIFT(6), KEY(EQUAL));
    CASE_US(AMPR, KEY_SHIFT(7), KEY_SHIFT(6));
    CASE_US(ASTR, KEY_SHIFT(8), KEY_SHIFT(QUOTE));
    CASE_US(LPRN, KEY_SHIFT(9), KEY_SHIFT(8));
    CASE_US(RPRN, KEY_SHIFT(0), KEY_SHIFT(9));
    CASE_US(LBRC, KEY(LBRACKET), SHIFT_DU(KEY_SHIFT(RBRACKET), KEY(RBRACKET)));
    CASE_US(RBRC, KEY(RBRACKET), SHIFT_DU(KEY_SHIFT(NONUS_HASH), KEY(NONUS_HASH)));
    CASE_US(LCBR, KEY_SHIFT(LBRACKET), KEY_SHIFT(RBRACKET));
    CASE_US(RCBR, KEY_SHIFT(RBRACKET), KEY_SHIFT(NONUS_HASH));
    CASE_US(GRV, KEY(GRAVE), SHIFT_DU(KEY_SHIFT(EQUAL), KEY_SHIFT(LBRACKET)));
    CASE_US(BSLS, KEY(BSLASH), SHIFT_DU(KEY_SHIFT(INT3), KEY(INT3)));
    CASE_US(PIPE, KEY_SHIFT(BSLASH), KEY_SHIFT(INT3));
    CASE_US(MINS, KEY(MINUS), SHIFT_DU(KEY_SHIFT(INT1), KEY(MINUS)));
    CASE_US(UNDS, KEY_SHIFT(MINUS), KEY_SHIFT(INT1));
    CASE_US(EQL, KEY(EQUAL), SHIFT_DU(KEY_SHIFT(SCOLON), KEY_SHIFT(MINUS)));
    CASE_US(PLUS, KEY_SHIFT(EQUAL), KEY_SHIFT(SCOLON));
    CASE_US(SCLN, KEY(SCOLON), SHIFT_DU(KEY_UPSHIFT(QUOTE), KEY(SCOLON)));
    CASE_US(QUOT, KEY(QUOTE), SHIFT_DU(KEY_SHIFT(2), KEY_SHIFT(7)));
    case JIS:
      if (record->event.pressed) {
        kb_layout = JIS_LAYOUT;
        set_default_kb_layout(kb_layout);
      }
      change_keyboard_mode = true;
      break;
    case US:
      if (record->event.pressed) {
        kb_layout = US_LAYOUT;
        set_default_kb_layout(kb_layout);
      }
      change_keyboard_mode = true;
      break;
    case QWERTY:
      if (record->event.pressed) {
        os_mode = MAC_MODE;
        set_single_persistent_default_layer(_QWERTY);
      }
      change_keyboard_mode = true;
      break;
    case QWERTY_WIN:
      if (record->event.pressed) {
        os_mode = WINDOWS_MODE;
        set_single_persistent_default_layer(_QWERTY_WIN);
      }
      change_keyboard_mode = true;
      break;
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

  // Normaly, oleds are off. Turn on only when a user change keyboard-mode.
  if (change_keyboard_mode == true) {
    show_mode();
    return false;
  }

  return true;
}