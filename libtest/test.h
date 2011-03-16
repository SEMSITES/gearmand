/* uTest
 * Copyright (C) 2011 Data Differential, http://datadifferential.com/
 * Copyright (C) 2006-2009 Brian Aker
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 */

/*
  Structures for generic tests.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <libtest/visibility.h>


typedef struct world_st world_st;
typedef struct collection_st collection_st;
typedef struct test_st test_st;

typedef enum {
  TEST_SUCCESS= 0, /* Backwards compatibility */
  TEST_FAILURE,
  TEST_MEMORY_ALLOCATION_FAILURE,
  TEST_SKIPPED,
  TEST_MAXIMUM_RETURN /* Always add new error code before */
} test_return_t;

typedef void *(*test_callback_create_fn)(test_return_t *error);
typedef test_return_t (*test_callback_fn)(void *);
typedef test_return_t (*test_callback_runner_fn)(test_callback_fn, void *);
typedef test_return_t (*test_callback_error_fn)(test_return_t, void *);


/**
  A structure describing the test case.
*/
struct test_st {
  const char *name;
  bool requires_flush;
  test_callback_fn test_fn;
};


/**
  A structure which describes a collection of test cases.
*/
struct collection_st {
  const char *name;
  test_callback_fn pre;
  test_callback_fn post;
  test_st *tests;
};


/**
  Structure which houses the actual callers for the test cases contained in
  the collections.
*/
typedef struct {
  test_callback_runner_fn pre;
  test_callback_runner_fn run;
  test_callback_runner_fn post;
} world_runner_st;


/**
  world_st is the structure which is passed to the test implementation to be filled.
  This must be implemented in order for the test framework to load the tests. We call
  get_world() in order to fill this structure.
*/

struct world_st {
  collection_st *collections;

  /* These methods are called outside of any collection call. */
  test_callback_create_fn create;
  test_callback_fn destroy;

  /* This is called a the beginning of any collection run. */
  test_callback_fn collection_startup;

  /* This called on a test if the test requires a flush call (the bool is from test_st) */
  test_callback_fn flush;

  /**
    These are run before/after the test. If implemented. Their execution is not controlled
    by the test.
  */
  test_callback_fn pre_run;
  test_callback_fn post_run;

  /**
    If an error occurs during the test, this is called.
  */
  test_callback_error_fn on_error;

  /**
    Runner represents the callers for the tests. If not implemented we will use
    a set of default implementations.
  */
  world_runner_st *runner;
};



/**
  @note world_stats_st is a simple structure for tracking test successes.
*/
typedef struct {
  uint32_t success;
  uint32_t skipped;
  uint32_t failed;
  uint32_t total;
} world_stats_st;

#ifdef	__cplusplus
extern "C" {
#endif

/* How we make all of this work :) */
LIBTEST_API
void get_world(world_st *world);

LIBTEST_INTERNAL_API
void create_core(void);

/**
  @note Friendly print function for errors.
*/
LIBTEST_INTERNAL_API
const char *test_strerror(test_return_t code);

#define test_truth(A) \
do \
{ \
  if (! (A)) { \
    fprintf(stderr, "\nAssertion failed in %s:%d: %s\n", __FILE__, __LINE__, #A);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_true_got(A,B) \
do \
{ \
  if (! (A)) { \
    fprintf(stderr, "\nAssertion failed at %s:%d: \"%s\" received \"%s\"\n", __FILE__, __LINE__, #A, (B));\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_false(A) \
do \
{ \
  if ((A)) { \
    fprintf(stderr, "\nAssertion failed in %s:%d: %s\n", __FILE__, __LINE__, #A);\
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#define test_strcmp(A,B) \
do \
{ \
  if (strcmp((A), (B))) \
  { \
    fprintf(stderr, "\n%s:%d: %s -> %s\n", __FILE__, __LINE__, (A), (B)); \
    create_core(); \
    return TEST_FAILURE; \
  } \
} while (0)

#ifdef	__cplusplus
}
#endif
