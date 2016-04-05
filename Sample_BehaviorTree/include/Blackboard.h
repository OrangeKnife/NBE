#pragma once
#include <unordered_map>
using namespace std;
namespace NBE
{
	namespace BT
	{
		struct sBlackboardData
		{
			size_t size;
			char* bytes;
		};

		class Blackboard
		{
		private:
			unordered_map<string, sBlackboardData> data;
		public:
			Blackboard() {}
			~Blackboard()
			{
				for (auto d : data)
				{
					delete[] d.second.bytes;
				}
				data.clear();
			}

			template<class T>
			T* GetData(const string& key)
			{
				//TODO: SHALL I REMOVE THE DATA ENTRY ?
				auto found = data.find(key);
				if (found != data.end())
					return reinterpret_cast<T*>(found->second.bytes);
				else
					return nullptr;
			}

			template<class T>
			void SetData(const string& key, const T& value)
			{
				auto found = data.find(key);
				if (found != data.end())
				{
					if (memcmp(found->second.bytes, &value, sizeof(T)) != 0)
						return;

					delete[] found->second.bytes;
				}

				auto& entry = data[key];
				

				entry.size = sizeof(T);
				entry.bytes = new char[entry.size];
				memcpy_s(entry.bytes, entry.size, &value, entry.size);
			}

			void RemoveData(const string& key)
			{
				auto found = data.find(key); 
				if (found != data.end())
				{
					delete[] found->second.bytes;
					data.erase(found);
				}
			}
		};
	}
}