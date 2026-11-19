#include "Iduel/Economy/GeneratorMarshaller.h"

#include <format>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>

namespace {
	using namespace Iduel;
	void SerializeThreshold(const Threshold& threshold, std::ostream& os) {
		os << std::format("t:{} {} {}\n",
			threshold.Index,
			threshold.IncomeMultiplier,
			threshold.SpeedMultiplier);
	}
	void SerializeGenerator(const Generator& generator, std::ostream& os) {
		os << "g:" << std::quoted(generator.Name) << ' '
			<< std::quoted(generator.Income.GetExpression()) << ' '
			<< std::quoted(generator.Cost.GetExpression()) << ' '
			<< generator.TimePerIncome.count() << '\n';
	}
	void SerializeHeader(const GeneratorFile& file, std::ostream& os) {
		os << "h:" << std::quoted(file.Name) << '\n';
	}

	Threshold ParseThreshold(const std::string& line) {
		Threshold threshold{};
		std::istringstream iss(line.substr(2));
		if(!(iss >> threshold.Index 
			>> threshold.IncomeMultiplier 
			>> threshold.SpeedMultiplier)) {
			throw std::invalid_argument("Failed to parse threshold line: " + line);
		}
		if (threshold.IncomeMultiplier <= 0.0 || threshold.SpeedMultiplier <= 0.0) {
			throw std::invalid_argument("Threshold multipliers must be positive");
		}
		if (!std::isfinite(threshold.IncomeMultiplier) || !std::isfinite(threshold.SpeedMultiplier)) {
			throw std::invalid_argument("Threshold multipliers must be finite");
		}
		return threshold;
	}

	Generator ParseGenerator(const std::string& line) {
		std::string name, income, cost;
		BaseTime::rep duration{};
		std::istringstream iss(line.substr(2));

		if(!(iss >> std::quoted(name)
			>> std::quoted(income)
			>> std::quoted(cost)
			>> duration)) {
			throw std::invalid_argument("Failed to parse generator line: " + line);
		}
		if (duration <= 0) throw std::invalid_argument("Duration must be positive");

		Generator result{};
		result.Name = name;
		result.Income.SetExpression(income);
		result.Cost.SetExpression(cost);
		result.TimePerIncome = BaseTime(duration);

		return result;
	}

	GeneratorFile ParseFileHeader(const std::string& line) {
		std::string name;
		std::istringstream iss(line.substr(2));
		if (!(iss >> std::quoted(name))) {
			throw std::invalid_argument("Failed to parse file header line: " + line);
		}
		return GeneratorFile{ .Name = name };
	}
}

namespace Iduel {
	void SerializeGenerators(const GeneratorFile& file, std::ostream& os) {
		SerializeHeader(file, os);
		for(const auto& generator : file.Generators) {
			SerializeGenerator(generator, os);
			for (const auto& threshold : generator.Thresholds) {
				SerializeThreshold(threshold, os);
			}
		}
	}

	GeneratorFile DeserializeGenerators(std::istream& is) {
		GeneratorFile result;
		std::string line;
		while(std::getline(is, line)) {
			if(line.find_first_not_of(" \t\r") == std::string::npos) continue;

			if(line.starts_with("h:")) {
				result = ParseFileHeader(line);
			} else if(line.starts_with("g:")) {
				result.Generators.push_back(ParseGenerator(line));
			} else if(line.starts_with("t:")) {
				if (result.Generators.empty()) throw std::invalid_argument("Threshold found before any generator");
				result.Generators.back().Thresholds.push_back(ParseThreshold(line));
			} else {
				throw std::invalid_argument("Unknown record type: " + line);
			}
		}

		if(is.bad() || (is.fail() && !is.eof())) {
			throw std::runtime_error("Failed to read from stream");
		}
		return result;
	}
}