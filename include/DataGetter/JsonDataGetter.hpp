#pragma once
#include "IDataGetter.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

class JsonDataGetter : public IDataGetter {
private:
	static constexpr auto* X1_STR		= "x1";
	static constexpr auto* X2_STR		= "x2";
	static constexpr auto* X3_STR		= "x3";
	static constexpr auto* FREEDOM_STR  = "freedom";

public:
	explicit JsonDataGetter(std::fstream& file) : file_(file) {
		this->ReadFile();
	}
	;
	JsonDataGetter(const JsonDataGetter&) = delete;

	JsonDataGetter(JsonDataGetter&&) noexcept = default;

	~JsonDataGetter() override = default;

	[[nodiscard]] std::vector<double> GetDataX1() const override {
		return this->json_[JsonDataGetter::X1_STR].get<std::vector<double>>();
	}

	[[nodiscard]] std::vector<double> GetDataX2() const override {
		return this->json_[JsonDataGetter::X2_STR].get<std::vector<double>>();
	}

	[[nodiscard]] std::vector<double> GetDataX3() const override {
		return this->json_[JsonDataGetter::X3_STR].get<std::vector<double>>();
	}

	[[nodiscard]] double GetFreedom() const override {
		return this->json_[JsonDataGetter::FREEDOM_STR].get<double>();
	}

private:
	std::reference_wrapper<std::fstream> file_;
	nlohmann::json json_{};

	void ReadFile() {
		if (this->file_.get().is_open()) {
			this->file_ >> this->json_;
		}
	}
};
