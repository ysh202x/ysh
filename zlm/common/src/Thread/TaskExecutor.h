#ifndef __TASK_EXECUTOR_H__
#define __TASK_EXECUTOR_H__

#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <queue>
#include "util/List.h"

namespace ysh_toolkit
{
 /**
* cpu负载计算器
 * CPU Load Calculator
 
 * [AUTO-TRANSLATED:46dad663]
*/
class ThreadLoadCounter
{
public:
    /**
     * 构造函数
     * @param max_size 统计样本数量
     * @param max_usec 统计时间窗口,亦即最近{max_usec}的cpu负载率
     * Constructor
     * @param max_size Number of statistical samples
     * @param max_usec Statistical time window, i.e., the CPU load rate for the most recent {max_usec}
     
     * [AUTO-TRANSLATED:718cb173]
     */
    ThreadLoadCounter(uint64_t max_size, uint64_t max_usec);
    ~ThreadLoadCounter() = default;

    /**
     * 线程进入休眠
     * Thread enters sleep
     
     * [AUTO-TRANSLATED:d831fad1]
     */
    void startSleep();


    /**
     * 休眠唤醒,结束休眠
     * Wake up from sleep, end sleep
     
     * [AUTO-TRANSLATED:361831f8]
     */
    void sleepWakeUp();

    /**
     * 返回当前线程cpu使用率，范围为 0 ~ 100
     * @return 当前线程cpu使用率
     * Returns the current thread's CPU usage rate, ranging from 0 to 100
     * @return Current thread's CPU usage rate
     
     * [AUTO-TRANSLATED:c9953342]
     */
    int load();

private:
    struct TimeRecord {
        TimeRecord(uint64_t tm, bool slp) {
            _time = tm;
            _sleep = slp;
        }

        bool _sleep;
        uint64_t _time;
    };

private:
    bool _sleeping = true;
    uint64_t _last_sleep_time;
    uint64_t _last_wake_time;
    uint64_t _max_size;
    uint64_t _max_usec;
    std::mutex _mtx;
    List<TimeRecord> _time_list;
};


};