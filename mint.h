#ifndef MINT_H
#define MINT_H


#define MINT_VERSION_MAJOR   0
#define MINT_VERSION_MINOR   0
#define MINT_VERSION_PATCH   0


// platform and compiler detection
#ifdef WIN32
# define MINT_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MAC_OS_X_VERSION_MIN_REQUIRED)
# define MINT_PLATFORM_MAC
#elif defined(__linux__)
# define MINT_PLATFORM_LINUX
#else
# define MINT_PLATFORM_UNKNOWN
#endif
#ifdef __clang__
# define MINT_COMPILER_CLANG
#endif
#ifdef _MSC_VER
# define MINT_COMPILER_MSVC
#else
# define MINT_COMPILER_UNKNOWN
#endif


// opt-out feature: colored text output to tty and consoles 
#ifndef MINT_OPT_NO_COLOR_TEXT
# define MINT_COLOR_TEXT
#endif


// function attributes
#define MINT_ATTRIBUTE(ATTR)    __attribute__((ATTR))

// isatty
#ifdef MINT_COMPILER_MSVC
# include <io.h>
# define MINT_IS_TTY()          (_isatty(_fileno(stdout)))
#elif defined(MINT_PLATFORM_LINUX)
# include <unistd.h>
# define MINT_IS_TTY()          (isatty(STDOUT_FILENO))
#else
# define MINT_IS_TTY()          0
#endif

// initializer functions, called before main and must have
// the function signature of void (*initializer)(void)
#ifdef MINT_COMPILER_MSVC
# ifdef _WIN64
#  define MINT_PREFIX ""
# else
#  define MINT_PREFIX "_"
# endif
# pragma section(".CRT$XCU", read)
# define MINT_INITIALIZER(FN)\
    static void FN(void);\
    __pragma(comment(linker, "/INCLUDE:" MINT_PREFIX #FN "_"))\
    __declspec(allocate(".CRT$XCU")) void(*FN##_)(void) = FN;\
    static void FN(void)
# undef MINT_PREFIX
#elif defined(MINT_PLATFORM_LINUX)
# define MINT_INITIALIZER(FN)   MINT_ATTRIBUTE(constructor) static void FN(void)
#endif

typedef struct mint_context   mint_context;
typedef struct mint_unit      mint_unit;
typedef void (*mint_fn)(mint_unit *);

struct mint_context {
    int unit_count;
    int unit_capacity;
    mint_unit *unit;
};

struct mint_unit {
    mint_fn fn;
    const char *name;
    int has_failed;
	int has_warning;
	int tests_passed;
};


// define some alias to save user keystrokes, but some macros such as ASSERT
// are very common. If you run into a name collision, either change the alias name
// here or use the MINT full name directly.
#ifndef UNIT_TEST
# define UNIT_TEST  MINT_UNIT_TEST
#endif
#ifndef WARN
# define WARN       MINT_WARN
#endif
#ifndef CHECK
# define CHECK      MINT_CHECK
#endif
#ifndef ASSERT
# define ASSERT     MINT_ASSERT
#endif


#define MINT_WARN(EXPR)\
	if (!(EXPR))\
	{\
		unit->has_warning = 1;\
		mint_print_fail("warning", #EXPR, __FILE__, __LINE__);\
	}\
	else unit->tests_passed++;

#define MINT_CHECK(EXPR)\
	if (!(EXPR))\
	{\
		unit->has_failed = 1;\
		mint_print_fail("check", #EXPR, __FILE__, __LINE__);\
	}\
	else unit->tests_passed++;

#define MINT_ASSERT(EXPR)\
    if (!(EXPR))\
	{\
		unit->has_failed = 1;\
		mint_print_fail("assert", #EXPR, __FILE__, __LINE__);\
        return;\
	}\
	else unit->tests_passed++;


#define MINT_UNIT_TEST(NAME)\
    static void mint_unit_##NAME (mint_unit *unit);\
    MINT_INITIALIZER( mint_unit_##NAME##_register )\
    {\
        mint_context *ctx = &mint_ctx;\
        if (ctx->unit_capacity == 0)\
        {\
            ctx->unit_count = 0;\
            ctx->unit_capacity = 32;\
            ctx->unit = (mint_unit *)malloc(sizeof(mint_unit) * ctx->unit_capacity);\
        }\
        if (ctx->unit_count == ctx->unit_capacity)\
        {\
            ctx->unit_capacity <<= 1;\
            ctx->unit = (mint_unit *)realloc(ctx->unit, sizeof(mint_unit) * ctx->unit_capacity);\
        }\
		int i = ctx->unit_count;\
        ctx->unit[i].fn = &mint_unit_##NAME;\
        ctx->unit[i].name = #NAME;\
        ctx->unit[i].has_failed = 0;\
        ctx->unit[i].has_warning = 0;\
        ctx->unit[i].tests_passed = 0;\
		ctx->unit_count++;\
    }\
    static void mint_unit_##NAME (mint_unit *unit)


// libc dependency of <stdlib.h> and <stdio.h>,
// which might be unavailable in certain contexts
#include <stdlib.h>
#include <stdio.h>


#ifndef MINT_IMPLEMENTATION
extern void mint_log(char c);
extern void mint_print_fail(const char *type, const char *msg, const char *file, int line);
extern mint_context mint_ctx;
#else

#ifdef MINT_COLOR_TEXT
static const char *mint_color_reset  = "\033[0m";
static const char *mint_color_red    = "\033[91m";
static const char *mint_color_green  = "\033[92m";
static const char *mint_color_yellow = "\033[93m";
#else
static const char *mint_color_reset  = "";
static const char *mint_color_red    = "";
static const char *mint_color_green  = "";
static const char *mint_color_yellow = "";
#endif

mint_context mint_ctx;

void mint_log(char c)
{
	printf("%s[%c]%s ", mint_color_yellow, c, mint_color_reset);
}

void mint_print_fail(const char *type, const char *msg, const char *file, int line)
{
	mint_log('-'); printf("  %s at %s:%d\n", type, file, line);
	mint_log('-'); printf("  %s\n", msg);
}

static int mint_main(int argc, char **argv)
{
    mint_context *ctx = &mint_ctx;
	mint_unit *unit;
	int num_units = ctx->unit_count;
	int num_units_passed = 0;
	int num_tests_passed = 0;

#ifdef MINT_COLOR_TEXT
	int use_color_text = MINT_IS_TTY();
	if (!use_color_text)
		mint_color_reset = mint_color_red = mint_color_green = mint_color_yellow = "";
#endif

	mint_log('#'); printf("running %d units\n", num_units);

    for (int i = 0; i < num_units; i++)
    {
		unit = ctx->unit + i;
		mint_log('+'); printf("%s (unit %d/%d)\n", unit->name, i + 1, num_units);
        unit->fn(unit);
		mint_log('+'); printf("%s ", unit->name);
		if (unit->has_failed)
			printf("%sfailed%s\n", mint_color_red, mint_color_reset);
		else
		{
			printf("%spassed%s\n", mint_color_green, mint_color_reset);
			num_units_passed++;
		}
		num_tests_passed += unit->tests_passed;
    }

	mint_log('#'); printf("total %d/%d units passed\n", num_units_passed, num_units);
	mint_log('#'); printf("total %d tests passed\n", num_tests_passed);

    return 0;
}

#endif // MINT_IMPLEMENTATION
#endif // MINT_H
