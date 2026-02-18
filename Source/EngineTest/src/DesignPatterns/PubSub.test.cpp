#include "CommonTest.h"

#include "DesignPatterns/PubSub.h"

struct PubSubTest : public testing::Test {
	PubSub<int> pubSub {};
};

TEST_F(PubSubTest, Publish_AfterSubscribe_NotifiesSubscriber) {
	int capturedValue{ 0 };
	auto handle = pubSub.Subscribe([&](const int& event) { capturedValue = event; });
	pubSub.Publish(42);

	ASSERT_EQ(42, capturedValue);
}

TEST_F(PubSubTest, Publish_WithMultipleSubscribers_NotifiesAll) {
	int capturedValue1{ 0 };
	auto h1 = pubSub.Subscribe([&](const int& event) { capturedValue1 = event; });

	int capturedValue2{ 0 };
	auto h2 = pubSub.Subscribe([&](const int& event) { capturedValue2 = event; });

	pubSub.Publish(42);

	ASSERT_EQ(42, capturedValue1);
	ASSERT_EQ(42, capturedValue2);
}

TEST_F(PubSubTest, Publish_AfterSubscribeThenUnsubscribe_DoesNotNotify) {
	int capturedValue{ 0 };
	auto onEvent = [&](const int& event) { capturedValue = event; FAIL(); };
	
	auto handle = pubSub.Subscribe(onEvent);
    handle.Destroy();
	pubSub.Publish(42);

	ASSERT_EQ(0, capturedValue);
}

TEST_F(PubSubTest, Alarm_OnEvent_IsCleared) {
	int val = 0;
    pubSub.Alarm([&](const int& event) { val = event; });

	pubSub.Publish(42);
    ASSERT_EQ(val, 42);
    
	pubSub.Publish(11);
    ASSERT_EQ(val, 42);
}