#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <functional>
#include <ostream>
#include <SFML/Graphics.hpp>

namespace hexlib
{
	template <typename Number, int w>
	struct _Hex
	{
		union
		{
			Number v[3];
			struct
			{
				Number q;
				Number r;
				Number s;
			};
		};
		_Hex() : v{ 0, 0, 0 } {}
		_Hex(Number q, Number r) : v{ q, r, -q - r } {}
		_Hex(Number q, Number r, Number s) : v{ q, r, s } {}
	};

	using Hex = _Hex<int, 1>;
	using HexDifference = _Hex<int, 0>;
	using FractionalHex = _Hex<double, 1>;
	using FractionalHexDifference = _Hex<double, 0>;

	using HexMap = std::unordered_set<Hex>;

	inline std::ostream& operator<< (std::ostream& os, Hex h)
	{
		os << "(" << std::to_string (h.q) << ", " << std::to_string (h.r) << ", " << std::to_string (h.s) << ")";
		return os;
	}

	struct Point
	{
		double x;
		double y;
		Point(double x_, double y_) : x{ x_ }, y{ y_ } {}
		Point() : x{ 0 }, y{ 0 } {}
	};
	inline bool operator== (Hex a, Hex b)
	{
		return a.q == b.q && a.r == b.r && a.s == b.s;
	}

	inline bool operator!= (Hex a, Hex b)
	{
		return !(a == b);
	}

	inline Hex operator+ (Hex a, Hex b)
	{
		return Hex(a.q + b.q, a.r + b.r, a.s + b.s);
	}

	inline Hex operator- (Hex a, Hex b)
	{
		return Hex(a.q - b.q, a.r - b.r, a.s - b.s);
	}

	inline Hex operator* (Hex a, int k)
	{
		return Hex(a.q * k, a.r * k, a.s * k);
	}

	inline int hex_length(Hex h)
	{
		return int((std::abs(h.q) + std::abs(h.r) + std::abs(h.s)) / 2);
	}

	inline int hex_distance(Hex a, Hex b)
	{
		return hex_length(a - b);
	}

	Hex hex_direction(int direction);
	Hex hex_neighbour(Hex h, int direction);


	struct Matrix
	{
		Matrix(double a, double b, double c, double d) : _a{ a }, _b{ b }, _c{ c }, _d{ d } {};
		Matrix() : _a{ 0 }, _b{ 0 }, _c{ 0 }, _d{ 0 } {}

		double _a;
		double _b;
		double _c;
		double _d;
	};

	inline bool operator== (Matrix a, Matrix b)
	{
		if (a._a == b._a && a._b == b._b && a._c == b._c && a._d == b._d)
		{
			return true;
		}
		return false;
	}

	inline bool operator!= (Matrix a, Matrix b)
	{
		return !(a == b);
	}

	inline Point operator*(Matrix m, Hex h)
	{
		double x = m._a * h.q + m._b * h.r;
		double y = m._c * h.q + m._d * h.r;
		return Point(x, y);
	}
	inline Point operator* (Matrix m, Point p)
	{
		double x = m._a * p.x + m._b * p.y;
		double y = m._c * p.x + m._d * p.y;
		return Point(x, y);
	}

	inline Point operator* (Point p, int k)
	{
		return Point(p.x * k, p.y * k);
	}

	inline Point operator+ (Point a, Point b)
	{
		return Point(a.x + b.x, a.y + b.y);
	}

	inline Point operator- (Point a, Point b)
	{
		return Point(a.x - b.x, a.y - b.y);
	}

	struct Orientation
	{
		Orientation(Matrix forward, Matrix inverse, double start_angle) :
			_forward{ forward }, _inverse{ inverse }, _start_angle{ start_angle } {};
		Orientation() : _forward{}, _inverse{}, _start_angle{ 0 } {}
		Matrix _forward;
		Matrix _inverse;
		double _start_angle;
	};

	enum class Orientation_enum { Pointy, Flat };

	inline bool operator== (Orientation a, Orientation b)
	{
		if (a._forward == b._forward && a._inverse == b._inverse && a._start_angle == b._start_angle)
		{
			return true;
		}
		return false;
	}

	inline bool operator!= (Orientation a, Orientation b)
	{
		return !(a == b);
	}

	extern const Orientation pointy;
	extern const Orientation flat;

	inline Orientation get_orientation(Orientation_enum orient)
	{
		switch (orient)
		{
		case Orientation_enum::Flat:
			return flat;
		case Orientation_enum::Pointy:
			return pointy;
		default:
			return pointy;
		}
	}

	struct Layout
	{
		Layout(Orientation orientation, Point size, Point origin) : _orientation{ orientation }, _size{ size }, _origin{ origin } {};
		Layout() {}
		Layout(const Layout& other) : _orientation{ other._orientation }, _size{ other._size }, _origin{ other._origin } {}
		Orientation _orientation;
		Point _size;
		Point _origin;
	};

	std::vector<Point> polygon_corners(const Layout& layout, Hex h);

	Point hex_to_pixel(Layout layout, Hex h);
	FractionalHex pixel_to_hex(Layout layout, Point p);
	Hex hex_round(FractionalHex h);
	std::vector<Hex> hex_linedraw(Hex a, Hex b);
	HexMap create_map(int width, int heigth, Orientation orientation);
	sf::VertexArray to_vertex_array (const HexMap& map, Layout layout);

	template <typename Location>
	struct Graph
	{
		using iterator = typename std::vector<Location>::iterator;
		std::unordered_map<Location, std::vector<Location>> edges;

		inline const std::vector<Location> neighbours(Location id)
		{
			return edges[id];
		}

		int cost(Hex from, Hex to) const
		{
			// to do
			return 1;
		}
	};
	using Hex_graph = Graph<Hex>;

	void fill_graph(HexMap& map, Hex_graph& graph);

	HexMap breadth_first_search(Hex_graph& graph, Hex start, Hex goal);

	using PQElement = std::pair<int, Hex>;
	inline bool operator< (const PQElement a, const PQElement b)
	{
		return a.first < b.first;
	}

	inline bool operator> (const PQElement a, PQElement b)
	{
		return a.first > b.first;
	}

	using pqueue = std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>>;

	void dijkstra_search(Hex_graph& graph, Hex start, Hex goal, std::unordered_map<Hex, Hex>& came_from, std::unordered_map<Hex, int>& cost_so_far);
	void a_star_search(Hex_graph& graph, Hex start, Hex goal, std::unordered_map<Hex, Hex>& came_from, std::unordered_map<Hex, int>& cost_so_far);
	std::vector<Hex> reconstruct_path(Hex start, Hex goal, std::unordered_map<Hex, Hex>& came_from);
}

namespace std
{
	template<> struct hash<hexlib::Hex>
	{
		size_t operator() (const hexlib::Hex& h) const
		{
			hash<int> int_hash;
			size_t hq = int_hash(h.q);
			size_t hr = int_hash(h.r);
			return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
		}
	};
}