#ifndef KEYMAP_H
#define KEYMAP_H

#include <linux/input-event-codes.h>

typedef struct {
const char *label;
const char *shift_label;
int key_code;
} KeyMapping;

static const KeyMapping key_map[] = {
  /* Row 0 */
  {"`", "~", KEY_GRAVE}, {"1", "!", KEY_1}, {"2", "#", KEY_2}, {"3", "#", KEY_3}, {"4", "$", KEY_4},
  {"5", "%", KEY_5}, {"6", "^", KEY_6}, {"7", "&", KEY_7}, {"8", "*", KEY_8}, {"9", "(", KEY_9},
  {"0", ")", KEY_0}, {"-", "_", KEY_MINUS}, {"=", "+", KEY_EQUAL}, {"Backspace", "Delete", KEY_BACKSPACE},

  /* Row 1 */
  {"Tab", "Tab", KEY_TAB}, {"q", "Q", KEY_Q}, {"w", "W", KEY_W}, {"e", "E", KEY_E}, {"r", "R", KEY_R},
  {"t", "T", KEY_T}, {"y", "Y", KEY_Y}, {"u", "U", KEY_U}, {"i", "I", KEY_I}, {"o", "O", KEY_O},
  {"p", "P", KEY_P}, {"[", "{", KEY_LEFTBRACE}, {"]", "}", KEY_RIGHTBRACE}, {"\\", "|", KEY_BACKSLASH},

  /* Row 2 */
  {"Escape", "Escape", KEY_ESC}, {"a", "A", KEY_A}, {"s", "S", KEY_S}, {"d", "D", KEY_D},
  {"f", "F", KEY_F}, {"g", "G", KEY_G}, {"h", "H", KEY_H}, {"j", "J", KEY_J}, {"k", "K", KEY_K},
  {"l", "L", KEY_L}, {";", ":", KEY_SEMICOLON}, {"'", "\"", KEY_APOSTROPHE}, {"Enter", "Enter", KEY_ENTER},

  /* Row 3 */
  {"Shift", "Shift", KEY_LEFTSHIFT}, {"z", "Z", KEY_Z}, {"x", "X", KEY_X}, {"c", "C", KEY_C},
  {"v", "V", KEY_V}, {"b", "B", KEY_B}, {"n", "N", KEY_N}, {"m", "M", KEY_M}, {",", "<", KEY_COMMA},
  {".", ">", KEY_DOT}, {"/", "?", KEY_SLASH}, {"Shift", "Shift", KEY_RIGHTSHIFT},

  /* Row 4 */
  {"Ctrl", "Ctrl", KEY_LEFTCTRL}, {"Super", "Super", KEY_LEFTMETA}, {"Alt", "Alt", KEY_LEFTALT},
  {"Space", "Space", KEY_SPACE}, {"Alt", "Alt", KEY_RIGHTALT}, {"Ctrl", "Ctrl", KEY_RIGHTCTRL}
};

#define KEY_MAP_SIZE (sizeof(key_map) / sizeof(KeyMapping))

#endif
