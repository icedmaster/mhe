#ifndef __FACTORY_HPP__
#define __FACTORY_HPP__

#include "hashmap.hpp"
#include "singleton.hpp"
#include "ref_ptr.hpp"
#include "types_cast.hpp"
#include "string.hpp"

namespace mhe {

template <class Base>
class Factory : public Singleton< Factory<Base> >
{
	friend class Singleton< Factory<Base> >;

	class AbstractCreator : public ref_counter
	{
	public:
		virtual ~AbstractCreator() {}
		virtual Base* create() const = 0;
	};

	template <class T>
	class Creator : public AbstractCreator
	{
	public:
		Base* create() const
		{
			return new T;
		}
	};
public:
	template <class T>
	void add(const char* name)
	{
		creators_[name] = ref_ptr<AbstractCreator>(new Creator<T>);
	}

	template <class T>
	void add()
	{
		add<T>(T::material_name());
	}

	Base* create(const char* name) const
	{
        typename hashmap<string, ref_ptr<AbstractCreator> >::const_iterator it = creators_.find(name);
		if (it == creators_.end()) return nullptr;
		return it->value->create();
	}

	template <class Arg0, class Arg1>
	Base* create(const char* name, Arg0& arg0, Arg1& arg1) const
	{
		Base* product = create(name);
		if (product == nullptr) return nullptr;
		if (!product->init(arg0, arg1))
		{
			delete product;
			return nullptr;
		}
		return product;
	}
private:
	Factory() {}
	~Factory() {}
	
	hashmap<string, ref_ptr<AbstractCreator> > creators_;
};

}

#endif
