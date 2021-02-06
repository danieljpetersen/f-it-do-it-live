#include "work_queue.h"
#include "engine.h"

void fi::backgroundThreadLoop()
{
    fi::Thread_Pool *ThreadPool = &fi::getThreadPool();
    while (ThreadPool->BackgroundThreadsRunning._a.load())
    {
        backgroundThreadPollWork();
    }
}

////////////////////////////////////////////////////////////

void fi::backgroundThreadPollWork()
{
    fi::Work_Queue_ID WorkOrder;

    if (fi::getThreadPool().WorkQueue.wait_dequeue_timed(WorkOrder, 1))
    {
        WorkOrder.WorkQueue->_processIndividualWorkItem();
        WorkOrder.WorkQueue->onIndividualWorkItemProcessed();
    }
}

////////////////////////////////////////////////////////////

void fi::Thread_Pool::addWork(fi::Work_Queue_ID WorkOrder)
{
    WorkQueue.enqueue(WorkOrder);
}

////////////////////////////////////////////////////////////

void fi::Thread_Pool::launch()
{
    if (Threads.empty() != true)
    {
        join();
    }

    BackgroundThreadsRunning._a.store(true);
    const int NumberOfBGThreads = std::max(1, (int)std::thread::hardware_concurrency() - 1);
    for (int i = 0; i < NumberOfBGThreads; i++)
    {
        Threads.push_back(std::thread(backgroundThreadLoop));
    }
}

////////////////////////////////////////////////////////////

void fi::Thread_Pool::join()
{
    BackgroundThreadsRunning._a.store(false);
    for (int i = 0; i < Threads.size(); i++)
    {
        Threads[i].join();
    }
    Threads.clear();
}

////////////////////////////////////////////////////////////

void fi::Thread_Pool::joinLaunch()
{
    join();
    launch();
}
