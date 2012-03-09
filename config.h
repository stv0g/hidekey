
#ifndef _CONFIG_H_
#define _CONFIG_H_

/* START of configuration */

#define DEBUG_LEVEL		0
#define ENCRYPTION		1
#define WRITE_PROTECTION	1
#define BTN_LONG_PRESS		40

/* slot types */
#define SLOT_EMPTY		0 /* an empty slot */
#define SLOT_STRING		1 /* a stored password as string */
#define SLOT_OTP		2 /* a stored sequence and seed for one time passwords */
#define SLOT_RANDOM		3 /* password gets generated each time */
#define SLOT_GENERATE 		4 /* password will be generated on key during configuration */

/* the most significant bits of slot.type have a special meaning */
#define SKIP_USER		(1 << 0) /* just print the password */
#define SKIP_RETURN		(1 << 1) /* omit trailing newline after password */
#define SKIP_DELIMITER		(1 << 2) /* omit delimiter between username and password */
#define SKIP_RESET		(1 << 3) /* do not return to first slot */

#ifndef E2END
#define E2END 511 /* for ATMega8 */
#endif

#define MAX_SLOTS		((E2END+1) / sizeof(struct slot))
#define MAX_USER_LEN		24
#define MAX_PASS_LEN		23
#define MAX_SERIAL_LEN		4

/* USB vendor requests */
#define USBRQ_VENDOR_GET_STATUS	1
#define USBRQ_VENDOR_SET_SLOT	2
#define USBRQ_VENDOR_SET_SERIAL	3
#define USBRQ_VENDOR_RESET	4

#define DEVICE_VID		0x16c0
#define DEVICE_PID		0x05dc

/* END of configuration */

/**
 * A slot stores one user/password combination in EEPROM
 */
struct slot {
	char type;
	char options;
	char delimiter; /* \0 is also possible */
	char user[MAX_USER_LEN];
	union {
		char string[MAX_PASS_LEN];
		struct {
			char seed[8];
			uint16_t sequence;
		} otp;
	} pass;
} __attribute__ ((packed));

enum error {
	ERROR_SUCCESS,
	ERROR_LOCKED,
	ERROR_OVERFLOW,
	ERROR_INVALID_SLOT,
	ERROR_UNKNOW
};

enum mode {
	IDLE,
	EXPECT_SLOT,
	EXPECT_SERIAL,
	EXPECT_REPORT
};

struct status {
	uint16_t counter; /* startups since last reset/flash */
	uint16_t maxSlots;
	uint16_t maxPassLen;
	uint16_t maxUserLen;
	uint16_t maxSerialLen;

	uint8_t error;
	uint8_t mode;
	uint8_t writable;
} __attribute__ ((packed));

#endif /* _CONFIG_H_ */
