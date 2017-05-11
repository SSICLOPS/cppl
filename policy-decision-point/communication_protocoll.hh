#pragma once

#include <stdint.h>


typedef uint16_t package_type_t;
typedef uint16_t data_len_t;
typedef uint16_t policy_len_t;

enum PackageHeaderFieldLen{
	TYPE = sizeof(package_type_t),
	DATA_LEN = sizeof(data_len_t),
	POLICY_LEN = sizeof(policy_len_t),
	COMMON_HEADER_LEN = TYPE + DATA_LEN,
	POLICY_ENABLED_DATA_HEADER_LEN = COMMON_HEADER_LEN + POLICY_LEN
};

enum PackageType{
	NODE_PARAM = 1,
	POLICY_ENABLED_DATA = 2
};
