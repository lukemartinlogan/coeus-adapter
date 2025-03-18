/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef HRUN_coeus_mdm_H_
#define HRUN_coeus_mdm_H_

#include "coeus_mdm_tasks.h"

namespace hrun::coeus_mdm {

template <typename T, typename = void>
struct is_streamable : std::false_type {};

template <typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream &>()
                                             << std::declval<T>())>>
    : std::true_type {};

template <typename T>
typename std::enable_if<is_streamable<T>::value>::type printArg(const T &arg) {
  std::cout << arg << " ";
}

template <typename T>
typename std::enable_if<!is_streamable<T>::value>::type printArg(const T &arg) {
  std::cout << "[non-streamable type] ";
}

template <typename... Args>
void printArgs(Args &&...args) {
  (printArg(args), ...);
  std::cout << std::endl;
}

/** Create coeus_mdm requests */
class Client : public TaskLibClient {
 public:
  /** Default constructor */
  Client() = default;

  /** Destructor */
  ~Client() = default;

  /** Async create a task state */
  HSHM_ALWAYS_INLINE
  LPointer<ConstructTask> AsyncCreate(const TaskNode &task_node,
                                      const DomainQuery &domain_id,
                                      const std::string &state_name,
                                      const std::string &db_path) {
    id_ = TaskStateId::GetNull();
    QueueManagerInfo &qm = HRUN_CLIENT->server_config_.queue_manager_;
    std::vector<PriorityInfo> queue_info;
    std::cout << "AsyncCreate path: " << db_path << std::endl;

    return CHI_ADMIN->AsyncCreateTaskState<ConstructTask>(
        task_node, domain_id, state_name, id_, queue_info, db_path);
  }
  HRUN_TASK_NODE_ROOT(AsyncCreate)
  template <typename... Args>
  HSHM_ALWAYS_INLINE void Create(Args &&...args) {
    hrun::coeus_mdm::printArgs(std::forward<Args>(args)...);
    LPointer<ConstructTask> task = AsyncCreate(std::forward<Args>(args)...);

    task->Wait();

    Init(task->id_, CHI_ADMIN->queue_id_);
    HRUN_CLIENT->DelTask(task);
  }

  /** Destroy task state + queue */
  HSHM_ALWAYS_INLINE
  void Destroy(const DomainQuery &domain_id) {
    CHI_ADMIN->DestroyTaskState(domain_id, id_);
  }

  /** Call a custom method */
  HSHM_ALWAYS_INLINE
  void AsyncMdm_insertConstruct(Mdm_insertTask *task, const TaskNode &task_node,
                                const DomainQuery &domain_id,
                                DbOperation db_op) {
    HRUN_CLIENT->ConstructTask<Mdm_insertTask>(task, task_node, domain_id, id_,
                                               db_op);
  }
  HSHM_ALWAYS_INLINE
  void Mdm_insert(const DomainQuery &domain_id, DbOperation db_op) {
    LPointer<hrunpq::TypedPushTask<Mdm_insertTask>> task =
        AsyncMdm_insert(domain_id, db_op);
    //    task.ptr_->Wait();
  }
  HRUN_TASK_NODE_PUSH_ROOT(Mdm_insert);
};

}  // namespace hrun::coeus_mdm

#endif  // HRUN_coeus_mdm_H_
