/************************************************************************************************//**
 * @file		tharness.c
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
 ***************************************************************************************************/
#include "tharness.h"


/* Private Types --------------------------------------------------------------------------------- */
typedef enum {
	THARNESS_NORMAL_STATE,
	THARNESS_IGNORING_STATE,
	THARNESS_IGNORED_STATE,
	THARNESS_FAILING_STATE,
	THARNESS_FAILED_STATE,
	THARNESS_RESULTS_STATE,
} TharnessState;

typedef enum {
	THARNESS_FAILED_EVENT,
	THARNESS_PASSED_EVENT,
	THARNESS_IGNORED_EVENT,
	THARNESS_RUN_TEST_EVENT,
	THARNESS_RUN_EXPECT_EVENT,
	THARNESS_RESULTS_EVENT,
} TharnessEvent;


/* Private Functions ----------------------------------------------------------------------------- */
static        void tharness_handle       (unsigned);
static inline void tharness_print_passed (const char* file, const char* func, int32_t line);
static inline void tharness_print_failed (const char* file, const char* func, int32_t line);
static inline void tharness_print_ignored(const char* file, const char* func, int32_t line);
static inline bool tharness_can_output   (void);
static inline void tharness_vprint       (int indent, const char* msg, va_list args);
static inline void tharness_vprint_line  (int indent, const char* msg, va_list args);


/* Global Variables ------------------------------------------------------------------------------ */
Tharness tharness;


/* tharness_init ********************************************************************************//**
 * @brief		Initializes a test harness before any tests are run. */
void tharness_init(bool verbose)
{
	tharness.total    = 0;
	tharness.failures = 0;
	tharness.ignores  = 0;
	tharness.state    = THARNESS_NORMAL_STATE;
	tharness.verbose  = verbose;
}


/* tharness_result ******************************************************************************//**
 * @brief		Prints the results after running all tests.
 * @desc		Example output on a new line:
 *
 *				3 Tests 1 Failed 0 Ignored
 *				OK
 */
int tharness_results(void)
{
	tharness_handle(THARNESS_RESULTS_EVENT);

	tharness_print_line(0, "\n%d Tests %d Failed %d Ignored", tharness.total, tharness.failures, tharness.ignores);

	if(tharness.failures == 0)
	{
		tharness_print_line(0, "OK");
	}
	else
	{
		tharness_print_line(0, "FAIL");
	}

	return tharness.failures;
}


/* tharness_run *********************************************************************************//**
 * @brief		Runs a tharness test. */
void tharness_run(void (*test)(void))
{
	tharness_handle(THARNESS_RUN_TEST_EVENT);

	test();
}


/* tharness_expect ******************************************************************************//**
 * @brief		Runs a tharness expect statement. The expect statement passes if condition is true or
 * 				fails if condition is false.
 * @param[in]	condition: result of the test. True passes the expect statement. False fails the
 * 				expect statement.
 * @param[in]	file: name of the file.
 * @param[in]	func: name of the function.
 * @param[in]	line: line number of the expect statement.
 * @param[in]	str: string of the statement under test.
 * @param[in]	msg: extra output if the test fails. The message is printed with variadic arguments.
 * @param[in]	...: variadic arguments passed with msg. */
void tharness_expect(bool condition, const char* file, const char* func, int32_t line, const char* str, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	tharness_handle(THARNESS_RUN_EXPECT_EVENT);

	if(tharness.state == THARNESS_NORMAL_STATE)
	{
		if(condition)
		{
			tharness_handle(THARNESS_PASSED_EVENT);
			tharness_print_passed(file, func, line);
		}
		else
		{
			tharness_handle(THARNESS_FAILED_EVENT);
			tharness_print_failed(file, func, line);
			tharness_print_line(1, "Expected %s", str);
			tharness_vprint_line(1, msg, args);
		}
	}

	va_end(args);
}


/* tharness_print *******************************************************************************//**
 * @brief		Prints a msg string. If printing begins on a new line, the message will be indented
 * 				with the specified number of tab characters up to a maximum of 4 tabs. Printing will
 * 				be suppressed if verbose output is disabled and the previous test passed.
 * @warning		Do not use newlines in the msg string. Doing so will break the indenting behavior.
 * 				However, newlines are acceptable at the end of the msg string. */
void tharness_print(int indent, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	tharness_vprint(indent, msg, args);

	va_end(args);
}


/* tharness_print_line **************************************************************************//**
 * @brief		Prints a string and terminates with a newline. If printing beings on a new line, the
 * 				message will be indented with the specified number of tab characters up to a maximum
 * 				of 4 tabs. Printing will be suppressed if verbose output is disabled and the previous
 * 				test passed.
 * @warning		Do not use newlines in the msg string. Doing so will break the indenting behavior. */
void tharness_print_line(int indent, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	tharness_vprint_line(indent, msg, args);

	va_end(args);
}


/* tharness_pass ********************************************************************************//**
 * @brief		Causes the current test to pass. */
void tharness_pass(const char* file, const char* func, int32_t line, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	tharness_handle(THARNESS_PASSED_EVENT);
	tharness_print_passed(file, func, line);
	tharness_vprint_line(1, msg, args);

	va_end(args);
}


/* tharness_fail ********************************************************************************//**
 * @brief		Causes the current test to fail. */
void tharness_fail(const char* file, const char* func, int32_t line, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	tharness_handle(THARNESS_FAILED_EVENT);
	tharness_print_failed(file, func, line);
	tharness_vprint_line(1, msg, args);

	va_end(args);
}


/* tharness_ignored *****************************************************************************//**
 * @brief		Causes current test to not run. */
void tharness_ignore(const char* file, const char* func, int32_t line, const char* msg, ...)
{
	va_list args;
	va_start(args, msg);

	tharness_handle(THARNESS_IGNORED_EVENT);
	tharness_print_ignored(file, func, line);
	tharness_vprint_line(1, msg, args);

	va_end(args);
}


/* tharness_handle ******************************************************************************//**
 * @brief		Handles tharness state. */
static void tharness_handle(unsigned event)
{
	switch(tharness.state)
	{
		/* This is the normal test harness state. This state is entered whenever a new TEST is run.
		 * This state is exited whenever an EXPECT statement fails or a test is IGNORED. */
		case THARNESS_NORMAL_STATE:
		{
			switch(event)
			{
				case THARNESS_FAILED_EVENT:
					tharness.failures++;
					tharness.state = THARNESS_FAILING_STATE;
					break;

				case THARNESS_IGNORED_EVENT:
					tharness.ignores++;
					tharness.state = THARNESS_IGNORING_STATE;
					break;

				case THARNESS_RUN_TEST_EVENT:
					tharness.total++;
					break;

				case THARNESS_RESULTS_EVENT:
					tharness.state = THARNESS_RESULTS_STATE;
					break;

				default: break;
			}
			break;
		}

		/* The ignoring state is entered whenever TEST_IGNORE is called. This state is exited for the
		 * ignored state on the next EXPECT, TEST_FAIL, TEST_IGNORE, or TEST_PASS call. This state
		 * exists to allow PRINT statements to output messages for the preceding TEST_IGNORE. Running
		 * a new test transitions to the normal state. */
		case THARNESS_IGNORING_STATE:
		{
			switch(event)
			{
				case THARNESS_FAILED_EVENT:
					tharness.state = THARNESS_IGNORED_STATE;
					break;

				case THARNESS_PASSED_EVENT:
					tharness.state = THARNESS_IGNORED_STATE;
					break;

				case THARNESS_IGNORED_EVENT:
					tharness.state = THARNESS_IGNORED_STATE;
					break;

				case THARNESS_RUN_TEST_EVENT:
					tharness.state = THARNESS_NORMAL_STATE;
					tharness.total++;
					break;

				case THARNESS_RUN_EXPECT_EVENT:
					tharness.state = THARNESS_IGNORED_STATE;
					break;

				case THARNESS_RESULTS_EVENT:
					tharness.state = THARNESS_RESULTS_STATE;
					break;

				default: break;
			}
			break;
		}

		/* This state is entered by any subsequent EXPECT, TEST_PASS, TEST_FAIL, or TEST_IGNORE
		 * following a TEST_IGNORE call. This state suppresses print output for the currently running
		 * test. Running a new test transitions to the normal state. */
		case THARNESS_IGNORED_STATE:
		{
			switch(event)
			{
				case THARNESS_RUN_TEST_EVENT:
					tharness.state = THARNESS_NORMAL_STATE;
					tharness.total++;
					break;

				case THARNESS_RESULTS_EVENT:
					tharness.state = THARNESS_RESULTS_STATE;
					break;

				default: break;
			}
			break;
		}

		/* The failing state is entered whenever an EXPECT statement fails or TEST_FAIL is called.
		 * This state is exited for the failed state on the next EXPECT, TEST_FAIL, TEST_IGNORE,
		 * or TEST_PASS call. This state exists to allow print statements to output messages for the
		 * last failing EXPECT statement. Runninga new test transitions to the normal state. */
		case THARNESS_FAILING_STATE:
		{
			switch(event)
			{
				case THARNESS_FAILED_EVENT:
					tharness.state = THARNESS_FAILED_STATE;
					break;

				case THARNESS_PASSED_EVENT:
					tharness.state = THARNESS_FAILED_STATE;
					break;

				case THARNESS_IGNORED_EVENT:
					tharness.state = THARNESS_FAILED_STATE;
					break;

				case THARNESS_RUN_TEST_EVENT:
					tharness.state = THARNESS_NORMAL_STATE;
					tharness.total++;
					break;

				case THARNESS_RUN_EXPECT_EVENT:
					tharness.state = THARNESS_FAILED_STATE;
					break;

				case THARNESS_RESULTS_EVENT:
					tharness.state = THARNESS_RESULTS_STATE;
					break;

				default: break;
			}
			break;
		}

		/* This state is entered by any subsequent EXPECT, TEST_PASS, TEST_FAIL, or TEST_IGNORE
		 * following a failing EXPECT statement or TEST_FAIL. This state suppresses print output for
		 * the currently running test. Running a new test transitions to the normal state. */
		case THARNESS_FAILED_STATE:
		{
			switch(event)
			{
				case THARNESS_RUN_TEST_EVENT:
					tharness.state = THARNESS_NORMAL_STATE;
					tharness.total++;
					break;

				case THARNESS_RESULTS_EVENT:
					tharness.state = THARNESS_RESULTS_STATE;
					break;

				default: break;
			}
			break;
		}

		default: break;
	}
}


/* tharness_passed ******************************************************************************//**
 * @brief		Outputs a message for a passing step in a test. */
static inline void tharness_print_passed(const char* file, const char* func, int32_t line)
{
	tharness_print_line(0, "%s:%d: %s: OK", file, line, func);
}


/* tharness_failed ******************************************************************************//**
 * @brief		Outputs a message for a failing step in a test. The test harness is modified to
 *				indicate that the test has failed. */
static inline void tharness_print_failed(const char* file, const char* func, int32_t line)
{
	tharness_print_line(0, "%s:%d: %s: FAIL", file, line, func);
}


/* tharness_failed ******************************************************************************//**
 * @brief		Outputs a message for an ignored step in a test. */
static inline void tharness_print_ignored(const char* file, const char* func, int32_t line)
{
	tharness_print_line(0, "%s:%d: %s: IGNORED", file, line, func);
}


/* tharness_can_output **************************************************************************//**
 * @brief		Returns true if output can be printed. */
static inline bool tharness_can_output(void)
{
	return
		tharness.verbose ||
		tharness.state == THARNESS_FAILING_STATE ||
		tharness.state == THARNESS_IGNORING_STATE ||
		tharness.state == THARNESS_RESULTS_STATE;
}


/* tharness_vprint ******************************************************************************//**
 * @brief		Performs the same operation as tharness_print but uses the va_list directly. */
static inline void tharness_vprint(int indent, const char* msg, va_list args)
{
	if(tharness_can_output())
	{
		if(tharness.at_new_line == true)
		{
			printf("%.*s", indent, "\t\t\t\t");
		}

		if(msg)
		{
			vprintf(msg, args);

			tharness.at_new_line = (msg[strlen(msg)-1] == '\n');
		}
	}
}


/* tharness_vprint_line *************************************************************************//**
 * @brief		Performs the same operation as tharness_print_line but uses the va_list directly. */
static inline void tharness_vprint_line(int indent, const char* msg, va_list args)
{
	if(msg)
	{
		tharness_vprint(indent, msg, args);
		tharness_print(indent, "\n");
	}
}


/******************************************* END OF FILE *******************************************/
