#include "GhostHunter/Investigation/Investigation.h"

namespace GhostHunter {
	void Investigation::Update(BaseTime elapsed) { 
		m_Elapsed += elapsed;
	}
	bool Investigation::IsComplete() const {
		return m_Elapsed >= m_Duration;
	}
}
