include(FetchContent)

FetchContent_Declare(
    OpenDDS
    GIT_REPOSITORY https://github.com/OpenDDS/OpenDDS.git
    GIT_TAG        53414662d60f5e222aad9ca4383da0d856a577df
    GIT_PROGRESS   ON
)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
FetchContent_MakeAvailable(OpenDDS)