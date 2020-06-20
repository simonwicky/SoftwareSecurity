
#include <inc/services/logging.hpp>
#include "inc/services/threadpool.hpp"

ThreadPool *ThreadPool::pool = nullptr;

std::thread *ThreadPool::runThread(const std::function<void()> &threadBody) {
  std::lock_guard<std::mutex> runGuard(threadCreationMutex);
  auto extendedThreadBody = [threadBody, this] {
    threadBody();
    this->registerForRemoval(std::this_thread::get_id());
  };
  auto createdThread = new std::thread(extendedThreadBody);
  auto id = createdThread->get_id();
  runningThreadMap.insert({id, createdThread});
  return createdThread;
}

void ThreadPool::registerForRemoval(std::thread::id threadID) {
  std::unique_lock<std::mutex> mapGuard(threadCreationMutex);
  std::unique_lock<std::mutex> lockGuard(threadsSetMutex);

  auto thread = runningThreadMap.at(threadID);
  runningThreadMap.erase(threadID);
  if (exit && runningThreadMap.empty())
    allExitVariable.notify_all();
  threadsSet.insert(thread);
}

void ThreadPool::removeFinishedThreads() {
  std::lock_guard<std::mutex> lockGuard(threadsSetMutex);
  for (auto thread : threadsSet) {
    if (thread->joinable())
      thread->join();
    delete thread;
  }
  threadsSet.clear();
}

void ThreadPool::allExit() {
  std::unique_lock<std::mutex> mapGuard(threadCreationMutex);
  if (!runningThreadMap.empty())
    allExitVariable.wait(mapGuard);
}

void ThreadPool::threadRemoverLoop() {
  LOG_INFO("Server garbage thread started");
  while (!pool->exit) {
    {
      std::unique_lock<std::mutex> lock(pool->sleepMutex);
      pool->sleepVariable.wait_for(lock, std::chrono::minutes(3));
    }
    pool->removeFinishedThreads();
  }

  LOG_INFO("Server garbage thread collector is exiting");
}

void ThreadPool::finishCollectingThreads() {
  this->exit = true;
  {
    std::unique_lock<std::mutex> lock(this->sleepMutex);
    this->sleepVariable.notify_all();
  }
  LOG_INFO("Waiting for collector thread");
  if (finishedCollector->joinable())
    finishedCollector->join();
  LOG_INFO("Waiting for all to finish");
  LOG_INFO("Sockets timeout after one minute, be patient");
  allExit();
  LOG_INFO("Deleting all threads");
  removeFinishedThreads();
}

ThreadPool::ThreadPool() : finishedCollector(nullptr), exit(false) {}

void ThreadPool::start() {
  pool = new ThreadPool();
  pool->finishedCollector = new std::thread(threadRemoverLoop);
}

void ThreadPool::finish() {
  pool->finishCollectingThreads();
  delete (pool);
}

std::thread *ThreadPool::run(const std::function<void()> &threadBody) {
  return pool->runThread(threadBody);
}
