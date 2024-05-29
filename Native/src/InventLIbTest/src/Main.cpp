#include "gtest/gtest.h"

#include "Core/Instrumentation/Logging.h"
#include "Core/Instrumentation/LogWriter/StdOutLogWriter.h"

int main(int argc, char **argv) {
    StdOutLogWriter logWriter{Log::Filter{}};
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}