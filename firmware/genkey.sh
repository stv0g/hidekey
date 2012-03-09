#!/bin/sh

KEYLEN=$1

#header
cat <<END
#ifndef _KEY_H_
#define _KEY_H_

/* this file is automatical generated and should NOT be edited by hand! */

/* 128bit key for encryption */
const uint8_t key_p[512] PROGMEM = {
END

# random key generation
printf "0x%02x" $(($RANDOM % 256))
for ((c = 1; c < KEYLEN; c++))
do
	printf ", 0x%02x" $(($RANDOM % 256))
done

# footer
cat <<END
};

#endif /* _KEY_H_ */
END
