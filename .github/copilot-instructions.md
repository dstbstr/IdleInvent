# Copilot Instructions

## Project Guidelines
- The user prefers sample apps to follow the same UI conventions as the other games: a Ui module with Screens, a landing page, and screen navigation rather than a single inline render function.

## Learning Preferences
- The user prefers guidance, design discussion, and teaching in Ask mode rather than having code implemented for them, so they can learn by doing the implementation themselves.
- For exploratory samples, the user accepts unusual/invalid parameter combinations producing unusual visuals as long as they do not crash, and prefers not to over-engineer validation beyond the sample's learning goals.

## Code Requirements
- For this project, the user prefers readable, simple coordinate arithmetic over defensive handling of impractically extreme INT_MIN/billion-cell cases unless real requirements later justify the added complexity.
- The user prefers unqualified `size_t` and uses a `T` prefix for template parameters generally (including non-type parameters such as `TWidth`/`THeight`), interpreting `T` as 'Template' rather than strictly 'Type'.
- The user generally does not use const for local variables because they find it harms readability and provides little protection; they prefer const for parameters and member functions to express const correctness.