#ifndef CHI_RANKCONSENSUS_METHODS_H_
#define CHI_RANKCONSENSUS_METHODS_H_

/** The set of methods in the admin task */
struct Method : public TaskMethod {
  TASK_METHOD_T kGetRank = 10;
  TASK_METHOD_T kCount = 11;
};

#endif  // CHI_RANKCONSENSUS_METHODS_H_