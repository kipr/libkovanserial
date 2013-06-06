#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SHA
#include <boost/test/unit_test.hpp>

#include <kovanserial/sha1.hpp>

BOOST_AUTO_TEST_CASE(abc)
{
	SHA1 abc;
	abc.update((const uint8_t *)"abc", 3);

	uint8_t computedDigest[20];
	abc.final(computedDigest);

	const static uint8_t correctDigest[20] = {
		0xA9, 0x99, 0x3E, 0x36, 0x47,
		0x06, 0x81, 0x6A, 0xBA, 0x3E,
		0x25, 0x71, 0x78, 0x50, 0xC2,
		0x6C, 0x9C, 0xD0, 0xD8, 0x9D
	};

	BOOST_CHECK(memcmp(computedDigest, correctDigest, 20) == 0);
}

BOOST_AUTO_TEST_CASE(million_as)
{
	SHA1 sha;
	const uint8_t a = (uint8_t)'a';
	for(uint64_t i = 0; i < 1000000; ++i) sha.update(&a, 1);

	uint8_t computedDigest[20];
	sha.final(computedDigest);

	const static uint8_t correctDigest[20] = {
		0x34, 0xAA, 0x97, 0x3C, 0xD4,
		0xC4, 0xDA, 0xA4, 0xF6, 0x1E,
		0xEB, 0x2B, 0xDB, 0xAD, 0x27,
		0x31, 0x65, 0x34, 0x01, 0x6F
	};

	BOOST_CHECK(memcmp(computedDigest, correctDigest, 20) == 0);
}