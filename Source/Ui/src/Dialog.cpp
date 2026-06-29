#include "Ui/Dialog.h"

#include <Instrumentation/Logging.h>
#include <Manage/TickManager.h>
#include <Utilities/Handle.h>
#include <Ui/UiUtil.h>

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <string>
#include <vector>

namespace {
	enum struct State : u8 { Hidden, Revealing, Revealed };

	// All reset on every Show().
	State              s_State{State::Hidden};
	Ui::Dialog::Config s_Config{};
	std::vector<std::string> s_Pages{};
	size_t             s_CurrentPage{0};
	size_t             s_RevealedChars{0};
	f32                s_RevealAccumulator{0.f};

	// Cached box size in pixels, computed at Show() against the live viewport.
	ImVec2             s_BoxSizePx{};

	// Released-and-re-pressed click detection. Prevents the click that triggers Show()
	// (or a page advance) from being read as the *next* state's input.
	bool s_MouseWasUpSinceTransition{false};

	// External Hide() requests can't call CloseCurrentPopup directly (must be inside the
	// popup scope). Flag here, honor it the next time Render() enters the popup.
	bool s_PendingClose{false};

	// Show() sets this so Render() can call OpenPopup at the same ID scope as
	// BeginPopupModal. Calling OpenPopup from Show() would scope the popup to whatever
	// window the caller happened to be inside, and BeginPopupModal wouldn't find it.
	bool s_PendingOpen{false};

	std::vector<ScopedHandle> s_TickHandles{};

	constexpr f32 BoxBottomPadFrac = 0.04f; // distance from bottom edge as fraction of viewport height

	constexpr ImGuiWindowFlags BoxFlags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoFocusOnAppearing;

	// Greedily pack words into lines that fit wrapWidth at the currently-pushed font,
	// then greedily pack lines into pages that fit maxLines. Words longer than wrapWidth
	// get their own line (they'll visibly overflow, but that won't happen with real text).
	std::vector<std::string> SplitIntoPages(const std::string& text, f32 wrapWidth, size_t maxLines) {
		std::vector<std::string> pages;
		if(text.empty() || wrapWidth <= 0.f || maxLines == 0) {
			pages.push_back(text);
			return pages;
		}

		// Step 1: build a list of words (each carrying a trailing space if present in the
		// source). Newlines in the source act as forced line breaks.
		struct Token { std::string Text; bool ForceBreak; };
		std::vector<Token> tokens;
		std::string current;
		for(size_t i = 0; i < text.size(); ++i) {
			const char c = text[i];
			if(c == '\n') {
				if(!current.empty()) { tokens.push_back({std::move(current), false}); current.clear(); }
				tokens.push_back({"", true});
			} else if(c == ' ') {
				current.push_back(c);
				tokens.push_back({std::move(current), false});
				current.clear();
			} else {
				current.push_back(c);
			}
		}
		if(!current.empty()) tokens.push_back({std::move(current), false});

		// Step 2: pack tokens into lines that fit wrapWidth.
		std::vector<std::string> lines;
		std::string line;
		for(const auto& tok : tokens) {
			if(tok.ForceBreak) {
				lines.push_back(std::move(line));
				line.clear();
				continue;
			}
			const auto candidate = line + tok.Text;
			const auto candidateWidth = ImGui::CalcTextSize(candidate.c_str()).x;
			if(candidateWidth <= wrapWidth || line.empty()) {
				line = std::move(candidate);
			} else {
				lines.push_back(std::move(line));
				line = tok.Text;
			}
		}
		if(!line.empty()) lines.push_back(std::move(line));

		// Step 3: pack lines into pages of <= maxLines.
		std::string page;
		size_t lineCount = 0;
		for(auto& l : lines) {
			if(lineCount == maxLines) {
				pages.push_back(std::move(page));
				page.clear();
				lineCount = 0;
			}
			if(!page.empty()) page.push_back('\n');
			page += l;
			++lineCount;
		}
		if(!page.empty()) pages.push_back(std::move(page));

		if(pages.empty()) pages.push_back({}); // never return empty list
		return pages;
	}

	void BeginPageReveal(size_t page) {
		s_CurrentPage = page;
		s_RevealedChars = 0;
		s_RevealAccumulator = 0.f;
		s_State = s_Pages.at(page).empty() ? State::Revealed : State::Revealing;
		s_MouseWasUpSinceTransition = false;
	}

	void TickReveal(BaseTime elapsed) {
		if(s_State != State::Revealing) return;

		const f32 dtSec = static_cast<f32>(elapsed.count()) * 0.001f;
		s_RevealAccumulator += s_Config.CharsPerSecond * dtSec;

		const auto totalChars = s_Pages.at(s_CurrentPage).size();
		const auto whole      = static_cast<size_t>(s_RevealAccumulator);
		if(whole == 0) return;

		s_RevealAccumulator -= static_cast<f32>(whole);
		s_RevealedChars = std::min(totalChars, s_RevealedChars + whole);
		if(s_RevealedChars >= totalChars) {
			s_State = State::Revealed;
			s_RevealAccumulator = 0.f;
		}
	}
}

namespace Ui::Dialog {
	bool Initialize() {
		TickManager::Get().Register(s_TickHandles, TickReveal);
		return true;
	}

	void ShutDown() {
		s_TickHandles.clear();
		s_State = State::Hidden;
		s_Config = {};
		s_Pages.clear();
		s_CurrentPage = 0;
		s_RevealedChars = 0;
		s_RevealAccumulator = 0.f;
		s_PendingClose = false;
		s_PendingOpen = false;
	}

	void Show(Config config) {
		DR_ASSERT_MSG(s_State == State::Hidden, "Dialog::Show called while another dialog is active");
		s_Config = std::move(config);
		s_PendingClose = false;

		// Resolve box pixel size against the live viewport so Show-time splitting
		// matches what Render will draw.
		const auto* viewport = ImGui::GetMainViewport();
		s_BoxSizePx = ImVec2{
			viewport->WorkSize.x * s_Config.SizeFrac.x,
			viewport->WorkSize.y * s_Config.SizeFrac.y
		};

		// Push the configured font so CalcTextSize measures with the right metrics,
		// then pop after splitting.
		ImGui::PushFont(GetFont(s_Config.Font));
		const auto padding = ImGui::GetStyle().WindowPadding;
		const f32 wrapWidth = std::max(1.f, s_BoxSizePx.x - padding.x * 2.f);
		const f32 lineHeight = ImGui::GetTextLineHeightWithSpacing();
		// Reserve the last line for the page indicator + click prompt.
		const f32 textHeight = std::max(lineHeight, s_BoxSizePx.y - padding.y * 2.f - lineHeight);
		const size_t maxLines = std::max<size_t>(1u, static_cast<size_t>(textHeight / lineHeight));
		s_Pages = SplitIntoPages(s_Config.Text, wrapWidth, maxLines);
		ImGui::PopFont();

		BeginPageReveal(0);
		// Defer OpenPopup to Render so the popup ID lives in the same scope as BeginPopupModal.
		s_PendingOpen = true;
	}

	void Hide() {
		// Can't call CloseCurrentPopup here -- we're (probably) outside the popup scope.
		// Mark intent; the next Render() pass will close it from inside.
		s_PendingClose = true;
	}

	bool IsActive() {
		return s_State != State::Hidden;
	}

	void Render() {
		if(s_State == State::Hidden) return;

		// Open the popup in the same ID scope as BeginPopupModal. Show() defers this so
		// it doesn't accidentally scope to whatever window the caller is currently inside.
		if(s_PendingOpen) {
			s_PendingOpen = false;
			ImGui::OpenPopup("##Dialog");
		}

		const auto* viewport = ImGui::GetMainViewport();
		const f32 bottomPadPx = viewport->WorkSize.y * BoxBottomPadFrac;
		const ImVec2 boxPos{
			viewport->WorkPos.x + (viewport->WorkSize.x - s_BoxSizePx.x) * 0.5f,
			viewport->WorkPos.y + viewport->WorkSize.y - s_BoxSizePx.y - bottomPadPx
		};

		ImGui::SetNextWindowPos(boxPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(s_BoxSizePx, ImGuiCond_Always);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, s_Config.BgColor);
		ImGui::PushFont(GetFont(s_Config.Font));
		ImGui::PushStyleColor(ImGuiCol_Text, s_Config.Color);

		// BeginPopupModal creates an input-blocking backdrop automatically -- ImGui handles
		// z-order and prevents clicks from reaching widgets in underlying windows. This is
		// the purpose-built modal feature; we no longer need a manual blocker window.
		if(ImGui::BeginPopupModal("##Dialog", nullptr, BoxFlags)) {
			// Honor a pending external Hide() before drawing anything that depends on s_State.
			if(s_PendingClose) {
				s_PendingClose = false;
				s_State = State::Hidden;
				s_Pages.clear();
				s_CurrentPage = 0;
				s_RevealedChars = 0;
				s_RevealAccumulator = 0.f;
				ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
				ImGui::PopStyleColor(); // Text
				ImGui::PopFont();
				ImGui::PopStyleColor(); // WindowBg
				return;
			}

			const auto& page = s_Pages.at(s_CurrentPage);
			ImGui::PushTextWrapPos(0.f);
			ImGui::TextUnformatted(page.data(), page.data() + s_RevealedChars);
			ImGui::PopTextWrapPos();

			// Bottom-right indicator when fully revealed. Format: "(p/n) [next]" or
			// "(p/n) [close]" depending on whether more pages remain.
			if(s_State == State::Revealed) {
				const bool hasNext = (s_CurrentPage + 1) < s_Pages.size();
				std::array<char, 64> buf{};
				std::snprintf(buf.data(), buf.size(), "(%zu/%zu) %s",
							  s_CurrentPage + 1, s_Pages.size(),
							  hasNext ? "[next]" : "[close]");
				const auto textSize = ImGui::CalcTextSize(buf.data());
				ImGui::SetCursorPos(ImVec2{
					ImGui::GetWindowContentRegionMax().x - textSize.x,
					ImGui::GetWindowContentRegionMax().y - textSize.y
				});
				ImGui::TextUnformatted(buf.data());
			}

			// Click handling inside the popup scope so CloseCurrentPopup is valid. The
			// released-since-transition gate stops the click that triggered Show() or a
			// page advance from bleeding into the next state.
			const bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
			if(!mouseDown) s_MouseWasUpSinceTransition = true;

			if(s_MouseWasUpSinceTransition && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				switch(s_State) {
					using enum State;
					case Revealing:
						s_RevealedChars = s_Pages.at(s_CurrentPage).size();
						s_State = Revealed;
						s_RevealAccumulator = 0.f;
						s_MouseWasUpSinceTransition = false;
						break;
					case Revealed:
						if(s_CurrentPage + 1 < s_Pages.size()) {
							BeginPageReveal(s_CurrentPage + 1);
						} else {
							s_State = Hidden;
							s_Pages.clear();
							s_CurrentPage = 0;
							s_RevealedChars = 0;
							s_RevealAccumulator = 0.f;
							ImGui::CloseCurrentPopup();
						}
						break;
					case Hidden:
						break;
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopStyleColor(); // Text
		ImGui::PopFont();
		ImGui::PopStyleColor(); // WindowBg
	}
}
