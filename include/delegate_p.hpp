#define JOIN(a, b) 		REAL_JOIN(a, b)
#define REAL_JOIN(a, b) a##b
#define DELEGATE_FUNC	JOIN(DelegateFunc, NUMBER_OF_ARGS)
#define STATIC_DELEGATE	JOIN(StaticDelegate, NUMBER_OF_ARGS)
#define CLASS_DELEGATE	JOIN(ClassDelegate, NUMBER_OF_ARGS) 
#define DELEGATE		JOIN(Delegate, NUMBER_OF_ARGS)

template <class Ret TEMPLATE_ARGS>
class DELEGATE_FUNC
{
public:
	virtual ~DELEGATE_FUNC() {}
	virtual Ret operator() (METHOD_ARGS) = 0;
};

template <class Ret TEMPLATE_ARGS>
class STATIC_DELEGATE : public DELEGATE_FUNC<Ret TEMPLATE_INST_ARGS>
{
	typedef Ret (*del_func)(METHOD_ARGS);
public:
	STATIC_DELEGATE(del_func f) :
		func(f)
	{}

	Ret operator() (METHOD_ARGS)
	{
		return (*func)(ARGS);
	}
private:
	del_func func;
};

template <class Obj, class Ret TEMPLATE_ARGS>
class CLASS_DELEGATE : public DELEGATE_FUNC<Ret TEMPLATE_INST_ARGS>
{
	typedef Ret (Obj::*del_func)(METHOD_ARGS);
public:
	CLASS_DELEGATE(Obj* o, del_func f) :
		obj(o), func(f)
	{}

	Ret operator() (METHOD_ARGS)
	{
		return (obj->*func)(ARGS);
	}
private:
	Obj* obj;
	del_func func;
};

template <class Ret TEMPLATE_ARGS>
class DELEGATE
{
	typedef DELEGATE_FUNC<Ret TEMPLATE_INST_ARGS> Delegate;
	typedef std::list< boost::shared_ptr<Delegate> > DelegatesList;
public:
	typedef Ret ReturnType;
public:
	DELEGATE() {}
	DELEGATE(Delegate* delegate)
	{
        delegates.push_back(boost::shared_ptr<Delegate>(delegate));
	}

	DELEGATE& operator= (Delegate* delegate)
	{
		delegates.clear();
		delegates.push_back(delegate);
		return *this;
	}

	DELEGATE& operator+= (Delegate* delegate)
	{
		delegates.push_back(boost::shared_ptr<Delegate>(delegate));
		return *this;
	}

	bool is_null() const
	{
		return delegates.empty();
	}

	Ret operator() (METHOD_ARGS)
	{
		typename DelegatesList::iterator prevend = delegates.end();
		--prevend;
		for (typename DelegatesList::iterator it = delegates.begin(); it != prevend; ++it)		
			(*(*it))(ARGS);
		return (*(*prevend))(ARGS);
	}
private:
	DelegatesList delegates;
};

template <class Ret TEMPLATE_ARGS>
DELEGATE_FUNC<Ret TEMPLATE_INST_ARGS>* create_delegate(Ret (*func)(METHOD_ARGS))
{
	return new STATIC_DELEGATE<Ret TEMPLATE_INST_ARGS>(func);
}

template <class Obj, class Ret TEMPLATE_ARGS>
DELEGATE_FUNC<Ret TEMPLATE_INST_ARGS>* create_delegate(Obj* o, Ret (Obj::*func)(METHOD_ARGS))
{
	return new CLASS_DELEGATE<Obj, Ret TEMPLATE_INST_ARGS>(o, func);
}

#undef JOIN
#undef REAL_JOIN
#undef DELEGATE_FUNC
#undef STATIC_DELEGATE
#undef CLASS_DELEGATE

