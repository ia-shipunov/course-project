#include <stdio.h>
#include "test_filters.h"

int tests_passed = 0;
int tests_failed = 0;

#define RUN_TEST(test) do { \
    printf("Running %s...\n", #test); \
    test(); \
    tests_passed++; \
} while(0)

int main() {
    printf("=== Image processing library tests ===\n");
    RUN_TEST(test_median_filter);
    RUN_TEST(test_gaussian_filter);
    RUN_TEST(test_sobel_edge);
    RUN_TEST(test_histogram_equalize);
    RUN_TEST(test_convolve_identity);

    printf("\nResults: %d passed, %d failed\n", tests_passed, tests_failed);
    if (tests_failed > 0) {
        printf("SOME TESTS FAILED\n");
        return 1;
    }
    printf("ALL TESTS PASSED\n");
    return 0;
}