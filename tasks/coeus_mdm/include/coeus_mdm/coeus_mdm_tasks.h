//
// Created by lukemartinlogan on 8/11/23.
//

#ifndef CHI_TASKS_TASK_TEMPL_INCLUDE_coeus_mdm_coeus_mdm_TASKS_H_
#define CHI_TASKS_TASK_TEMPL_INCLUDE_coeus_mdm_coeus_mdm_TASKS_H_

#include "chimaera/chimaera_namespace.h"
#include "common/DbOperation.h"

namespace chi::coeus_mdm {

#include "coeus_mdm_methods.h"
CHI_NAMESPACE_INIT

CHI_BEGIN(Create)
/**
 * A task to create coeus_mdm
 * */
struct CreateTaskParams {
  CLS_CONST char *lib_name_ = "coeus_coeus_mdm";
  std::string db_file_;

  HSHM_INLINE_CROSS_FUN
  CreateTaskParams() = default;

  HSHM_INLINE_CROSS_FUN
  CreateTaskParams(const hipc::CtxAllocator<CHI_ALLOC_T> &alloc,
                   const std::string &db_file)
      : db_file_(db_file) {}

  template <typename Ar>
  HSHM_INLINE_CROSS_FUN void serialize(Ar &ar) {
    ar(db_file_);
  }
};
typedef chi::Admin::CreatePoolBaseTask<CreateTaskParams> CreateTask;
CHI_END(Create)

CHI_BEGIN(Destroy)
/** A task to destroy coeus_mdm */
typedef chi::Admin::DestroyContainerTask DestroyTask;
CHI_END(Destroy)

CHI_BEGIN(Mdm_insert)
/** The Mdm_insertTask task */
struct Mdm_insertTask : public Task, TaskFlags<TF_SRL_SYM> {
  /** SHM default constructor */
  HSHM_INLINE explicit Mdm_insertTask(
      const hipc::CtxAllocator<CHI_ALLOC_T> &alloc)
      : Task(alloc) {}

  /** Emplace constructor */
  HSHM_INLINE explicit Mdm_insertTask(
      const hipc::CtxAllocator<CHI_ALLOC_T> &alloc, const TaskNode &task_node,
      const PoolId &pool_id, const DomainQuery &dom_query)
      : Task(alloc) {
    // Initialize task
    task_node_ = task_node;
    prio_ = TaskPrioOpt::kLowLatency;
    pool_ = pool_id;
    method_ = Method::kMdm_insert;
    task_flags_.SetBits(0);
    dom_query_ = dom_query;

    // Custom
  }

  /** Duplicate message */
  void CopyStart(const Mdm_insertTask &other, bool deep) {}

  /** (De)serialize message call */
  template <typename Ar>
  void SerializeStart(Ar &ar) {}

  /** (De)serialize message return */
  template <typename Ar>
  void SerializeEnd(Ar &ar) {}
};
CHI_END(Mdm_insert);

CHI_AUTOGEN_METHODS

}  // namespace chi::coeus_mdm

#endif  // CHI_TASKS_TASK_TEMPL_INCLUDE_coeus_mdm_coeus_mdm_TASKS_H_
