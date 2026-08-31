#include "Pets/Combat/HuntRules.h"
#include "Pets/Combat/HuntTypes.h"

namespace Pets {
    using namespace Combat;

    struct HuntRulesTest : public ::testing::Test {
        Inventory inventory;
        Roster<HuntCombatant> roster;
        std::unique_ptr<HuntRules> rules{};

        CombatantId preyId{};
        PreyStats preyStats{
            .CurrentHp = 10,
            .MaxHp = 50,
            .Armor = 5,
            .Dodge = 0.1f,
            .CaptureHpLevel = 0.2f,
            .CaptureChance = 0.5f,
            .Gold = 100,
            .Xp = 50,
            .FleeTime = OneMinute,
        };
        HuntCombatant prey{.ActionInterval = OneSecond, .Stats = preyStats};

        CombatantId deadId{};
        PreyStats deadStats{.CurrentHp = 0};
        HuntCombatant deadPrey{.ActionInterval = OneSecond, .Stats = deadStats};

        CombatantId partyId{};
        PartyStats partyStats{
            .Attack = 20,
            .Piercing = 5,
        };
        HuntCombatant party{.ActionInterval = OneSecond, .Stats = partyStats};

        void SetUp() override {
            rules = std::make_unique<HuntRules>(inventory);
            partyId = roster.Add(Social::ToFactionId(HuntFaction::Party), party);
            preyId = roster.Add(Social::ToFactionId(HuntFaction::Prey), prey);
            deadId = roster.Add(Social::ToFactionId(HuntFaction::Prey), deadPrey);
        }
        void TearDown() override { rules.reset(); }
    };

    TEST_F(HuntRulesTest, IsDisabled_WithParty_ReturnsFalse) { ASSERT_FALSE(rules->IsDisabled(roster, partyId)); }
    TEST_F(HuntRulesTest, IsDisabled_WithLivingPrey_ReturnsFalse) { ASSERT_FALSE(rules->IsDisabled(roster, preyId)); }
    TEST_F(HuntRulesTest, IsDisabled_WithDeadPrey_ReturnsTrue) { ASSERT_TRUE(rules->IsDisabled(roster, deadId)); }
    TEST_F(HuntRulesTest, IsDisabled_WithUnknownCombatant_ReturnsTrue) {
        CombatantId id{123};
        ASSERT_TRUE(rules->IsDisabled(roster, id));
    }

    TEST_F(HuntRulesTest, CanSubmit_WithDisabled_ReturnsFalse) {
        ActionRequest action{.Kind = ActionRequestKind::Attack, .Target = preyId};
        ASSERT_FALSE(rules->CanSubmit(roster, deadId, action));
    }

    TEST_F(HuntRulesTest, CanSubmit_WithUnknownTarget_ReturnsFalse) {
        ActionRequest action{.Kind = ActionRequestKind::Attack, .Target = CombatantId{999}};
        ASSERT_FALSE(rules->CanSubmit(roster, partyId, action));
    }

    TEST_F(HuntRulesTest, CanSubmit_WithDeadTarget_ReturnsFalse) {
        ActionRequest action{.Kind = ActionRequestKind::Attack, .Target = deadId};
        ASSERT_FALSE(rules->CanSubmit(roster, partyId, action));
    }

    TEST_F(HuntRulesTest, Update_WithNoFlee_ReturnsNothing) {
        auto result = rules->Update(roster, OneSecond);
        ASSERT_FALSE(result.EncounterFinished);
        ASSERT_TRUE(result.Events.empty());
    }

    TEST_F(HuntRulesTest, Update_WithFlee_ReturnsFledAndEncounterFinished) {
        auto result = rules->Update(roster, OneHour);
        ASSERT_TRUE(result.EncounterFinished);
        ASSERT_EQ(result.Events.size(), 1);
        ASSERT_EQ(result.Events[0].Kind, ActionResultKind::PreyFled);
    }

    TEST_F(HuntRulesTest, Resolve_WithAttack_AppliesDamage) {
        auto initialHp = preyStats.CurrentHp;
        auto attack = ActionRequest{.Kind = ActionRequestKind::Attack, .Target = preyId};
        rules->Resolve(roster, partyId, attack);

        auto& updatedPrey = roster.Get(preyId);
        auto* updatedStats = std::get_if<PreyStats>(&updatedPrey.Stats);
        ASSERT_NE(updatedStats, nullptr);
        ASSERT_EQ(updatedStats->CurrentHp, initialHp - partyStats.Attack);
    }

    TEST_F(HuntRulesTest, Resolve_WithCapture_EndsEncounter) {
        auto capture = ActionRequest{.Kind = ActionRequestKind::Capture, .Target = preyId};
        auto result = rules->Resolve(roster, partyId, capture);
        ASSERT_TRUE(result.EncounterFinished);
        ASSERT_FALSE(result.Events.empty());
        ASSERT_EQ(result.Events[0].Kind, ActionResultKind::Captured);
    }

    TEST_F(HuntRulesTest, Resolve_WithFlee_EndsEncounter) {
        auto flee = ActionRequest{.Kind = ActionRequestKind::Flee, .Target = preyId};
        auto result = rules->Resolve(roster, preyId, flee);
        ASSERT_TRUE(result.EncounterFinished);
        ASSERT_FALSE(result.Events.empty());
        ASSERT_EQ(result.Events[0].Kind, ActionResultKind::PreyFled);
    }

    TEST_F(HuntRulesTest, Resolve_WithDefend_IncreasesArmor) {
        auto defend = ActionRequest{.Kind = ActionRequestKind::Defend, .Target = preyId};
        auto result = rules->Resolve(roster, preyId, defend);
        ASSERT_EQ(result.Events[0].Kind, ActionResultKind::Defended);
        auto& updatedPrey = roster.Get(preyId);
        auto* updatedStats = std::get_if<PreyStats>(&updatedPrey.Stats);
        ASSERT_NE(updatedStats, nullptr);
        ASSERT_TRUE(updatedStats->Armor > preyStats.Armor);
    }

    TEST_F(HuntRulesTest, Resolve_WithHide_IncreasesDodge) {
        auto dodge = ActionRequest{.Kind = ActionRequestKind::Hide, .Target = preyId};
        auto result = rules->Resolve(roster, preyId, dodge);
        ASSERT_EQ(result.Events[0].Kind, ActionResultKind::Hidden);
        auto& updatedPrey = roster.Get(preyId);
        auto* updatedStats = std::get_if<PreyStats>(&updatedPrey.Stats);
        ASSERT_NE(updatedStats, nullptr);
        ASSERT_TRUE(updatedStats->Dodge > preyStats.Dodge);
    }

    struct ResolveItemTest : public HuntRulesTest {
        ActionRequest PreyItem(CombatItemKind item) {
            return ActionRequest{
                .Kind = ActionRequestKind::Item, .Target = preyId, .Context = ItemContext{.ItemId = item}
            };
        }
        ActionRequest PartyItem(CombatItemKind item) {
            return ActionRequest{
                .Kind = ActionRequestKind::Item, .Target = partyId, .Context = ItemContext{.ItemId = item}
            };
        }
    };

    TEST_F(ResolveItemTest, Resolve_WithNoInventory_DoesNothing) {
        auto action = PreyItem(CombatItemKind::Distraction);
        auto result = rules->Resolve(roster, partyId, action);
        ASSERT_TRUE(result.Events.empty());
    }

    TEST_F(ResolveItemTest, Resolve_WithInventory_ReducesInventory) {
        inventory.Add(CombatItemKind::Distraction, 1);
        auto action = PreyItem(CombatItemKind::Distraction);
        rules->Resolve(roster, partyId, action);

        ASSERT_EQ(0, inventory.Count(CombatItemKind::Distraction));
    }

    TEST_F(ResolveItemTest, Resolve_WithInventory_IncludesItemInResult) {
        inventory.Add(CombatItemKind::Distraction, 1);
        auto action = PreyItem(CombatItemKind::Distraction);
        auto result = rules->Resolve(roster, partyId, action);

        ASSERT_EQ(1, result.Events.size());
        auto event = result.Events[0];
        ASSERT_EQ(event.Kind, ActionResultKind::ItemUsed);

        auto* itemContext = std::get_if<CombatItemKind>(&event.Context);
        ASSERT_NE(itemContext, nullptr);
        ASSERT_EQ(*itemContext, CombatItemKind::Distraction);
    }

    TEST_F(ResolveItemTest, Resolve_WithDistraction_IncreasesFleeTime) {
        inventory.Add(CombatItemKind::Distraction, 1);

        auto initialFleeTime = preyStats.FleeTime;
        auto action = PreyItem(CombatItemKind::Distraction);
        auto result = rules->Resolve(roster, partyId, action);

        auto& updatedPrey = roster.Get(preyId);
        auto* updatedStats = std::get_if<PreyStats>(&updatedPrey.Stats);
        ASSERT_NE(updatedStats, nullptr);
        ASSERT_TRUE(updatedStats->FleeTime > initialFleeTime);
    }

    TEST_F(ResolveItemTest, Resolve_WithNet_IncreasesCaptureChance) {
        inventory.Add(CombatItemKind::Net, 1);
        auto initialCaptureChance = preyStats.CaptureChance;
        auto action = PreyItem(CombatItemKind::Net);
        auto result = rules->Resolve(roster, partyId, action);

        auto& updatedPrey = roster.Get(preyId);
        auto* updatedStats = std::get_if<PreyStats>(&updatedPrey.Stats);
        ASSERT_NE(updatedStats, nullptr);
        ASSERT_TRUE(updatedStats->CaptureChance > initialCaptureChance);
    }

    TEST_F(ResolveItemTest, Resolve_WithSpeedPotion_IncreasesSpeed) {
        inventory.Add(CombatItemKind::SpdPotion, 1);
        auto action = PartyItem(CombatItemKind::SpdPotion);
        auto result = rules->Resolve(roster, partyId, action);

        ASSERT_EQ(1, result.ScheduleChanges.size());
        auto change = result.ScheduleChanges[0];
        
        ASSERT_EQ(change.Combatant, partyId);
        ASSERT_TRUE(change.SpeedModifier.Apply(1.0f) > 1.0f);
    }

    TEST_F(ResolveItemTest, Resolve_WithAtkPotion_IncreasesAttack) {
        inventory.Add(CombatItemKind::AtkPotion, 1);
        auto initialAttack = partyStats.Attack;
        auto action = PartyItem(CombatItemKind::AtkPotion);
        auto result = rules->Resolve(roster, partyId, action);
        auto& updatedParty = roster.Get(partyId);
        auto* updatedStats = std::get_if<PartyStats>(&updatedParty.Stats);
        ASSERT_NE(updatedStats, nullptr);
        ASSERT_TRUE(updatedStats->Attack > initialAttack);
    }

    TEST_F(ResolveItemTest, Resolve_WithPiercePotion_IncreasesPierce) {
        inventory.Add(CombatItemKind::PiercePotion, 1);
        auto initialPiercing = partyStats.Piercing;
        auto action = PartyItem(CombatItemKind::PiercePotion);
        auto result = rules->Resolve(roster, partyId, action);
        auto& updatedParty = roster.Get(partyId);
        auto* updatedStats = std::get_if<PartyStats>(&updatedParty.Stats);
        ASSERT_NE(updatedStats, nullptr);
        ASSERT_TRUE(updatedStats->Piercing > initialPiercing);
    }
} // namespace Pets