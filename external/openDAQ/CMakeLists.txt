set(OPENDAQ_ENABLE_TESTS false)

FetchContent_Declare(
    openDAQ
    GIT_REPOSITORY https://github.com/openDAQ/openDAQ.git
    GIT_TAG        148d32b1afcbf5ddea221e1a8e9805393a880a58
    GIT_PROGRESS   ON
    SYSTEM
    FIND_PACKAGE_ARGS 3.0.0 GLOBAL
)

FetchContent_MakeAvailable(openDAQ)
