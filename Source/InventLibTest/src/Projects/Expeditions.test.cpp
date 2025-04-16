#include "CommonTest.h"

#include "InventLib/Projects/Expeditions.h"

constexpr auto epsilon = 0.01F;

TEST(Odds, ToRelativeOdds_With333_Returns333) { 
	auto result = Invent::ToRelativeOdds({.3F, .3F, .3F});
	ASSERT_NEAR(result.Artifact, .3F, epsilon);
	ASSERT_NEAR(result.Success, .3F, epsilon);
	ASSERT_NEAR(result.Nothing, .3F, epsilon);
    ASSERT_NEAR(result.Tragedy, 0.1F, epsilon);
}

TEST(Odds, ToRelativeOdds_With444_Returns442) {
    auto result = Invent::ToRelativeOdds({.4F, .4F, .4F});
    ASSERT_NEAR(result.Artifact, .4F, epsilon);
    ASSERT_NEAR(result.Success, .4F, epsilon);
    ASSERT_NEAR(result.Nothing, .2F, epsilon);
	ASSERT_NEAR(result.Tragedy, 0.0F, epsilon);
}

TEST(Odds, ToRelativeOdds_With555_Returns550) {
	auto result = Invent::ToRelativeOdds({.5F, .5F, .5F});
	ASSERT_NEAR(result.Artifact, .5F, epsilon);
	ASSERT_NEAR(result.Success, .5F, epsilon);
	ASSERT_NEAR(result.Nothing, 0.0F, epsilon);
	ASSERT_NEAR(result.Tragedy, 0.0F, epsilon);
}

TEST(Odds, ToRelativeOdds_With999_Returns910) {
    auto result = Invent::ToRelativeOdds({.9F, .9F, .9F});
    ASSERT_NEAR(result.Artifact, .9F, epsilon);
    ASSERT_NEAR(result.Success, .1F, epsilon);
    ASSERT_NEAR(result.Nothing, 0.0F, epsilon);
    ASSERT_NEAR(result.Tragedy, 0.0F, epsilon);
}

TEST(Odds, ToRelativeOdds_WithGreaterThan100_Returns100) {
    auto result = Invent::ToRelativeOdds({1.1F, 1.1F, 1.1F});
    ASSERT_NEAR(result.Artifact, 1.0F, epsilon);
    ASSERT_NEAR(result.Success, 0.0F, epsilon);
    ASSERT_NEAR(result.Nothing, 0.0F, epsilon);
    ASSERT_NEAR(result.Tragedy, 0.0F, epsilon);
}

TEST(ExpeditionOutcome, GetExpeditionOutcome_1000Times_HasMultipleOutcomes) {
    auto outcomes = std::vector<Invent::ExpeditionOutcome>{};
    for(auto i = 0; i < 1'000; i++) {
        outcomes.push_back(Invent::GetExpeditionOutcome(Invent::Expeditions::CreateTools, false, {0, 1.0F}));
    }

    auto artifact = std::count(outcomes.begin(), outcomes.end(), Invent::ExpeditionOutcome::Artifact);
    auto resources = std::count(outcomes.begin(), outcomes.end(), Invent::ExpeditionOutcome::Resources);
    auto nothing = std::count(outcomes.begin(), outcomes.end(), Invent::ExpeditionOutcome::Nothing);
    auto tragedy = std::count(outcomes.begin(), outcomes.end(), Invent::ExpeditionOutcome::Trajedy);

    ASSERT_GT(artifact, 0);
    ASSERT_GT(resources, 0);
    ASSERT_GT(nothing, 0);
    ASSERT_GT(tragedy, 0);
}