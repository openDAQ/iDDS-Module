#include "gtest/gtest.h"
#include "gmock/gmock.h"

int main(int argc, char* argv[]) {
    using namespace ::testing;
    InitGoogleMock(&argc, argv);
    InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
