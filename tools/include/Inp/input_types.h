#ifndef _input_types_h
#define _input_types_h

//Coop level for input
#define INP_EXCLUSIVE     0x00000001
#define INP_NONEXCLUSIVE  0x00000002
#define INP_FOREGROUND    0x00000004
#define INP_BACKGROUND    0x00000008
#define INP_NOWINKEY      0x00000010

//public constants
#define SAMPLE_BUFFER_SIZE	16

#define MAXKBKEY			256
#define MAXJOYSTICKBTN		32
#define MAXMOUSEBTN			8

//////////////////////////////////////////////////////////////////////////////////////
//Key Codes
#define INP_ESCAPE          0x01
#define INP_1               0x02
#define INP_2               0x03
#define INP_3               0x04
#define INP_4               0x05
#define INP_5               0x06
#define INP_6               0x07
#define INP_7               0x08
#define INP_8               0x09
#define INP_9               0x0A
#define INP_0               0x0B
#define INP_MINUS           0x0C    /* - on main keyboard */
#define INP_EQUALS          0x0D
#define INP_BACK            0x0E    /* backspace */
#define INP_TAB             0x0F
#define INP_Q               0x10
#define INP_W               0x11
#define INP_E               0x12
#define INP_R               0x13
#define INP_T               0x14
#define INP_Y               0x15
#define INP_U               0x16
#define INP_I               0x17
#define INP_O               0x18
#define INP_P               0x19
#define INP_LBRACKET        0x1A
#define INP_RBRACKET        0x1B
#define INP_RETURN          0x1C    /* Enter on main keyboard */
#define INP_LCONTROL        0x1D
#define INP_A               0x1E
#define INP_S               0x1F
#define INP_D               0x20
#define INP_F               0x21
#define INP_G               0x22
#define INP_H               0x23
#define INP_J               0x24
#define INP_K               0x25
#define INP_L               0x26
#define INP_SEMICOLON       0x27
#define INP_APOSTROPHE      0x28
#define INP_GRAVE           0x29    /* accent grave */
#define INP_LSHIFT          0x2A
#define INP_BACKSLASH       0x2B
#define INP_Z               0x2C
#define INP_X               0x2D
#define INP_C               0x2E
#define INP_V               0x2F
#define INP_B               0x30
#define INP_N               0x31
#define INP_M               0x32
#define INP_COMMA           0x33
#define INP_PERIOD          0x34    /* . on main keyboard */
#define INP_SLASH           0x35    /* / on main keyboard */
#define INP_RSHIFT          0x36
#define INP_MULTIPLY        0x37    /* * on numeric keypad */
#define INP_LMENU           0x38    /* left Alt */
#define INP_SPACE           0x39
#define INP_CAPITAL         0x3A
#define INP_F1              0x3B
#define INP_F2              0x3C
#define INP_F3              0x3D
#define INP_F4              0x3E
#define INP_F5              0x3F
#define INP_F6              0x40
#define INP_F7              0x41
#define INP_F8              0x42
#define INP_F9              0x43
#define INP_F10             0x44
#define INP_NUMLOCK         0x45
#define INP_SCROLL          0x46    /* Scroll Lock */
#define INP_NUMPAD7         0x47
#define INP_NUMPAD8         0x48
#define INP_NUMPAD9         0x49
#define INP_SUBTRACT        0x4A    /* - on numeric keypad */
#define INP_NUMPAD4         0x4B
#define INP_NUMPAD5         0x4C
#define INP_NUMPAD6         0x4D
#define INP_ADD             0x4E    /* + on numeric keypad */
#define INP_NUMPAD1         0x4F
#define INP_NUMPAD2         0x50
#define INP_NUMPAD3         0x51
#define INP_NUMPAD0         0x52
#define INP_DECIMAL         0x53    /* . on numeric keypad */
#define INP_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define INP_F11             0x57
#define INP_F12             0x58
#define INP_F13             0x64    /*                     (NEC PC98) */
#define INP_F14             0x65    /*                     (NEC PC98) */
#define INP_F15             0x66    /*                     (NEC PC98) */
#define INP_KANA            0x70    /* (Japanese keyboard)            */
#define INP_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define INP_CONVERT         0x79    /* (Japanese keyboard)            */
#define INP_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define INP_YEN             0x7D    /* (Japanese keyboard)            */
#define INP_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define INP_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define INP_PREVTRACK       0x90    /* Previous Track (INP_CIRCUMFLEX on Japanese keyboard) */
#define INP_AT              0x91    /*                     (NEC PC98) */
#define INP_COLON           0x92    /*                     (NEC PC98) */
#define INP_UNDERLINE       0x93    /*                     (NEC PC98) */
#define INP_KANJI           0x94    /* (Japanese keyboard)            */
#define INP_STOP            0x95    /*                     (NEC PC98) */
#define INP_AX              0x96    /*                     (Japan AX) */
#define INP_UNLABELED       0x97    /*                        (J3100) */
#define INP_NEXTTRACK       0x99    /* Next Track */
#define INP_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define INP_RCONTROL        0x9D
#define INP_MUTE            0xA0    /* Mute */
#define INP_CALCULATOR      0xA1    /* Calculator */
#define INP_PLAYPAUSE       0xA2    /* Play / Pause */
#define INP_MEDIASTOP       0xA4    /* Media Stop */
#define INP_VOLUMEDOWN      0xAE    /* Volume - */
#define INP_VOLUMEUP        0xB0    /* Volume + */
#define INP_WEBHOME         0xB2    /* Web home */
#define INP_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define INP_DIVIDE          0xB5    /* / on numeric keypad */
#define INP_SYSRQ           0xB7
#define INP_RMENU           0xB8    /* right Alt */
#define INP_PAUSE           0xC5    /* Pause */
#define INP_HOME            0xC7    /* Home on arrow keypad */
#define INP_UP              0xC8    /* UpArrow on arrow keypad */
#define INP_PRIOR           0xC9    /* PgUp on arrow keypad */
#define INP_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define INP_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define INP_END             0xCF    /* End on arrow keypad */
#define INP_DOWN            0xD0    /* DownArrow on arrow keypad */
#define INP_NEXT            0xD1    /* PgDn on arrow keypad */
#define INP_INSERT          0xD2    /* Insert on arrow keypad */
#define INP_DELETE          0xD3    /* Delete on arrow keypad */
#define INP_LWIN            0xDB    /* Left Windows key */
#define INP_RWIN            0xDC    /* Right Windows key */
#define INP_APPS            0xDD    /* AppMenu key */
#define INP_POWER           0xDE    /* System Power */
#define INP_SLEEP           0xDF    /* System Sleep */
#define INP_WAKE            0xE3    /* System Wake */
#define INP_WEBSEARCH       0xE5    /* Web Search */
#define INP_WEBFAVORITES    0xE6    /* Web Favorites */
#define INP_WEBREFRESH      0xE7    /* Web Refresh */
#define INP_WEBSTOP         0xE8    /* Web Stop */
#define INP_WEBFORWARD      0xE9    /* Web Forward */
#define INP_WEBBACK         0xEA    /* Web Back */
#define INP_MYCOMPUTER      0xEB    /* My Computer */
#define INP_MAIL            0xEC    /* Mail */
#define INP_MEDIASELECT     0xED    /* Media Select */

#define INP_MAXCODE			0xEE

/*
 *  Alternate names for keys, to facilitate transition from DOS.
 */
#define INP_BACKSPACE       INP_BACK            /* backspace */
#define INP_NUMPADSTAR      INP_MULTIPLY        /* * on numeric keypad */
#define INP_LALT            INP_LMENU           /* left Alt */
#define INP_CAPSLOCK        INP_CAPITAL         /* CapsLock */
#define INP_NUMPADMINUS     INP_SUBTRACT        /* - on numeric keypad */
#define INP_NUMPADPLUS      INP_ADD             /* + on numeric keypad */
#define INP_NUMPADPERIOD    INP_DECIMAL         /* . on numeric keypad */
#define INP_NUMPADSLASH     INP_DIVIDE          /* / on numeric keypad */
#define INP_RALT            INP_RMENU           /* right Alt */
#define INP_UPARROW         INP_UP              /* UpArrow on arrow keypad */
#define INP_PGUP            INP_PRIOR           /* PgUp on arrow keypad */
#define INP_LEFTARROW       INP_LEFT            /* LeftArrow on arrow keypad */
#define INP_RIGHTARROW      INP_RIGHT           /* RightArrow on arrow keypad */
#define INP_DOWNARROW       INP_DOWN            /* DownArrow on arrow keypad */
#define INP_PGDN            INP_NEXT            /* PgDn on arrow keypad */

/*
 *  Alternate names for keys originally not used on US keyboards.
 */
#define INP_CIRCUMFLEX      INP_PREVTRACK       /* Japanese keyboard */
//////////////////////////////////////////////////////////////////////////////////////

//Input states
#define INPUT_UP			0		//if button is untouched
#define INPUT_RELEASED		1		//if button was released (only used with buffer update)
#define INPUT_DOWN			0x80	//if button is down

//enums
typedef enum { //X/Y axis
	eGamePadLeft,
	eGamePadRight,
	eGamePadUp,
	eGamePadDown,
	eGamePadNum
} eGamePadArrows;

//structs

//input state for given key
typedef struct _inpState {
	u8	ofs;		//key/btn-code
	u8	state;		//INPUT_(UP,RELEASED,DOWN)
} inpState;

//handles
typedef struct _inputJoystick *hJOYSTICK;

#endif