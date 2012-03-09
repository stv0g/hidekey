#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <usbdrv.h>

#include "hidekey.h"

int descriptorStringSerial[MAX_SERIAL_LEN+1]; /* plus 1 for USB_STRING_DESCRIPTOR_HEADER() byte */
int descriptorStringSerial_e[MAX_SERIAL_LEN+1] EEMEM = {
	USB_STRING_DESCRIPTOR_HEADER(1), '0' /* this is the default serial number and should be changed prior shipping */
};


uint16_t counter_e EEMEM = 0; /* how often the stick has been started/enumerated */
struct slot slots_e[MAX_SLOTS] EEMEM; /* is intitialized dynamically during first enumeration */

#ifdef ENCRYPTION
#include "key.h"
#else
#warning "Encryption is DISABLED! This should only be done for debugging purposes."
#endif /* ENCRYPTION */

struct status status; /* status for USBRQ_VENDOR_GET_STATUS */

struct slot currentSlot;
struct slot updateSlot;

uint16_t currentSlotIndex = 0;
uint16_t updateSlotIndex;

/* used by usbFunctionWrite() */
uint8_t bytesRemaining;
uint8_t bytesOffset;

/* USB report descriptor (length is defined in usbconfig.h)
   This has been changed to conform to the USB keyboard boot protocol */
char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] PROGMEM = {
	0x05, 0x01,            // USAGE_PAGE (Generic Desktop)
	0x09, 0x06,            // USAGE (Keyboard)
	0xa1, 0x01,            // COLLECTION (Application)
	0x05, 0x07,            //   USAGE_PAGE (Keyboard)
	0x19, 0xe0,            //   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,            //   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,            //   LOGICAL_MINIMUM (0)
	0x25, 0x01,            //   LOGICAL_MAXIMUM (1)
	0x75, 0x01,            //   REPORT_SIZE (1)
	0x95, 0x08,            //   REPORT_COUNT (8)
	0x81, 0x02,            //   INPUT (Data,Var,Abs)
	0x95, 0x01,            //   REPORT_COUNT (1)
	0x75, 0x08,            //   REPORT_SIZE (8)
	0x81, 0x03,            //   INPUT (Cnst,Var,Abs)
	0x95, 0x05,            //   REPORT_COUNT (5)
	0x75, 0x01,            //   REPORT_SIZE (1)
	0x05, 0x08,            //   USAGE_PAGE (LEDs)
	0x19, 0x01,            //   USAGE_MINIMUM (Num Lock)
	0x29, 0x05,            //   USAGE_MAXIMUM (Kana)
	0x91, 0x02,            //   OUTPUT (Data,Var,Abs)
	0x95, 0x01,            //   REPORT_COUNT (1)
	0x75, 0x03,            //   REPORT_SIZE (3)
	0x91, 0x03,            //   OUTPUT (Cnst,Var,Abs)
	0x95, 0x06,            //   REPORT_COUNT (6)
	0x75, 0x08,            //   REPORT_SIZE (8)
	0x15, 0x00,            //   LOGICAL_MINIMUM (0)
	0x25, 0x65,            //   LOGICAL_MAXIMUM (101)
	0x05, 0x07,            //   USAGE_PAGE (Keyboard)
	0x19, 0x00,            //   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,            //   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,            //   INPUT (Data,Ary,Abs)
	0xc0                   // END_COLLECTION
};

/* ASCII to USB scan/usage codes according to German keyboard layout!
   Keep in mind to change these for other keyboard layouts! */
const uint8_t asciiKeyMap[][2] PROGMEM = {
	{0x2C, 0},		// SPACE (ASCII: 32)
	{0x1E, SHIFT_R},	// !
	{0x1F, SHIFT_R},	// "
	{0x32, 0},		// #
	{0x21, SHIFT_R},	// $
	{0x22, SHIFT_R},	// %
	{0x23, SHIFT_R},	// &
	{0x32, SHIFT_R},	// '
	{0x25, SHIFT_R},	// (
	{0x26, SHIFT_R},	// )
	{0x30, SHIFT_R},	// *
	{0x30, 0},		// +
	{0x36, 0},		// ,
	{0x38, 0},		// -
	{0x37, 0},		// .
	{0x24, SHIFT_R},	// /
	{0x27, 0},		// 0
	{0x1E, 0},		// 1
	{0x1F, 0},		// 2
	{0x20, 0},		// 3
	{0x21, 0},		// 4
	{0x22, 0},		// 5
	{0x23, 0},		// 6
	{0x24, 0},		// 7
	{0x25, 0},		// 8
	{0x26, 0},		// 9
	{0x37, SHIFT_R},	// :
	{0x36, SHIFT_R},	// ;
	{0x64, 0},		// <
	{0x27, SHIFT_R},	// =
	{0x64, SHIFT_R},	// >
	{0x2D, SHIFT_R},	// ?
	{0x14, ALT_R},		// @
	{0x04, SHIFT_R},	// A
	{0x05, SHIFT_R},	// B
	{0x06, SHIFT_R},	// C
	{0x07, SHIFT_R},	// D
	{0x08, SHIFT_R},	// E
	{0x09, SHIFT_R},	// F
	{0x0A, SHIFT_R},	// G
	{0x0B, SHIFT_R},	// H
	{0x0C, SHIFT_R},	// I
	{0x0D, SHIFT_R},	// J
	{0x0E, SHIFT_R},	// K
	{0x0F, SHIFT_R},	// L
	{0x10, SHIFT_R},	// M
	{0x11, SHIFT_R},	// N
	{0x12, SHIFT_R},	// O
	{0x13, SHIFT_R},	// P
	{0x14, SHIFT_R},	// Q
	{0x15, SHIFT_R},	// R
	{0x16, SHIFT_R},	// S
	{0x17, SHIFT_R},	// T
	{0x18, SHIFT_R},	// U
	{0x19, SHIFT_R},	// V
	{0x1A, SHIFT_R},	// W
	{0x1B, SHIFT_R},	// X
	{0x1D, SHIFT_R},	// Y
	{0x1C, SHIFT_R},	// Z
	{0x25, ALT_R},		// [
	{0x2D, ALT_R},		// \	'\\'
	{0x26, ALT_R},		// ]
	{0x35, SHIFT_R},	// ^
	{0x38, SHIFT_R},	// _
	{0x2E, 0},		// `
	{0x04, 0},		// a
	{0x05, 0},		// b
	{0x06, 0},		// c
	{0x07, 0},		// d
	{0x08, 0},		// e
	{0x09, 0},		// f
	{0x0A, 0},		// g
	{0x0B, 0},		// h
	{0x0C, 0},		// i
	{0x0D, 0},		// j
	{0x0E, 0},		// k
	{0x0F, 0},		// l
	{0x10, 0},		// m
	{0x11, 0},		// n
	{0x12, 0},		// o
	{0x13, 0},		// p
	{0x14, 0},		// q
	{0x15, 0},		// r
	{0x16, 0},		// s
	{0x17, 0},		// t
	{0x18, 0},		// u
	{0x19, 0},		// v
	{0x1A, 0},		// w
	{0x1B, 0},		// x
	{0x1D, 0},		// y
	{0x1C, 0},		// z
	{0x24, ALT_R},		// {
	{0x64, ALT_R},		// |
	{0x27, ALT_R},		// }
	{0x30, ALT_R}		// ~ (ASCII: 126)
};

static uint8_t reportBuffer[8];	/* contains the USB HID report sent to the PC */
static uint8_t idleRate = 0;	/* in 4 ms units */
static uint8_t protocolVer = 1;	/* 0 is boot protocol, 1 is report protocol */

void pgm_read_block(uint8_t *pTarget, const uint8_t *pSource, size_t len) {
	for(size_t i = 0; i < len; i++) {
		*pTarget++ = pgm_read_byte(pSource++);
	}
}

void getPassword(struct slot *slot, char *str, size_t n) {
	switch (slot->type) {
		case SLOT_STRING:
		case SLOT_GENERATE:
			memcpy(str, slot->pass.string, n);
			break;

		case SLOT_OTP:
			snprintf(str, n, "s%u", slot->pass.otp.sequence);
			break;

		case SLOT_RANDOM:
			genPassword(str, MAX_PASS_LEN/2);
			break; 

		default: /* unknown password mode */
			str[0] = '\0'; /* empty string */
			break;
	}

	str[n-1] = '\0';
}

void genPassword(char *str, size_t n) {
	uint8_t i;

	for (i = 0; i < n-1; i++) {
		str[i] = '!' + rand() % ('!' - '~'); /* all printable ASCII characters */
	}

	str[n-1] = '\0';
}

char convToMultiASCII(uint8_t in) {
	in -= '!';
	in += (in >= 67) ? 94*rand(2) : 94*rand(3);

	return in; 
}

char convFromMultiASCII(uint8_t in) {
	in = in % 94;
	in += '!';
	return in;
}

void setSlot(struct slot *slot, uint16_t index) {
	struct slot tmpSlot = *slot; // TODO workaround?
	uint8_t *slotRaw = (uint8_t *) &tmpSlot;

	/* prepare slot, on-stick password generation */
	if (slot->type == SLOT_GENERATE) {
		genPassword(tmpSlot.pass.string, MAX_PASS_LEN/2);
	}

#ifdef ENCRYPTION
	/* read key */
	uint8_t key[sizeof(struct slot)];
	pgm_read_block(key, (uint8_t *)(key_p + sizeof(struct slot) * index), sizeof(struct slot));

	/* encrypt slot */
	for (uint16_t i = 0; i < sizeof(struct slot); i++) {
		slotRaw[i] ^= key[i];
	}
#endif /* ENCRYPTION */

	/* store slot */
	eeprom_write_block(slotRaw, &slots_e[index], sizeof(struct slot));
}

void getSlot(struct slot *slot, uint16_t index) {
	/* read slot */
	uint8_t *slotRaw = (uint8_t *) slot;
	eeprom_read_block(slotRaw, &slots_e[index], sizeof(struct slot));

#ifdef ENCRYPTION
	/* read key */
	uint8_t key[sizeof(struct slot)];
	pgm_read_block(key, (uint8_t *)(key_p + sizeof(struct slot) * index), sizeof(struct slot));

	/* decrypt key */
	for (uint16_t i = 0; i < sizeof(struct slot); i++) {
		slotRaw[i] ^= key[i];
	}
#endif /* ENCRYPTION */
}

void usbReset() {
	DDRB |= (1 << PB0) | (1 << PB1); /* enable USB pins as output */
	_delay_ms(15); /* sleeping for 15ms */
	DDRB &= ~(1 << PB0) & ~(1 << PB1); /* disbale USB pins as output */
}

void hardwareInit() {
	usbReset();

	/* LEDs */
	DDRC |= (1 << PC0) | (1 << PC1); /* set LED pins as output */
	PORTC &= ~(1 << PC0) & ~(1 << PC1); /* set LEDs (active low) */

	/* push button */
	BTN_DDR &= ~(1 << BTN_BIT1);	/* set button pin as input */
	BTN_DDR |= (1 << BTN_BIT2);	/* set button pin as output */
	BTN_PORT |= (1 << BTN_BIT1);	/* enable pullup for button */
	BTN_PORT &= ~(1 << BTN_BIT2);

#ifdef WRITE_PROTECTION
	/* write protect jumper */
	WRLOCK_DDR &= ~(1 << WRLOCK_BIT);
	WRLOCK_PORT |= (1 << WRLOCK_BIT); /* enable pullup for button */
#endif /* WRITE_PROTECTION */

	TCCR0 = (1 << CS02); /* CPU prescaler = 512 */
}

void hidBuildReport(char chr) {
	for (int i = 0; i < 8; i++) {
		reportBuffer[i] = 0x0; /* clear report */
	}

	if (chr >= ' ' && chr <= '~') {
		uint16_t map = pgm_read_word(asciiKeyMap[chr - ' ']);

		reportBuffer[0] = map >> 8; /* modifier */
		reportBuffer[2] = map & 0xff; /* key */
	}
	else switch (chr) {
		case 0x8E: // Ä
			reportBuffer[0] = SHIFT_R;
		case 0x84: // ä
			reportBuffer[2] = 0x34;
			break;

		case 0x99: // Ö
			reportBuffer[0] = SHIFT_R;
		case 0x94: // ö
			reportBuffer[2] = 0x33;
			break;

		case 0x9A: // Ü
			reportBuffer[0] = SHIFT_R;
		case 0x81: // ü
			reportBuffer[2] = 0x2F;
			break;

		case '\b':
			reportBuffer[2] = 0x2A;
			break;

		case '\t':
			reportBuffer[2] = 0x2B;
			break;

		case '\n':
			reportBuffer[2] = 0x28;
			break;
	}
}

void hidSendChar(char chr) {
	while (!usbInterruptIsReady()); /* wait until previous transmission is finished */

	hidBuildReport(chr); /* build HID report buffer for one character */
	usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
}

void hidSendString(char *str) {
	while (*str) {
		hidSendChar(*str); /* send character string one by one */
		hidSendChar(0); /* release any keys */
		str++;
	}
}

usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq) {
	usbMsgLen_t len = 0;
	usbMsgPtr = NULL;

	/* 3 is the type of string descriptor, in this case the device serial number */
	if (rq->wValue.bytes[1] == USBDESCR_STRING && rq->wValue.bytes[0] == 3) {
		/* read serial number from eeprom */
		eeprom_read_block(descriptorStringSerial, descriptorStringSerial_e, sizeof(descriptorStringSerial));

		usbMsgPtr = (uint8_t *) descriptorStringSerial;
		len = *((uint8_t *) descriptorStringSerial);
	}

	return len;
}

uint8_t usbFunctionSetup(uint8_t data[8]) {
	usbRequest_t *rq = (void *)data;
	usbMsgPtr = reportBuffer;

	if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
		switch (rq->bRequest) {
			case USBRQ_HID_GET_IDLE:
				usbMsgPtr = &idleRate;
				return 1;

			case USBRQ_HID_SET_IDLE:
				idleRate = rq->wValue.bytes[1];
				return 0;

			case USBRQ_HID_GET_REPORT:
				hidBuildReport(0);
				return sizeof(reportBuffer);

			case USBRQ_HID_SET_REPORT:
				if (rq->wLength.word == 1) {
					status.mode = EXPECT_REPORT;
					return USB_NO_MSG;
				}
				return 0;

			case USBRQ_HID_SET_PROTOCOL:
				if (rq->wValue.bytes[1] < 1) {
					protocolVer = rq->wValue.bytes[1];
				}
				return 0;

			case USBRQ_HID_GET_PROTOCOL:
				usbMsgPtr = &protocolVer;
				return 1;
		}
	}
	else if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_VENDOR) {
		switch (rq->bRequest) {
			case USBRQ_VENDOR_SET_SERIAL:
				if (writable()) {
					status.mode = EXPECT_SERIAL;
					bytesRemaining = rq->wLength.word & 0xff; /* string length */
					bytesOffset = 1; /* first index is reserved for USB_STRING_DESCRIPTOR_HEADER() */

					if (bytesRemaining > MAX_SERIAL_LEN) {
						status.error = ERROR_OVERFLOW;
						return 0;
					}

					PORTC &= ~(1 << PC1); /* set LED */
					return USB_NO_MSG; /* use usbFunctionWrite() */
				}
				else {
					status.error = ERROR_LOCKED;
					return 0;
				}

			case USBRQ_VENDOR_SET_SLOT:
				if (writable() || !WRITE_PROTECTION) {
					status.mode = EXPECT_SLOT;
					updateSlotIndex = rq->wIndex.word;
					bytesRemaining = sizeof(struct slot);
					bytesOffset = 0;

					if (updateSlotIndex >= MAX_SLOTS) {
						status.error = ERROR_INVALID_SLOT;
						return 0;
					}

					PORTC &= ~(1 << PC1); /* set LED */
					return USB_NO_MSG; /* use usbFunctionWrite() */
				}
				else {
					status.error = ERROR_LOCKED;
					return 0;
				}

			case USBRQ_VENDOR_GET_STATUS: {
				status.writable = 0;
				if (WRITE_PROTECTION) {
					status.writable = (writable()) ? 1 : 2;
				}

				usbMsgPtr = (uint8_t *) &status;
				return sizeof(status);
			}

			case USBRQ_VENDOR_RESET:
				if (!writable() && WRITE_PROTECTION) {
					status.error = ERROR_LOCKED;
				}

				eeprom_write_word(&counter_e, 0);
				usbReset(); // TODO check

				return 0;
		}
	}

	return 0;
}

uint8_t usbFunctionWrite(uint8_t *data, uint8_t len) {
	if(len > bytesRemaining) { /* if this is the last incomplete chunk */
		len = bytesRemaining; /* limit to the amount we can store */
	}
	bytesRemaining -= len;

	for(uint8_t i = 0; i < len; i++) {
		switch (status.mode) {
			case EXPECT_SLOT:
				((uint8_t *) &updateSlot)[bytesOffset++] = data[i];
				break;

			case EXPECT_SERIAL:
				descriptorStringSerial[bytesOffset++] = data[i];
				break;

			case EXPECT_REPORT: // TODO
			default: break;
		}
	}

	if (bytesRemaining == 0) { /* store slot into eeprom when completed */
		switch (status.mode) {
			case EXPECT_SLOT:
				setSlot(&updateSlot, updateSlotIndex);
				break;

			case EXPECT_SERIAL:
				descriptorStringSerial[0] = USB_STRING_DESCRIPTOR_HEADER(bytesOffset-1);
				eeprom_write_block(descriptorStringSerial, descriptorStringSerial_e, sizeof(descriptorStringSerial));
				break;

			case EXPECT_REPORT: // TODO
			default: break;
		}

		currentSlotIndex = 0; /* reset slot */
		status.mode = IDLE;
		status.error = ERROR_SUCCESS;
		PORTC |= (1 << PC1); /* clear LED */

		return 1;
	}

	return 0;
}

int main(void) {
	uint8_t updateNeeded = 0;
	uint8_t idleCounter = 0;

	uint8_t len = 0;
	uint8_t debounce = 0;
	uint16_t pressed = 0;

	status.counter = eeprom_read_word(&counter_e);

	status.maxSlots = MAX_SLOTS;
	status.maxUserLen = MAX_USER_LEN;
	status.maxPassLen = MAX_PASS_LEN;
	status.maxSerialLen = MAX_SERIAL_LEN;

	status.mode = IDLE;
	status.error = ERROR_SUCCESS;

	/* Initialize EEPROM */
	if (status.counter == 0) {
		struct slot empty = { SLOT_EMPTY };
		struct slot slots[] = {
			{ SLOT_STRING,	0, '\t', "HIDeKey v1.0", { .string = "" } },
			{ SLOT_RANDOM,	0, '\t', "demo1_random" },
			{ SLOT_GENERATE,0, '\t', "demo2_random_gen" },
			{ SLOT_STRING,	0, '\t', "demo3_string", { .string = "constant" } },
			{ SLOT_OTP,	0, '\t', "demo4_otp", { .otp = { .seed = {1, 2, 3, 4, 5, 6, 7, 8}, .sequence = 0 } } }
		};

		uint16_t i;
		for (i = 0; i < (sizeof(slots) / sizeof(struct slot)); i++) {
			setSlot(&slots[i], i);
		}

		for (; i < MAX_SLOTS; i++) {
			setSlot(&empty, i);
		}
	}

	wdt_enable(WDTO_2S); /* enable watchdog timer 2s */
	srand((status.counter * 256) % RAND_MAX); /* crypto initialization */

	/* increment and store startup counter */
	status.counter++;
	eeprom_write_word(&counter_e, status.counter);

	usbInit();	/* initialize USB stack processing */
	hardwareInit();	/* setup io & starts timer */
	sei();		/* enable global interrupts */

	PORTC |= (1 << PC1); /* disable startup LED */

	/* main loop */
	for (;;) {
		wdt_reset(); /* reset the watchdog */
		usbPoll(); /* poll the USB stack */

		if (TIFR & (1 << TOV0)) { /* timer0 overflow? */
			TIFR = 1 << TOV0; /* reset timer overflow flag */

			if (buttonPressed()) pressed++;
			else if (debounce > 0) debounce--;
			else if (pressed > 0) {
				if (pressed < BTN_LONG_PRESS) {
					do { /* find next non-empty slot */
						currentSlotIndex %= MAX_SLOTS; /* avoid overflow */
						getSlot(&currentSlot, currentSlotIndex);
						currentSlotIndex++; /* increment slot */
					} while (currentSlot.type == SLOT_EMPTY); /* skip empty slots */
				}

				if (currentSlot.options & SKIP_USER || pressed > BTN_LONG_PRESS) {
					char pass[MAX_PASS_LEN];
					getPassword(&currentSlot, pass, MAX_PASS_LEN);

					if (~currentSlot.options & SKIP_DELIMITER) {
						if (currentSlot.delimiter) {
							hidSendChar(currentSlot.delimiter);
							hidSendChar(0);
						}
					}

					hidSendString(pass);

					if (~currentSlot.options & SKIP_RETURN) {
						hidSendChar('\n');
						hidSendChar(0);
					}

					if (currentSlot.type == SLOT_OTP) {
						currentSlot.pass.otp.sequence++;
						setSlot(&currentSlot, currentSlotIndex - 1);
					}

					if (currentSlot.options & SKIP_RESET) {
						currentSlotIndex = 0;
					}

					len = 0;
				}
				else {
					/* delete old username */
					for (uint8_t i = 0; i < len; i++) {
						hidSendChar('\b');
						hidSendChar(0);
					}

					/* write new username */
					hidSendString(currentSlot.user);
					len = strlen(currentSlot.user);
				}

				pressed = 0;
				debounce = 10;
			}

			if (idleRate != 0) { /* do we need periodic reports? */
				if (idleCounter > 3) { /* yes, but not yet */
					idleCounter -= 3;
				} else { /* yes, it is time now */
					updateNeeded = 1;
					idleCounter = idleRate;
				}
			}
		}


		/* if an update is needed, send the report */
		if(updateNeeded && usbInterruptIsReady()) {
			updateNeeded = 0;
			usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
		}
	}

	return 0;
}

