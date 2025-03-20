#ifndef CHI_COEUS_MDM_METHODS_H_
#define CHI_COEUS_MDM_METHODS_H_

/** The set of methods in the admin task */
struct Method : public TaskMethod {
  TASK_METHOD_T kMdm_insert = 10;
  TASK_METHOD_T kCount = 11;
};

#endif  // CHI_COEUS_MDM_METHODS_H_