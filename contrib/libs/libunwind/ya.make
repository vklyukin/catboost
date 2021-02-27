# Generated by devtools/yamaker from nixpkgs 5852a21819542e6809f68ba5a798600e69874e76.

LIBRARY()



VERSION(2020-11-23)

LICENSE(Apache-2.0)

DISABLE(USE_LTO)

ADDINCL(
    contrib/libs/libunwind/include
)

NO_RUNTIME()

NO_UTIL()

NO_SANITIZE()

NO_SANITIZE_COVERAGE()

CFLAGS(
    GLOBAL -D_libunwind_
    -D_LIBUNWIND_IS_NATIVE_ONLY
    -fno-exceptions
    -fno-rtti
    -funwind-tables
)

IF (SANITIZER_TYPE == memory)
    CFLAGS(-fPIC)
ENDIF()

SRCS(
    src/Unwind-EHABI.cpp
    src/Unwind-seh.cpp
    src/Unwind-sjlj.c
    src/UnwindLevel1-gcc-ext.c
    src/UnwindLevel1.c
    src/UnwindRegistersRestore.S
    src/UnwindRegistersSave.S
    src/libunwind.cpp
)

IF (OS_DARWIN OR OS_IOS)
    SRCS(
        src/Unwind_AppleExtras.cpp
    )
ENDIF()

END()

RECURSE_FOR_TESTS(
    ut
)
