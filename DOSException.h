
#ifndef __DOS32_DOS_EXCEPTION_H__
#define __DOS32_DOS_EXCEPTION_H__

#include "Exception.h"

#include <cstdint>


class DOSException
{
	public:
		DOSException( uint8_t errorCode );
		virtual ~DOSException() {}

		std::string getErrorMessage() const;
		uint8_t getErrorCode() const;
		uint8_t getErrorClass() const;
		uint8_t getRecommendedAction() const;
		uint8_t getErrorLocus() const;

		enum ErrorCode
		{
			ERROR_NO_ERROR = 0x00,
			ERROR_FUNCTION_NOT_SUPPORTED = 0x01,
			ERROR_PATH_NOT_FOUND = 0x03,
			ERROR_OUT_OF_HANDLES = 0x04,
			ERROR_INVALID_HANDLE = 0x06,
			ERROR_INVALID_DRIVE = 0x0F
		};

		enum ErrorClass
		{
			CLASS_UNKNOWN = 0x0D
		};

		enum RecommendedAction
		{
			ACTION_CLEANUP_ABORT = 0x04
		};

		enum ErrorLocus
		{
			LOCUS_UNKNOWN = 0x01
		};

	private:
		uint8_t mErrorCode;
		uint8_t mErrorClass;
		uint8_t mRecommendedAction;
		uint8_t mErrorLocus;
};


#endif	// __DOS32_DOS_EXCEPTION_H__
