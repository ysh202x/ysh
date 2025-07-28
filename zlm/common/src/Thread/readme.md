# ZLToolKit Thread Module

## 概述


Thread模块是ZLToolKit框架的核心线程管理模块，提供了完整的异步任务执行、线程池管理和负载均衡功能。该模块采用现代C++设计，支持跨平台，为高性能网络应用提供强大的并发处理能力。

## 架构设计

### 分层架构
```
┌─────────────────────────────────────┐
│           WorkThreadPool            │  ← 高级封装，单例模式
├─────────────────────────────────────┤
│           EventPoller               │  ← 事件驱动线程
├─────────────────────────────────────┤
│            ThreadPool               │  ← 传统线程池
├─────────────────────────────────────┤
│          TaskExecutor               │  ← 基础任务执行器
└─────────────────────────────────────┘
```

### 核心特性
- **异步编程**: 完整的异步任务执行框架
- **负载均衡**: 基于CPU使用率的智能负载分配
- **线程安全**: 所有组件都是线程安全的
- **跨平台**: 支持Windows、Linux、macOS等平台
- **高性能**: 支持CPU亲和性和线程优先级设置


## 核心组件

### TaskExecutor - 任务执行器
- 异步任务执行 (`async`)
- 高优先级任务执行 (`async_first`)
- 同步任务执行 (`sync`)
- CPU负载统计

### ThreadPool - 通用线程池
- 多线程任务执行
- 线程优先级管理
- CPU亲和性设置

### WorkThreadPool - 工作线程池
- 基于EventPoller
- 单例模式管理
- 智能负载均衡

### TaskQueue - 任务队列
- 线程安全的任务队列
- 基于信号量的任务控制
- 支持优先级任务

## 使用示例

```cpp
// 创建任务执行器
auto executor = std::make_shared<TaskExecutor>();

// 异步执行任务
auto task = executor->async([]() {
    std::cout << "异步任务执行" << std::endl;
});

// 创建线程池
ThreadPool pool(4, ThreadPool::PRIORITY_HIGH);

// 提交任务
pool.async([]() {
    std::cout << "线程池任务执行" << std::endl;
});

// 获取工作线程池
auto& work_pool = WorkThreadPool::Instance();
auto poller = work_pool.getPoller();
```

## 主要特性

- 异步编程框架
- 负载均衡
- 线程安全
- 跨平台支持
- CPU亲和性
- 任务取消机制

## 架构设计

```
WorkThreadPool (高级封装)
    ↓
EventPoller (事件驱动)
    ↓
ThreadPool (传统线程池)
    ↓
TaskExecutor (基础执行器)
```

该模块与Poller模块配合，形成完整的事件驱动异步编程模型。