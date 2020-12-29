
#ifndef __DOS32_DESCRIPTOR_TABLE_H__
#define __DOS32_DESCRIPTOR_TABLE_H__

#include <cstdint>
#include <map>


namespace host {
class LDT;
}

enum DescriptorType {
	DESC_TYPE_OS,		// provided by OS
	DESC_TYPE_LDT		// in Local Descriptor Table
};

class Descriptor
{
	public:
		Descriptor( uint16_t sel );  // OS descriptor
		Descriptor( host::LDT *ldt, uint32_t base, uint32_t limit );  // LDT descriptor
		~Descriptor();

		DescriptorType getType() const;
		uint32_t getLimit() const;
		uint32_t getBase() const;
		uint16_t getSel() const;

		bool setLimit( uint32_t limit );

	private:
		DescriptorType mType;
		uint32_t mBase;
		uint32_t mLimit;
		uint16_t mSel;
		host::LDT *mLDT;
};

class DescriptorTable
{
	public:
		DescriptorTable();
		~DescriptorTable();

		Descriptor *getDesc( uint16_t sel );

		void allocOSDesc( uint16_t sel );
		void allocLDTDesc( uint32_t base, uint32_t limit, uint16_t &sel );

		uint16_t getOSCodeSel() const;
		uint16_t getOSDataSel() const;

	private:
		host::LDT *mLDT;
		std::map<uint16_t, Descriptor *> mDesc;
		uint16_t mOSCodeSel;
		uint16_t mOSDataSel;

		bool setDesc( uint16_t sel, Descriptor *desc );
};


#endif  // __DOS32_DESCRIPTOR_TABLE_H__
