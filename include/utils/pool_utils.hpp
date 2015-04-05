#ifndef __POOL_UTILS_HPP__
#define __POOL_UTILS_HPP__

#define POOL_ELEMENT_METHODS(Type)										\
	private: Type id_;													\
public: void set_id(Type id) { id_ = id; }								\
	Type id() const { return id_; }										\
    typedef Type IdType;												\
	static const Type invalid_id = static_cast<Type>(-1);

#define POOL_STRUCT(Type)														\
	Type id;																					\
	typedef Type IdType;															\
	static const Type invalid_id = static_cast<Type>(-1);

template <class Pool>
inline typename Pool::type& create_and_get(Pool& pool)
{
	typename Pool::index_type id = pool.create();
	return pool.get(id);
}

#endif
