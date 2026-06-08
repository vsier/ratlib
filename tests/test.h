// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#ifndef RAT_TEST_H_
#define RAT_TEST_H_


typedef int test_fn(void);

int test_fail(const char *file, int line, const char *expr);
int test_run(const char *name, test_fn *fn);

#define TEST_CHECK(_expr) do { if (!(_expr)) return test_fail(__FILE__, __LINE__, #_expr); } while (0)
#define TEST_RUN(_fn) do { int testResult = test_run(#_fn, (_fn)); if (testResult) return testResult; } while (0)

int test_ralcArena(void);
int test_ralcPool(void);
int test_rdyn(void);
int test_rhash(void);
int test_rmap(void);
int test_rlist(void);
int test_rbit(void);
int test_rcrc(void);
int test_rcmp(void);
int test_rarr(void);
int test_rring(void);
int test_rorder(void);

#endif
