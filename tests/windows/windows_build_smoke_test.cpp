#include <gtest/gtest.h>

#include "suborno/platform/windows/tsf/module_smoke.hpp"

TEST(WindowsBuildSmokeTest, LoadsTheNativeWindowsModuleTarget) {
    EXPECT_NE(suborno_windows_module_smoke(), 0U);
}
