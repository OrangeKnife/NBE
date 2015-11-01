#pragma once
#include "Constant.h"
#include "MyAssert.h"
#include <list>
#include <memory>
namespace NBE
{
	template <typename T_eventData>
	class NEventHub
	{
	protected:
		struct CallBackData {
			CallBackData(T_eventData& _eventData, void* _handlerDataPointer)
				:eventData(_eventData), handlerDataPointer(_handlerDataPointer){}

			template <typename className, void (className::*Function)(T_eventData*, void*)>
			static inline void ClassMethodStub(void* instance, T_eventData* eventdata, void* argPointer)
			{
				return (static_cast<className*>(instance)->*Function)(eventdata,argPointer);
			}

			template <class className, void (className::*Function)(T_eventData*, void*)>
			void Bind(className* instance)
			{
				handler = instance;
				handlerFunc = &ClassMethodStub<className, Function>;
			}

			template <void(*Function)(T_eventData*, void*)>
			static inline void FunctionStub(void*, T_eventData* eventdata, void* argPointer)
			{
				return (Function)(eventdata, argPointer);
			}

			template <void(*Function)(T_eventData*, void*)>
			void Bind()
			{
				handler = nullptr;
				handlerFunc = &FunctionStub<Function>;
			}

			void inline call(T_eventData* eventdata)
			{
				handlerFunc(handler, eventdata, handlerDataPointer);
			}

			T_eventData eventData;
			void* handler;
			void (*handlerFunc)(void*, T_eventData* ,void*);
			void* handlerDataPointer;
		};
	public:
		
		static NEventHub* getPtr()
		{
			static std::shared_ptr< NEventHub<T_eventData> > instance( new NEventHub<T_eventData>() );
			return instance.get();
		}

		NEventHub() : head(0), tail(0), maxEventNum(MAX_EVENT_NUM)
		{
			myassert(MAX_EVENT_NUM > 2);
		}

		virtual ~NEventHub()
		{
			//delete[] buffer;
			for (auto &it : callBacks)
				delete it;
		}

		template<typename className, typename void(className::*functionName)(T_eventData* , void*)>
		void addEventCallBack(T_eventData& eventData, className* handler, void* handlerDataPointer)
		{
			auto newCallback = new CallBackData(eventData, handlerDataPointer);
			newCallback->Bind<className, functionName>(handler);
			callBacks.push_back(newCallback);
		}

		template<typename void(*functionName)(T_eventData*,void*)>
		void addEventCallBack(T_eventData& eventData, void* handlerDataPointer)
		{
			auto newCallback = new CallBackData(eventData, handlerDataPointer);
			newCallback->Bind<functionName>();
			callBacks.push_back(newCallback);
		}

		template<typename className, typename void(className::*functionName)(T_eventData*, void*)>
		void removeEventCallBack(T_eventData& eventData, className* handler)
		{
			for (auto it = callBacks.begin(); it != callBacks.end();)
			{
				if ((*it)->eventData == eventData && (*it)->handler == handler && 
					(*it)->handlerFunc == &CallBackData::ClassMethodStub<className, functionName>)
				{
					delete (*it);
					it = callBacks.erase(it);
				}
				else{
					++it;
				}			
			}
		}

		template<typename void(*functionName)(T_eventData*, void*)>
		void removeEventCallBack(T_eventData& eventData)
		{
			for (auto it = callBacks.begin(); it != callBacks.end();)
			{
				if ((*it)->eventData == eventData &&
					(*it)->handlerFunc == &CallBackData::FunctionStub<functionName> )
				{
					delete (*it);
					it = callBacks.erase(it);
				}
				else {
					++it;
				}
			}
		}

		
		void fire(T_eventData& eventData)
		{
			for (auto &it : callBacks)
			{
				if ((it)->eventData == eventData)
				{
					(it)->call(&eventData);
				}
			}
		}

		void queueEvent(T_eventData& eventData)
		{
			myassert(head != (tail + 1) % maxEventNum);
			buffer[tail] = eventData;
			tail = (tail + 1) % maxEventNum;
		}

		void fireAllQueuedEvents()
		{
			if (head == tail) return;
			for (;head != tail;)
			{ 
				for (auto &it : callBacks)
				{
					if ((it)->eventData == buffer[head])
					{
						(it)->call(&buffer[head]);
					}
				}
				 
				//delete buffer[head];
				head = (head + 1) % maxEventNum;
			}
		}


	protected:
		T_eventData buffer[MAX_EVENT_NUM];//ring buffer
		std::list<CallBackData*> callBacks;
		size_t head, tail;
		const size_t maxEventNum;
	};

#define ADDCLASSCALLBACK(eventDataName, className, functionName, eventData, handler, handlerDataPtr) \
	NEventHub<##eventDataName>::getPtr()->addEventCallBack<##className, &##className::##functionName>(eventData,handler,handlerDataPtr);
}