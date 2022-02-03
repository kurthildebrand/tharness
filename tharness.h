/************************************************************************************************//**
 * @file		tharness.h
 *
 * @copyright	Copyright 2022 Kurt Hildebrand.
 * @license		Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * 				file except in compliance with the License. You may obtain a copy of the License at
 *
 * 				http://www.apache.org/licenses/LICENSE-2.0
 *
 * 				Unless required by applicable law or agreed to in writing, software distributed under
 * 				the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 * 				ANY KIND, either express or implied. See the License for the specific language
 * 				governing permissions and limitations under the License.
 *
 * @desc		Tharness is a testing framework for c.
 *
 ***************************************************************************************************/
#ifndef THARNESS_H
#define THARNESS_H

#if __STDC_VERSION__ < 199901L
#error Compile with C99 or higher!
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Includes -------------------------------------------------------------------------------------- */
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>


/* Public Types ---------------------------------------------------------------------------------- */
typedef struct {
	unsigned total;			/// Total number of tests run.
	unsigned failures;		/// Total number of failing tests.
	unsigned ignores;		/// Total number of tests ignored.
	unsigned state;
	bool at_new_line;		/// Indicates if printing is at the start of a new line.
	bool verbose;			/// False suppresses non-failing and non-ignored output.
} Tharness;


/* Global Variables ------------------------------------------------------------------------------ */
extern Tharness tharness;


/* Public Macros --------------------------------------------------------------------------------- */
#define TEST(name)	void name(void)

#define RUN(test) \
	tharness_run(test)
#define EXPECT(...)	\
	THARNESS_APPEND_NARGS(EXPECT, __VA_ARGS__)
#define EXPECT_MESSAGE(condition, ...) \
	tharness_expect((condition),  __FILE__, __func__, __LINE__, #condition, __VA_ARGS__)
#define EXPECT1(condition) \
	tharness_expect((condition), __FILE__, __func__, __LINE__, #condition, 0)
#define EXPECT2(condition, ...) \
	EXPECT_MESSAGE(condition, __VA_ARGS__)


#define PRINT(...) \
	tharness_print(1, __VA_ARGS__)
#define PRINT_LINE(...)	\
	tharness_print_line(1,  __VA_ARGS__)


#define TEST_PASS(...) \
	tharness_pass(__FILE__, __func__, __LINE__, 0)
#define TEST_PASS_MESSAGE(...) \
	tharness_pass(__FILE__, __func__, __LINE__, __VA_ARGS__)

#define TEST_FAIL(...) \
	tharness_fail(__FILE__, __func__, __LINE__, 0)
#define TEST_FAIL_MESSAGE(...) \
	tharness_fail(__FILE__, __func__, __LINE__, __VA_ARGS__)


#define TEST_IGNORE(...) \
	tharness_ignore(__FILE__, __func__, __LINE__, 0)
#define TEST_IGNORE_MESSAGE(...) \
	tharness_ignore(__FILE__, __func__, __LINE__, __VA_ARGS__)


#define TEST_ABORT() \
	return



/* THARNESS_APPEND_NARGS ************************************************************************//**
 * @brief		Concatenates the string 'base' with the number of arguments passed to the variadic
 * 				macro. The format is base ## N or baseN where N is the number of arguments. This
 * 				is a tharness specific macro. It:
 *
 * 					returns 1 if 0 or 1 arguments are given, or
 * 					returns 2 if 2 or more arguments are given
 *
 * 				Example:
 *
 *					#define EXPECT(...)	THARNESS_APPEND_NARGS(EXPECT, __VA_ARGS__)
 *					EXPECT(a)		EXPECT1(a)
 *					EXPECT(a,b)		EXPECT2(a,b)
 *					EXPECT(a,b,c)	EXPECT2(a,b,c)
 *					EXPECT(a,b,c,d)	EXPECT2(a,b,c,d)
 *
 * 				This macro is used to select between EXPECT and EXPECT_MESSAGE depending on if the
 * 				message string is given to EXPECT. */
#define THARNESS_APPEND_NARGS(base, ...) \
	THARNESS_APPEND_NARGS_(base, THARNESS_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define THARNESS_APPEND_NARGS_(base, count, ...) \
	THARNESS_APPEND_NARGS__(base, count, __VA_ARGS__)
#define THARNESS_APPEND_NARGS__(base, count, ...) \
	base ## count(__VA_ARGS__)


/* THARNESS_COUNT_ARGS **************************************************************************//**
 * @brief		Counts the number of arguments passed into a variadic macro. This macro can handle a
 * 				maximum of 64 arguments. This macro is also tharness specific. It:
 *
 * 					returns 1 if 0 or 1 arguments are given, or
 * 					returns 2 if 2 or more arguments are given.
 *
 * 				Example:
 *
 * 					printf("%d", THARNESS_COUNT_ARGS(a));		// outputs 1
 * 					printf("%d", THARNESS_COUNT_ARGS(a, b));	// outputs 2
 * 					printf("%d", THARNESS_COUNT_ARGS(a, b, c));	// outputs 2
 */
#define THARNESS_COUNT_ARGS(...) \
    THARNESS_COUNT_ARGS_(__VA_ARGS__, THARNESS_RSEQ_N())
#define THARNESS_COUNT_ARGS_(...) \
    THARNESS_ARG_N(__VA_ARGS__)
#define THARNESS_ARG_N( \
     _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
    _61,_62,_63,  N, ...) N
#define THARNESS_RSEQ_N() \
    2,2,2,2,              \
    2,2,2,2,2,2,2,2,2,2,  \
    2,2,2,2,2,2,2,2,2,2,  \
    2,2,2,2,2,2,2,2,2,2,  \
    2,2,2,2,2,2,2,2,2,2,  \
    2,2,2,2,2,2,2,2,2,2,  \
    2,2,2,2,2,2,2,2,1,0


/* Public Functions ------------------------------------------------------------------------------ */
void tharness_init      (bool);
int  tharness_results   (void);
void tharness_run       (void (*test)(void));
void tharness_expect    (bool, const char*, const char*, int32_t, const char*, const char*, ...);
void tharness_print     (int, const char*, ...);
void tharness_print_line(int, const char*, ...);
void tharness_pass      (const char*, const char*, int32_t, const char*, ...);
void tharness_fail      (const char*, const char*, int32_t, const char*, ...);
void tharness_ignore    (const char*, const char*, int32_t, const char*, ...);


#ifdef __cplusplus
}
#endif

#endif // THARNESS_H
/******************************************* END OF FILE *******************************************/
