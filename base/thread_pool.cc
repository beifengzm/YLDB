#include "base/thread_pool.h"

namespace qsdb::base {

ThreadPool::ThreadPool(size_t thread_num) : stop_(false) {
    /*** [TODO]实现功能: 构造一个线程池
     * 提示：1. 创建执行数目的线程，每个线程不停的从任务队列中取任务，然后执行
     *      2. 这里需要考虑 stop_ 为 true 时如何处理(这里需要注意，stop_ 为 true 时，任务队列中)
     *      2. 需要注意，stop_ 为 true 时，应当将任务队列中剩余的任务处理完再结束
     *   @param thread_num 线程池中需要创建的线程数目
    ***/
}

void ThreadPool::AddTask(Task* task) {
    /*** [TODO]实现功能: 向任务队列中添加任务
     * 提示：1. 这里需要注意 stop_ 是否为 true 的情况
     *   @param task 待添加的任务
    ***/
}

void ThreadPool::JoinAll() {
    /*** 
     * [TODO]实现功能: join 所有线程
    ***/
}

void ThreadPool::StopAll() {
    /*** [TODO]实现功能: 终止线程池
     * 提示：1. 将 stop_ 置为 true
     *      2. 调用 task_queue_ 的 Stop 函数
     *      3. 调用 JoinAll
    ***/
}

} // namespace qsdb::base
