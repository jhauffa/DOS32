
#ifndef __DOS32_REGISTERS_H__
#define __DOS32_REGISTERS_H__

enum Register {
	REG_EAX = 0, REG_ECX = 1, REG_EDX = 2, REG_EBX = 3, REG_ESP = 4, REG_EBP = 5,
	REG_ESI = 6, REG_EDI = 7
};

enum SegmentRegister {
	SEG_ES = 0, SEG_CS = 1, SEG_SS = 2, SEG_DS = 3, SEG_FS = 4, SEG_GS = 5, SEG_NONE = 6
};

#endif  // __DOS32_REGISTERS_H__
