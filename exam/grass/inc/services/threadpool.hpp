#pragma once

#include <mutex>
#include <thread>
#include <map>
#include <condition_variable>
#include <set>
#include <functional>


class ThreadPool {

  static void threadRemoverLoop();

  // thread creation
  std::mutex threadCreationMutex;
  std::map<std::thread::id, std::thread *> runningThreadMap;

  // thread removal
  std::thread *finishedCollector;
  std::mutex threadsSetMutex;
  std::set<std::thread *> threadsSet;
  std::condition_variable allExitVariable;
  // sleep mutex
  std::mutex sleepMutex;
  std::condition_variable sleepVariable;

  explicit ThreadPool();

  /**
   * @brief call back method to register that a thread is exiting and it is ready to be removed, after this call no
   * locks are taken by a thread
   * @param threadID
   */
  void registerForRemoval(std::thread::id threadID);

  /**
   * @brief removes all threads which registered themselves for removal, calls join on them (no risk of deadlock as the
   * threads should exit very soon
   */
  void removeFinishedThreads();

  /**
   * Finishes collecting threads, counts on a fact that all threads were notified that they should exit, either directly
   * by the pool or the Server or Client.
   */
  void finishCollectingThreads();

  /**
   * Waits for all threads to exit
   */
  void allExit();

  /**
   * @brief run a function in a separate thread in the pool
   * @param threadBody
   * @return pointer to a started thread
   */
  std::thread *runThread(const std::function<void()> &threadBody);

  /**
   * @brief singleton instance of a ThreadPool
   */
  static ThreadPool *pool;

  /**
 * @brief flag to signal that a pool is exiting
 */
  volatile bool exit;


public:

  ThreadPool(const ThreadPool &pool) = delete;

  ThreadPool(ThreadPool &&pool) = delete;

  ~ThreadPool() {
    if (finishedCollector != nullptr)
      delete (finishedCollector);
    finishedCollector = nullptr;
  }

  /**
   * @brief execute a function on a separate thread, pool is responsible for deallocating memory on the heap
   * @param threadBody
   * @return
   */
  static std::thread *run(const std::function<void()> &threadBody);

  /**
   * @brief start a ThreadPool
   */
  static void start();

  /**
   * @brief Finish a ThreadPool
   */
  static void finish();
};
