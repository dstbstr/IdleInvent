#pragma once

namespace Walker {
	class Wallet {
	public:
		constexpr Money GetBalance() const { return m_Balance; }
		constexpr bool CanAfford(Money cost) const { return cost >= Zero && m_Balance >= cost;}
		constexpr void Add(Money amount) { 
			if(amount < Zero) return;
			m_Balance += amount; 
		}
		constexpr bool Spend(Money amount) { 
			if(!CanAfford(amount)) return false;

			m_Balance -= amount; 
			return true;
		}

	private:
		Money m_Balance{};
	};
}