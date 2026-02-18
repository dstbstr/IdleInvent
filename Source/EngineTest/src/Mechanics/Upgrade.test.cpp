#include "CommonTest.h"
#include "Mechanics/Upgrade.h"
#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"
#include "Mechanics/ScaleFunc.h"

#include <format>
#include <string>

enum struct Quality : u8 {
	Unset,
	Low,
	Medium,
	High,
	COUNT 
};
std::string ToString(Quality q) {
	switch(q) {
		case Quality::Low: return "Low";
		case Quality::Medium: return "Medium";
		case Quality::High: return "High";
		default: return "Unknown";
	}
}

enum struct Thing : u8 {
	Unset,
	ThingA,
	ThingB,
	COUNT 
};
std::string ToString(Thing t) {
	switch(t) {
		case Thing::ThingA: return "ThingA";
		case Thing::ThingB: return "ThingB";
		default: return "Unknown";
	}
}

enum struct ResourceName : u8 {
	Cash,
	Sticks,
	Stones,
	COUNT
};
std::string ToString(ResourceName r) {
	switch(r) {
		case ResourceName::Cash: return "Cash";
		case ResourceName::Sticks: return "Sticks";
		case ResourceName::Stones: return "Stones";
		default: return "Unknown";
	}
}
static_assert(ResourceEnum<ResourceName>);

struct QualityUpgrade {
	using IdType = Thing;
	using LevelType = Quality;
	Thing Id{Thing::Unset};
	Quality Level{Quality::Unset};
    size_t UpgradeCalls{0};

	std::string Describe() const {
		return std::format("{} {}", ToString(Level), ToString(Id));
	}
    void OnUpgrade() { UpgradeCalls++; }
};
static_assert(UpgradableType<QualityUpgrade>);

struct QualityUpgradeTest : public testing::Test {
    QualityUpgrade q {Thing::ThingA, Quality::Low};

	ResourceCollection upgradeCost =
        CreateRc<ResourceName>(std::pair(ResourceName::Cash, 100), std::pair(ResourceName::Sticks, 10));
    ResourceCollection enough =
        CreateRc<ResourceName>(std::pair(ResourceName::Cash, 250), std::pair(ResourceName::Sticks, 25));
    ResourceCollection notEnough =
        CreateRc<ResourceName>(std::pair(ResourceName::Cash, 99), std::pair(ResourceName::Sticks, 9));
    ResourceCollection missingOne = CreateRc<ResourceName>(std::pair(ResourceName::Cash, 250));

	void SetUp() override {
		auto& costs = UpgradeManager::_Details::GetUpgradeCosts<QualityUpgrade>();
        costs[Thing::ThingA][Quality::Medium] = upgradeCost;
        ServiceLocator::Get().CreateIfMissing<PubSub<UpgradeEvent<QualityUpgrade>>>();
	}
	void TearDown() override {
		auto& costs = UpgradeManager::_Details::GetUpgradeCosts<QualityUpgrade>();
        costs.clear();
	}
};

TEST_F(QualityUpgradeTest, CanUpgrade_WithEnoughResources_ReturnsTrue) {
    ASSERT_TRUE(UpgradeManager::CanUpgrade(q, enough));
}
TEST_F(QualityUpgradeTest, CanUpgrade_NotEnough_ReturnsFalse) {
    ASSERT_FALSE(UpgradeManager::CanUpgrade(q, notEnough));
}
TEST_F(QualityUpgradeTest, CanUpgrade_MissingResource_ReturnsFalse) {
    ASSERT_FALSE(UpgradeManager::CanUpgrade(q, missingOne));
}
TEST_F(QualityUpgradeTest, CanUpgrade_MaxQuality_ReturnsFalse) {
    auto& costs = UpgradeManager::_Details::GetUpgradeCosts<QualityUpgrade>();
    costs[Thing::ThingA][Quality::COUNT] = upgradeCost; // ensure that a record exists
    q.Level = Quality::High;
    ASSERT_FALSE(UpgradeManager::CanUpgrade(q, enough));
}
TEST_F(QualityUpgradeTest, Upgrade_WithResources_RemovesResources) {
	UpgradeManager::Upgrade(q, enough);
    ASSERT_EQ(enough.at(ResourceName::Cash).Current, 150);
    ASSERT_EQ(enough.at(ResourceName::Sticks).Current, 15);
}
TEST_F(QualityUpgradeTest, Upgrade_WithResources_RaisesEvent) {
    bool raised = false;
    auto& ps = ServiceLocator::Get().GetRequired<PubSub<UpgradeEvent<QualityUpgrade>>>();
    auto handle = ps.Subscribe([&](const UpgradeEvent<QualityUpgrade>&) { raised = true; });

	UpgradeManager::Upgrade(q, enough);
    ASSERT_TRUE(raised);
}
TEST_F(QualityUpgradeTest, Upgrade_WithResources_IncrementsLevel) {
    UpgradeManager::Upgrade(q, enough);
    ASSERT_EQ(q.Level, Quality::Medium);
}
TEST_F(QualityUpgradeTest, Upgrade_WithResources_CallsOnUpdate) {
	UpgradeManager::Upgrade(q, enough);
    ASSERT_EQ(q.UpgradeCalls, 1);
}

struct NumericUpgrade {
    using IdType = Thing;
    using LevelType = u32;
    Thing Id{Thing::Unset};
    u32 Level{0};
    std::string Describe() const { return std::format("{} {}", ToString(Id), Level); }
    void OnUpgrade() { Level++; }
};
static_assert(UpgradableType<NumericUpgrade>);

struct NumericUpgradeTest : public testing::Test {
    NumericUpgrade q{Thing::ThingA, 0};

    ResourceCollection enough = CreateRc<ResourceName>(std::pair(ResourceName::Cash, 2500));
    ResourceCollection notEnough = CreateRc<ResourceName>(std::pair(ResourceName::Cash, 1));

    void SetUp() override {
        auto& costFns = UpgradeManager::_Details::GetUpgradeCostFns<NumericUpgrade>();
        costFns[Thing::ThingA] = [](u32 level) {
            auto cash = Scale::Linear<u32, 2>(level);
            return CreateRc<ResourceName>(std::pair(ResourceName::Cash, cash));
        };
        ServiceLocator::Get().CreateIfMissing<PubSub<UpgradeEvent<NumericUpgrade>>>();
    }
    void TearDown() override {
        auto& costs = UpgradeManager::_Details::GetUpgradeCosts<NumericUpgrade>();
        costs.clear();
    }
};

TEST_F(NumericUpgradeTest, CanUpgrade_WithEnoughResources_ReturnsTrue) {
    ASSERT_TRUE(UpgradeManager::CanUpgrade(q, enough));
}
TEST_F(NumericUpgradeTest, CanUpgrade_NotEnough_ReturnsFalse) {
    ASSERT_FALSE(UpgradeManager::CanUpgrade(q, notEnough));
}
TEST_F(NumericUpgradeTest, CanUpgrade_AfterManyLevels_ReturnsTrue) {
	q.Level = 10;
    ASSERT_TRUE(UpgradeManager::CanUpgrade(q, enough));
}

struct MissingLevelType {
    using IdType = Thing;
    void OnUpgrade() {}
    Thing Id{Thing::Unset};
};
static_assert(!UpgradableType<MissingLevelType>);

struct MissingAlias : MissingLevelType {
    Quality Level{Quality::Unset};
};
static_assert(!UpgradableType<MissingAlias>);

struct WrongLevelType {
    using IdType = Thing;
    using LevelType = std::string;
    void OnUpgrade() {}
    Thing Id{Thing::Unset};
    LevelType Level{""};
};
static_assert(!UpgradableType<WrongLevelType>);