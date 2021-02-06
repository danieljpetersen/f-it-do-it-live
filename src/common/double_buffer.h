#ifndef FI_DOUBLE_BUFFER
#define FI_DOUBLE_BUFFER

#include <algorithm>
#include <mutex>
#include "singleton.h"

namespace fi
{
    class Double_Buffer_Interface
    {
    public:
        virtual void swapPointers() = 0;
        virtual void copyData() = 0;
		virtual void reset() = 0;

        bool ShouldCopySwap = true; // set to false to prevent copying / swapping
    };

    ////////////////////////////////////////////////////////////

	class Double_Buffer_Object_Tracker : public fi::Singleton<Double_Buffer_Object_Tracker>
    {
    public:
        std::vector<Double_Buffer_Interface*> Objects;

        void _registerObject(Double_Buffer_Interface *Obj)
        {
            Objects.push_back(Obj);
        }
    
        void _swapPointers()
        {
            for (int i = 0; i < Objects.size(); i++)
            {
                if (Objects[i]->ShouldCopySwap)
                {
                    Objects[i]->swapPointers();
                }
            }
        }
    
        void _copyData()
        {
            for (int i = 0; i < Objects.size(); i++)
            {
                if (Objects[i]->ShouldCopySwap)
                {
                    Objects[i]->copyData();
                }
            }
        }
        
        void _copyDataForce()
        {
            for (int i = 0; i < Objects.size(); i++)
            {
                Objects[i]->copyData();
            }
        }
    
        void _swapPointersForce()
        {
            for (int i = 0; i < Objects.size(); i++)
            {
                Objects[i]->swapPointers();
            }
        }

        void reset()
		{
        	for (int i = 0; i < Objects.size(); i++)
			{
        		Objects[i]->reset();
			}
		}
    };

    ////////////////////////////////////////////////////////////

    template <typename T>
    class Lock_Guard_Wrapper
    {
    public:
        Lock_Guard_Wrapper()
        {

        }

        T *Data;
        std::unique_ptr<std::lock_guard<std::recursive_mutex>> LockGuard;
    };

    ////////////////////////////////////////////////////////////

    template <typename T>
    class Double_Buffer : public Double_Buffer_Interface
    {
    private:
        T aData;
        T bData;

        std::recursive_mutex WriteAccessMutex;
        T *WritePtr;

    public:
        T *ReadPtr;

        Lock_Guard_Wrapper<T> getWriteAccess()
        {
            Lock_Guard_Wrapper<T> DataLockGuard;

            DataLockGuard.LockGuard = std::unique_ptr<std::lock_guard<std::recursive_mutex>>(new std::lock_guard<std::recursive_mutex>(WriteAccessMutex));
            DataLockGuard.Data = WritePtr;

            return std::move(DataLockGuard);
        }

        Double_Buffer()
        {
            reset();
            Double_Buffer_Object_Tracker::instance()._registerObject(this);
        }

        ~Double_Buffer()
        {
            Double_Buffer_Object_Tracker::instance().Objects.erase(std::remove(Double_Buffer_Object_Tracker::instance().Objects.begin(), Double_Buffer_Object_Tracker::instance().Objects.end(), this), Double_Buffer_Object_Tracker::instance().Objects.end());
        }

        void swapPointers()
        {
            T *Temp = &(*WritePtr);
            WritePtr = &(*ReadPtr);
            ReadPtr = &(*Temp);
        }

        void copyData()
        {
            (*WritePtr) = (*ReadPtr);
        }

        void reset()
        {
            ReadPtr = &aData;
            WritePtr = &bData;
        }
    };


    ////////////////////////////////////////////////////////////

    //template <typename T, typename Creation_Request_T, typename Destruction_Request_T>
    //class Double_Buffer_With_Creation_Destruction_Requests : public Double_Buffer_Interface
    //{
    //private:
    //    T aData;
    //    T bData;
    //
    //    std::recursive_mutex WriteAccessMutex;
    //    T *WritePtr;
    //
    //public:
    //    T *ReadPtr;
    //
    //    Lock_Guard_Wrapper<T> getWriteAccess()
    //    {
    //        Lock_Guard_Wrapper<T> DataLockGuard;
    //
    //        DataLockGuard.LockGuard = std::unique_ptr<std::lock_guard<std::recursive_mutex>>(new std::lock_guard<std::recursive_mutex>(WriteAccessMutex));
    //        DataLockGuard.Data = WritePtr;
    //
    //        return std::move(DataLockGuard);
    //    }
    //
    //    Double_Buffer_With_Creation_Destruction_Requests()
    //    {
    //        reset();
    //        Double_Buffer_Object_Tracker::instance()._registerObject(this);
    //    }
    //
    //    ~Double_Buffer_With_Creation_Destruction_Requests()
    //    {
    //        Double_Buffer_Object_Tracker::instance().Objects.erase(std::remove(Double_Buffer_Object_Tracker::instance().Objects.begin(), Double_Buffer_Object_Tracker::instance().Objects.end(), this), Double_Buffer_Object_Tracker::instance().Objects.end());
    //    }
    //
    //    void swapPointers()
    //    {
    //        T *Temp = &(*WritePtr);
    //        WritePtr = &(*ReadPtr);
    //        ReadPtr = &(*Temp);
    //    }
    //
    //    void copyData()
    //    {
    //        (*WritePtr) = (*ReadPtr);
    //    }
    //
    //    void reset()
    //    {
    //        ReadPtr = &aData;
    //        WritePtr = &bData;
    //    }
    //};
}

#endif
