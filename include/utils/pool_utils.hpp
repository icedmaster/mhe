#ifndef __POOL_UTILS_HPP__
#define __POOL_UTILS_HPP__

#define POOL_ELEMENT_METHODS(Type)										\
	private: Type id_;													\
public: void set_id(Type id) { id_ = id; }								\
	Type id() const { return id_; }										\
	typedef Type IdType;

#endif
