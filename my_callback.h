#pragma once
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#define BYTECODE_LEN 128

/* Ugly code - You've been warned.*/
#define CallClassFunc(ClassType, FuncName, ClassPtr, argnum, ...)\
{																	\
	void (ClassType::*cast_helper)() = &ClassType::FuncName;	    \
	uint32_t cast_final;												\
	memcpy(&cast_final, &cast_helper, 4);							\
	CallFunc(cast_final, (argnum+1), ClassPtr, __VA_ARGS__);		\
}


/* Best practice would be to fill bytecode_struct -> bytes with 0xCC, so if something went wrong, exception would be thrown */
struct bytecode_struct
{
	uint32_t counter = 0;
	uint8_t bytes[BYTECODE_LEN] = { 0 };
};

void CallFunc(uint32_t func_addr, uint32_t arg_num, ...);
