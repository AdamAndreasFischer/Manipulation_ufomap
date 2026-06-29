// UFO
#include "test_tree.hpp"

// Catch2
#include <catch2/catch_test_macros.hpp>

// STL
#include <algorithm>
#include <iostream>
#include <random>
#include <utility>

using namespace ufo;

using Octree           = TestTree<3>;
using OctreeWithCenter = TestTree<3, true>;

TEST_CASE("[Octree] constructor")
{
	SECTION("Default constructor")
	{
		Octree tree(0.1f, 17);
		REQUIRE(tree.size() == 1);
	}
}

TEST_CASE("[Octree] comparison")
{
	SECTION("Untouched")
	{
		Octree tree1(0.1f, 17);
		Octree tree2(0.1f, 17);
		REQUIRE(tree1 == tree2);
	}

	SECTION("Equal")
	{
		Octree tree1(0.1f, 17);
		tree1.create(OctCoord(0, 0, 0));
		tree1.clear();
		Octree tree2(0.1f, 17);
		REQUIRE(tree1 == tree2);

		tree1.create(OctCoord(0));
		tree2.create(OctCoord(0));
		REQUIRE(tree1 == tree2);
	}

	SECTION("Not equal")
	{
		Octree tree1(0.1f, 17);
		Octree tree2(0.1f, 17);
		tree2.create(OctCoord(0, 0, 0));
		Octree tree3(0.01f, 17);
		Octree tree4(0.1f, 16);
		REQUIRE(tree1 != tree2);
		REQUIRE(tree1 != tree3);
		REQUIRE(tree1 != tree4);
	}
}

TEST_CASE("[Octree] create")
{
	std::vector<Vec3f>                    v;
	std::mt19937                          gen(42);
	std::uniform_real_distribution<float> dis_x(-20.0f, 20.0f);
	std::uniform_real_distribution<float> dis_y(-20.0f, 20.0f);
	std::uniform_real_distribution<float> dis_z(0.0f, 4.0f);
	for (std::size_t i{}; 10'000 > i; ++i) {
		v.emplace_back(dis_x(gen), dis_y(gen), dis_z(gen));
	}

	Octree tree(0.1f, 17);
	Octree tree_seq(0.1f, 17);
	Octree tree_par(0.1f, 17);
	Octree tree_tbb(0.1f, 17);
	Octree tree_omp(0.1f, 17);

	auto start = std::chrono::high_resolution_clock::now();
	auto n     = tree.create(v.begin(), v.end());
	auto stop  = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms = stop - start;

	start      = std::chrono::high_resolution_clock::now();
	auto n_seq = tree_seq.create(execution::seq, v.begin(), v.end());
	stop       = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> seq_ms = stop - start;

	start      = std::chrono::high_resolution_clock::now();
	auto n_par = tree_par.create(execution::par, v.begin(), v.end());
	stop       = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> par_ms = stop - start;

	start      = std::chrono::high_resolution_clock::now();
	auto n_tbb = tree_tbb.create(execution::tbb_par, v.begin(), v.end());
	stop       = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> tbb_ms = stop - start;

	start      = std::chrono::high_resolution_clock::now();
	auto n_omp = tree_omp.create(execution::omp_par, v.begin(), v.end());
	stop       = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> omp_ms = stop - start;

	std::cout << "None (1st iter): " << ms.count() << " ms\n";
	std::cout << "Seq  (1st iter): " << seq_ms.count() << " ms\n";
	std::cout << "Par  (1st iter): " << par_ms.count() << " ms\n";
	std::cout << "TBB  (1st iter): " << tbb_ms.count() << " ms\n";
	std::cout << "OMP  (1st iter): " << omp_ms.count() << " ms\n";

	start = std::chrono::high_resolution_clock::now();
	n     = tree.create(v.begin(), v.end());
	stop  = std::chrono::high_resolution_clock::now();
	ms    = stop - start;

	start  = std::chrono::high_resolution_clock::now();
	n_seq  = tree_seq.create(execution::seq, v.begin(), v.end());
	stop   = std::chrono::high_resolution_clock::now();
	seq_ms = stop - start;

	start  = std::chrono::high_resolution_clock::now();
	n_par  = tree_par.create(execution::par, v.begin(), v.end());
	stop   = std::chrono::high_resolution_clock::now();
	par_ms = stop - start;

	start  = std::chrono::high_resolution_clock::now();
	n_tbb  = tree_tbb.create(execution::tbb_par, v.begin(), v.end());
	stop   = std::chrono::high_resolution_clock::now();
	tbb_ms = stop - start;

	start  = std::chrono::high_resolution_clock::now();
	n_omp  = tree_omp.create(execution::omp_par, v.begin(), v.end());
	stop   = std::chrono::high_resolution_clock::now();
	omp_ms = stop - start;

	std::cout << "None (2st iter): " << ms.count() << " ms\n";
	std::cout << "Seq  (2nd iter): " << seq_ms.count() << " ms\n";
	std::cout << "Par  (2nd iter): " << par_ms.count() << " ms\n";
	std::cout << "TBB  (2nd iter): " << tbb_ms.count() << " ms\n";
	std::cout << "OMP  (2nd iter): " << omp_ms.count() << " ms\n";

	std::vector<OctCode> c_seq;
	std::vector<OctCode> c_par;
	std::vector<OctCode> c_omp;

	std::transform(n_seq.begin(), n_seq.end(), std::back_inserter(c_seq),
	               [&tree_seq](auto n) { return tree_seq.code(n); });
	std::transform(n_par.begin(), n_par.end(), std::back_inserter(c_par),
	               [&tree_par](auto n) { return tree_par.code(n); });
	std::transform(n_omp.begin(), n_omp.end(), std::back_inserter(c_omp),
	               [&tree_omp](auto n) { return tree_omp.code(n); });

	REQUIRE(c_seq == c_par);
	REQUIRE(c_seq == c_omp);

	SECTION("SERIAL") {}

	SECTION("Parallel") {}
}

TEST_CASE("[Octree] with and without center")
{
	SECTION("Center")
	{
		Octree           tree1(0.1f, 17);
		OctreeWithCenter tree2(0.1f, 17);

		REQUIRE(tree1.center(tree1.index()) == tree2.center(tree2.index()));

		TreeIndex node1 = tree1.create(OctCoord(0, 0, 0));
		TreeIndex node2 = tree2.create(OctCoord(0, 0, 0));

		REQUIRE(tree1.center(node1) == tree2.center(node2));
	}

	SECTION("Center axis")
	{
		Octree           tree1(0.1f, 17);
		OctreeWithCenter tree2(0.1f, 17);

		REQUIRE(tree1.centerAxis(tree1.index(), 2) == tree2.centerAxis(tree2.index(), 2));

		TreeIndex node1 = tree1.create(OctCoord(0, 0, 0));
		TreeIndex node2 = tree2.create(OctCoord(0, 0, 0));

		REQUIRE(tree1.centerAxis(node1, 1) == tree2.centerAxis(node2, 1));
	}
}

TEST_CASE("[Octree] swap")
{
	using std::swap;

	Octree tree1(0.1f, 17);
	tree1.create(OctCoord(0, 0, 0));

	Octree tree2 = tree1;

	Octree tree3(0.1f, 17);

	REQUIRE((tree1 == tree2 && tree1 != tree3));

	swap(tree2, tree3);

	REQUIRE((tree1 != tree2 && tree1 == tree3));
}

TEST_CASE("[Octree] traverse")
{
	SECTION("Index version")
	{
		Octree tree(0.1f, 17);
		tree.traverse([](TreeIndex) { return true; });
		tree.traverse(TreeCoord<3>(OctCoord(0), 15), [](TreeIndex) { return true; });
	}

	SECTION("Node version")
	{
		Octree tree(0.1f, 17);
		tree.traverse([](TreeNode<3> const& node) { return true; });
		tree.traverse([&tree](TreeNode<3> const& node) { return tree.depth(node) > 14; },
		              pred::True(), false);

		tree.traverse(
		    TreeCoord<3>(OctCoord(0), 15), [](TreeNode<3> const& node) { return true; },
		    true);
		tree.traverse(
		    TreeCoord<3>(OctCoord(0), 15),
		    [&tree](TreeNode<3> const& node) { return tree.depth(node) > 14; }, pred::True(),
		    false);
	}
}