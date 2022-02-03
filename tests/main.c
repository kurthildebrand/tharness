#include "tharness.h"

TEST(test_assert)
{
	/* A passing message */
	EXPECT(1);
	PRINT_LINE("Passing test message");
	PRINT_LINE("Another passing test message");

	/* A failing message */
	EXPECT(0);
	PRINT_LINE("Test message");
	PRINT_LINE("Another test message");
}

TEST(test_failed)
{
	TEST_FAIL();
	PRINT_LINE("Output after fail");
}

TEST(test_ignored)
{
	TEST_IGNORE();
	PRINT_LINE("Output after ignore");
}

TEST(test_ints)
{
	int a = 10;
	int b = 20;

	/* Passing tests */
	EXPECT(a != b);
	EXPECT(a < b);

	/* Failing tests */
	EXPECT(a == b, "This is a test");
	EXPECT(a > b);
}

TEST(test_arrays)
{
	/* Arrays not equal:        x <-- here */
	int a[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
	int b[] = { 0, 10, 20, 30, 41, 50, 60, 70, 80, 90 };

	unsigned i;
	for(i = 0; i < sizeof(a) / sizeof(a[0]); i++)
	{
		if(a[i] != a[i])
		{
			TEST_FAIL("Arrays not equal.");
		}
	}

	TEST_PASS("Arrays equal");


	for(i = 0; i < sizeof(a) / sizeof(a[0]); i++)
	{
		if(a[i] != b[i])
		{
			TEST_FAIL("Arrays not equal. a[%d] == %d, b[%d] == %d", i, a[i], i, b[i]);
		}
	}

	TEST_PASS("Arrays are equal");
}


int main()
{
	// fprintf(stdout, "This is a test\n");

	// return 0;

	tharness_init(false);

	RUN(test_assert);
	RUN(test_failed);
	RUN(test_ignored);
	RUN(test_ints);
	RUN(test_arrays);

	return tharness_results();
}
