# Global Agent Instructions

## Output Rules

- Be terse. No filler, no preamble, no sign-offs.
- Do not produce planning documents, spec sheets, or multi-section outlines unless I explicitly request one.
- When explaining something, use 1-3 sentences. If I want more detail, I'll ask.
- No bullet-point walls. If a list has more than 5 items, you're probably over-explaining.
- Don't narrate what you're about to do, just do it. Wrong: "I'll start by reading the file then I'll look for the bug..." Right: *[read file]*
- Code changes are better than prose about code changes. Showing a diff is more valuable than a paragraph description about the diff.
- If a plan is requested, keep it simple and flat, not nested with sections and subsections.

## Working Style

- Be direct, skip preamble.
- When editing files, read them first. Don't guess at the contents, as they may have changed.
- Prefer small, incremental changes. If changes are more than 50-100 lines of code, they're probably too big.
- If something is ambiguous, ask.

## Code Style

- Match the existing style of whatever file you're editing.
- If there's a linter config, respect it.
- Prefer readability and maintainability over cleverness.

## When You Are Stuck

- Don't loop on a failed approach, ask for help.
- If a CLI command fails, read the error before retrying.
- If you need additional context, ask - don't confabulate file contents.


# Copilot Instructions

## General Guidelines
- In Ask mode, the user’s goal is to learn. Provide small, focused code changes and explanations; code samples are welcome but avoid broad implementations so the user can work through the change.

## Project Guidelines
- The user prefers sample apps to follow the same UI conventions as the other games: a Ui module with Screens, a landing page, and screen navigation rather than a single inline render function.

## Learning Preferences
- The user prefers guidance, design discussion, and teaching in Ask mode rather than having code implemented for them, so they can learn by doing the implementation themselves.
- For exploratory samples, the user accepts unusual/invalid parameter combinations producing unusual visuals as long as they do not crash, and prefers not to over-engineer validation beyond the sample's learning goals.

## Code Requirements
- For this project, the user prefers readable, simple coordinate arithmetic over defensive handling of impractically extreme INT_MIN/billion-cell cases unless real requirements later justify the added complexity.
- The user prefers unqualified `size_t` and uses a `T` prefix for template parameters generally (including non-type parameters such as `TWidth`/`THeight`), interpreting `T` as 'Template' rather than strictly 'Type'.
- The user generally does not use const for local variables because they find it harms readability and provides little protection; they prefer const for parameters and member functions to express const correctness.

## Platform Considerations
- IdleInvent is primarily focused on mobile applications, so desktop window-resize handling is generally not a priority unless a concrete requirement arises.