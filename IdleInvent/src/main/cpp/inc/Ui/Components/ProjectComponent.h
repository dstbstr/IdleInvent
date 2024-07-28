#pragma once

namespace Invent {
    struct Project;
    struct Life;
}

namespace Ui::Components::Project {
    enum struct StartCondition { None, RequireResources};
    void Render(Invent::Life& life, Invent::Project& project, StartCondition startCondition = StartCondition::None);
} // namespace Ui::Components::Project