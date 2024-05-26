#include "CommonTest.h"
#include "Core/Instrumentation/DebugContext.h"

TEST(DebugContext, ContainsFileName) {
	Debug::Context ctx;

	ASSERT_FALSE(ctx.FileName.empty());
	ASSERT_THAT(ctx.FileName, EndsWith("DebugContext.test.cpp"));	
}

TEST(DebugContext, ContainsFunctionName) {
	Debug::Context ctx;

	ASSERT_FALSE(ctx.Function.empty());
	ASSERT_THAT(ctx.Function, HasSubstr("ContainsFunctionName"s));
}

TEST(DebugContext, ContainsLineNumber) {
	Debug::Context ctx;

	ASSERT_NE(0, ctx.LineNumber);
}

TEST(DebugContext, ContainsThreadId) {
	Debug::Context ctx;

	ASSERT_EQ(std::this_thread::get_id(), ctx.ThreadId);
}