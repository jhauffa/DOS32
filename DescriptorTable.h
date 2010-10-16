
#ifndef __DOS32_DESCRIPTOR_TABLE_H__
#define __DOS32_DESCRIPTOR_TABLE_H__

#include <stdint.h>
#include <map>


enum DescriptorType {
	DESC_TYPE_OS,		// provided by OS
	DESC_TYPE_LDT,		// in Local Descriptor Table
	DESC_TYPE_ALIAS		// emulated in exception handler
};

class Descriptor
{
	public:
		Descriptor( uint16_t sel );  // OS descriptor
		Descriptor( uint32_t base, uint32_t limit );  // LDT descriptor
		Descriptor( uint16_t sel, uint16_t aliasSel );  // alias descriptor
		~Descriptor();

		DescriptorType getType() const;
		uint32_t getLimit() const;
		uint32_t getBase() const;
		uint16_t getSel() const;
		uint16_t getAliasSel() const;

		bool setLimit( uint32_t limit );
		bool setAliasSel( uint16_t aliasSel );

	private:
		DescriptorType mType;
		uint32_t mBase;
		uint32_t mLimit;
		uint16_t mSel;
		uint16_t mAliasSel;
};

class DescriptorTable
{
	public:
		DescriptorTable();
		~DescriptorTable();

		Descriptor *getDesc( uint16_t sel, bool resolveAlias );

		void allocOsDesc( uint16_t sel );
		void allocLdtDesc( uint32_t base, uint32_t limit, uint16_t &sel );
		void allocAliasDesc( uint16_t sel, uint16_t aliasSel );

		uint16_t getOsCodeSel() const;
		uint16_t getOsDataSel() const;

	private:
		std::map<uint16_t, Descriptor *> mDesc;
		uint16_t mOsCodeSel;
		uint16_t mOsDataSel;

		bool setDesc( uint16_t sel, Descriptor *desc );
};


#endif  // __DOS32_DESCRIPTOR_TABLE_H__
