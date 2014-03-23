#ifndef __COMPONENT_MANAGER_HPP__
#define __COMPONENT_MANAGER_HPP__

#include "component.hpp"
#include "simple_messages.hpp"
#include "fixed_size_vector.hpp"
#include "engine_config.hpp"
#include "assert.hpp"

namespace mhe {
namespace game {

class Engine;

class ComponentManager
{
	class ComponentPoolBase
	{
	public:
		virtual ~ComponentPoolBase() {}
		virtual std::string name() const = 0;
		virtual void update(cmn::uint tick, Engine* engine) = 0;
	};

	template <class T>
	class ComponentPool : public ComponentPoolBase
	{
		typedef boost::shared_ptr<T> curr_component_ptr;
		typedef fixed_size_vector<curr_component_ptr, initial_component_pool_size> pool;
		typedef T component_type;

		friend class ComponentManager;
	public:
		ComponentPool(bool /*initialize*/)
		{
			for (size_t i = 0; i < initial_component_pool_size; ++i)
				pool_[i].reset(new T);
		}

		ComponentPool() {}

		curr_component_ptr get() const
		{
			typename pool::iterator it = pool_.next_predefined_element();
			ASSERT(it != pool_.end(), "ComponentPool::get failed. Maybe you need to increase the pool capacity");
			return *it;
		}

		std::string name() const
		{
			return component_type::type_name();
		}	

		void add(const curr_component_ptr& component)
		{
			pool_.push_back(component);
		}

		void update(cmn::uint tick, Engine* engine)
		{
			for (typename pool::iterator it = pool_.begin(); it != pool_.end();)
			{
				const curr_component_ptr& component = *it;
				if (check_for_component_delete(component))
				{
					DEBUG_LOG("destroy component:" << component->full_name());
					destroy_component(component);
					pool_.erase(it++);
				}
				else
				{
					component->Component::update(tick, engine);
					++it;
				}
			}
		}

		bool check_for_component_delete(const curr_component_ptr& component) const
		{
			return (component.use_count() == 1);
		}

		void destroy_component(const curr_component_ptr& component)
		{
			component->Component::update(DestroyMessage());
		}
	private:
		mutable pool pool_;
	};

    typedef boost::shared_ptr<ComponentPoolBase> component_pool_ptr;
	typedef fixed_size_vector<component_pool_ptr, 8> pool_vector;
	typedef std::map<std::string, component_ptr> component_map;
	typedef ComponentPool<Component> pool;
public:
	void add(const component_ptr& component);
	void add(Component* component);

	void update(cmn::uint tick, Engine* engine);

	template <class T>
	boost::shared_ptr<T> create(const std::string& name, bool common_pool = true) const
	{
		return init_component<T>(create_component<T>(common_pool), name);
	}

	template <class T, class A1>
	boost::shared_ptr<T> create(const std::string& name, A1 arg1, bool common_pool = true) const
	{
		return init_component<T, A1>(create_component<T>(common_pool), name, arg1);
	}	

	template <class T, class A1, class A2>
	boost::shared_ptr<T> create(const std::string& name, A1 arg1, A2 arg2, bool common_pool = true) const
	{
		return init_component<T, A1, A2>(create_component<T>(common_pool), name, arg1);
	}	
private:
	template <class T>
	const ComponentPool<T>* get_pool(const std::string& name) const
	{
		for (size_t i = 0; i < pools_.size(); ++i)
		{
			if (pools_[i]->name() == name)
				return static_cast< const ComponentPool<T>* >(pools_[i].get());
		}
		return nullptr;
	}

	template <class T>
	boost::shared_ptr<T> create_component(bool common_pool) const
	{
		if (common_pool)
		{
			boost::shared_ptr<T> component(new T());
			common_pool_.add(component);
			return component;
		}
		const std::string& type_name = T::type_name();
		const ComponentPool<T>* pool = get_pool<T>(type_name);
		if (pool != nullptr)
			return pool->get();
		boost::shared_ptr< ComponentPool<T> > new_pool(new ComponentPool<T>(true));
		pools_.push_back(new_pool);
		return new_pool->get();
	}

	template <class T>
	boost::shared_ptr<T> init_component(const boost::shared_ptr<T>& component, const std::string& name) const
	{
		component->set_component_name(name);		
		return component;
	}

	template <class T, class A1>
	boost::shared_ptr<T> init_component(const boost::shared_ptr<T>& component, const std::string& name, A1 arg1) const
	{
		component->set_component_name(name);
		component->init_component(arg1);
		return component;
	}

	template <class T, class A1, class A2>
	boost::shared_ptr<T> init_component(const boost::shared_ptr<T>& component, const std::string& name,
										A1 arg1, A2 arg2) const
	{
		component->set_component_name(name);
		component->init_component(arg1, arg2);
		return component;
	}

	mutable pool common_pool_;
	mutable pool_vector pools_;
};

}}

#endif
