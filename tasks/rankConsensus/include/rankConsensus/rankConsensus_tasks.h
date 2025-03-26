//
// Created by lukemartinlogan on 8/11/23.
//

#ifndef CHI_TASKS_TASK_TEMPL_INCLUDE_rankConsensus_rankConsensus_TASKS_H_
#define CHI_TASKS_TASK_TEMPL_INCLUDE_rankConsensus_rankConsensus_TASKS_H_

#include "chimaera/chimaera_namespace.h"

namespace chi::rankConsensus {

#include "rankConsensus_methods.h"
CHI_NAMESPACE_INIT

CHI_BEGIN(Create)
/**
 * A task to create rankConsensus
 * */
struct CreateTaskParams {
  CLS_CONST char *lib_name_ = "coeus_rankConsensus";

  HSHM_INLINE_CROSS_FUN
  CreateTaskParams() = default;

  HSHM_INLINE_CROSS_FUN
  CreateTaskParams(const hipc::CtxAllocator<CHI_ALLOC_T> &alloc) {}

  template <typename Ar>
  HSHM_INLINE_CROSS_FUN void serialize(Ar &ar) {}
};
typedef chi::Admin::CreatePoolBaseTask<CreateTaskParams> CreateTask;
CHI_END(Create)

CHI_BEGIN(Destroy)
/** A task to destroy rankConsensus */
typedef chi::Admin::DestroyContainerTask DestroyTask;
CHI_END(Destroy)

CHI_BEGIN(GetRank)
/** The GetRankTask task */
struct GetRankTask : public Task, TaskFlags<TF_SRL_SYM> {
  OUT int rank_;

  /** SHM default constructor */
  HSHM_INLINE explicit GetRankTask(const hipc::CtxAllocator<CHI_ALLOC_T> &alloc)
      : Task(alloc) {}

  /** Emplace constructor */
  HSHM_INLINE explicit GetRankTask(const hipc::CtxAllocator<CHI_ALLOC_T> &alloc,
                                   const TaskNode &task_node,
                                   const PoolId &pool_id,
                                   const DomainQuery &dom_query)
      : Task(alloc) {
    // Initialize task
    task_node_ = task_node;
    prio_ = TaskPrioOpt::kLowLatency;
    pool_ = pool_id;
    method_ = Method::kGetRank;
    task_flags_.SetBits(0);
    dom_query_ = dom_query;

    // Custom
  }

  /** Duplicate message */
  void CopyStart(const GetRankTask &other, bool deep) { rank_ = other.rank_; }

  /** (De)serialize message call */
  template <typename Ar>
  void SerializeStart(Ar &ar) {}

  /** (De)serialize message return */
  template <typename Ar>
  void SerializeEnd(Ar &ar) {
    ar & rank_;
  }
};
CHI_END(GetRank)

CHI_AUTOGEN_METHODS
}  // namespace chi::rankConsensus

#endif  // CHI_TASKS_TASK_TEMPL_INCLUDE_rankConsensus_rankConsensus_TASKS_H_
