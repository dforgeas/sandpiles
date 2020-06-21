#include<valarray>
#include<algorithm>
#include<iterator>
#include<fstream>
#include<string>

template<int N>
struct Sandpile
{
	std::valarray<unsigned char> a;
	Sandpile& operator+=(const Sandpile &s) {
		a += s.a;
		topple();
		return *this;
	}
	Sandpile operator+(const Sandpile &s) const {
		Sandpile result(*this);
		result += s;
		return result;
	}
	Sandpile& operator-=(const Sandpile &s) {
		a -= s.a;
		return *this;
	}
	Sandpile& operator-(const Sandpile &s) const {
		Sandpile result(*this);
		result -= s;
		return result;
	}
	// nonsensical operator just to be able to store in std::map
	bool operator<(const Sandpile &s) const {
		return std::lexicographical_compare(std::begin(a), std::end(a),
			       std::begin(s.a), std::end(s.a));
	}
	bool operator==(const Sandpile &s) const {
		return (a == s.a).min();
	}
	bool operator!=(const Sandpile &s) const {
		return !(*this == s);
	}
	static Sandpile makeMax() {
		Sandpile result;
		result.a.resize(N * N, 3);
		return result;
	}
	static Sandpile makeEmpty() {
		Sandpile result;
		result.a.resize(N * N);
		return result;
	}
	static Sandpile makeSixes() {
		Sandpile result;
		result.a.resize(N * N, 6);
		return result;
	}
	static Sandpile makeIdentity() {
		auto sixes(makeSixes());
		sixes.topple();
		auto result(makeSixes());
		result -= sixes;
		result.topple();
		return result;
	}
	friend std::ostream& operator<<(std::ostream&out, const Sandpile&s) {
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				out << +s.a[N * i + j] << ' ';
			}
			out << '\n';
		}
		return out;
	}
private:
	void topple() {
		bool stable = false;
		while (not stable) {
			stable = true;
			for (int i = 0; i < N; ++i)
				for (int j = 0; j < N; ++j) {
					if (a[N * i + j] >= 4) {
						stable = false;
						a[N * i + j] -= 4;
						if (i > 0) a[N * (i-1) + j]++;
						if ((i+1) < N) a[N * (i+1) + j]++;
						if (j > 0) a[N * i + j-1]++;
						if ((j+1) < N) a[N * i + j+1]++;
					}
				}
		}
	}
};

template <int N>
Sandpile<N> readPGM(const char*const filename) {
	using namespace std::string_literals;
	std::ifstream in(filename, std::ios::binary);
	if (not in) {
		throw std::runtime_error(""); // expected case, no need for an error message
	}
	std::string header;
	if (not (std::getline(in, header) and header == "P5 " + std::to_string(N) + ' ' + std::to_string(N) + " 3")) {
		throw std::runtime_error("invalid sandpile header in "s + filename);
	}
	Sandpile<N> result;
	result.a.resize(N * N);
	in.read(reinterpret_cast<char*>(&result.a[0]), result.a.size());
	if (in.gcount() != result.a.size()) {
		throw std::runtime_error("invalid sandpile data in "s + filename);
	}
	return result;
}

inline std::string makeFilename(const int N) {
	using namespace std::string_literals;
	return "sandpile"s + std::to_string(N) + ".pgm";
}

template <int N>
bool tryToReadIdentity(std::ostream &out) {
	const auto filename(makeFilename(N));
	try {
		const auto identityPile(readPGM<N>(filename.c_str()));
#if 0 // would make sense but it seemingly takes as much time as writeIdentity
		const auto pile(Sandpile<N>::makeMax());
		const auto newPile(pile + identityPile);
		const bool ok(newPile == pile);
		if (not ok) {
			out << "Read " << filename << "but it isn't the indentity pile for size " << N << '.' << std::endl;
		}
		return ok;
#endif
		return true;
	} catch (std::exception const& exc) {
		if (*exc.what()) { // if it doesn't start with a nul terminator
			out << exc.what() << std::endl;
		}
		return false;
	}
}

template<int N>
void writePGM(const Sandpile<N> &s, const char*const filename) {
	std::ofstream out(filename, std::ios::binary);
	out << "P5 " << N << ' ' << N << " 3\n";
	out.write(reinterpret_cast<const char*>(&s.a[0]), s.a.size());
}

template<int N>
void writeIdentity() {
	const auto pile(Sandpile<N>::makeIdentity());
	const auto filename(makeFilename(N));
	writePGM(pile, filename.c_str());
}

inline void decoFacP(std::ostream &out, int n) {
	out << n << ": ";
	while (n >= 2 and n % 2 == 0) {
		out << "2 ";
		n /= 2;
	}
	while (n >= 3 and n % 3 == 0) {
		out << "3 ";
		n /= 3;
	}
	int step = 4;
	for (int f = 5; f <= n; f += (step = 6 - step)) {
		while (n % f == 0) {
			out << f << ' ';
			n /= f;
		}
	}
	out << std::endl;
}

template<int N>
void makeIdentity(std::ostream &out) {
	decoFacP(out, N);
	if (not tryToReadIdentity<N>(out)) {
		writeIdentity<N>();
	}
}

template<>
void makeIdentity<0>(std::ostream &out) {
	// do nothing
}

template <int...Ns>
void makeIdentities(std::ostream &out) {
	const int dummy[] = {
		(makeIdentity<Ns>(out), 0)...
	};
	(void)dummy;
}

