#pragma once

#include "Iduel/Economy/Generator.h"

#include <iosfwd>
#include <vector>

namespace Iduel {
	struct GeneratorFile {
		std::string Name{};
		std::vector<Generator> Generators{};
	};

	void SerializeGenerators(const GeneratorFile& file, std::ostream& os);
	GeneratorFile DeserializeGenerators(std::istream& is);
}