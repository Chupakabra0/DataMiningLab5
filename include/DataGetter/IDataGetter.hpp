#pragma once
#include <vector>
#include <Eigen/Dense>

using Eigen::Dynamic;
using Eigen::MatrixXd;

struct IDataGetter {
public:
	virtual ~IDataGetter() = default;

	[[nodiscard]] virtual std::vector<double> GetDataX1() const = 0;
	[[nodiscard]] virtual std::vector<double> GetDataX2() const = 0;
	[[nodiscard]] virtual std::vector<double> GetDataX3() const = 0;

	[[nodiscard]] virtual double GetFreedom() const = 0;
};
