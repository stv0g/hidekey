
#ifndef _HIDEKEY_H_
#define _HIDEKEY_H_

#include "../config.h"

/* Push button */
#define BTN_PORT	PORTB
#define BTN_DDR		DDRB
#define BTN_PIN		PINB
#define BTN_BIT1	3	/* we set PB5 to low and enable pullup on PB3 */
#define BTN_BIT2	5
#define buttonPressed()	(~BTN_PIN & (1 << BTN_BIT1))

/* Write lock jumper */
#define WRLOCK_PORT	PORTC
#define WRLOCK_DDR	DDRC
#define WRLOCK_PIN	PINC
#define WRLOCK_BIT	2
#define writable()	(~WRLOCK_PIN & (1 << WRLOCK_BIT))	/* eeprom is writable if jumper is set */

/* The LED states */
#define LED_NUM		(1 << 0)
#define LED_CAPS	(1 << 1)
#define LED_SCROLL	(1 << 2)
#define LED_COMPOSE	(1 << 3)
#define LED_KANA 	(1 << 4)

/* The control keys */
#define CTRL_L		(1 << 0)
#define SHIFT_L		(1 << 1)
#define ALT_L		(1 << 2)
#define GUI_L		(1 << 3) /* windows logo */
#define CTRL_R		(1 << 4)
#define SHIFT_R		(1 << 5)
#define ALT_R		(1 << 6)
#define GUI_R		(1 << 7) /* windows logo */

void usbReset(void);
void hardwareInit(void);

void genPassword(char *str, size_t n);
void getPassword(struct slot *slot, char *str, size_t n);
uint8_t getSeed(void);

void hidBuildReport(char chr);
void hidSendChar(char chr);
void hidSendString(char *str);

void setSlot(struct slot *slot, uint16_t index);
void getSlot(struct slot *slot, uint16_t index);

#endif /* _HIDEKEY_H_ */
