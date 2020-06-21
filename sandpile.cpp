#include"sandpile.hpp"

#include<iostream>
#include<map>
#include<cassert>
#include<utility>

template<int...I>
static void identities_impl(std::integer_sequence<int, I...>)
{
	makeIdentities<I...>(std::cout);
}

static void identities() {
	identities_impl(std::make_integer_sequence<int, 1026>{});
}

#if 0
static void identities() {
	makeIdentities<1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
		17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
		34, 35, 36, 37, 49, 50, 51, 299, 300, 301, 302, 307, 397, 450, 498, 499, 500>(std::cout);
}
#endif

template<int N>
static void increment(Sandpile<N> &s) {
	s.a[0]++;
	for (int i = 0; i < N * N; i++) {
		if (s.a[i] >= 4) {
			s.a[i] -= 4;
			if ((i+1) < N * N) s.a[i+1]++;
		} else {
			break;
		}
	}
}

int main()
{
	constexpr int N = 3;
	std::ios::sync_with_stdio(false);
	const auto maxpile(Sandpile<N>::makeMax());
	const auto emptypile(Sandpile<N>::makeEmpty());
	{
		Sandpile<N> test(maxpile);
		increment(test);
		assert(test != maxpile);
		assert(test == emptypile);
	}
	{
		const Sandpile<3> maxpile(Sandpile<3>::makeMax()), identity{{2, 1, 2, 1, 0, 1, 2, 1, 2}};
		std::cout << maxpile << "+\n" << identity << "=\n" << maxpile+identity << std::endl;
	}
	auto pile(emptypile);
	std::map<Sandpile<N>, int> map;
	std::size_t tries = 0;
	do {
		const auto plus(maxpile + pile);
#if 0
		std::cout << maxpile << "+\n" << pile << "=\n" << plus << '\n';
#endif
		if (plus == emptypile) {
			std::cerr << "found max + something = empty:\n" << pile << std::endl;
		}
		if (pile != emptypile and plus == maxpile) {
			std::cerr << "found the identity: max + identity = max:\n" << pile << std::endl;
		}
		map[plus]++;
		tries++;
		increment(pile);
	} while(pile != emptypile);
	std::cout << "Tried " << tries << " piles and got a set of " << map.size() << " piles\n";

	identities();
}
