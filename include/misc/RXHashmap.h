//it seems that using hashmap is the most difficult things to do in a portable way in c++ ....


#ifndef  __RXHASHMAP_____
#define  __RXHASHMAP_____

#ifdef __GNUC__

#if (__GNUC__ >= 3)
#include <ext/hash_map>
namespace RXHashNamespace=__gnu_cxx; 
#define RX_HASHNAMESPACE __gnu_cxx
#else
#include <hash_map>
#define RXHashNamespace std //fake namespace aliasing. Gcc 2.95 doesn't support namespace alias on std
#define RX_HASHNAMESPACE std
#endif

#include <string>
namespace RX_HASHNAMESPACE
{
	template<>
	struct hash<std::string>
	{
		size_t operator()( const std::string& x ) const
		{
			return __stl_hash_string(x.c_str());
		}
	};
}




#else //#ifdef __GNUC__
#include <hash_map>
#if (_MSC_VER >= 1310) //VS7.1 and up
#define RX_HASHNAMESPACE stdext
namespace RXHashNamespace=stdext; 
#else
//VS 7.0 needs this (or hashmap<std::string, T> won't compile
size_t std::hash_compare<std::string>::operator ()(const std::string& val) const
{
	size_t keyVal = 0;
	for(size_t i = 0, l = val.size(); i < l; ++i)
	{
		keyVal += val[i];
	}
	ldiv_t div = ldiv((long)keyVal, 127773);
	div.rem = 16807 * div.rem - 2836 * div.quot;
	if(div.rem < 0)
	{
		div.rem += 2147483647;
	}
	return div.rem;
}


size_t std::hash_compare<std::wstring>::operator ()(const std::wstring& val) const
{
	size_t keyVal = 0;
	for(size_t i = 0, l = val.size(); i < l; ++i)
	{
		keyVal += val[i];
	}
	ldiv_t div = ldiv((long)keyVal, 127773);
	div.rem = 16807 * div.rem - 2836 * div.quot;
	if(div.rem < 0)
	{
		div.rem += 2147483647;
	}
	return div.rem;
}

#define RX_HASHNAMESPACE std
namespace RXHashNamespace=std; 

#endif
#endif







#ifdef __GNUC__

//Mimic the comportement of VS7.1 hashmap
template<typename T>
class  vsToGccHash
{
public:
	size_t operator()(const T & x) const 
	{
		return (size_t)x;
	}
};

//Specialisation for some type
template<>
class  vsToGccHash<std::string>
{
public:
	size_t operator()(const std::string & x) const 
	{
		return RXHashNamespace::__stl_hash_string(x.c_str());
	}
};



#endif

#ifdef _MSC_VER
#define ADD_TYPENAME
#else
#define ADD_TYPENAME typename
#endif

template<typename T, typename T2>
#ifdef __GNUC__
class RXTPtrHash : public RXHashNamespace::hash_map<T, T2 *, vsToGccHash<T> >
#else 
class RXTPtrHash : public RXHashNamespace::hash_map<T, T2 *>
#endif
{
public:
	~RXTPtrHash()
	{
		ADD_TYPENAME RXTPtrHash<T,T2>::iterator it = this->begin();


		while(it != this->end())
		{
			delete it->second;
			++it;
		}

	}
};



template<typename T, typename T2>
#ifdef __GNUC__
class RXTHashMap : public RXHashNamespace::hash_map<T, T2 *, vsToGccHash<T> >
#else 
class RXTHashMap : public RXHashNamespace::hash_map<T, T2 *>
#endif
{
};


#undef ADD_TYPENAME

#endif





