// Parses input into escape codes, text, etc. Although it's called VT100Terminal, it's more of an
// xterm emulator. The real work of acting on escape codes is handled by the delegate.

#import <Cocoa/Cocoa.h>
#import "iTermCursor.h"
#import "ScreenChar.h"
#import "VT100Grid.h"
#import "VT100TerminalDelegate.h"

typedef struct VT100TCC VT100TCC;

typedef enum {
    MOUSE_FORMAT_XTERM = 0,       // Regular 1000 mode
    MOUSE_FORMAT_XTERM_EXT = 1,   // UTF-8 1005 mode
    MOUSE_FORMAT_URXVT = 2,       // rxvt's 1015 mode
    MOUSE_FORMAT_SGR = 3          // SGR 1006 mode
} MouseFormat;

typedef enum {
    // X11 button number
    MOUSE_BUTTON_LEFT = 0,       // left button
    MOUSE_BUTTON_MIDDLE = 1,     // middle button
    MOUSE_BUTTON_RIGHT = 2,      // right button
    MOUSE_BUTTON_NONE = 3,       // no button pressed - for 1000/1005/1015 mode
    MOUSE_BUTTON_SCROLLDOWN = 4, // scroll down
    MOUSE_BUTTON_SCROLLUP = 5    // scroll up
} MouseButtonNumber;

// Indexes into key_strings.
typedef enum {
    TERMINFO_KEY_LEFT, TERMINFO_KEY_RIGHT, TERMINFO_KEY_UP, TERMINFO_KEY_DOWN,
    TERMINFO_KEY_HOME, TERMINFO_KEY_END, TERMINFO_KEY_PAGEDOWN,
    TERMINFO_KEY_PAGEUP, TERMINFO_KEY_F0, TERMINFO_KEY_F1, TERMINFO_KEY_F2,
    TERMINFO_KEY_F3, TERMINFO_KEY_F4, TERMINFO_KEY_F5, TERMINFO_KEY_F6,
    TERMINFO_KEY_F7, TERMINFO_KEY_F8, TERMINFO_KEY_F9, TERMINFO_KEY_F10,
    TERMINFO_KEY_F11, TERMINFO_KEY_F12, TERMINFO_KEY_F13, TERMINFO_KEY_F14,
    TERMINFO_KEY_F15, TERMINFO_KEY_F16, TERMINFO_KEY_F17, TERMINFO_KEY_F18,
    TERMINFO_KEY_F19, TERMINFO_KEY_F20, TERMINFO_KEY_F21, TERMINFO_KEY_F22,
    TERMINFO_KEY_F23, TERMINFO_KEY_F24, TERMINFO_KEY_F25, TERMINFO_KEY_F26,
    TERMINFO_KEY_F27, TERMINFO_KEY_F28, TERMINFO_KEY_F29, TERMINFO_KEY_F30,
    TERMINFO_KEY_F31, TERMINFO_KEY_F32, TERMINFO_KEY_F33, TERMINFO_KEY_F34,
    TERMINFO_KEY_F35, TERMINFO_KEY_BACKSPACE, TERMINFO_KEY_BACK_TAB,
    TERMINFO_KEY_TAB, TERMINFO_KEY_DEL, TERMINFO_KEY_INS, TERMINFO_KEY_HELP,
    TERMINFO_KEYS
} VT100TerminalTerminfoKeys;

#define NUM_CHARSETS 4  // G0...G3. Values returned from -charset go from 0 to this.
#define NUM_MODIFIABLE_RESOURCES 5

@interface VT100Terminal : NSObject

@property(nonatomic, assign) id<VT100TerminalDelegate> delegate;
@property(nonatomic, copy) NSString *termType;
@property(nonatomic, assign) NSStringEncoding encoding;
@property(nonatomic, readonly) BOOL reportFocus;

- (void)putStreamData:(NSData*)data;
- (void)putStreamData:(const char *)buffer length:(int)length;

// Returns true if a new token was parsed, false if there was nothing left to do.
- (BOOL)parseNextToken;
- (NSData *)streamData;
- (void)clearStream;

- (void)setForegroundColor:(int)fgColorCode alternateSemantics:(BOOL)altsem;
- (void)setBackgroundColor:(int)bgColorCode alternateSemantics:(BOOL)altsem;

- (void)resetCharset;
- (void)resetPreservingPrompt:(BOOL)preservePrompt;

- (NSData *)keyArrowUp:(unsigned int)modflag;
- (NSData *)keyArrowDown:(unsigned int)modflag;
- (NSData *)keyArrowLeft:(unsigned int)modflag;
- (NSData *)keyArrowRight:(unsigned int)modflag;
- (NSData *)keyHome:(unsigned int)modflag;
- (NSData *)keyEnd:(unsigned int)modflag;
- (NSData *)keyInsert;
- (NSData *)keyDelete;
- (NSData *)keyBackspace;
- (NSData *)keyPageUp:(unsigned int)modflag;
- (NSData *)keyPageDown:(unsigned int)modflag;
- (NSData *)keyFunction:(int)no;
- (NSData *)keypadData: (unichar) unicode keystr: (NSString *) keystr;

- (NSData *)mousePress:(int)button withModifiers:(unsigned int)modflag atX:(int)x Y:(int)y;
- (NSData *)mouseRelease:(int)button withModifiers:(unsigned int)modflag atX:(int)x Y:(int)y;
- (NSData *)mouseMotion:(int)button withModifiers:(unsigned int)modflag atX:(int)x Y:(int)y;

- (BOOL)reverseVideo;
- (BOOL)originMode;
- (BOOL)wraparoundMode;
- (void)setWraparoundMode:(BOOL)mode;
- (BOOL)isAnsi;
- (BOOL)autorepeatMode;
- (BOOL)insertMode;
- (int)charset;
- (MouseMode)mouseMode;

- (screen_char_t)foregroundColorCode;
- (screen_char_t)backgroundColorCode;
- (screen_char_t)foregroundColorCodeReal;
- (screen_char_t)backgroundColorCodeReal;

- (NSData *)reportActivePositionWithX:(int)x Y:(int)y withQuestion:(BOOL)q;

- (void)setDisableSmcupRmcup:(BOOL)value;

- (BOOL)bracketedPasteMode;

- (void)setInsertMode:(BOOL)mode;
- (void)setCursorMode:(BOOL)mode;
- (void)setKeypadMode:(BOOL)mode;
- (void)setAllowKeypadMode:(BOOL)allow;
- (void)setMouseMode:(MouseMode)mode;
- (void)setMouseFormat:(MouseFormat)format;

// Call appropriate delegate methods to handle the last parsed token. Call this after -parseNextToken
// returns YES.
- (void)executeToken;

// Inspect previous parsed token. Can use after -parseNextToken returns YES.
- (BOOL)lastTokenWasASCII;
- (NSString *)lastTokenString;

@end

