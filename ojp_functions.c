#include <stdint.h>
#include <string.h>
#include <stdio.h>

uint16_t ojp_ntohs(uint16_t val)
{
	uint8_t bytes[2] = { 0 };
	memcpy(&bytes, &val, 2);
	return ((uint16_t) bytes[1] << 0) | ((uint16_t) bytes[0] << 8);
}



uint32_t ojp_ntohl(uint32_t val)
{
	uint8_t bytes[4] = { 0 };
	memcpy(&bytes, &val, 4);
	return ((uint32_t) bytes[3] << 0) | ((uint32_t) bytes[2] << 8) | ((uint32_t) bytes[1] << 16) | ((uint32_t) bytes[0] << 24);
}



uint16_t ojp_htons(uint16_t val)
{
	uint8_t bytes[2] = { 0 };
	memcpy(&bytes, &val, 2);
	return ((uint16_t) bytes[1] << 0) | ((uint16_t) bytes[0] << 8);
}



uint32_t ojp_htonl(uint32_t val)
{
	uint8_t bytes[4] = { 0 };
	memcpy(&bytes, &val, 4);
	return ((uint32_t) bytes[3] << 0) | ((uint32_t) bytes[2] << 8) | ((uint32_t) bytes[1] << 16) | ((uint32_t) bytes[0] << 24);
}

/*
 * bitOffset % 8
 */
uint8_t get_local_offset(uint32_t bitOffset)
{
    uint32_t x;
    uint32_t m;
    x = (bitOffset >> 3);
    m = x << 3;
    /* equivalent ot x/8 * 8. Checks to see if x is multiple of 8 */
    if( m == bitOffset)
    {
        return 0;
    }
    else
    {
        return bitOffset - m;
    }
}


/* Inserts val, bit size of bitSize, at bit offset bitOffset, in buffer, as big endian.
 * Returns new bitOffset = bitSize + bitOffset
 * be = big endian. Insert as left shifted big endian.
 */
uint32_t insert_l_be_shifted(uint8_t * bufferOut, uint32_t bitOffset, uint32_t bitSize, uint32_t val)
{
    int ii;
    uint32_t jj;
    uint32_t o; /* offset from the right */
    uint8_t arrayval[sizeof(val)];
    uint8_t result[sizeof(val) + 1];
    uint8_t sl; /* Shift from left */

    uint32_t bytes;
    val = ojp_ntohl(val);

    memset(result, 0, sizeof(val) + 1);
    memcpy(arrayval, &val, sizeof(val)); /* We are shifting to left. result[0] will be empty */

    /* Either align beginning or end. Easier to align end when shifting right */
    sl = get_local_offset(bitOffset + bitSize);

    /* Copy from LSB to MSB */
    result[sizeof(val)] = arrayval[sizeof(val)-1] << (8 - sl);

    /* Copy all remaining but last one */
    for(ii = sizeof(val) - 1; ii >= 1; --ii)
    {
        /* remember endianess. 0 is the MSB */
        result[ii] = (arrayval[ii] >> sl) | (arrayval[ii - 1] << (8-sl));
    }
    result[0] = arrayval[0] >> sl;

    jj = 0;
    o = bitOffset >> 3;
    /* Find the # of meaningful bytes */
    bytes = ((bitSize + (7 - sl)) >> 3) + 1; /* Position of left most bit / 8*/

    /* Copy the meaningful bytes. Result is sizeof(val) + 1, sizeof(val) is last index */
    for(ii = sizeof(val) + 1 - bytes; ii < sizeof(val) + 1; ++ii)
    {
        bufferOut[ o + jj] |= result[ii];
        ++jj;
    }
    return bitOffset + bitSize;
}

/* Extract a uint32_t value starting from buffer[0] offset s from left(MSB). Assumed to be big endian */
uint32_t extract_l_shifted(uint8_t *buffer, uint32_t bitOffset, int bitSize)
{
    int ii;
    uint32_t dummy;
    uint8_t arrayval[sizeof(dummy) + 1];
    uint32_t o;
    uint32_t len;
    uint32_t retval;
    uint8_t sl;


    if(bitOffset < 0 || bitSize < 0)
    {
        printf("Warning: shift_array_l used incorrectly. Leaving function\n");
        return 0;
    }
    memset(arrayval, 0, sizeof(dummy) + 1);
    o = bitOffset >> 3; /* Location in buffer (first bit) */
    len = (bitOffset + (bitSize)) >> 3; /* Location of last bit in buffer */
    memcpy(arrayval, &buffer[o], len - o + 1);

    sl = get_local_offset(bitOffset); /* Location of bit in a byte. We will shift 8 - that*/
    for (ii = 0; ii < sizeof(dummy); ++ii) /* ignore last index. It will be 0 anyway. Shift array right */
    {
        arrayval[ii] = (arrayval[ii] << sl) | (arrayval[ii + 1] >> (8 - sl));
    }
    /* Generate mask for left side (MSB) */
    memcpy(&retval, arrayval, sizeof(dummy));
    retval = ojp_htonl(retval); /* Convert to MSB.  Earlier during memcpy, leftmost of buffer was copied to array[0] */

    /* Since this is aligned at the left most bit, we can simply shift right by 32 - bitsize. */
    retval = retval >> (32 - bitSize);
    return retval;

}
