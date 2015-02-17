/*
* Copyright(C) NXP Semiconductors, 2011
* All rights reserved.
*
* Copyright (C) Dean Camera, 2011.
*
* LUFA Library is licensed from Dean Camera by NXP for NXP customers 
* for use with NXP's LPC microcontrollers.
*
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* LPC products.  This software is supplied "AS IS" without any warranties of
* any kind, and NXP Semiconductors and its licensor disclaim any and 
* all warranties, express or implied, including all implied warranties of 
* merchantability, fitness for a particular purpose and non-infringement of 
* intellectual property rights.  NXP Semiconductors assumes no responsibility
* or liability for the use of the software, conveys no license or rights under any
* patent, copyright, mask work right, or any other intellectual property rights in 
* or to any products. NXP Semiconductors reserves the right to make changes
* in the software without notification. NXP Semiconductors also makes no 
* representation or warranty that such application will be suitable for the
* specified use without further testing or modification.
* 
* Permission to use, copy, modify, and distribute this software and its 
* documentation is hereby granted, under NXP Semiconductors' and its 
* licensor's relevant copyrights in the software, without fee, provided that it 
* is used in conjunction with NXP Semiconductors microcontrollers.  This 
* copyright, permission, and disclaimer notice must appear in all copies of 
* this code.
*/



/** \file
 *  \brief Endianness and Byte Ordering macros and functions.
 *
 *  \copydetails Group_Endianness
 */

/** \ingroup Group_Endianness
 *  \defgroup Group_ByteSwapping Byte Reordering
 *  \brief Macros and functions for forced byte reordering.
 */

/** \ingroup Group_Endianness
 *  \defgroup Group_EndianConversion Endianness Conversion
 *  \brief Macros and functions for automatic endianness conversion.
 */

/** \ingroup Group_Common
 *  \defgroup Group_Endianness Endianness and Byte Ordering
 *  \brief Convenience macros and functions relating to byte (re-)ordering
 *
 *  Common library convenience macros and functions relating to byte (re-)ordering.
 *
 *  @{
 */

#ifndef __LPCUSBlib_ENDIANNESS_H__
#define __LPCUSBlib_ENDIANNESS_H__

	/* Enable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			extern "C" {
		#endif

	/* Preprocessor Checks: */
		#if !defined(__INCLUDE_FROM_COMMON_H)
			#error Do not include this file directly. Include LPCUSBlib/Common/Common.h instead to gain this functionality.
		#endif
		
		#if !(defined(ARCH_BIG_ENDIAN) || defined(ARCH_LITTLE_ENDIAN))
			#error ARCH_BIG_ENDIAN or ARCH_LITTLE_ENDIAN not set for the specified architecture.
		#endif

	/* Public Interface - May be used in end-application: */
		/* Macros: */
			/** Swaps the byte ordering of a 16-bit value at compile-time. Do not use this macro for swapping byte orderings
			 *  of dynamic values computed at runtime, use \ref SwapEndian_16() instead. The result of this macro can be used
			 *  inside struct or other variable initializers outside of a function, something that is not possible with the
			 *  inline function variant.
			 *
			 *  \ingroup Group_ByteSwapping
			 *
			 *  \param[in] x  16-bit value whose byte ordering is to be swapped.
			 *
			 *  \return Input value with the byte ordering reversed.
			 */
			#define SWAPENDIAN_16(x)            (uint16_t)((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))

			/** Swaps the byte ordering of a 32-bit value at compile-time. Do not use this macro for swapping byte orderings
			 *  of dynamic values computed at runtime- use \ref SwapEndian_32() instead. The result of this macro can be used
			 *  inside struct or other variable initializers outside of a function, something that is not possible with the
			 *  inline function variant.
			 *
			 *  \ingroup Group_ByteSwapping
			 *
			 *  \param[in] x  32-bit value whose byte ordering is to be swapped.
			 *
			 *  \return Input value with the byte ordering reversed.
			 */
			#define SWAPENDIAN_32(x)            (uint32_t)((((x) & 0xFF000000UL) >> 24UL) | (((x) & 0x00FF0000UL) >> 8UL) | \
			                                               (((x) & 0x0000FF00UL) << 8UL)  | (((x) & 0x000000FFUL) << 24UL))

			#if defined(ARCH_BIG_ENDIAN) && !defined(le16_to_cpu)
				#define le16_to_cpu(x)           SwapEndian_16(x)
				#define le32_to_cpu(x)           SwapEndian_32(x)
				#define be16_to_cpu(x)           (x)
				#define be32_to_cpu(x)           (x)
				#define cpu_to_le16(x)           SwapEndian_16(x)
				#define cpu_to_le32(x)           SwapEndian_32(x)
				#define cpu_to_be16(x)           (x)
				#define cpu_to_be32(x)           (x)
				#define LE16_TO_CPU(x)           SWAPENDIAN_16(x)
				#define LE32_TO_CPU(x)           SWAPENDIAN_32(x)
				#define BE16_TO_CPU(x)           (x)
				#define BE32_TO_CPU(x)           (x)
				#define CPU_TO_LE16(x)           SWAPENDIAN_16(x)
				#define CPU_TO_LE32(x)           SWAPENDIAN_32(x)
				#define CPU_TO_BE16(x)           (x)
				#define CPU_TO_BE32(x)           (x)			
			#elif !defined(le16_to_cpu)
				/** \name Run-time endianness conversion */
				//@{
			
				/** Performs a conversion between a Little Endian encoded 16-bit piece of data and the
				 *  Endianness of the currently selected CPU architecture.
				 *
				 *  On little endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for run-time conversion of data - for compile-time endianness
				 *        conversion, use \ref LE16_TO_CPU instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define le16_to_cpu(x)           (x)

				/** Performs a conversion between a Little Endian encoded 32-bit piece of data and the
				 *  Endianness of the currently selected CPU architecture.
				 *
				 *  On little endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for run-time conversion of data - for compile-time endianness
				 *        conversion, use \ref LE32_TO_CPU instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define le32_to_cpu(x)           (x)

				/** Performs a conversion between a Big Endian encoded 16-bit piece of data and the
				 *  Endianness of the currently selected CPU architecture.
				 *
				 *  On big endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for run-time conversion of data - for compile-time endianness
				 *        conversion, use \ref BE16_TO_CPU instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define be16_to_cpu(x)           SwapEndian_16(x)

				/** Performs a conversion between a Big Endian encoded 32-bit piece of data and the
				 *  Endianness of the currently selected CPU architecture.
				 *
				 *  On big endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for run-time conversion of data - for compile-time endianness
				 *        conversion, use \ref BE32_TO_CPU instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define be32_to_cpu(x)           SwapEndian_32(x)

				/** Performs a conversion on a natively encoded 16-bit piece of data to ensure that it
				 *  is in Little Endian format regardless of the currently selected CPU architecture.
				 *
				 *  On little endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for run-time conversion of data - for compile-time endianness
				 *        conversion, use \ref CPU_TO_LE16 instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define cpu_to_le16(x)           (x)

				/** Performs a conversion on a natively encoded 32-bit piece of data to ensure that it
				 *  is in Little Endian format regardless of the currently selected CPU architecture.
				 *
				 *  On little endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for run-time conversion of data - for compile-time endianness
				 *        conversion, use \ref CPU_TO_LE32 instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define cpu_to_le32(x)           (x)

				/** Performs a conversion on a natively encoded 16-bit piece of data to ensure that it
				 *  is in Big Endian format regardless of the currently selected CPU architecture.
				 *
				 *  On big endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for run-time conversion of data - for compile-time endianness
				 *        conversion, use \ref CPU_TO_BE16 instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define cpu_to_be16(x)           SwapEndian_16(x)

				/** Performs a conversion on a natively encoded 32-bit piece of data to ensure that it
				 *  is in Big Endian format regardless of the currently selected CPU architecture.
				 *
				 *  On big endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for run-time conversion of data - for compile-time endianness
				 *        conversion, use \ref CPU_TO_BE32 instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define cpu_to_be32(x)           SwapEndian_32(x)

				//@}

				/** \name Compile-time endianness conversion */
				//@{

				/** Performs a conversion between a Little Endian encoded 16-bit piece of data and the
				 *  Endianness of the currently selected CPU architecture.
				 *
				 *  On little endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for compile-time conversion of data - for run time endianness
				 *        conversion, use \ref le16_to_cpu instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define LE16_TO_CPU(x)           (x)

				/** Performs a conversion between a Little Endian encoded 32-bit piece of data and the
				 *  Endianness of the currently selected CPU architecture.
				 *
				 *  On little endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for compile-time conversion of data - for run time endianness
				 *        conversion, use \ref le32_to_cpu instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define LE32_TO_CPU(x)           (x)

				/** Performs a conversion between a Big Endian encoded 16-bit piece of data and the
				 *  Endianness of the currently selected CPU architecture.
				 *
				 *  On big endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for compile-time conversion of data - for run-time endianness
				 *        conversion, use \ref be16_to_cpu instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define BE16_TO_CPU(x)           SWAPENDIAN_16(x)

				/** Performs a conversion between a Big Endian encoded 32-bit piece of data and the
				 *  Endianness of the currently selected CPU architecture.
				 *
				 *  On big endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for compile-time conversion of data - for run-time endianness
				 *        conversion, use \ref be32_to_cpu instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define BE32_TO_CPU(x)           SWAPENDIAN_32(x)

				/** Performs a conversion on a natively encoded 16-bit piece of data to ensure that it
				 *  is in Little Endian format regardless of the currently selected CPU architecture.
				 *
				 *  On little endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for compile-time conversion of data - for run-time endianness
				 *        conversion, use \ref cpu_to_le16 instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define CPU_TO_LE16(x)           (x)

				/** Performs a conversion on a natively encoded 32-bit piece of data to ensure that it
				 *  is in Little Endian format regardless of the currently selected CPU architecture.
				 *
				 *  On little endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for compile-time conversion of data - for run-time endianness
				 *        conversion, use \ref cpu_to_le32 instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define CPU_TO_LE32(x)           (x)

				/** Performs a conversion on a natively encoded 16-bit piece of data to ensure that it
				 *  is in Big Endian format regardless of the currently selected CPU architecture.
				 *
				 *  On big endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for compile-time conversion of data - for run-time endianness
				 *        conversion, use \ref cpu_to_be16 instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define CPU_TO_BE16(x)           SWAPENDIAN_16(x)

				/** Performs a conversion on a natively encoded 32-bit piece of data to ensure that it
				 *  is in Big Endian format regardless of the currently selected CPU architecture.
				 *
				 *  On big endian architectures, this macro does nothing.
				 *
				 *  \note This macro is designed for compile-time conversion of data - for run-time endianness
				 *        conversion, use \ref cpu_to_be32 instead.
				 *
				 *  \ingroup Group_EndianConversion
				 *
				 *  \param[in] x  Data to perform the endianness conversion on.
				 *
				 *  \return Endian corrected version of the input value.
				 */
				#define CPU_TO_BE32(x)           SWAPENDIAN_32(x)

				//! @}
			#endif

		/* Inline Functions: */
			/** Function to reverse the byte ordering of the individual bytes in a 16 bit value.
			 *
			 *  \ingroup Group_ByteSwapping
			 *
			 *  \param[in] Word  Word of data whose bytes are to be swapped.
			 */
			static inline uint16_t SwapEndian_16(const uint16_t Word) ATTR_WARN_UNUSED_RESULT ATTR_CONST;
			static inline uint16_t SwapEndian_16(const uint16_t Word)
			{
				if (GCC_IS_COMPILE_CONST(Word))
				  return SWAPENDIAN_16(Word);
			
				uint8_t Temp;

				union
				{
					uint16_t Word;
					uint8_t  Bytes[2];
				} Data;

				Data.Word = Word;

				Temp = Data.Bytes[0];
				Data.Bytes[0] = Data.Bytes[1];
				Data.Bytes[1] = Temp;

				return Data.Word;
			}

			/** Function to reverse the byte ordering of the individual bytes in a 32 bit value.
			 *
			 *  \ingroup Group_ByteSwapping
			 *
			 *  \param[in] DWord  Double word of data whose bytes are to be swapped.
			 */
			static inline uint32_t SwapEndian_32(const uint32_t DWord) ATTR_WARN_UNUSED_RESULT ATTR_CONST;
			static inline uint32_t SwapEndian_32(const uint32_t DWord)
			{
				if (GCC_IS_COMPILE_CONST(DWord))
				  return SWAPENDIAN_32(DWord);

				uint8_t Temp;

				union
				{
					uint32_t DWord;
					uint8_t  Bytes[4];
				} Data;

				Data.DWord = DWord;

				Temp = Data.Bytes[0];
				Data.Bytes[0] = Data.Bytes[3];
				Data.Bytes[3] = Temp;

				Temp = Data.Bytes[1];
				Data.Bytes[1] = Data.Bytes[2];
				Data.Bytes[2] = Temp;

				return Data.DWord;
			}

			/** Function to reverse the byte ordering of the individual bytes in a n byte value.
			 *
			 *  \ingroup Group_ByteSwapping
			 *
			 *  \param[in,out] Data    Pointer to a number containing an even number of bytes to be reversed.
			 *  \param[in]     Length  Length of the data in bytes.
			 */
			static inline void SwapEndian_n(void* const Data,
			                                uint8_t Length) ATTR_NON_NULL_PTR_ARG(1);
			static inline void SwapEndian_n(void* const Data,
			                                uint8_t Length)
			{
				uint8_t* CurrDataPos = (uint8_t*)Data;

				while (Length > 1)
				{
					uint8_t Temp = *CurrDataPos;
					*CurrDataPos = *(CurrDataPos + Length - 1);
					*(CurrDataPos + Length - 1) = Temp;

					CurrDataPos++;
					Length -= 2;
				}
			}

	/* Disable C linkage for C++ Compilers: */
		#if defined(__cplusplus)
			}
		#endif

#endif

/** @} */
