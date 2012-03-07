#include "minunit.h"
#include <lcthw/tstree.h>
#include <string.h>
#include <assert.h>
#include <lcthw/bstrlib.h>


TSTree *node = NULL;
char *valueA = "VALUEA";
char *valueB = "VALUEB";
char *value2 = "VALUE2";
char *value4 = "VALUE4";
char *reverse = "VALUER";
int traverse_count = 0;

struct tagbstring test1 = bsStatic("TEST");
struct tagbstring test2 = bsStatic("TEST2");
struct tagbstring test3 = bsStatic("TSET");
struct tagbstring test4 = bsStatic("T");

char *test_insert() 
{
    node = TSTree_insert(node, bdata(&test1), blength(&test1), valueA);
    mu_assert(node != NULL, "Failed to insert into tst.");

    node = TSTree_insert(node, bdata(&test2), blength(&test2), value2);
    mu_assert(node != NULL, "Failed to insert into tst with second name.");

    node = TSTree_insert(node, bdata(&test3), blength(&test3), reverse);
    mu_assert(node != NULL, "Failed to insert into tst with reverse name.");

    node = TSTree_insert(node, bdata(&test4), blength(&test4), value4);
    mu_assert(node != NULL, "Failed to insert into tst with second name.");

    return NULL;
}

char *test_search_exact()
{
    // tst returns the last one inserted
    void *res = TSTree_search(node, bdata(&test1), blength(&test1));
    mu_assert(res == valueA, "Got the wrong value back, should get A not B.");

    // tst does not find if not exact
    res = TSTree_search(node, "TESTNO", strlen("TESTNO"));
    mu_assert(res == NULL, "Should not find anything.");

    return NULL;
}

char *test_search_suffix()
{
    void *res = TSTree_search_suffix(node, bdata(&test1), blength(&test1));
    debug("result: %p, expected: %p", res, reverse);
    mu_assert(res == reverse, "Got the wrong value.");

    res = TSTree_search_suffix(node, "TESTNO", strlen("TESTNO"));
    mu_assert(res == NULL, "Reverse search should find nothing.");

    res = TSTree_search_suffix(node, "EST", strlen("EST"));
    mu_assert(res != NULL, "Reverse search should find by the suffix.");

    res = TSTree_search_suffix(node, "ESTO", strlen("ESTO"));
    mu_assert(res == NULL, "Reverse search should not find invalid suffix.");

    return NULL;
}

char *test_search_prefix()
{
    void *res = TSTree_search_prefix(node, bdata(&test1), blength(&test1));
    debug("result: %p, expected: %p", res, valueA);
    mu_assert(res == valueA, "Got wrong valueA by prefix.");

    res = TSTree_search_prefix(node, bdata(&test1), 1);
    debug("result: %p, expected: %p", res, valueA);
    mu_assert(res == value4, "Got wrong value4 for prefix of 1.");

    res = TSTree_search_prefix(node, "TE", strlen("TE"));
    mu_assert(res != NULL, "Should find for short prefix.");

    res = TSTree_search_prefix(node, "TE--", strlen("TE--"));
    mu_assert(res != NULL, "Should find for partial prefix.");


    return NULL;
}

void TSTree_traverse_test_cb(void *value, void *data)
{
    assert(value != NULL && "Should not get NULL value.");
    assert(data == valueA && "Expecting valueA as the data.");
    traverse_count++;
}

char *test_traverse()
{
    traverse_count = 0;
    TSTree_traverse(node, TSTree_traverse_test_cb, valueA);
    debug("traverse count is: %d", traverse_count);
    mu_assert(traverse_count == 4, "Didn't find 4 keys.");

    return NULL;
}

char *test_collect()
{
    DArray *found = TSTree_collect(node, "TE", 2, NULL);
    debug("collect found %d values", (int)DArray_count(found));

    mu_assert(DArray_count(found) == 3, "Didn't find 2 with prefix TE.");
    DArray_destroy(found);

    found = TSTree_collect(node, "T", 1, NULL);
    debug("collect found %d values", (int)DArray_count(found));
    mu_assert(DArray_count(found) == 4, "Didn't find 4 with prefix T.");
    DArray_destroy(found);

    found = TSTree_collect(node, "TEST2", 5, NULL);
    debug("collect found %d values", (int)DArray_count(found));
    mu_assert(DArray_count(found) == 1, "Didn't find 1 with prefix TEST2.");
    DArray_destroy(found);

    found = TSTree_collect(node, "XNOT", 4, NULL);
    mu_assert(DArray_count(found) == 0, "Should not find any with prefix XNOT.");
    DArray_destroy(found);

    return NULL;
}

char *test_destroy()
{
    TSTree_destroy(node);

    return NULL;
}

char * all_tests() {
    mu_suite_start();

    mu_run_test(test_insert);
    mu_run_test(test_search_exact);
    mu_run_test(test_search_suffix);
    mu_run_test(test_search_prefix);
    mu_run_test(test_traverse);
    mu_run_test(test_collect);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
