#include "stdafx.h"
#include "hexlib.h"
#include <stdexcept>



constexpr double pi = 3.141592653589793;

namespace hexlib
{

	const std::vector<Hex> hex_directions =
	{
		Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
		Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
	};

	inline Hex hexlib::hex_direction(int direction)
	{
		return hex_directions[(6 + (direction % 6)) % 6];
	}

	inline Hex hexlib::hex_neighbour(Hex h, int direction)
	{
		return h + hex_direction(direction);
	}

	const Orientation pointy = Orientation(Matrix(std::sqrt(3.0), std::sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0),
		Matrix(std::sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0),
		0.5);
	const Orientation flat = Orientation(Matrix(3.0 / 2.0, 0.0, std::sqrt(3.0) / 2.0, std::sqrt(3.0)),
		Matrix(2.0 / 3.0, 0.0, -1.0 / 3.0, std::sqrt(3.0) / 3.0),
		0.0);

	Point hex_corner_offset(const Layout& layout, int corner)
	{
		Point size = layout._size;
		double angle = 2.0 * pi * (layout._orientation._start_angle + corner) / 6;
		return Point(size.x * std::cos(angle), size.y * std::sin(angle));
	}

	std::vector<Point> polygon_corners(const Layout& layout, Hex h)
	{
		std::vector<Point> corners;
		Point center = hex_to_pixel(layout, h);
		for (int i = 0; i < 6; ++i)
		{
			Point offset = hex_corner_offset(layout, i);
			corners.emplace_back(Point(center.x + offset.x, center.y + offset.y));
		}
		return corners;
	}

	Point hexlib::hex_to_pixel(Layout layout, Hex h)
	{
		const Matrix& M = layout._orientation._forward;
		Point p = M * h;
		p.x *= layout._size.x;
		p.y *= layout._size.y;
		return p + layout._origin;
	}

	FractionalHex hexlib::pixel_to_hex(Layout layout, Point p)
	{
		double x = (p.x - layout._origin.x) / layout._size.x;
		double y = (p.y - layout._origin.y) / layout._size.y;
		Point pr = layout._orientation._inverse * Point(x, y);
		return FractionalHex(pr.x, pr.y);
	}

	Hex hexlib::hex_round(FractionalHex h)
	{
		int q = static_cast<int> (std::round(h.q));
		int r = static_cast<int> (std::round(h.r));
		int s = static_cast<int> (std::round(h.s));
		double q_diff = std::abs(q - h.q);
		double r_diff = std::abs(r - h.r);
		double s_diff = std::abs(s - h.s);
		if (q_diff > r_diff && q_diff > s_diff)
		{
			q = -r - s;
		}
		else if (r_diff > s_diff)
		{
			r = -q - s;
		}
		else
		{
			s = -q - r;
		}
		return Hex(q, r, s);
	}

	double lerp(double a, double b, double t)
	{
		return a * (1.0 - t) + b * t;
	}

	FractionalHex hex_lerp(Hex a, Hex b, double t)
	{
		return FractionalHex(lerp(a.q, b.q, t), lerp(a.r, b.r, t), lerp(a.s, b.s, t));
	}

	std::vector<Hex> hex_linedraw(Hex a, Hex b)
	{
		int N = hex_distance(a, b);
		std::vector<Hex> results{};
		double step = 1.0 / std::fmax(N, 1);
		for (int i = 0; i <= N; ++i)
		{
			results.emplace_back(hex_round(hex_lerp(a, b, step * i)));
		}
		return results;
	}

	HexMap create_map(int width, int heigth, Orientation orientation)
	{
		HexMap map;
		if (orientation == pointy)
		{
			for (int a = 0; a < heigth; ++a)
			{
				int a_offset = a >> 1;
				for (int b = -a_offset; b < width - a_offset; ++b)
				{
					map.insert(Hex(b, a, -b - a));
				}
			}
		}
		if (orientation == flat)
		{
			for (int a = 0; a < width; ++a)
			{
				int a_offset = a >> 1;
				for (int b = -a_offset; b < heigth - a_offset; ++b)
				{
					map.insert(Hex(a, b, -a - b));
				}
			}
		}
		return map;
	}

	void fill_graph(HexMap& map, Hex_graph& graph)
	{
		for (auto node : map)
		{
			for (int dir = 0; dir < 6; ++dir)
			{
				auto neighbour = hex_neighbour(node, dir);
				if (map.count(node) > 0)  // neighbour is in the map
				{
					graph.edges[node].push_back(neighbour);
				}
			}
		}
	}

	HexMap breadth_first_search(Hex_graph& graph, Hex start, Hex goal)
	{
		std::queue<Hex> frontier;
		frontier.push(start);
		HexMap visited;
		visited.insert(start);

		while (!frontier.empty())
		{
			auto current = frontier.front();
			frontier.pop();
			if (current == goal)
			{
				break;
			}
			for (auto& next : graph.neighbours(current))
			{
				if (!visited.count(next))
				{
					frontier.push(next);
					visited.insert(next);
				}
			}
		}
		return visited;
	}

	void dijkstra_search(Hex_graph& graph, Hex start, Hex goal, std::unordered_map<Hex, Hex>& came_from, std::unordered_map<Hex, int>& cost_so_far)
	{
		pqueue frontier;
		frontier.emplace(0, start);

		came_from[start] = start;
		cost_so_far[start] = 0;

		while (!frontier.empty())
		{
			auto current = frontier.top().second;
			frontier.pop();
			if (current == goal)
			{
				break;
			}

			for (auto& next : graph.neighbours(current))
			{
				int new_cost = cost_so_far[current] + graph.cost(current, next);
				if (!cost_so_far.count(next) || new_cost < cost_so_far[next])
				{
					cost_so_far[next] = new_cost;
					came_from[next] = current;
					frontier.emplace(new_cost, next);
				}
			}
		}
	}

	inline int heuristic(Hex a, Hex b)
	{
		return hex_distance(a, b);
	}

	void a_star_search(Hex_graph& graph, Hex start, Hex goal, std::unordered_map<Hex, Hex>& came_from, std::unordered_map<Hex, int>& cost_so_far)
	{
		pqueue frontier;
		frontier.emplace(0, start);
		came_from[start] = start;
		cost_so_far[start] = 0;

		while (!frontier.empty())
		{
			auto current = frontier.top().second;
			frontier.pop();
			if (current == goal)
			{
				break;
			}
			for (auto& next : graph.neighbours(current))
			{
				int new_cost = cost_so_far[current] + graph.cost(current, next);
				if (!cost_so_far.count(next) || new_cost < cost_so_far[next])
				{
					cost_so_far[next] = new_cost;
					int priority = new_cost + heuristic(next, goal);
					frontier.emplace(priority, next);
					came_from[next] = current;
				}
			}
		}

	}

	std::vector<Hex> reconstruct_path(Hex start, Hex goal, std::unordered_map<Hex, Hex>& came_from)
	{
		std::vector<Hex> path;
		Hex current = goal;
		while (current != start)
		{
			path.push_back(current);
			current = came_from[current];
		}
		path.push_back(start);
		std::reverse(path.begin(), path.end());
		return path;
	}


}