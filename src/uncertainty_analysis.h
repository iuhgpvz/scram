/*
 * Copyright (C) 2014-2015 Olzhas Rakhimov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/// @file uncertainty_analysis.h
/// Provides functionality for uncertainty analysis
/// with Monte Carlo method.

#ifndef SCRAM_SRC_UNCERTAINTY_ANALYSIS_H_
#define SCRAM_SRC_UNCERTAINTY_ANALYSIS_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "event.h"
#include "probability_analysis.h"
#include "settings.h"

namespace scram {

/// @class UncertaintyAnalysis
/// Uncertainty analysis and statistics
/// for top event or gate probabilities
/// from minimal cut sets
/// and probability distributions of basic events.
class UncertaintyAnalysis : private ProbabilityAnalysis {
 public:
  using BasicEventPtr = std::shared_ptr<BasicEvent>;

  /// Uncertainty analysis
  /// on the fault tree represented by the root gate
  /// with Binary decision diagrams.
  ///
  /// @param[in] root The top event of the fault tree.
  /// @param[in] settings Analysis settings for probability calculations.
  ///
  /// @note This technique does not require cut sets.
  UncertaintyAnalysis(const GatePtr& root, const Settings& settings);

  /// Sets the databases of basic events with probabilities.
  /// Resets the main basic event database
  /// and clears the previous information.
  /// This information is the main source
  /// for calculations and internal indices for basic events.
  ///
  /// @param[in] basic_events The database of basic events in cut sets.
  ///
  /// @note  If not enough information is provided,
  ///        the analysis behavior is undefined.
  void UpdateDatabase(
      const std::unordered_map<std::string, BasicEventPtr>& basic_events);

  /// Performs quantitative analysis on minimal cut sets
  /// containing basic events provided in the databases.
  /// It is assumed that the analysis is called only once.
  ///
  /// @param[in] min_cut_sets Minimal cut sets with string IDs of events.
  ///                         Negative event is indicated by "'not' + id"
  ///
  /// @note  Undefined behavior if analysis called two or more times.
  void Analyze(const std::set< std::set<std::string> >& min_cut_sets) noexcept;

  /// @returns Mean of the final distribution.
  inline double mean() const { return mean_; }

  /// @returns Standard deviation of the final distribution.
  inline double sigma() const { return sigma_; }

  /// @returns Error factor for 95% confidence level.
  inline double error_factor() const { return error_factor_; }

  /// @returns 95% confidence interval of the mean.
  inline const std::pair<double, double>& confidence_interval() const {
    return confidence_interval_;
  }

  /// @returns The distribution histogram.
  const std::vector<std::pair<double, double> >& distribution() const {
    return distribution_;
  }

  /// @returns Quantiles of the distribution.
  const std::vector<double>& quantiles() const { return quantiles_; }

  /// @returns Warnings generated upon analysis.
  inline const std::string warnings() const {
    return ProbabilityAnalysis::warnings();
  }

  /// @returns Analysis time spent on sampling and simulations.
  inline double analysis_time() const { return analysis_time_; }

 private:
  /// Performs Monte Carlo Simulation
  /// by sampling the probability distributions
  /// and providing the final sampled values of the final probability.
  void Sample() noexcept;

  /// Gathers basic events that have distributions.
  ///
  /// @param[out] basic_events The gathered uncertain basic events.
  ///
  /// @todo Mark BDD graph branches that do not need sampling.
  void FilterUncertainEvents(std::vector<int>* basic_events) noexcept;

  /// Calculates statistical values from the final distribution.
  void CalculateStatistics() noexcept;

  std::vector<double> sampled_results_;  ///< Storage for sampled values.
  const Settings kSettings_;  ///< All settings for analysis.
  double mean_;  ///< The mean of the final distribution.
  double sigma_;  ///< The standard deviation of the final distribution.
  double error_factor_;  ///< Error factor for 95% confidence level.
  double analysis_time_;  ///< Time for uncertainty calculations and sampling.
  /// The confidence interval of the distribution.
  std::pair<double, double> confidence_interval_;
  /// The histogram density of the distribution with lower bounds and values.
  std::vector<std::pair<double, double> > distribution_;
  /// The quantiles of the distribution.
  std::vector<double> quantiles_;
};

}  // namespace scram

#endif  // SCRAM_SRC_UNCERTAINTY_ANALYSIS_H_
