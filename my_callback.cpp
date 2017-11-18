#include <windows.h>
#include "my_callback.h"

void generate_bytecode_call_eax(bytecode_struct* bytecode, uint32_t func_addr) //Uses register eax.
{
#ifdef DEBUG
	printf("\tcall_eax addr: 0x%x\n", func_addr);
#endif
	uint8_t tramp[7] = { 0xB8, 0xD3, 0xC0, 0xAD, 0xDE, // mov eax, addr
		0xFF, 0xD0 // call eax
	};
	memcpy(tramp + 1, &func_addr, 4);
	memcpy(&bytecode->bytes[bytecode->counter], &tramp, 7);
	bytecode->counter += 7;
}
void generate_bytecode_push32(bytecode_struct* bytecode, uint32_t var_addr)
{
#ifdef DEBUG
	printf("\tpush32 0x%x \n", var_addr);
#endif
	uint8_t tramp[5] = { 0x68, 0xDE, 0xC0,0xAD, 0xD3, // push 0xD3ADC0DE
	};
	memcpy(tramp + 1, &var_addr, 4);

	memcpy(&bytecode->bytes[bytecode->counter], &tramp, 5);
	bytecode->counter += 5;
}
// Do i need it in the end?!
void generate_bytecode_push8(bytecode_struct* bytecode, uint32_t var_addr)
{
#ifdef DEBUG
	printf("\tpush8 0x%.2x \n", var_addr);
#endif
	uint8_t tramp[2] = { 0x6A, 0xCC, // push 0xCC
	};
	memcpy(tramp + 1, &var_addr, 1);

	memcpy(&bytecode->bytes[bytecode->counter], &tramp, 2);
	bytecode->counter += 2;
}
void generate_bytecode_pop(bytecode_struct* bytecode, uint8_t n)
{
#ifdef DEBUG
	printf("\tpop%d - not saving the val\n", n * 8);
#endif
	uint8_t tramp[3] = { 0x83, 0xC4,0x04 // add esp, 4
	};
	memcpy(tramp + 2, &n, 1);
	memcpy(&bytecode->bytes[bytecode->counter], &tramp, 3);
	bytecode->counter += 3;
}
void generate_bytecode_byte(bytecode_struct* bytecode, uint8_t byte)
{
	bytecode->bytes[bytecode->counter++] = byte;
}
void CallFunc(uint32_t func_addr, uint32_t arg_num, ...)
{
#ifdef DEBUG
	printf("CREATE:\n");
#endif
	bytecode_struct* bytecode = new bytecode_struct;
	/* Create  byte-code*/
	/*
	0x60, 0x61 = pushad, popad were added to make sure, that no registers were changed.
	It is a practice, that was used by me back in the times => reduces problems if the instructions were depanding on registers, which shall not happen.
	0xC3 = return - since we are essencially creating a function, it needs to return to its original flow.
	There are pushad and popad in the before the main call in the static call, so therefore, these shall not be needed.

	Pushes and pops.
	Pushes have their own functions, so it could take less space in the bytecode => instructions that take less bytes are used.
	Pop's => There are adding bytes to the stack, therefore there is no possibility to retrieve the value (return type of callback must be void)
	Pop's => Even through one byte is pushed, due to x86 architecture, it takes 4 bytes on the stack. Therefore, 4 must be removed.
	*/
	// fucntion header
	//TODO: remove pushad?


	generate_bytecode_byte(bytecode, 0x60); // ASM: pushad
	va_list vl;
	va_start(vl, arg_num);
	for (uint32_t i = 0; i<arg_num; i++)
		generate_bytecode_push32(bytecode, va_arg(vl, uint32_t));
	generate_bytecode_call_eax(bytecode, (uint32_t)func_addr);
	generate_bytecode_pop(bytecode, 4 * arg_num);

	va_end(vl);
	generate_bytecode_byte(bytecode, 0x61); // ASM: popad
	generate_bytecode_byte(bytecode, 0xC3); // ASM: return 

											//Convert the std::string bytecode, to some C-level type, without classes nor wrappers
											//There is an option, to access bytecode->bytes directly from asm, byt im lazy.
											//TODO: Access bytecode->bytes directly from ASM.
	uint8_t* tramp = bytecode->bytes;

	//DEBUG DISPLAY FOR TROUBLESHOOTING THE BYTECODE
#ifdef DEBUG
	printf("BYTECODE:");
	for (int i = 0; i < 128; i++)
	{
		if (i % 16 == 0)
			printf("\n");
		if (i % 8 == 0)
			printf("\t");
		printf("%.2x ", tramp[i]);
	}
	printf("\n");
	printf("EXECUTE:\n");
#endif
	// Change the memory-page attributes, so it can be executed
	DWORD old;
	VirtualProtect((LPVOID)tramp,
		BYTECODE_LEN, //It will change it for the whole page anyways.
		PAGE_EXECUTE_READWRITE,
		&old);
	__asm {
		pushad // push all the registers
		mov eax, tramp		//Weird calling convenction
		call eax				//Weird calling convenction
		popad //pop all the registes
	};
	//Restore the original Page attributes.
	VirtualProtect((LPVOID)tramp,
		BYTECODE_LEN, //It will change it for the whole page anyways
		old,
		&old);
	delete bytecode;
}