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

#include "coeus/HermesEngine.h"

namespace coeus {
/**
 * Construct the HermesEngine.
 *
 * This is a wrapper around Init_, which performs the main custom
 * initialization of the engine. The PluginEngineInterface will store
 * the "io" variable in the "m_IO" variable.
 * */



HermesEngine::HermesEngine(adios2::core::IO &io,//NOLINT
                           const std::string &name,
                           const adios2::Mode mode,
                           adios2::helper::Comm comm)
    : adios2::plugin::PluginEngineInterface(io, name, mode, comm.Duplicate()) {
  Hermes = std::make_shared<coeus::Hermes>();
  //  mpiComm = std::make_shared<coeus::MPI>(comm.Duplicate());
  Init_();
  TRACE_FUNC("Init HermesEngine");
  engine_logger->info("rank {} with name {} and mode {}", rank, name, adios2::ToString(mode));


}

/**
 * Test initializer
 * */
HermesEngine::HermesEngine(std::shared_ptr<coeus::IHermes> h,
                           std::shared_ptr<coeus::MPI> mpi,
                           adios2::core::IO &io, const std::string &name,
                           const adios2::Mode mode, adios2::helper::Comm comm)
    : adios2::plugin::PluginEngineInterface(io, name, mode, comm.Duplicate()) {
  Hermes = h;
  Init_();

  engine_logger->info("rank {} with name {} and mode {}", rank, name, adios2::ToString(mode));

}

/**
 * Initialize the engine.
 * */
void HermesEngine::Init_() {
   auto start_time_log = std::chrono::high_resolution_clock::now();
  // initiate the trace manager
   std::random_device rd;  // Obtain a random seed
    std::mt19937 gen(rd()); // Mersenne Twister generator
    std::uniform_int_distribution<> dis(1, 10000);

    // Generate a random number
    int randomNumber = dis(gen);

    // Step 2: Convert the random number to a string
    std::string randomNumberStr = std::to_string(randomNumber);

    // Step 3: Add the random number to a base string
    std::string baseString = "logs/engine_test_";
    std::string logname = baseString + randomNumberStr + ".txt";

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::trace);
  console_sink->set_pattern("%^[Coeus engine] [%!:%# @ %s] [%l] %$ %v");

  // File log
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      logname, true);
  file_sink->set_level(spdlog::level::trace);
  file_sink->set_pattern("%^[Coeus engine] [%!:%# @ %s] [%l] %$ %v");
 
  
  // File log for metadata collection
  #ifdef Meta_enabled
  auto file_sink2 = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/metadataCollect_get.txt", true);
  file_sink2->set_level(spdlog::level::trace);
  file_sink2->set_pattern("%v");
  spdlog::logger logger2("metadata_logger_get", {file_sink2});
  logger2.set_level(spdlog::level::trace);
  meta_logger_get = std::make_shared<spdlog::logger>(logger2);
  meta_logger_get->info(
      "Name, shape, start, Count, Constant Shape, Time, selectionSize, sizeofVariable, ShapeID, steps, stepstart, blockID");

  auto file_sink3 = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      "logs/metadataCollect_put.txt", true);
  file_sink3->set_level(spdlog::level::trace);
  file_sink3->set_pattern("%v");
  spdlog::logger logger3("metadata_logger_put", {file_sink3});
  logger3.set_level(spdlog::level::trace);
  meta_logger_put = std::make_shared<spdlog::logger>(logger3);
  meta_logger_put->info(
      "Name, shape, start, Count, Constant Shape, Time, selectionSize, sizeofVariable, ShapeID, steps, stepstart, blockID");
#endif

  //Merge Log

  spdlog::logger logger("debug_logger", {console_sink, file_sink});
  logger.set_level(spdlog::level::debug);
  engine_logger = std::make_shared<spdlog::logger>(logger);
 auto stop_time_log = std::chrono::high_resolution_clock::now();
  std::cout << "log_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_log - start_time_log).count() << std::endl;

 auto start_time_hermes_setup = std::chrono::high_resolution_clock::now();
  // hermes setup
  if (!Hermes->connect()) {
    engine_logger->warn("Could not connect to Hermes", rank);
    throw coeus::common::ErrorException(HERMES_CONNECT_FAILED);
  }
  if (rank == 0) std::cout << "Connected to Hermes" << std::endl;
 auto stop_time_hermes_setup = std::chrono::high_resolution_clock::now();
  std::cout << "hermes_setup_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_hermes_setup - start_time_hermes_setup).count() << std::endl;


    auto start_time_rank_consensus = std::chrono::high_resolution_clock::now();    
  // add rank with consensus
  rank_consensus.CreateRoot(DomainId::GetLocal(), "rankConsensus");
  rank = rank_consensus.GetRankRoot(DomainId::GetLocal());
  const size_t bufferSize = 1024;  // Define the buffer size
  char buffer[bufferSize];         // Create a buffer to hold the hostname
  // Get the hostname

  comm_size = m_Comm.Size();
  pid_t processId = getpid();

  //Identifier, should be the file, but we don't get it
  uid = this->m_IO.m_Name;
auto stop_time_rank_consensus = std::chrono::high_resolution_clock::now();
  std::cout << "rank_consensus_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_rank_consensus - start_time_rank_consensus).count() << std::endl;


  auto start_time_parameter_time = std::chrono::high_resolution_clock::now();

  // Configuration Setup through the Adios xml configuration
  auto params = m_IO.m_Parameters;
  if (params.find("OPFile") != params.end()) {
    std::string opFile = params["OPFile"];
    if (rank == 0)
      std::cout << "OPFile: " << opFile << std::endl;
    try {
      operationMap = YAMLParser(opFile).parse();
    } catch (std::exception &e) {
      engine_logger->warn("Could not parse operation file", rank);
      throw e;
    }
  }
  if (params.find("ppn") != params.end()) {
    ppn = stoi(params["ppn"]);
    if (rank == 0)
      std::cout << "PPN: " << ppn << std::endl;
  }

    if (params.find("limit") != params.end()) {
        limit = stoi(params["limit"]);
        if (rank == 0)
            std::cout << "limit: " << limit << std::endl;
    }

  if (params.find("lookahead") != params.end()) {
    lookahead = stoi(params["lookahead"]);
    if (rank == 0)
      std::cout << "lookahead: " << lookahead << std::endl;
  }
  else{
    lookahead = 2;
  }

  if (params.find("VarFile") != params.end()) {
    std::string varFile = params["VarFile"];
    if (rank == 0)
      std::cout << "varFile: " << varFile << std::endl;
    try {
      variableMap = YAMLParser(varFile).parse();
    } catch (std::exception &e) {
      engine_logger->warn("Could not parse variable file", rank);
      throw e;
    }
  }

  //Hermes setup

  if (params.find("db_file") != params.end()) {
    db_file = params["db_file"];
    db = new SQLiteWrapper(db_file);
    client.CreateRoot(DomainId::GetGlobal(), "db_operation", db_file);
    if (rank % ppn == 0) {
      db->createTables();
    }
  } else {
    throw std::invalid_argument("db_file not found in parameters");
  }
  if(params.find("adiosOutput") != params.end()) {
      adiosOutput = params["adiosOutput"];
  }
  open = true;
    auto stop_time_parameter_time = std::chrono::high_resolution_clock::now();
  std::cout << "parameter_time_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_parameter_time - start_time_parameter_time).count() << std::endl;

}

/**
 * Close the Engine.
 * */
void HermesEngine::DoClose(const int transportIndex) {
  TRACE_FUNC("engine close");
  std::cout << "compare_time is : " << compare_time << std::endl;
  std::cout << "begin_step_time is: " << begin_step_time << std::endl;
  std::cout << "compute_derived_time is: " << compute_derived_time << std::endl;
  std::cout << "put_time is: " << put_time << std::endl;
  open = false;
}

HermesEngine::~HermesEngine() {
  TRACE_FUNC();
  delete db;

}

/**
 * Handle step operations.
 * */

bool HermesEngine::Promote(int step){
    bool success = true;
    if(step < total_steps) {
        auto var_locations = db->getAllBlobs(currentStep + lookahead, rank);
        for (const auto &location : var_locations) {
            success &= Hermes->Prefetch(location.bucket_name, location.blob_name);
        }
    }
    return success;
}

bool HermesEngine::Demote(int step){
    bool success = true;
    if (step > 0) {
        auto start_time_demote_getAllBlobs = std::chrono::high_resolution_clock::now();
        auto var_locations = db->getAllBlobs(step, rank);
        auto stop_time_demote_getAllBlobs = std::chrono::high_resolution_clock::now();
        std::cout << "demote_getAllBlobs_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_demote_getAllBlobs - start_time_demote_getAllBlobs).count() << std::endl;
        auto start_time_hermes_demote = std::chrono::high_resolution_clock::now();
        for (const auto &location: var_locations) {
            success &= Hermes->Demote(location.bucket_name, location.blob_name);
        }
        auto stop_time_hermes_demote = std::chrono::high_resolution_clock::now();
        std::cout << "hermes_demote_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_hermes_demote - start_time_hermes_demote).count() << std::endl;
    }
    return success;
}

adios2::StepStatus HermesEngine::BeginStep(adios2::StepMode mode,
                                           const float timeoutSeconds) {
  

  auto start_time_increment_step = std::chrono::high_resolution_clock::now();
  IncrementCurrentStep();
   auto stop_time_increment_step = std::chrono::high_resolution_clock::now();
    std::cout << "increment_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_increment_step - start_time_increment_step).count() << std::endl;

    auto start_time_metadata = std::chrono::high_resolution_clock::now();
  if (m_OpenMode == adios2::Mode::Read) {
    if (total_steps == -1)
      total_steps = db->GetTotalSteps(uid);
    if (currentStep > total_steps) {
      return adios2::StepStatus::EndOfStream;
    }
    LoadMetadata();
  }
  auto stop_time_metadata = std::chrono::high_resolution_clock::now();
    std::cout << "metadata_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_metadata - start_time_metadata).count() << std::endl;

    auto start_time_get_bucket = std::chrono::high_resolution_clock::now();

    std::string bucket_name =  adiosOutput + "_step_" + std::to_string(currentStep) + "_rank" + std::to_string(rank);

    Hermes->GetBucket(bucket_name);
  auto stop_time_get_bucket = std::chrono::high_resolution_clock::now();
    std::cout << "get_bucket_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_get_bucket - start_time_get_bucket).count() << std::endl;
auto start_time_derived_part = std::chrono::high_resolution_clock::now();
// derived part
  if(m_OpenMode == adios2::Mode::Read){
      for(int i = 0; i < num_layers; i++) {
          Promote(currentStep + lookahead + i);
      }
  }
  if(m_OpenMode == adios2::Mode::Write){
      for(int i = 0; i < num_layers; i++) {
          Demote(currentStep - lookahead - i);
      }
  }
    auto stop_time_derived_part = std::chrono::high_resolution_clock::now();
    std::cout << "derived_part_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_derived_part - start_time_derived_part).count() << std::endl;

  return adios2::StepStatus::OK;
}



//derived part
void HermesEngine::ComputeDerivedVariables() {
    int time1 = 0;
    int time2 = 0;
    int time3 = 0;

    auto start_time_ComputeDerivedVariables_1 = std::chrono::high_resolution_clock::now();

        auto const &m_VariablesDerived = m_IO.GetDerivedVariables();
  auto const &m_Variables = m_IO.GetVariables();
        auto stop_time_ComputeDerivedVariables_1 = std::chrono::high_resolution_clock::now();
        std::cout << "ComputeDerivedVariables_1," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_ComputeDerivedVariables_1 - start_time_ComputeDerivedVariables_1).count() << std::endl;

        // parse all derived variables
  if(rank == 0) {
      std::cout << " Parsing " << m_VariablesDerived.size() << " derived variables"
                << std::endl;
  }

  for (auto it = m_VariablesDerived.begin(); it != m_VariablesDerived.end();
       it++) {
    // identify the variables used in the derived variable
    auto derivedVar =
        dynamic_cast<adios2::core::VariableDerived *>((*it).second.get());
    std::vector<std::string> varList = derivedVar->VariableNameList();


    //for(auto i: varList) {
       // std::cout << "Compute Derived Variables: " << i << std::endl;
   // }
    // to create a mapping between variable name and the varInfo (dim and data
    // pointer)
      std::map<std::string, adios2::MinVarInfo> nameToVarInfo;
      auto start_time_ComputeDerivedVariables_2 = std::chrono::high_resolution_clock::now();
    for (auto varName : varList) {
      auto itVariable = m_Variables.find(varName);
          if (itVariable == m_Variables.end())
            std::cout <<"throw error commented" <<std::endl;
      // extract the dimensions and data for each variable
      adios2::core::VariableBase *varBase = itVariable->second.get();
      auto blob = Hermes->bkt->Get(varName);

      adios2::MinBlockInfo blk({0, 0, itVariable->second.get()->m_Start.data(),
                                itVariable->second.get()->m_Count.data(),
                                adios2::MinMaxStruct(), blob.data()});


        // if this is the first block for the variable
      auto entry = nameToVarInfo.find(varName);
      if (entry == nameToVarInfo.end()) {
        // create an mvi structure and add the new block to it
        int varDim = itVariable->second.get()->m_Shape.size();
        adios2::MinVarInfo mvi(varDim,
                               itVariable->second.get()->m_Shape.data());
        mvi.BlocksInfo.push_back(blk);
        nameToVarInfo.insert({varName, mvi});
      } else {
        // otherwise add the current block to the existing mvi
        entry->second.BlocksInfo.push_back(blk);
      }
    }
      auto stop_time_ComputeDerivedVariables_2 = std::chrono::high_resolution_clock::now();
      time1 = time1 + std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_ComputeDerivedVariables_2 - start_time_ComputeDerivedVariables_2).count();
    // compute the values for the derived variables that are not type
    // ExpressionString
      auto start_time_ComputeDerivedVariables_3 = std::chrono::high_resolution_clock::now();
    std::vector<std::tuple<void *, adios2::Dims, adios2::Dims>>
        DerivedBlockData;
    if (derivedVar->GetDerivedType() !=
        adios2::DerivedVarType::ExpressionString) {
        std::map<std::string, std::unique_ptr<adios2::MinVarInfo>> NameToMVI;
        for (auto &pair : nameToVarInfo) {
            NameToMVI[pair.first] = std::make_unique<adios2::MinVarInfo>(std::move(pair.second));
        }
      DerivedBlockData = derivedVar->ApplyExpression(NameToMVI);
    }
      auto stop_time_ComputeDerivedVariables_3 = std::chrono::high_resolution_clock::now();
      time2 = time2 + std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_ComputeDerivedVariables_3 - start_time_ComputeDerivedVariables_3).count();
      auto start_time_ComputeDerivedVariables_4 = std::chrono::high_resolution_clock::now();
    for (auto derivedBlock : DerivedBlockData) {
#define DEFINE_VARIABLE_PUT(T)       \
  if (adios2::helper::GetDataType<T>() == derivedVar->m_Type) { \
    T* data = static_cast<T *>(std::get<0>(derivedBlock));\
    PutDerived(*derivedVar, data);   \
  }
  ADIOS2_FOREACH_ATTRIBUTE_PRIMITIVE_STDTYPE_1ARG(DEFINE_VARIABLE_PUT)
#undef DEFINE_VARIABLE_PUT
      free(std::get<0>(derivedBlock));
    }
      auto stop_time_ComputeDerivedVariables_4 = std::chrono::high_resolution_clock::now();
      time3 = time3 + std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_ComputeDerivedVariables_4 - start_time_ComputeDerivedVariables_4).count();

  }
   std::cout <<  "ComputeDerivedVariables_2," << time1 << std::endl;
   std::cout <<  "ComputeDerivedVariables_3," << time2 << std::endl;
        std::cout <<  "ComputeDerivedVariables_4," << time3 << std::endl;


    }



void HermesEngine::IncrementCurrentStep() {

  currentStep++;
}

size_t HermesEngine::CurrentStep() const {

  return currentStep;
}

void HermesEngine::EndStep() {
     auto start_time_derived_variable = std::chrono::high_resolution_clock::now();
    ComputeDerivedVariables();
    auto stop_time_derived_variable = std::chrono::high_resolution_clock::now();
  std::cout << "derived_variable_setup_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_derived_variable - start_time_derived_variable).count() << std::endl;
   
    auto start_time_endstep_dpOP = std::chrono::high_resolution_clock::now();
  if (m_OpenMode == adios2::Mode::Write) {
    if (rank % ppn == 0) {
      DbOperation db_op(uid, currentStep);
      client.Mdm_insertRoot(DomainId::GetLocal(), db_op);
    }
  }
  auto stop_time_endstep_dpOP = std::chrono::high_resolution_clock::now();
  std::cout << "endstep_dpOP_setup_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_endstep_dpOP - start_time_endstep_dpOP).count() << std::endl;
   

  auto start_time_delete_bkt = std::chrono::high_resolution_clock::now();
  delete Hermes->bkt;
    auto stop_time_delete_bkt = std::chrono::high_resolution_clock::now();
  std::cout << "delete_bkt_setup_time," << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time_delete_bkt - start_time_delete_bkt).count() << std::endl;
   
}

/**
 * Metadata operations.
 * */
bool HermesEngine::VariableMinMax(const adios2::core::VariableBase &Var,
                                  const size_t Step,
                                  adios2::MinMaxStruct &MinMax) {

  // We initialize the min and max values
  MinMax.Init(Var.m_Type);

  // Obtain the blob from Hermes using the filename and variable name
  hermes::Blob blob = Hermes->bkt->Get(Var.m_Name);

#define DEFINE_VARIABLE(T)                                                     \
  if (adios2::helper::GetDataType<T>() == Var.m_Type) {                        \
    size_t dataSize = blob.size() / sizeof(T);                                 \
    const T *data = reinterpret_cast<const T *>(blob.data());                  \
    for (size_t i = 0; i < dataSize; ++i) {                                    \
      void *elementPtr =                                                       \
          const_cast<void *>(static_cast<const void *>(&data[i]));             \
      ApplyElementMinMax(MinMax, Var.m_Type, elementPtr);                      \
    }                                                                          \
  }
  ADIOS2_FOREACH_STDTYPE_1ARG(DEFINE_VARIABLE)
#undef DEFINE_VARIABLE
  return true;
}

void HermesEngine::ApplyElementMinMax(adios2::MinMaxStruct &MinMax,
                                      adios2::DataType Type, void *Element) {

  switch (Type) {
  case adios2::DataType::Int8:
    ElementMinMax<int8_t>(MinMax, Element);
    break;
  case adios2::DataType::Int16:
    ElementMinMax<int16_t>(MinMax, Element);
    break;
  case adios2::DataType::Int32:
    ElementMinMax<int32_t>(MinMax, Element);
    break;
  case adios2::DataType::Int64:
    ElementMinMax<int64_t>(MinMax, Element);
    break;
  case adios2::DataType::UInt8:
    ElementMinMax<uint8_t>(MinMax, Element);
    break;
  case adios2::DataType::UInt16:
    ElementMinMax<uint16_t>(MinMax, Element);
    break;
  case adios2::DataType::UInt32:
    ElementMinMax<uint32_t>(MinMax, Element);
    break;
  case adios2::DataType::UInt64:
    ElementMinMax<uint64_t>(MinMax, Element);
    break;
  case adios2::DataType::Float:
    ElementMinMax<float>(MinMax, Element);
    break;
  case adios2::DataType::Double:
    ElementMinMax<double>(MinMax, Element);
    break;
  case adios2::DataType::LongDouble:
    ElementMinMax<long double>(MinMax, Element);
    break;
  default:
    /*
     *     case adios2::DataType::None
     *     adios2::DataType::Char
     *     case adios2::DataType::FloatComplex
     *     case adios2::DataType::DoubleComplex
     *     case adios2::DataType::String
     *     case adios2::DataType::Struct
     */
    break;
  }
}

template <typename T>
T *HermesEngine::SelectUnion(adios2::PrimitiveStdtypeUnion &u) {
  TRACE_FUNC();
  return reinterpret_cast<T *>(&u);


}

template <typename T>
void HermesEngine::ElementMinMax(adios2::MinMaxStruct &MinMax, void *element) {
  TRACE_FUNC("MinMax operation");
  T *min = SelectUnion<T>(MinMax.MinUnion);
  T *max = SelectUnion<T>(MinMax.MaxUnion);
  T *value = static_cast<T *>(element);
  if (*value < *min) {
    min = value;
  }
  if (*value > *max) {
    max = value;
  }
}

void HermesEngine::LoadMetadata() {
  TRACE_FUNC(rank);
  auto metadata_vector = db->GetAllVariableMetadata(currentStep, rank);
  for (auto &variableMetadata : metadata_vector) {
    DefineVariable(variableMetadata);
  }


}

void HermesEngine::DefineVariable(const VariableMetadata &variableMetadata) {
  if (currentStep != 1) {
    // If the metadata is defined delete current value to update it
    m_IO.RemoveVariable(variableMetadata.name);
  }

#define DEFINE_VARIABLE(T)                                                     \
  if (adios2::helper::GetDataType<T>() ==                                      \
      adios2::helper::GetDataTypeFromString(variableMetadata.dataType)) {      \
    adios2::core::Variable<T> *variable = &(m_IO.DefineVariable<T>(            \
        variableMetadata.name, variableMetadata.shape, variableMetadata.start, \
        variableMetadata.count, variableMetadata.constantShape));              \
    variable->m_AvailableStepsCount = 1;                                       \
    variable->m_SingleValue = false;                                           \
    variable->m_Min = std::numeric_limits<T>::max();                           \
    variable->m_Max = std::numeric_limits<T>::min();                           \
    variable->m_Engine = this;                                                 \
  }
  ADIOS2_FOREACH_STDTYPE_1ARG(DEFINE_VARIABLE)
#undef DEFINE_VARIABLE
}


template<typename T>
void HermesEngine::DoGetSync_(const adios2::core::Variable<T> &variable,
                              T *values) {
  TRACE_FUNC(variable.m_Name, adios2::ToString(variable.m_Count));
  auto blob = Hermes->bkt->Get(variable.m_Name);
  std::string name = variable.m_Name;
#ifdef Meta_enabled
  // add spdlog method to extract the variable metadata
  metaInfo metaInfo(variable, adiosOpType::get);
  meta_logger_get->info("metadata: {}", metaInfoToString(metaInfo));
  globalData.insertGet(name);
  meta_logger_get->info("order: {}", globalData.GetMapToString());
#endif
  //finish metadata extraction
  memcpy(values, blob.data(), blob.size());

}





template<typename T>
void HermesEngine::DoGetDeferred_(
    const adios2::core::Variable<T> &variable, T *values) {
    TRACE_FUNC(variable.m_Name, adios2::ToString(variable.m_Count));
    auto blob = Hermes->bkt->Get(variable.m_Name);
    std::string name = variable.m_Name;

    //finish metadata extraction
    memcpy(values, blob.data(), blob.size());
}

//    }



template<typename T>
void HermesEngine::DoPutSync_(const adios2::core::Variable<T> &variable,
                              const T *values) {
  TRACE_FUNC(variable.m_Name, adios2::ToString(variable.m_Count));
    auto app_start_time = std::chrono::high_resolution_clock::now();
  std::string name = variable.m_Name;
  Hermes->bkt->Put(name, variable.SelectionSize() * sizeof(T), values);

#ifdef Meta_enabled
  metaInfo metaInfo(variable, adiosOpType::put);
  meta_logger_put->info("metadata: {}", metaInfoToString(metaInfo));

#endif
  // database
  VariableMetadata vm(variable.m_Name, variable.m_Shape, variable.m_Start,
                      variable.m_Count, variable.IsConstantDims(), true,
                      adios2::ToString(variable.m_Type));
  BlobInfo blobInfo(Hermes->bkt->name, name);
  DbOperation db_op(currentStep, rank, std::move(vm), name, std::move(blobInfo));
  client.Mdm_insertRoot(DomainId::GetLocal(), db_op);

   
}


template<typename T>
void HermesEngine::DoPutDeferred_(
    const adios2::core::Variable<T> &variable, const T *values) {
    auto app_start_time = std::chrono::high_resolution_clock::now();
  std::string name = variable.m_Name;

  Hermes->bkt->Put(name, variable.SelectionSize() * sizeof(T), values);
#ifdef Meta_enabled
  metaInfo metaInfo(variable, adiosOpType::put);
  meta_logger_put->info("metadata: {}", metaInfoToString(metaInfo));
#endif
  // database
  VariableMetadata vm(variable.m_Name, variable.m_Shape, variable.m_Start,
                      variable.m_Count, variable.IsConstantDims(), true,
                      adios2::ToString(variable.m_Type));
  BlobInfo blobInfo(Hermes->bkt->name, name);
  DbOperation db_op(currentStep, rank, std::move(vm), name, std::move(blobInfo));
       client.Mdm_insertRoot(DomainId::GetLocal(), db_op);

    auto app_end_time = std::chrono::high_resolution_clock::now(); // Record end time of the application
    auto app_duration = std::chrono::duration_cast<std::chrono::milliseconds>(app_end_time - app_start_time);
    put_time = put_time + app_duration.count();
}






template <typename T>
void HermesEngine::PutDerived(adios2::core::VariableDerived variable,
                                  T *values) {
    std::string name = variable.m_Name;
    int total_count = 1;
    for (auto count: variable.m_Count) {
        total_count *= count;
    }
    Hermes->bkt->Put(name, total_count * sizeof(T), values);
    DbOperation db_op = generateMetadata(variable, (float *) values, total_count);
    client.Mdm_insertRoot(DomainId::GetLocal(), db_op);
    // switch the bucket

    int current_bucket = stoi(adiosOutput);
    if (current_bucket > 1) {

        // time here
        auto app_start_time = std::chrono::high_resolution_clock::now();
        T* values2 = new T[total_count];

        std::string previous_bucket_name =
                std::to_string(current_bucket - 1) + "_step_" + std::to_string(currentStep) + "_rank" +
                std::to_string(rank);
        if (db->FindVariable(currentStep, rank, name,previous_bucket_name)) {
            std::cout << "find it " << std::endl;
            auto reader_get_start_time = std::chrono::high_resolution_clock::now();
            Hermes->GetBucket(previous_bucket_name);
            auto blob = Hermes->bkt->Get(name);
            memcpy(values2, blob.data(), blob.size());

            for (int i = 0; i < total_count; ++i) {
                if (static_cast<int>(values[i]) - static_cast<int>(values2[i]) > 0.01) {
                    auto app_end_time = std::chrono::system_clock::now();
                    std::time_t end_time_t = std::chrono::system_clock::to_time_t(app_end_time);
                    engine_logger->info("The difference happened at {}", std::ctime(&end_time_t));
                }
            }
        }
        auto app_end_time = std::chrono::high_resolution_clock::now(); // Record end time of the application
        auto app_duration = std::chrono::duration_cast<std::chrono::milliseconds>(app_end_time - app_start_time);
        compare_time = compare_time + app_duration.count();

    }
}







template<typename T>
DbOperation HermesEngine::generateMetadata(adios2::core::Variable<T> variable) {
  VariableMetadata vm(variable.m_Name, variable.m_Shape, variable.m_Start,
                      variable.m_Count, variable.IsConstantDims(), true,
                      adios2::ToString(variable.m_Type));
  BlobInfo blobInfo(Hermes->bkt->name, variable.m_Name);
  return DbOperation(currentStep, rank, std::move(vm), variable.m_Name, std::move(blobInfo));
}

DbOperation HermesEngine::generateMetadata(adios2::core::VariableDerived variable, float *values, int total_count) {
  VariableMetadata vm(variable.m_Name, variable.m_Shape, variable.m_Start,
                     variable.m_Count, variable.IsConstantDims(), true,
                     adios2::ToString(variable.m_Type));
  BlobInfo blobInfo(Hermes->bkt->name, variable.m_Name);

//  if(total_count < 1) {
//    return DbOperation(currentStep, rank, std::move(vm), variable.m_Name, std::move(blobInfo));
//  }
//
//  float min = std::numeric_limits<float>::max();
//  float max = std::numeric_limits<float>::lowest();
//
//  for (size_t i = 0; i < total_count; i++) {
//    // Calculate the address of the current element
//    char* elementPtr = reinterpret_cast<char*>(values) + (i * variable.m_ElementSize);
//    // Cast the element to the correct type
//    float element = *reinterpret_cast<float*>(elementPtr);
//
//    // Update min and max
//    if (element < min) min = element;
//    if (element > max) max = element;
//  }
//  if (min == std::numeric_limits<float>::max() || max == std::numeric_limits<float>::lowest()) {
//    std::cout << "BUUUUUG : Bucekt " << Hermes->bkt->name << " blob " << variable.m_Name << " min " << min << " max " << max
//              << " total count " << total_count << std::endl;
//  }
//  derivedSemantics derived_semantics(min, max);
//
//  std::cout << "step_" << currentStep << "_rank" << rank <<  variable.m_Name << " derived min " << min << " max " << max << std::endl;
//  return DbOperation(currentStep, rank, std::move(vm), variable.m_Name, std::move(blobInfo), derived_semantics);
    return DbOperation(currentStep, rank, std::move(vm), variable.m_Name, std::move(blobInfo));

}



} // namespace coeus
/**
 * This is how ADIOS figures out where to dynamically load the engine.
 * */
extern "C" {

/** C wrapper to create engine */
coeus::HermesEngine *EngineCreate(adios2::core::IO &io, // NOLINT
                                  const std::string &name,
                                  const adios2::Mode mode,
                                  adios2::helper::Comm comm) {
  (void)mode;
  return new coeus::HermesEngine(io, name, mode, comm.Duplicate());
}

/** C wrapper to destroy engine */
void EngineDestroy(coeus::HermesEngine *obj) { delete obj; }
}
