#ifndef __REF_COUNTER_HPP__
#define __REF_COUNTER_HPP__

#include "types.hpp"

namespace mhe {

class ref_counter
{
public:
	ref_counter() :
		refs_(0), weaks_(0)
	{}

	void add_ref()
	{
		#ifdef MHE_THREAD_SAFE_REFS
		atomics_inc(refs_);
		#else
		++refs_;
		#endif
	}

	bool rem_ref()
	{
		#ifdef MHE_THREAD_SAFE_REFS
		atomic_dec(refs_);
		#else
		--refs_;
		#endif
		return refs_ != 0;
	}

	void add_weak()
	{
		#ifdef MHE_THREAD_SAFE_REFS
		atomics_inc(weaks_);
		#else
		++weaks_;
		#endif
	}

	void rem_weak()
	{
		#ifdef MHE_THREAD_SAFE_REFS
		atomic_dec(weaks_);
		#else
		--weaks_;
		#endif
	}

	uint16_t refs_number() const
	{
		return refs_;
	}

	uint16_t weaks_number() const
	{
		return weaks_;
	}
private:
	uint16_t refs_;
	uint16_t weaks_;
};

}

#endif
