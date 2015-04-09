#ifndef __HASHMAP_HPP__
#define __HASHMAP_HPP__

#include "fixed_size_vector.hpp"
#include "hash.hpp"

namespace mhe {

template <class KV, size_t Buckets, size_t BucketSize, bool IsFixedSize>
struct HashTableTraits
{};

template<class KV, size_t Buckets, size_t BucketSize>
struct HashTableTraits<KV, Buckets, BucketSize, false>
{
	struct Node
	{
		KV kv;
	};

	typedef fixed_size_vector<Node, BucketSize> SingleBucketType;
	typedef fixed_size_vector<SingleBucketType, Buckets> BucketsType;
};

template <class ContainerTraits, class KV, bool B>
struct HashTableIteratorTraits
{};

template<class ContainerTraits, class KV>
struct HashTableIteratorTraits<ContainerTraits, KV, false>
{
	typedef KV type;
	typedef typename ContainerTraits::BucketsType& buckets_type;
};

template<class ContainerTraits, class KV>
struct HashTableIteratorTraits<ContainerTraits, KV, true>
{
	typedef const KV type;
	typedef const typename ContainerTraits::BucketsType& buckets_type;
};

template <class K, class V, size_t Buckets = 64, size_t BucketSize = 8, bool IsFixedSize = false>
class hashmap
{
private:
	struct KV
	{
		K key;
		V value;
		KV() {}
		KV(const K& k, const V& v) : key(k), value(v) {}
	};

	static const hash_type end_bucket = 0xffffffff;

	typedef HashTableTraits<KV, Buckets, BucketSize, IsFixedSize> CurrentTraits;
	typedef typename HashTableTraits<KV, Buckets, BucketSize, IsFixedSize>::Node Node;
public:
	typedef hashmap<K, V, Buckets, BucketSize, IsFixedSize> this_type;
	typedef K key_type;
	typedef V value_type;
public:
	template <class Contrainer, class ContainerTraits, bool Const>
	class Iterator
	{
#ifndef MHE_VS
		friend class Container;
#else
		friend class this_type;
#endif

		typedef typename HashTableIteratorTraits<ContainerTraits, KV, Const>::type value_type;
		typedef typename HashTableIteratorTraits<ContainerTraits, KV, Const>::buckets_type buckets_type;
	public:
		Iterator(buckets_type buckets, hash_type bucket, size_t index) : buckets_(buckets),
			bucket_(bucket),
			index_(index)
		{
		}

		value_type* operator->()
		{
			return &(buckets_[bucket_][index_].kv);
		}
		
		Iterator& operator++()
		{
			next();
			return *this;
		}

		Iterator operator++(int /*v*/)
		{
			Iterator cp = *this;
			next();
			return cp;
		}

		bool operator== (const Iterator& other) const
		{
			return bucket_ == other.bucket_ && index_ == other.index_;
		}

		bool operator!= (const Iterator& other) const
		{
			return bucket_ != other.bucket_ || index_ != other.index_;
		}
	private:
		void next()
		{
			while (bucket_ < buckets_.size())
			{
				if (++index_ >= buckets_[bucket_].size())
				{
					++bucket_;
					index_ = 0;
				}
				if (bucket_ >= buckets_.size()) break;
				if (!buckets_[bucket_].empty()) return;
			}
			bucket_ = end_bucket;
			index_ = 0;
		}

		buckets_type buckets_;
		size_t bucket_;
		size_t index_;
	};

	typedef Iterator<this_type, CurrentTraits, false> iterator;
	typedef Iterator<this_type, CurrentTraits, true> const_iterator;
public:
	hashmap() : buckets_(Buckets), size_(0) {}

	const_iterator begin() const
	{
		return find_first_element();
	}

	iterator begin()
	{
		return find_first_element();
	}

	const_iterator end() const
	{
		return const_iterator(buckets_, end_bucket, 0);
	}

	iterator end()
	{
		return iterator(buckets_, end_bucket, 0);
	}

	size_t size() const
	{
		return size_;
	}

	bool empty() const
	{
		return size_ == 0;
	}

	iterator insert(const K& k, const V& v)
	{
		return insert_impl(k, v);
	}

	iterator erase(iterator it)
	{
		iterator prev = it++;
		erase_impl(prev.bucket_, prev.index_);
		return it;
	}

	iterator erase(const K& k)
	{
		iterator it = find_in_bucket(hash(k) % buckets_.capacity(), k);
		if (it == end()) return end();
		return erase(it);
	}

	void clear()
	{
		clear_impl();
	}

	value_type& operator[] (const K& k)
	{
		hash_type h = hash(k);
		h %= buckets_.capacity();
		iterator it = find_in_bucket(h, k);
		if (it == end())
		{
			iterator it = insert_into_bucket(h, k, value_type());
			return it->value;
		}
		return it->value;
	}

	const_iterator find(const K& k) const
	{
		hash_type h = hash(k);
		h %= buckets_.capacity();
		return find_in_bucket(h, k);
	}

	iterator find(const K& k)
	{
		hash_type h = hash(k);
		h %= buckets_.capacity();
		return find_in_bucket(h, k);
	}
private:
	iterator insert_impl(const K& k, const V& v)
	{
		hash_type h = hash(k);
		h %= buckets_.capacity();
		if (buckets_[h].empty()) return insert_into_bucket(h, k, v);
		else
		{
			iterator it = find_in_bucket(h, k);
			if (it == end())
				insert_into_bucket(h, k, v);
			else
			{
				it->value = v;
				return it;
			}
		}
		return end();
	}

	void erase_impl(size_t bucket, size_t index)
	{
		buckets_[bucket].erase(buckets_[bucket].begin() + index);
		--size_;
	}

	void clear_impl()
	{
		for (size_t i = 0; i < buckets_.size(); ++i)
			buckets_[i].clear();
		size_ = 0;
	}

	iterator insert_into_bucket(hash_type bucket, const K& k, const V& v)
	{
		Node node;
		node.kv.key = k;
		node.kv.value = v;

		buckets_[bucket].push_back(node);
		++size_;
		return iterator(buckets_, bucket, buckets_[bucket].size() - 1);
	}

	iterator find_in_bucket(hash_type bucket, const K& k)
	{
		for (size_t i = 0; i < buckets_[bucket].size(); ++i)
		{
			if (buckets_[bucket][i].kv.key == k)
				return iterator(buckets_, bucket, i);
		}
		return end();
	}

	const_iterator find_in_bucket(hash_type bucket, const K& k) const
	{
		for (size_t i = 0; i < buckets_[bucket].size(); ++i)
		{
			if (buckets_[bucket][i].kv.key == k)
				return const_iterator(buckets_, bucket, i);
		}
		return end();
	}

	const_iterator find_first_element() const
	{
		for (size_t i = 0; i < buckets_.size(); ++i)
		{
			if (buckets_[i].empty()) continue;
			return const_iterator(buckets_, i, 0);
		}
		return end();
	}

	iterator find_first_element()
	{
		for (size_t i = 0; i < buckets_.size(); ++i)
		{
			if (buckets_[i].empty()) continue;
			return iterator(buckets_, i, 0);
		}
		return end();
	}
	
	fixed_size_vector< fixed_size_vector<Node, BucketSize>, Buckets > buckets_;
	size_t size_; // cached
};

}

#endif
