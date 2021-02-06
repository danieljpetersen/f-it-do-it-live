#ifndef FI_WORK_QUEUE_H
#define FI_WORK_QUEUE_H

#include "moodycamel_queue/concurrent_queue.h"
#include "moodycamel_queue/blocking_concurrent_queue.h"
#include "atomic_wrapper.h"

namespace fi
{
    class Work_Queue_Interface
    {
    public:
        virtual bool isAllWorkComplete() = 0; // all work items
        virtual void waitForAllWorkComplete_Idle() = 0; // all work items;
        virtual void waitForAllWorkComplete_Help() = 0; // all work items
        virtual void onIndividualWorkItemProcessed() = 0; // an individual work item
        virtual void _processIndividualWorkItem() = 0; // an individual work item
    };

    ////////////////////////////////////////////////////////////

    class Work_Queue_ID
    {
    public:
        Work_Queue_Interface *WorkQueue;
    };

    ////////////////////////////////////////////////////////////

    void backgroundThreadPollWork();
    void backgroundThreadLoop();

    class Thread_Pool
    {
    public:
        std::vector<std::thread> Threads;
        atomicwrapper<bool> BackgroundThreadsRunning;
        moodycamel::BlockingConcurrentQueue<Work_Queue_ID> WorkQueue;

        void addWork(Work_Queue_ID WorkOrder);
        void launch();
        void join();
        void joinLaunch();
    };

    fi::Thread_Pool &getThreadPool();

    ////////////////////////////////////////////////////////////
    template <typename T>
    class Work_Queue : public Work_Queue_Interface
    {
    public:
        Work_Queue()
        {
            WorkQueueID.WorkQueue = this;
            WorkJobCount._a.store(0);
            WorkCompletionCount._a.store(0);
        }

        void addWork(T WorkOrder);
        bool isAllWorkComplete() override;
        void waitForAllWorkComplete_Idle() override;
        void waitForAllWorkComplete_Help() override;
        void onIndividualWorkItemProcessed() override;
        void _processIndividualWorkItem() override;

    private:
        Work_Queue_ID WorkQueueID;
        moodycamel::BlockingConcurrentQueue<T> WorkQueue;

        virtual void asyncWork(T WorkOrder) = 0;

        atomicwrapper<int> WorkCompletionCount;
        atomicwrapper<int> WorkJobCount;
    };

    // ----

    template<typename T>
    void Work_Queue<T>::addWork(T MyWorkOrder)
    {
        WorkJobCount._a.fetch_add(1);
        WorkQueue.enqueue(MyWorkOrder);
        getThreadPool().addWork(WorkQueueID);
    }

    // ----

    template<typename T>
    void Work_Queue<T>::onIndividualWorkItemProcessed()
    {
        WorkCompletionCount._a.fetch_add(1);
    }

    // ----

    template<typename T>
    void Work_Queue<T>::_processIndividualWorkItem()
    {
        T WorkOrder;
        WorkQueue.wait_dequeue(WorkOrder);
        asyncWork(WorkOrder);
    }

    template<typename T>
    bool Work_Queue<T>::isAllWorkComplete()
    {
        return WorkCompletionCount._a.load() == WorkJobCount._a.load();
    }

    template<typename T>
    void Work_Queue<T>::waitForAllWorkComplete_Idle()
    {
        while (isAllWorkComplete() != true) {}
    }

    template<typename T>
    void Work_Queue<T>::waitForAllWorkComplete_Help()
    {
        while (isAllWorkComplete() != true)
        {
            backgroundThreadPollWork();
        }
    }
}

#endif