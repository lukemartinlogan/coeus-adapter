#ifndef CHI_RANKCONSENSUS_LIB_EXEC_H_
#define CHI_RANKCONSENSUS_LIB_EXEC_H_

/** Execute a task */
void Run(u32 method, Task *task, RunContext &rctx) override {
  switch (method) {
    case Method::kCreate: {
      Create(reinterpret_cast<CreateTask *>(task), rctx);
      break;
    }
    case Method::kDestroy: {
      Destroy(reinterpret_cast<DestroyTask *>(task), rctx);
      break;
    }
    case Method::kGetRank: {
      GetRank(reinterpret_cast<GetRankTask *>(task), rctx);
      break;
    }
  }
}
/** Execute a task */
void Monitor(MonitorModeId mode, MethodId method, Task *task, RunContext &rctx) override {
  switch (method) {
    case Method::kCreate: {
      MonitorCreate(mode, reinterpret_cast<CreateTask *>(task), rctx);
      break;
    }
    case Method::kDestroy: {
      MonitorDestroy(mode, reinterpret_cast<DestroyTask *>(task), rctx);
      break;
    }
    case Method::kGetRank: {
      MonitorGetRank(mode, reinterpret_cast<GetRankTask *>(task), rctx);
      break;
    }
  }
}
/** Delete a task */
void Del(const hipc::MemContext &mctx, u32 method, Task *task) override {
  switch (method) {
    case Method::kCreate: {
      CHI_CLIENT->DelTask<CreateTask>(mctx, reinterpret_cast<CreateTask *>(task));
      break;
    }
    case Method::kDestroy: {
      CHI_CLIENT->DelTask<DestroyTask>(mctx, reinterpret_cast<DestroyTask *>(task));
      break;
    }
    case Method::kGetRank: {
      CHI_CLIENT->DelTask<GetRankTask>(mctx, reinterpret_cast<GetRankTask *>(task));
      break;
    }
  }
}
/** Duplicate a task */
void CopyStart(u32 method, const Task *orig_task, Task *dup_task, bool deep) override {
  switch (method) {
    case Method::kCreate: {
      chi::CALL_COPY_START(
        reinterpret_cast<const CreateTask*>(orig_task), 
        reinterpret_cast<CreateTask*>(dup_task), deep);
      break;
    }
    case Method::kDestroy: {
      chi::CALL_COPY_START(
        reinterpret_cast<const DestroyTask*>(orig_task), 
        reinterpret_cast<DestroyTask*>(dup_task), deep);
      break;
    }
    case Method::kGetRank: {
      chi::CALL_COPY_START(
        reinterpret_cast<const GetRankTask*>(orig_task), 
        reinterpret_cast<GetRankTask*>(dup_task), deep);
      break;
    }
  }
}
/** Duplicate a task */
void NewCopyStart(u32 method, const Task *orig_task, FullPtr<Task> &dup_task, bool deep) override {
  switch (method) {
    case Method::kCreate: {
      chi::CALL_NEW_COPY_START(reinterpret_cast<const CreateTask*>(orig_task), dup_task, deep);
      break;
    }
    case Method::kDestroy: {
      chi::CALL_NEW_COPY_START(reinterpret_cast<const DestroyTask*>(orig_task), dup_task, deep);
      break;
    }
    case Method::kGetRank: {
      chi::CALL_NEW_COPY_START(reinterpret_cast<const GetRankTask*>(orig_task), dup_task, deep);
      break;
    }
  }
}
/** Serialize a task when initially pushing into remote */
void SaveStart(
    u32 method, BinaryOutputArchive<true> &ar,
    Task *task) override {
  switch (method) {
    case Method::kCreate: {
      ar << *reinterpret_cast<CreateTask*>(task);
      break;
    }
    case Method::kDestroy: {
      ar << *reinterpret_cast<DestroyTask*>(task);
      break;
    }
    case Method::kGetRank: {
      ar << *reinterpret_cast<GetRankTask*>(task);
      break;
    }
  }
}
/** Deserialize a task when popping from remote queue */
TaskPointer LoadStart(    u32 method, BinaryInputArchive<true> &ar) override {
  TaskPointer task_ptr;
  switch (method) {
    case Method::kCreate: {
      task_ptr.ptr_ = CHI_CLIENT->NewEmptyTask<CreateTask>(
             HSHM_DEFAULT_MEM_CTX, task_ptr.shm_);
      ar >> *reinterpret_cast<CreateTask*>(task_ptr.ptr_);
      break;
    }
    case Method::kDestroy: {
      task_ptr.ptr_ = CHI_CLIENT->NewEmptyTask<DestroyTask>(
             HSHM_DEFAULT_MEM_CTX, task_ptr.shm_);
      ar >> *reinterpret_cast<DestroyTask*>(task_ptr.ptr_);
      break;
    }
    case Method::kGetRank: {
      task_ptr.ptr_ = CHI_CLIENT->NewEmptyTask<GetRankTask>(
             HSHM_DEFAULT_MEM_CTX, task_ptr.shm_);
      ar >> *reinterpret_cast<GetRankTask*>(task_ptr.ptr_);
      break;
    }
  }
  return task_ptr;
}
/** Serialize a task when returning from remote queue */
void SaveEnd(u32 method, BinaryOutputArchive<false> &ar, Task *task) override {
  switch (method) {
    case Method::kCreate: {
      ar << *reinterpret_cast<CreateTask*>(task);
      break;
    }
    case Method::kDestroy: {
      ar << *reinterpret_cast<DestroyTask*>(task);
      break;
    }
    case Method::kGetRank: {
      ar << *reinterpret_cast<GetRankTask*>(task);
      break;
    }
  }
}
/** Deserialize a task when popping from remote queue */
void LoadEnd(u32 method, BinaryInputArchive<false> &ar, Task *task) override {
  switch (method) {
    case Method::kCreate: {
      ar >> *reinterpret_cast<CreateTask*>(task);
      break;
    }
    case Method::kDestroy: {
      ar >> *reinterpret_cast<DestroyTask*>(task);
      break;
    }
    case Method::kGetRank: {
      ar >> *reinterpret_cast<GetRankTask*>(task);
      break;
    }
  }
}

#endif  // CHI_RANKCONSENSUS_LIB_EXEC_H_