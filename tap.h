#ifndef __TAP_H__
#define __TAP_H__

#include <stdio.h>
#include <stdlib.h>

#define NO_PLAN   -1
#define ok(...)   ok_at_loc(__FILE__, __LINE__, __VA_ARGS__, NULL)
#define pass(...) ok(1, ## __VA_ARGS__)
#define fail(...) ok(0, ## __VA_ARGS__)

void plan         (int tests);
int  ok_at_loc    (const char *file, int line, int test, const char *fmt, ...);
int  diag         (const char *fmt, ...);
int  note         (const char *fmt, ...);
int  exit_status  (void);
void skippy       (int n, const char *fmt, ...);
void ctodo        (int ignore, const char *fmt, ...);
void cendtodo     ();

#define skip(test, ...)  do {if (test) {skippy(__VA_ARGS__, NULL); break;}
#define endskip          } while (0)

#define todo(...)        ctodo(0, ## __VA_ARGS__, NULL)
#define endtodo          cendtodo()

#ifdef _WIN32
#   define dies_ok_common(code, for_death, ...) \
        skippy(1, "Death detection is not supported on MSWin32")
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <sys/wait.h>
    int tap_test_died (int status);
#   define dies_ok_common(code, for_death, ...)                 \
        do {                                                    \
            tap_test_died(1);                                   \
            int cpid = fork();                                  \
            switch (cpid) {                                     \
            case -1:                                            \
                perror("fork error");                           \
                exit(EXIT_FAILURE);                             \
            case 0: /* child  */                                \
                close(1); close(2);                             \
                code                                            \
                tap_test_died(0);                               \
                exit(EXIT_SUCCESS);                             \
            }                                                   \
            if (waitpid(cpid, NULL, 0) < 0) {                   \
                perror("waitpid error");                        \
                exit(EXIT_FAILURE);                             \
            }                                                   \
            int it_died = tap_test_died(0);                     \
            if (!it_died) {code}                                \
            ok(for_death ? it_died : !it_died, ## __VA_ARGS__); \
        } while (0)
#endif

#define dies_ok(code, ...)  dies_ok_common(code, 1, ## __VA_ARGS__)
#define lives_ok(code, ...) dies_ok_common(code, 0, ## __VA_ARGS__)

#endif
