#pragma once

namespace Walker {
	class UpgradeKey;
	class WalkerRates;

	namespace WalkerUi {
		bool RenderUpgradeRow(const char* label, const UpgradeKey& key, WalkerRates& rates);
	}
}