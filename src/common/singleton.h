#ifndef FI_SINGLETON_H
#define FI_SINGLETON_H

namespace fi
{
	template <typename T>
	class Singleton : public Non_Copyable, public Non_Movable
	{
	public:
		static T& instance()
		{
			static T instance;
			return instance;
		}
	};
}

#endif