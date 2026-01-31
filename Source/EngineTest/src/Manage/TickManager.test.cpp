#include "CommonTest.h"
#include "Manage/TickManager.h"

struct TestTickable {
	BaseTime Total{0};
	void Tick(BaseTime elapsed) {
		Total += elapsed;
	}
};

static_assert(Tickable<TestTickable>);

TEST(TickManagerTest, Tick_AfterRegister_CallsTick) {
	TickManager manager;
    TestTickable tickable;
    manager.Register(tickable);

	manager.Tick(OneSecond);

	ASSERT_EQ(tickable.Total, OneSecond);
}

TEST(TickManagerTest, Tick_AfterUnregister_DoesNotTick) {
    TickManager manager;
    TestTickable tickable;

	auto handle = manager.Register(tickable);
	manager.Unregister(handle);
	manager.Tick(OneSecond);
    ASSERT_EQ(tickable.Total, BaseTime(0));
}