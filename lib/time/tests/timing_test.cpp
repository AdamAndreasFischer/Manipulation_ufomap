// UFO
#include <ufo/time/timing.hpp>

// Catch2
#include <catch2/catch_test_macros.hpp>

// STL
#include <thread>

TEST_CASE("Timing")
{
	volatile int a{};

	using namespace std::chrono_literals;

	ufo::Timing t("Total", {{"First", {{"A", {"1", "2"}}, "B", "C"}}, "Second", "Third"});

	double first_agg{};
	double a_agg{};
	double b_agg{};

	double first_max{};
	double a_max{};
	double b_max{};

	std::size_t iter = 100000;
	for (std::size_t i{}; iter != i; ++i) {
		t.start("First");
		t.start("A");
		t.stop();

		t.start("B");
		// t.start("1");
		// t.stop();
		a += random();
		t.stop();
		t.stop();

		// t.start("Second");
		// t.start("A");
		// t.stop();
		// t.stop();

		// t.start("Third");
		// t.start("Looooooooooong");
		// t.stop();
		// t.stop();

		auto first_start = std::chrono::high_resolution_clock::now();
		auto a_start     = std::chrono::high_resolution_clock::now();
		auto a_stop      = std::chrono::high_resolution_clock::now();

		auto b_start = std::chrono::high_resolution_clock::now();
		a += random();
		auto b_stop     = std::chrono::high_resolution_clock::now();
		auto first_stop = std::chrono::high_resolution_clock::now();

		auto first_elapsed = std::chrono::duration<double, std::chrono::nanoseconds::period>(
		    first_stop - first_start);
		auto a_elapsed =
		    std::chrono::duration<double, std::chrono::nanoseconds::period>(a_stop - a_start);
		auto b_elapsed =
		    std::chrono::duration<double, std::chrono::nanoseconds::period>(b_stop - b_start);
		first_agg += first_elapsed.count();
		a_agg += a_elapsed.count();
		b_agg += b_elapsed.count();

		first_max = std::max(first_max, first_elapsed.count());
		a_max     = std::max(a_max, a_elapsed.count());
		b_max     = std::max(b_max, b_elapsed.count());

		// std::thread t1([&t]() {
		// 	t.start("First");
		// 	t.start("B");
		// 	t.stopAll();
		// });

		// t1.join();

		// ufo::Timing t2;
		// t[""] = std::move(t2);

		std::printf("\r%d", a);
	}

	std::printf("\n");

	t.printNanoseconds(true, true);
	std::printf("Mean: %.4f, %.4f, %.4f\n", (first_agg / iter), (a_agg / iter),
	            (b_agg / iter));
	std::printf("Max:  %.4f, %.4f, %.4f\n", first_max, a_max, b_max);

	// ufo::Timing t("Test");

	// for (std::size_t i {}; 1000 > i; ++i) {
	// 	t.start("Firstaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa").start("A");
	// 	t.stop();
	// 	t.start("B");
	// 	t.stop(2);
	// 	t.start("Second");
	// 	t.start("A");
	// 	t.stop();
	// 	t.start("B");
	// 	t.stopAll();
	// }

	// t.printNanoseconds("UFO", true, true);

	// ufo::Timing t2("Viz");
	// ufo::Timing t3("Integration");

	// auto& a = t3.start();

	// std::thread t1([&t = a]() {
	// 	std::this_thread::sleep_for(1ms);
	// 	t.start("Ray casting");
	// 	std::this_thread::sleep_for(10ms);
	// 	t.stop();
	// });

	// std::this_thread::sleep_for(4ms);
	// t3.start("Marching cubes");
	// std::this_thread::sleep_for(10ms);
	// t3.stop();

	// t1.join();

	// t3.stopAll();

	// ufo::Timing t("Tag", t2, t3);
	// t.printMilliseconds(true, true, true, 3, 10, 1);

	// ufo::Timing t("Test");

	// std::thread t1([&t]() {
	// 	std::this_thread::sleep_for(10ms);
	// 	t.start("Hej");
	// 	std::this_thread::sleep_for(10ms);
	// 	t.stop();
	// });

	// std::this_thread::sleep_for(10ms);
	// t.start("Hej");
	// std::this_thread::sleep_for(10ms);
	// t.stop();

	// t1.join();

	// ufo::Timing t("Test");

	// std::thread t1{[&t]() {
	// 	ufo::Timing& a = t.start("Thread 1");
	// 	std::this_thread::sleep_for(5ms);
	// 	std::thread t2([&t = a]() {
	// 		std::this_thread::sleep_for(5ms);
	// 		t.start("Thread 1.2");
	// 		std::this_thread::sleep_for(5ms);
	// 		t.stop();
	// 	});
	// 	t.start("Wow");
	// 	std::this_thread::sleep_for(10ms);
	// 	t.stopAll();

	// 	t2.join();
	// }};

	// std::thread t2{[&t]() {
	// 	t.start("Thread 2");
	// 	std::this_thread::sleep_for(5ms);
	// 	t.stop();
	// }};

	// t1.join();
	// t2.join();

	// t.printNanoseconds(true, true, true, 1, 10, 2);

	// ufo::Timing t("Test");

	// auto& a = t.start("First").start("A");

	// a.start("B").start("C").start("A");

	// a.stopAll();

	// a.start().start("B").start("C");

	// t.stopAll();

	// t.printMilliseconds(true, true, true, 1, 10, 2);

	// for (std::size_t i{}; 1000 > i; ++i) {
	// 	t.start("First").start("A");

	// 	a.start("B").start("C").start("A");

	// 	a.stopAll();

	// 	a.start();
	// 	a.start("B").start("C");

	// 	t.stopAll();

	// 	t.printMilliseconds(true, true, true, 1, 10, 2);
	// }

	// ufo::Timing t("Test");

	// // t.start();
	// t.start("First");

	// t.start("A");

	// t.stop();
	// t.stop();
	// t.start("Second");
	// t.stop();
	// t.stop();

	// // std::thread t1([&t]() {
	// // 	t.stopAll();
	// // 	// std::this_thread::sleep_for(10ms);
	// // 	// t.start("Hej");
	// // 	// std::this_thread::sleep_for(10ms);
	// // 	// t.stop();
	// // });

	// // t.stopAll();

	// // // std::this_thread::sleep_for(10ms);
	// // // t.start("Hej");
	// // // std::this_thread::sleep_for(10ms);
	// // // t.stop();

	// // t1.join();

	// t.printMilliseconds(true, true, true, 2, 10, 2);
}