#ifndef __RW___TWO_PLAYER
#define __RW___TWO_PLAYER


#ifdef __GNUC__ 
#if (__GNUC__ >= 3)
#include <ext/hash_map>
#else
#include <hash_map>
#endif
#endif

#include <stddef.h>

class RXPlayer;

class RXTwoPlayer {
public:
	RXTwoPlayer(const RXPlayer * _p1, const RXPlayer *_p2);

	operator size_t() const;

	//bool operator < (const RXTwoPlayer &r)const;

	const RXPlayer * getP1() const { return p1;}
	const RXPlayer * getP2() const { return p2;}

private:
	const RXPlayer * p1, *p2;
};



//Hash code computation. Note how it's easy and pleasant to do portable code....

#ifdef __GNUC__
#if (__GNUC__ >= 3)
namespace __gnu_cxx {
	template <> struct hash<RXTwoPlayer>
	{
		size_t operator()(const ::RXTwoPlayer &truc) const { return (size_t)(truc); }
	};
};
#else 
namespace std {
	__STL_TEMPLATE_NULL struct hash<RXTwoPlayer>
	{
		size_t operator()(const ::RXTwoPlayer &truc) const { return (size_t)(truc); }
	};
}
#endif

#endif

#endif
