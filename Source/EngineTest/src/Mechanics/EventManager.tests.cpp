#include "CommonTest.h"

#include "Mechanics/EventManager.h"

struct E1 : IEvent {
    int Updates;

    E1(BaseTime duration, int updates) : IEvent(duration), Updates(updates) {}

    void OnUpdate() override { Updates++; }
    bool IsComplete() const override { return Done; }
    bool Done{false};
};

struct E2 : IEvent {
    E2(BaseTime duration) : IEvent(duration) {}
};

struct EventManagerTest : testing::Test {
    EventManager eventManager;

    static void SetUpTestSuite() {
        EventManager::Initialize(); 
    }
};

TEST_F(EventManagerTest, StartEvent_AddsEvent) {
    auto handle = eventManager.StartEvent<E1>(OneMinute, 0);
    const auto* event = eventManager.GetEvent(handle);

    ASSERT_NE(event, nullptr);
}

TEST_F(EventManagerTest, GetEvent_WithUnknownHandle_ReturnsNullptr) {
    const auto* event = eventManager.GetEvent(0);
    ASSERT_EQ(event, nullptr);
}

TEST_F(EventManagerTest, Update_WithOneEvent_UpdatesEvent) {
    auto handle = eventManager.StartEvent<E1>(OneMinute, 0);
    eventManager.Update(OneSecond);
    const auto* event = eventManager.GetEvent(handle);
    ASSERT_NE(event, nullptr);
    ASSERT_EQ(event->Elapsed, OneSecond);
    ASSERT_EQ(event->Ttl, OneMinute - OneSecond);
    const auto* e1 = static_cast<const E1*>(event);
    ASSERT_EQ(e1->Updates, 1);
}

TEST_F(EventManagerTest, StartEvent_PublishesEventStart) {
    bool received{false};
    ServiceLocator::Get().GetOrCreate<PubSub<EventStart>>().Alarm([&](const EventStart&) {
        received = true;
    });
    eventManager.StartEvent<E1>(OneMinute, 0);
    ASSERT_TRUE(received);
}

TEST_F(EventManagerTest, Update_PastDuration_PublishEventEnd) {
    bool received{false};
    ServiceLocator::Get().GetOrCreate<PubSub<EventEnd>>().Alarm([&](const EventEnd&) {
        received = true;
    });
    eventManager.StartEvent<E2>(OneSecond);
    eventManager.Update(OneSecond * 2);
    ASSERT_TRUE(received);
}

TEST_F(EventManagerTest, Update_PastDuration_InvalidatesHandle) {
    auto handle = eventManager.StartEvent<E2>(OneSecond);
    eventManager.Update(OneSecond * 2);
    const auto* event = eventManager.GetEvent(handle);
    ASSERT_EQ(event, nullptr);
}