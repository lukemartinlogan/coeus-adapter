/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Coeus-adapter. The full Coeus-adapter copyright      *
 * notice, including terms governing use, modification, and redistribution,  *
 * is contained in the COPYING file, which can be found at the top directory.*
 * If you do not have access to the file, you may request a copy             *
 * from scslab@iit.edu.                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef COEUS_INCLUDE_COEUS_HERMESENGINE_H_
#define COEUS_INCLUDE_COEUS_HERMESENGINE_H_

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <adios2.h>
#include <adios2/core/VariableDerived.h>
#include <adios2/engine/plugin/PluginEngineInterface.h>
#include <common/ErrorCodes.h>
#include <comms/Bucket.h>
#include <comms/Hermes.h>
#include <comms/MPI.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

#include "ContainerManager.h"
#include "adios2/helper/adiosType.h"
#include "coeus/MetadataSerializer.h"
#include "coeus_mdm/coeus_mdm_client.h"
#include "common/DbOperation.h"
#include "common/SQlite.h"
#include "common/Timer.h"
#include "common/Tracer.h"
#include "common/VariableMetadata.h"
#include "common/YAMLParser.h"
#include "common/globalVariable.h"
#include "rankConsensus/rankConsensus_client.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace coeus {

class HermesEngine : public adios2::plugin::PluginEngineInterface {
 public:
  std::shared_ptr<coeus::IHermes> Hermes;
  std::string uid;
  SQLiteWrapper *db;
  std::string db_file;
  std::string adiosOutput;
  int lookahead;
  int index = 0;
  chi::coeus_mdm::Client client;
  int num_layers = 4;
  int ppn;
  int limit = 0;
  chi::rankConsensus::Client rank_consensus;
  //  FileLock* lock;
  //  DbQueueWorker* db_worker;
  GlobalVariable globalData;
  /** Construct the HermesEngine */
  HermesEngine(adios2::core::IO &io,  // NOLINT
               const std::string &name, const adios2::Mode mode,
               adios2::helper::Comm comm);

  HermesEngine(std::shared_ptr<coeus::IHermes> h,  // NOLINT
               std::shared_ptr<coeus::MPI> mpi, adios2::core::IO &io,
               const std::string &name, const adios2::Mode mode,
               adios2::helper::Comm comm);

  /** Destructor */
  ~HermesEngine() override;

  /**
   * Define the beginning of a step. A step is typically the offset from
   * the beginning of a file. It is measured as a size_t.
   *
   * Logically, a "step" represents a snapshot of the data at a specific time,
   * and can be thought of as a frame in a video or a snapshot of a simulation.
   * */

  adios2::StepStatus BeginStep(adios2::StepMode mode,
                               const float timeoutSeconds = -1.0) override;

  /** Define the end of a step */
  void EndStep() override;

  /**
   * Returns the current step
   * */
  size_t CurrentStep() const final;

  /** Execute all deferred puts */
  void PerformPuts() override { engine_logger->info("rank {}", rank); }

  /** Execute all deferred gets */
  void PerformGets() override { engine_logger->info("rank {}", rank); }

  bool VariableMinMax(const adios2::core::VariableBase &, const size_t Step,
                      adios2::MinMaxStruct &MinMax) override;

 private:
  bool open = false;

  int currentStep = 0;
  int total_steps = -1;
  int reader_get_time = 0;
  int compare_time = 0;
  int inintial_time = 0;
  int begin_step_time = 0;
  int compute_derived_time = 0;
  int put_time = 0;
  int flag1 = 0;
  int flag2 = 0;
  int flag3 = 0;
  int flag4 = 0;
  int flag5 = 0;
  int flag6 = 0;
  int flag7 = 0;

  //  std::shared_ptr<coeus::MPI> mpiComm;
  uint rank;
  int comm_size;

  YAMLMap variableMap;
  YAMLMap operationMap;

  std::vector<std::string> listOfVars;

  std::shared_ptr<spdlog::logger> engine_logger;
  std::shared_ptr<spdlog::logger> meta_logger_put;
  std::shared_ptr<spdlog::logger> meta_logger_get;
  void IncrementCurrentStep();

  template <typename T>
  T *SelectUnion(adios2::PrimitiveStdtypeUnion &u);

  template <typename T>
  void ElementMinMax(adios2::MinMaxStruct &MinMax, void *element);

  void LoadMetadata();

  void DefineVariable(const VariableMetadata &variableMetadata);
  // adios.defineVariable()
  // adios.put(variable) -> PutDefereed()
  // adios.put( variable, "U+V") -> CalculateDerivedQuantity() -> Put(varaible)
 protected:
  /** Initialize (wrapper around Init_)*/
  void Init() override { Init_(); }

  /** Actual engine initialization */
  void Init_();

  /** Close a particular transport */
  void DoClose(const int transportIndex = -1) override;

  /** Place data in Hermes */
  template <typename T>
  void DoPutSync_(const adios2::core::Variable<T> &variable, const T *values);

  /** Place data in Hermes asynchronously */
  template <typename T>
  void DoPutDeferred_(const adios2::core::Variable<T> &variable,
                      const T *values);

  void ComputeDerivedVariables();

  template <typename T>
  DbOperation generateMetadata(adios2::core::Variable<T> variable);
  DbOperation generateMetadata(adios2::core::VariableDerived variable,
                               float *values, int total_count);

  template <typename T>
  void PutDerived(adios2::core::VariableDerived variable, T *values);

  /** Get data from Hermes (sync) */
  template <typename T>
  void DoGetSync_(const adios2::core::Variable<T> &variable, T *values);
  //  void HermesEngine::DoGetDerivedVariableSync_(const
  //  adios2::core::Variable<T> &variable,
  //                                                     T *values);
  /** Get data from Hermes (async) */
  template <typename T>
  void DoGetDeferred_(const adios2::core::Variable<T> &variable, T *values);
  //  void DoGetDerivedVariableDeferred_(const adios2::core::Variable<T>
  //  &variable,
  //                            T *values);
  /** Calls to support Adios native queries */
  void ApplyElementMinMax(adios2::MinMaxStruct &MinMax, adios2::DataType Type,
                          void *Element);

  bool Demote(int step);
  bool Promote(int step);
  /**
   * Declares DoPutSync and DoPutDeferred for a number of predefined types.
   * ADIOS2_FOREACH_STDTYPE_1ARG is a macro which iterates over every
   * known type (e.g., int, double, float, etc).
   * */

#define declare_type(T)                                                     \
  void DoPutSync(adios2::core::Variable<T> &variable, const T *values)      \
      override {                                                            \
    DoPutSync_(variable, values);                                           \
  }                                                                         \
  void DoPutDeferred(adios2::core::Variable<T> &variable, const T *values)  \
      override {                                                            \
    DoPutDeferred_(variable, values);                                       \
  }                                                                         \
  void DoGetSync(adios2::core::Variable<T> &variable, T *values) override { \
    DoGetSync_(variable, values);                                           \
  }                                                                         \
  void DoGetDeferred(adios2::core::Variable<T> &variable, T *values)        \
      override {                                                            \
    DoGetDeferred_(variable, values);                                       \
  }
  ADIOS2_FOREACH_STDTYPE_1ARG(declare_type)
#undef declare_type
};

}  // namespace coeus

#endif  // COEUS_INCLUDE_COEUS_HERMESENGINE_H_
