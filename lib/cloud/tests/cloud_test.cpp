// UFO
#include <ufo/cloud/cloud.hpp>
#include <ufo/utility/type_traits.hpp>

// Catch2
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

namespace ufo
{
template <class T, class... Types>
struct CloudElement
    : T
    , Types... {
	constexpr CloudElement() = default;

	constexpr CloudElement(T const& t, Types const&... types) : T(t), Types(types)... {}

	constexpr CloudElement(T const& t, CloudElement<Types...> const& ce)
	    : T(t), Types(ce)...
	{
	}
};

template <class T>
struct CloudElement<T> {
	constexpr CloudElement() = default;

	constexpr CloudElement(T const& t) : T(t) {}
};

template <class... Types>
using OldCloud = std::vector<CloudElement<Types...>>;
}  // namespace ufo

struct Point {
	float x = 0.0;
	float y = 0.0;
	float z = 0.0;

	Point() = default;

	Point(float x, float y, float z) : x(x), y(y), z(z) {}

	Point& operator+=(Point rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	friend bool operator==(Point lhs, Point rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	friend bool operator!=(Point lhs, Point rhs) { return !(lhs == rhs); }
};

struct Occupancy {
	float occupancy = 0.5;

	Occupancy() = default;

	Occupancy(float occupancy) : occupancy(occupancy) {}

	Occupancy& operator=(float occupancy)
	{
		this->occupancy = occupancy;
		return *this;
	}

	friend bool operator==(Occupancy lhs, Occupancy rhs)
	{
		return lhs.occupancy == rhs.occupancy;
	}

	friend bool operator!=(Occupancy lhs, Occupancy rhs) { return !(lhs == rhs); }
};

struct Color {
	float red   = 0.0;
	float green = 0.0;
	float blue  = 0.0;

	Color() = default;

	Color(float red, float green, float blue) : red(red), green(green), blue(blue) {}

	friend bool operator==(Color lhs, Color rhs)
	{
		return lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue;
	}

	friend bool operator!=(Color lhs, Color rhs) { return !(lhs == rhs); }
};

std::ostream& operator<<(std::ostream& out, Point p)
{
	return out << "X: " << p.x << " Y: " << p.y << " Z: " << p.z;
}

std::ostream& operator<<(std::ostream& out, Occupancy o)
{
	return out << "Occupancy: " << o.occupancy;
}

std::ostream& operator<<(std::ostream& out, Color c)
{
	return out << "Red: " << c.red << " Green: " << c.green << " Blue: " << c.blue;
}

template <class... Ts, std::enable_if_t<ufo::contains_type_v<Point, Ts...>, bool> = true>
void applyTranslation(ufo::Cloud<Ts...>& cloud, Point const& translation)
{
	// for (auto e : cloud) {
	for (Point& p : cloud) {
		// e += translation;
		// e.template get<Point>() += translation;
		p += translation;
	}
}

template <class... Ts, std::enable_if_t<ufo::contains_type_v<Point, Ts...>, bool> = true>
void applyTranslation2(ufo::Cloud<Ts...>& cloud, Point const& translation)
{
	// for (Point& e : cloud.template get<Point>()) {
	for (Point& e : ufo::get<Point>(cloud)) {
		e += translation;
	}
}

template <class... Ts, std::enable_if_t<ufo::contains_type_v<Point, Ts...>, bool> = true>
void applyTranslation2(ufo::OldCloud<Ts...>& cloud, Point const& translation)
{
	// for (Point& e : cloud.template get<Point>()) {
	for (Point& e : cloud) {
		e += translation;
	}
}
TEST_CASE("Cloud reserve")
{
	ufo::Cloud<Point> cloud;
	REQUIRE(cloud.size() == 0);
	REQUIRE(cloud.capacity() == 0);

	cloud.reserve(10);

	REQUIRE(cloud.size() == 0);
	REQUIRE(cloud.capacity() == 10);
}

TEST_CASE("Cloud shrink to fit")
{
	ufo::Cloud<Point> cloud;
	REQUIRE(cloud.size() == 0);
	REQUIRE(cloud.capacity() == 0);

	cloud.reserve(10);

	REQUIRE(cloud.size() == 0);
	REQUIRE(cloud.capacity() == 10);

	cloud.emplace_back(Point(1, 1, 1));
	cloud.emplace_back(Point(2, 2, 2));

	REQUIRE(cloud.size() == 2);
	REQUIRE(cloud.capacity() == 10);

	cloud.shrink_to_fit();

	REQUIRE(cloud.size() == 2);
	REQUIRE(cloud.capacity() == 2);
}

TEST_CASE("Cloud clear") {
	ufo::Cloud<Point> cloud;
	REQUIRE(cloud.size() == 0);
	cloud.emplace_back(Point(1, 1, 1));
	REQUIRE(cloud.size() == 1);\
	cloud.clear();
	REQUIRE(cloud.size() == 0);
}

TEST_CASE("Cloud popback")
{
	ufo::Cloud<Point> cloud;
	cloud.emplace_back(Point(1, 1, 1));
	cloud.emplace_back(Point(2, 2, 2));

	REQUIRE(cloud.size() == 2);
	cloud.pop_back();
	REQUIRE(cloud.size() == 1);
	cloud.pop_back();
	REQUIRE(cloud.size() == 0);
}

TEST_CASE("Cloud resize")
{
	SECTION("Resize with default value")
	{
		ufo::Cloud<Point> cloud;

		REQUIRE(cloud.size() == 0);
		cloud.resize(2);
		REQUIRE(cloud.size() == 2);
	}

	SECTION("Resize with value")
	{
		ufo::Cloud<Point> cloud;

		REQUIRE(cloud.size() == 0);

		cloud.resize(2, Point(1, 2, 3));

		REQUIRE(cloud.size() == 2);
		REQUIRE(cloud[0].get<Point>() == Point(1, 2, 3));
		REQUIRE(cloud[1].get<Point>() == Point(1, 2, 3));
	}
}

TEST_CASE("Cloud")
{
	ufo::Cloud<Occupancy, Color, Point> cloud;

	cloud.emplace_back(0.5, Color{1.0, 0.0, 1.0});
	cloud.emplace_back();
	cloud.emplace_back(3.0);

	decltype(cloud)::value_type value = cloud[0];

	std::cout << value << std::endl;

	std::cout << cloud[0] << std::endl;

	std::cout << static_cast<Occupancy>(cloud[0]) << std::endl;

	cloud[0].get<Occupancy>() = 0.75;

	std::cout << static_cast<Occupancy>(cloud[0]) << std::endl;

	cloud[0].set<Occupancy>(1.0);

	std::cout << static_cast<Occupancy>(cloud[0]) << std::endl;

	static_cast<Occupancy&>(cloud[0]) = 0.3;

	std::cout << static_cast<Occupancy>(cloud[0]) << std::endl;

	std::cout << cloud[0].get<Color>() << std::endl;

	cloud[0].set<Color>(1.0, 0.3, 0.0);

	std::cout << cloud[0].get<Color>() << std::endl;

	std::cout << std::as_const(cloud)[0].occupancy << std::endl;

	decltype(cloud)::value_type ce = cloud[0];

	std::cout << ce.occupancy << std::endl;

	cloud.push_back(cloud[0]);

	cloud[1].set<Occupancy>(0.0);

	std::cout << cloud[0] << std::endl;
	std::cout << cloud[1] << std::endl;

	std::cout << "Before:" << std::endl;
	for (auto const& e : cloud) {
		std::cout << e << std::endl;
	}

	for (auto e : cloud) {
		e.set<Color>(0.0, 0.0, 0.0);
	}

	std::cout << "After:" << std::endl;
	for (auto const& e : cloud) {
		std::cout << e << std::endl;
	}

	ufo::Cloud<Point, Color>    c1(1000);
	ufo::Cloud<Point, Color>    c2(1000);
	ufo::OldCloud<Point, Color> oc(1000);

	// std::cout << "Before" << std::endl;
	// for (auto const& e : c1) {
	// 	std::cout << e << std::endl;
	// }

	applyTranslation(c1, Point(1, -2, 5));
	applyTranslation2(c2, Point(1, -2, 5));
	applyTranslation2(oc, Point(1, -2, 5));

	REQUIRE(c1.max_size() == c2.max_size());
	REQUIRE(c1.capacity() == c2.capacity());

	// std::cout << "After" << std::endl;
	// for (auto const& e : c1) {
	// 	std::cout << e << std::endl;
	// }

	REQUIRE(c1 == c2);

	BENCHMARK("applyTranslation 1") { applyTranslation(c1, Point(1, -2, 5)); };
	BENCHMARK("applyTranslation 2") { applyTranslation2(c2, Point(1, -2, 5)); };
	BENCHMARK("old applyTranslation") { applyTranslation2(oc, Point(1, -2, 5)); };

	ufo::Cloud<Color> c3(5);

	std::cout << "Before" << std::endl;
	for (auto const& e : c3) {
		std::cout << e << std::endl;
	}

	// applyTranslation(c3, Point(1, -2, 5));

	std::cout << "After" << std::endl;
	for (auto const& e : c3) {
		std::cout << e << std::endl;
	}

	ufo::Cloud<Point, Occupancy, Color> c4(3);
	c4[0] = Color(1.0, 0.0, 0.5);

	std::cout << "Last" << std::endl;
	for (auto const& e : c4) {
		std::cout << e << std::endl;
	}
}
