#include "base/task_queue.h"

namespace qsdb::base {

void TaskQueue::Add(Task* task) {
    /*** [TODO]实现功能: 向任务队列中添加一个任务
     * 提示：1. 该数据结构是线程安全的，注意加锁以及唤醒正在等待中的线程(思考是唤醒一个还是唤醒所有的)
     *      2. 后续这几个函数都需要考虑是否需要加锁
     *   @param task 待添加的任务
    ***/
}

Task* TaskQueue::Pop() {
    /*** [TODO]实现功能: 从任务队列中 pop 一个任务
     * 提示：1. 先检查队列是否为空，如果为空，则阻塞，等待 Add 函数唤醒
     *      2. 除了检查队列是否为空，还需要考虑 stop_ 是否为 true 的情况，如果 stop_ 为 true，即使队列为空，
     *         也应该返回了，这是应该返回 nullptr
     *      3. 弹出取队列中第一个任务，并返回
     *   @return 返回的任务
    ***/
}

void TaskQueue::Stop() {
    /*** [TODO]实现功能: 终止
     * 提示：1. 将 stop_ 置为 true
     *      2. 唤醒正在等待中的线程
    ***/
}

bool TaskQueue::Empty() const {
    /*** [TODO]实现功能: 判断任务队列是否为空
     * @return 任务队列是否为空
    ***/
}

} // namespace qsdb::base
