// UFO
#include <ufo/cloud/point_cloud.hpp>
#include <ufo/map/integrator/angular_integrator.hpp>
#include <ufo/map/ufomap.hpp>
#include <ufo/math/transform3.hpp>
#include <ufo/vision/camera.hpp>

// STL
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <future>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// C STL
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// TOML
#include <toml++/toml.hpp>

// STB
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// PCL
#include <pcl/filters/crop_box.h>
#include <pcl/filters/uniform_sampling.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/registration/gicp6d.h>
#include <pcl/registration/icp.h>
#include <pcl/search/kdtree.h>
#include <pcl/surface/mls.h>

#define RESET       "\033[0m"
#define BLACK       "\033[30m"        /* Black */
#define RED         "\033[31m"        /* Red */
#define GREEN       "\033[32m"        /* Green */
#define YELLOW      "\033[33m"        /* Yellow */
#define BLUE        "\033[34m"        /* Blue */
#define MAGENTA     "\033[35m"        /* Magenta */
#define CYAN        "\033[36m"        /* Cyan */
#define WHITE       "\033[37m"        /* White */
#define BOLDBLACK   "\033[1m\033[30m" /* Bold Black */
#define BOLDRED     "\033[1m\033[31m" /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m" /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m" /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m" /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m" /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m" /* Bold White */

using Map   = ufo::Map3D<ufo::OccupancyMap, ufo::ColorMapLabW32f>;
using Cloud = ufo::PointCloud<3, float, ufo::Lab32f>;

struct Dataset {
	std::vector<std::pair<std::filesystem::path, ufo::Transform3f>> data;
	ufo::Transform3f                                                transform;

	Dataset(toml::table const& config)
	{
		std::cout << "Loading dataset\n";

		std::filesystem::path dir;
		if (auto v = config["dataset_path"].value<std::string_view>(); v) {
			dir = *v;
		} else {
			std::cerr << BOLDRED << " - Missing dataset_path.\n" << RESET;
			exit(EXIT_FAILURE);
		}

		if ("" == dir) {
			std::cerr << BOLDRED << " - Empty dataset_path.\n" << RESET;
			exit(EXIT_FAILURE);
		}

		if (auto v = config["transform"].as_array(); v) {
			std::vector<float> components;
			v->for_each([&components](auto&& e) {
				if constexpr (toml::is_number<decltype(e)>) {
					components.push_back(*e);
				} else {
					std::cerr
					    << BOLDRED
					    << " - 'transform' should consist of 7 floats in the format [x, y, z, "
					       "qw, qx, qy, qz] "
					       "(e.g., [0, 0, 0, 1, 0, 0, 0])\n"
					    << RESET;
					exit(EXIT_FAILURE);
				}
			});
			if (7 != components.size()) {
				std::cerr << BOLDRED
				          << " - 'transform' should be in the format [x, y, z, qw, qx, qy, qz] "
				             "(e.g., [0, 0, 0, 1, 0, 0, 0])\n"
				          << RESET;
				exit(EXIT_FAILURE);
			}

			transform = ufo::Transform3f(
			    ufo::Quatf(components[3], components[4], components[5], components[6]),
			    ufo::Vec3f(components[0], components[1], components[2]));
		} else {
			std::cerr << BOLDRED << " - Missing transform.\n" << RESET;
			exit(EXIT_FAILURE);
		}

		std::vector<std::filesystem::path> clouds;
		std::vector<ufo::Transform3f>      poses;
		for (auto const& entry : std::filesystem::recursive_directory_iterator(dir)) {
			if (!entry.is_regular_file()) {
				continue;
			}

			auto const& path = entry.path();

			if (".pcd" == path.extension()) {
				clouds.push_back(path);
			} else if (".tsv" == path.extension()) {
				if (!poses.empty()) {
					std::cerr << BOLDRED << " - Multiple poses files (i.e., '.tsv' files).\n"
					          << RESET;
					exit(EXIT_FAILURE);
				}

				std::ifstream data(path, std::ios::in | std::ios::binary);
				std::string   s;
				while (std::getline(data, s)) {
					std::stringstream  ss(s);
					std::vector<float> elements;

					std::string tmp;
					while (getline(ss, tmp, '\t')) {
						elements.push_back(std::stof(tmp));
					}

					poses.emplace_back(
					    normalize(ufo::Quatf(elements[3], elements[4], elements[5], elements[6])),
					    ufo::Vec3f(elements[0], elements[1], elements[2]));
				}
			}
		}

		std::cout << " - Found " + std::to_string(clouds.size()) + " clouds and " +
		                 std::to_string(poses.size()) + " poses.\n";

		if (clouds.size() != poses.size()) {
			std::cerr << BOLDRED << " - Number of clouds is not the same as number of poses\n"
			          << RESET;
			exit(EXIT_FAILURE);
		}

		assert(clouds.size() == poses.size());

		std::sort(clouds.begin(), clouds.end());

		data.reserve(clouds.size());
		std::transform(clouds.begin(), clouds.end(), poses.begin(), std::back_inserter(data),
		               [](auto const& a, auto const& b) { return std::make_pair(a, b); });
	}

	[[nodiscard]] std::size_t size() const { return data.size(); }

	[[nodiscard]] Cloud cloud(std::size_t index) const
	{
		std::cout << "Loading cloud " << data[index].first.string() << "\n";

		pcl::PointCloud<pcl::PointXYZRGBA>::Ptr pcl_cloud(
		    new pcl::PointCloud<pcl::PointXYZRGBA>);
		if (-1 ==
		    pcl::io::loadPCDFile<pcl::PointXYZRGBA>(data[index].first.string(), *pcl_cloud)) {
			std::cerr << BOLDRED << "Could not read file " + data[index].first.string() + ".\n"
			          << RESET;
			exit(EXIT_FAILURE);
		}

		for (auto& p : *pcl_cloud) {
			p.x *= 0.001f;
			p.y *= 0.001f;
			p.z *= 0.001f;
		}

		// ufo::Mat4x4f    tf = static_cast<ufo::Mat4x4f>(this->tf(index));
		// Eigen::Matrix4f pcl_tf{
		//     // clang-format off
		// 			{tf[0][0], tf[1][0], tf[2][0], tf[3][0]},
		// 			{tf[0][1], tf[1][1], tf[2][1], tf[3][1]},
		// 			{tf[0][2], tf[1][2], tf[2][2], tf[3][2]},
		// 			{tf[0][3], tf[1][3], tf[2][3], tf[3][3]},
		//     // clang-format on
		// };

		// pcl::transformPointCloud(*pcl_cloud, *pcl_cloud, pcl_tf);

		// // TODO: Do not hardcode this
		// pcl::CropBox<pcl::PointXYZRGBA> filter;
		// filter.setMin(Eigen::Vector4f(-2.7, -2.7, -1.0, 0.0));
		// filter.setMax(Eigen::Vector4f(2.7, 2.7, 10.0, 0.0));
		// filter.setInputCloud(pcl_cloud);
		// pcl::PointCloud<pcl::PointXYZRGBA>::Ptr ds(new pcl::PointCloud<pcl::PointXYZRGBA>);
		// filter.filter(*ds);
		// *pcl_cloud = *ds;

		// pcl::transformPointCloud(*pcl_cloud, *pcl_cloud, pcl_tf.inverse());

		Cloud cloud;
		cloud.reserve(pcl_cloud->size());
		for (auto const& point : *pcl_cloud) {
			cloud.emplace_back(ufo::Vec3f(point.x, point.y, point.z),
			                   ufo::convert<ufo::ColorType::LAB32F>(
			                       ufo::toLinearRGB(ufo::RGB8u{point.r, point.g, point.b})));
		}

		return cloud;
	}

	[[nodiscard]] ufo::Transform3f tf(std::size_t index) const
	{
		return data[index].second * transform;
	}
};

// new code from here
struct ConvexQuadXY {
	// Vertices in clockwise order (TL, TR, BR, BL) matching the top-down corner convention.
	// Used to test whether a world-space XY point falls inside the original rig boundary.
	std::array<std::array<float, 2>, 4> verts;

	// Returns true if (px, py) is inside the convex quad.
	// For clockwise winding a point is inside when all 2D cross products are <= 0.
	bool contains(float px, float py) const
	{
		for (std::size_t i = 0; i < 4; ++i) {
			auto const& a     = verts[i];
			auto const& b     = verts[(i + 1) % 4];
			float       cross = (b[0] - a[0]) * (py - a[1]) - (b[1] - a[1]) * (px - a[0]);
			if (cross > 0.0f) return false;
		}
		return true;
	}
};
// new code ends here

struct Renderer {
	ufo::Image<ufo::Ray3> rays;
	std::filesystem::path save_dir;
	ufo::RGB8u            background_color;
	float                 min_dist = 0.01;
	float                 max_dist = 2.7;
	// new code from here
	ConvexQuadXY inner_boundary;
	// new code ends here

	Renderer(toml::table const& config)
	{
		std::cout << "Creating renderer\n";

		if (auto v = config["output_dir"].value<std::string_view>(); v) {
			save_dir = *v;
		} else {
			std::cerr << BOLDRED << " - Missing output_dir.\n" << RESET;
			exit(EXIT_FAILURE);
		}

		if ("" == save_dir) {
			std::cerr << BOLDRED << " - Empty output_dir.\n" << RESET;
			exit(EXIT_FAILURE);
		}

		if (auto v = config["near_clip"].value<float>(); v) {
			min_dist = *v;
		} else {
			std::cout << YELLOW << " - Missing near_clip\n" << RESET;
			exit(EXIT_FAILURE);
		}

		if (auto v = config["far_clip"].value<float>(); v) {
			max_dist = *v;
		} else {
			std::cout << YELLOW << " - Missing far_clip\n" << RESET;
			exit(EXIT_FAILURE);
		}

		unsigned width  = 160;
		unsigned height = 160;

		if (auto v = config["width"].value<std::uint32_t>(); v) {
			width = *v;
		} else {
			std::cout << YELLOW << " - Missing width, using default width of '" << width
			          << "' instead.\n"
			          << RESET;
		}

		if (auto v = config["height"].value<std::uint32_t>(); v) {
			height = *v;
		} else {
			std::cout << YELLOW << " - Missing height, using default height of '" << height
			          << "' instead.\n"
			          << RESET;
		}

		float elevation = 5.0f;

		if (auto v = config["elevation"].value<float>(); v) {
			elevation = *v;
		} else {
			std::cout << YELLOW << " - Missing elevation, using default elevation of '"
			          << elevation << "' instead.\n"
			          << RESET;
		}

		auto coord_f = [&config, elevation](std::string const& name,
		                                    ufo::Vec2f const&  default_val = ufo::Vec2f{}) {
			if (auto v = config[name].as_array(); v) {
				std::vector<float> components;
				v->for_each([&name, &components](auto&& e) {
					if constexpr (toml::is_number<decltype(e)>) {
						components.push_back(*e);
					} else {
						std::cerr << BOLDRED << " - coordinate '" + name
						          << "' should consist of 2 floats in the format [x, y] (e.g., [0, "
						             "0])\n"
						          << RESET;
						exit(EXIT_FAILURE);
					}
				});
				if (2 != components.size()) {
					std::cerr
					    << BOLDRED << " - coordinate '" + name
					    << "' should consist of 2 floats in the format [x, y] (e.g., [0, 0])\n"
					    << RESET;
					exit(EXIT_FAILURE);
				}
				return ufo::Vec3f(components[0], components[1], elevation);
			} else {
				std::cerr << BOLDRED << " - Missing coordinate '" + name
				          << "', using default coordinate '" << default_val << "' instead.\n"
				          << RESET;
				return ufo::Vec3f(default_val, elevation);
			}
		};

		ufo::Vec3f top_left     = coord_f("top_left", ufo::Vec2f(-2.0f, 2.0f));
		ufo::Vec3f top_right    = coord_f("top_right", ufo::Vec2f(2.0f, 2.0f));
		ufo::Vec3f bottom_right = coord_f("bottom_right", ufo::Vec2f(2.0f, -2.0f));
		ufo::Vec3f bottom_left  = coord_f("bottom_left", ufo::Vec2f(-2.0f, -2.0f));

		// new code from here
		// Load the original (pre-expansion) rig corners as the clutter-rejection boundary.
		// Points outside this quad are discarded before insertion into the UFO map.
		auto inner_tl = coord_f("inner_top_left",     ufo::Vec2f( 2.95f,  1.70f));
		auto inner_tr = coord_f("inner_top_right",    ufo::Vec2f( 2.80f, -1.80f));
		auto inner_br = coord_f("inner_bottom_right", ufo::Vec2f(-0.90f, -1.83f));
		auto inner_bl = coord_f("inner_bottom_left",  ufo::Vec2f(-1.12f,  1.74f));
		inner_boundary.verts = {{
		    {inner_tl.x, inner_tl.y},
		    {inner_tr.x, inner_tr.y},
		    {inner_br.x, inner_br.y},
		    {inner_bl.x, inner_bl.y},
		}};
		// new code ends here

		rays = ufo::Image<ufo::Ray3>(width, height);
		for (std::size_t r{}; rays.rows() > r; ++r) {
			for (std::size_t c{}; rays.cols() > c; ++c) {
				auto top          = ufo::lerp(top_left, top_right, float(c) / rays.cols());
				auto bottom       = ufo::lerp(bottom_left, bottom_right, float(c) / rays.cols());
				rays(r, c).origin = ufo::lerp(top, bottom, float(r) / rays.rows());
				rays(r, c).direction = ufo::Vec3f(0, 0, -1);
			}
		}

		if (auto v = config["background_color"].as_array(); v) {
			std::vector<std::uint8_t> components;
			v->for_each([&components](auto&& e) {
				if constexpr (toml::is_number<decltype(e)>) {
					components.push_back(*e);
				} else {
					std::cerr
					    << BOLDRED
					    << " - background_color should consist of 3 8-bit unsigned integers in "
					       "the format [r, g, b] (e.g., [0, 32, 255])\n"
					    << RESET;
					exit(EXIT_FAILURE);
				}
			});
			if (3 != components.size()) {
				std::cerr
				    << BOLDRED
				    << " - background_color should consist of 3 8-bit unsigned integers in the "
				       "format [r, g, b] (e.g., [0, 32, 255])\n"
				    << RESET;
				exit(EXIT_FAILURE);
			}
			background_color = ufo::RGB8u{components[0], components[1], components[2]};
		} else {
			std::cerr << BOLDRED << " - Missing background_color, using default color '"
			          << background_color << "' instead.\n"
			          << RESET;
		}
	}

	void render(Map& map)
	{
		pcl::PointCloud<pcl::PointXYZRGB>::Ptr pcl_cloud(
		    new pcl::PointCloud<pcl::PointXYZRGB>);
		for (auto node : map.query(ufo::pred::Leaf{} && ufo::pred::Occupied{})) {
			auto coord = map.center(node.index);
			auto color = map.colorSRGB<ufo::ColorType::RGB8U>(node.index);
			pcl_cloud->push_back(pcl::PointXYZRGB(coord.x, coord.y, coord.z, color.red,
			                                      color.green, color.blue));
		}

		// pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree(
		//     new pcl::search::KdTree<pcl::PointXYZRGB>);

		// // Output has the PointNormal type in order to store the normals calculated by MLS
		// pcl::PointCloud<pcl::PointXYZRGBNormal> mls_points;

		// // Init object (second point type is for the normals, even if unused)
		// pcl::MovingLeastSquares<pcl::PointXYZRGB, pcl::PointXYZRGBNormal> mls;

		// mls.setComputeNormals(true);

		// // Set parameters
		// mls.setInputCloud(pcl_cloud);
		// mls.setPolynomialOrder(2);
		// mls.setSearchMethod(tree);
		// // mls.setSearchRadius(0.03);
		// mls.setSearchRadius(0.01);
		// mls.setSqrGaussParam(0.0001);
		// mls.setComputeNormals(true);
		// mls.setUpsamplingMethod(
		//     pcl::MovingLeastSquares<pcl::PointXYZRGB,
		//                             pcl::PointXYZRGBNormal>::VOXEL_GRID_DILATION);
		// mls.setDilationVoxelSize(0.005f);
		// mls.setDilationIterations(4);

		// // Reconstruct
		// mls.process(mls_points);

		// map.clear();

		// for (auto const& point : mls_points) {
		// 	ufo::Vec3f n(point.x, point.y, point.z);
		// 	ufo::RGB8u c{point.r, point.g, point.b};
		// 	map.occupancyUpdate(n, 0.6f, false);
		// 	map.colorAdd(n, ufo::toLinearRGB(c), false);
		// }

		// map.propagate(ufo::execution::par);
		// map.modifiedReset();

		// pcl_cloud->clear();
		// for (auto node : map.query(ufo::pred::Leaf{} && ufo::pred::Occupied{})) {
		// 	auto coord = map.center(node.index);
		// 	auto color = map.colorSRGB<ufo::ColorType::RGB8U>(node.index);
		// 	pcl_cloud->push_back(pcl::PointXYZRGB(coord.x, coord.y, coord.z, color.red,
		// 	                                      color.green, color.blue));
		// }

		pcl::io::savePCDFile((save_dir / "map.pcd").string(), *pcl_cloud, true);

		ufo::Image<ufo::TraceResult<3>> nodes(rays.rows(), rays.cols());
		map.trace(ufo::execution::par, rays.begin(), rays.end(), nodes.begin(),
		          ufo::pred::Leaf{} && ufo::pred::Occupied{}, min_dist, max_dist);

		ufo::Image<ufo::RGB8u> raw_rgb_image(rays.rows(), rays.cols());
		ufo::transform(ufo::execution::par, nodes.begin(), nodes.end(), raw_rgb_image.begin(),
		               [this, &map](auto hit) {
			               return map.valid(hit.node)
			                          ? map.colorSRGB<ufo::ColorType::RGB8U>(hit.node)
			                          : background_color;
		               });

		double max_depth = 0;
		for (auto const& n : nodes) {
			double depth = map.valid(n.node) ? n.distance : max_dist;
			max_depth    = std::max(max_depth, depth);
		}

		ufo::Image<std::uint8_t> raw_depth_image(rays.rows(), rays.cols());
		ufo::transform(ufo::execution::par, nodes.begin(), nodes.end(),
		               raw_depth_image.begin(), [this, &map, max_depth](auto hit) {
			               double depth = map.valid(hit.node) ? hit.distance : max_depth;
			               depth        = std::round(depth * 255 / max_depth);
			               return static_cast<std::uint8_t>(depth);
		               });

		ufo::Image<int> seen(raw_rgb_image.rows(), raw_rgb_image.cols(), 0);
		std::cout << raw_rgb_image(92, 95) << "\n";
		std::cout << raw_rgb_image(95, 92) << "\n";
		std::set<std::pair<int, int>> open;
		for (int iter{}; 1 > iter; ++iter) {
			for (int r{}; raw_rgb_image.rows() > r; ++r) {
				for (int c{}; raw_rgb_image.cols() > c; ++c) {
					if (255 == raw_depth_image(r, c)) {
						continue;
					}

					seen(r, c) = 1;
					open.insert(std::pair(c, r));

					while (!open.empty()) {
						auto cur = *open.begin();
						open.erase(cur);
						for (int y = -1; 1 >= y; ++y) {
							for (int x = -1; 1 >= x; ++x) {
								int ry = cur.second + y;
								int cx = cur.first + x;
								if (0 > ry || raw_rgb_image.rows() <= ry || 0 > cx ||
								    raw_rgb_image.cols() <= cx || 0 < seen(ry, cx)) {
									continue;
								}

								if (255 == raw_depth_image(ry, cx)) {
									seen(ry, cx) = 2;
									continue;
								}

								seen(ry, cx) = 1;
								open.insert(std::pair(cx, ry));
							}
						}
					}

					std::size_t num_white{};
					std::size_t num_green{};

					for (int r{}; raw_rgb_image.rows() > r; ++r) {
						for (int c{}; raw_rgb_image.cols() > c; ++c) {
							if (1 != seen(r, c)) {
								continue;
							}

							auto color = raw_rgb_image(r, c);

							if (130 < color.red && 160 > color.red && 190 < color.green &&
							    210 > color.green && 130 < color.blue && 150 > color.blue) {
								++num_green;
							} else if (220 < color.red && 220 < color.green && 220 < color.blue) {
								++num_white;
							}
						}
					}

					// if (5 > num_green && 30 > num_white) {
					// 	for (int r{}; raw_rgb_image.rows() > r; ++r) {
					// 		for (int c{}; raw_rgb_image.cols() > c; ++c) {
					// 			if (0 == seen(r, c)) {
					// 				continue;
					// 			}

					// 			raw_rgb_image(r, c)   = background_color;
					// 			raw_depth_image(r, c) = 255;
					// 		}
					// 	}
					// }

					std::fill(seen.begin(), seen.end(), 0);
					open.clear();
				}
			}
		}

		auto f = [this](std::string const& name, auto raw_image) {
			std::vector<char> image;

			auto comp = sizeof(typename decltype(raw_image)::value_type) / sizeof(char);
			if (0 != stbi_write_png_to_func(
			             [](void* context, void* data, int size) {
				             std::vector<char>* v = static_cast<std::vector<char>*>(context);
				             std::size_t        s = v->size();
				             v->resize(s + size);
				             std::memcpy(v->data() + s, data, size);
			             },
			             &image, raw_image.cols(), raw_image.rows(), comp, raw_image.data(),
			             raw_image.cols() * comp)) {
				std::cerr << BOLDRED << "Error writing image\n" << RESET;
			}

			std::filesystem::path path = save_dir / (name + ".png");
			std::cout << "Saving file " << path << '\n';
			std::ofstream file(path, std::ios::binary | std::ios::out);
			file.write(image.data(), image.size());
			file.close();

			image.clear();
		};

		f("rgb", raw_rgb_image);
		f("depth", raw_depth_image);
	}
};

[[nodiscard]] Map createMap(toml::table const& config)
{
	std::cout << "Creating UFOMap\n";

	float resolution = 0.1f;
	if (auto v = config["resolution"].value<float>(); v) {
		resolution = *v;
	} else {
		std::cout << YELLOW << " - Missing UFOMap resolution, using '" << resolution
		          << "'.\n";
	}

	std::cout << " - Resolution: " << resolution << " m\n";

	return Map(resolution);
}

[[nodiscard]] ufo::AngularIntegrator<3> createIntegrator(toml::table const& config)
{
	std::cout << "Creating UFOMap integrator\n";

	ufo::AngularIntegrator<3> integrator;
	integrator.linearize_rgb = true;

	if (auto v = config["angular_resolution"].value<float>(); v) {
		integrator.angularResolution(ufo::radians(*v));
	} else {
		std::cout << YELLOW << " - Missing 'angular_resolution', using '"
		          << ufo::degrees(integrator.angularResolution()) << "'.\n";
	}

	if (auto v = config["min_distance"].value<float>(); v) {
		integrator.min_distance = *v;
	} else {
		std::cout << YELLOW << " - Missing 'min_distance', using '" << integrator.min_distance
		          << "'.\n";
	}

	if (auto v = config["max_distance"].value<float>(); v) {
		integrator.max_distance = *v;
	} else {
		std::cout << YELLOW << " - Missing 'max_distance', using '" << integrator.max_distance
		          << "'.\n";
	}

	if (auto v = config["sensor_angular_resolution"].value<float>(); v) {
		integrator.sensor_angular_resolution = ufo::radians(*v);
	} else {
		std::cout << YELLOW << " - Missing 'sensor_angular_resolution', using '"
		          << ufo::degrees(integrator.sensor_angular_resolution) << "'.\n";
	}

	if (auto v = config["translation_error"].value<float>(); v) {
		integrator.translation_error = *v;
	} else {
		std::cout << YELLOW << " - Missing 'translation_error', using '"
		          << integrator.translation_error << "'.\n";
	}

	if (auto v = config["orientation_error"].value<float>(); v) {
		integrator.orientation_error = *v;
	} else {
		std::cout << YELLOW << " - Missing 'orientation_error', using '"
		          << integrator.orientation_error << "'.\n";
	}

	if (auto v = config["occupancy_hit"].value<float>(); v) {
		integrator.occupancy_hit = *v;
	} else {
		std::cout << YELLOW << " - Missing 'occupancy_hit', using '"
		          << integrator.occupancy_hit << "'.\n";
	}

	if (auto v = config["occupancy_miss"].value<float>(); v) {
		integrator.occupancy_miss = *v;
	} else {
		std::cout << YELLOW << " - Missing 'occupancy_miss', using '"
		          << integrator.occupancy_miss << "'.\n";
	}

	std::cout << " - Angular resolution: " << ufo::degrees(integrator.angularResolution())
	          << "°\n";
	std::cout << " - Min. distance: " << integrator.min_distance << " m\n";
	std::cout << " - Max. distance: " << integrator.max_distance << " m\n";
	std::cout << " - Sensor angular resolution: "
	          << ufo::degrees(integrator.sensor_angular_resolution) << "°\n";
	std::cout << " - Translation error: " << integrator.translation_error << " m\n";
	std::cout << " - Orientation error: " << ufo::degrees(integrator.orientation_error)
	          << "°\n";
	std::cout << " - Occupancy hit: " << (100 * integrator.occupancy_hit) << "%\n";
	std::cout << " - Occupancy miss: " << (100 * integrator.occupancy_miss) << "%\n";

	return integrator;
}

int main(int argc, char* argv[])
{
	if (2 != argc) {
		std::cerr << BOLDRED << "Run the program like: `./UFOManipulation config.toml`\n"
		          << RESET;
		return EXIT_FAILURE;
	}

	toml::table tbl;
	try {
		tbl = toml::parse_file(argv[1]);
	} catch (toml::parse_error const& err) {
		std::cerr << BOLDRED << "Error parsing file '" << *err.source().path << "':\n"
		          << err.description() << "\n (" << err.source().begin << ")\n"
		          << RESET;
		return EXIT_FAILURE;
	}

	Map      map        = createMap(*tbl["map"].as_table());
	auto     integrator = createIntegrator(*tbl["integrator"].as_table());
	Renderer renderer(*tbl["renderer"].as_table());
	Dataset  dataset(*tbl["dataset"].as_table());

	Cloud              acc;
	std::vector<float> min_dist;
	std::vector<float> max_dist;
	ufo::Transform3f   prev;
	std::size_t        num{};
	for (std::size_t i{}; dataset.size() > i; ++i) {
		Cloud cloud = dataset.cloud(i);
		auto  tf    = dataset.tf(i);
		if (prev != tf) {
			if (0 < acc.size()) {
				for (std::size_t j{}; acc.size() > j; ++j) {
					if (0.10 < std::sqrt(max_dist[j]) - std::sqrt(min_dist[j])) { //Reduced filtering threshold
						acc.view<ufo::Vec3f>()[j] =
						    ufo::transform(ufo::inverse(prev), ufo::Vec3f(10, 10, 10));
					} else {
						acc.view<ufo::Vec3f>()[j] =
						    acc.view<ufo::Vec3f>()[j] / static_cast<float>(num);
					}
					acc.view<ufo::Lab32f>()[j].lightness /= static_cast<float>(num);
					acc.view<ufo::Lab32f>()[j].a /= static_cast<float>(num);
					acc.view<ufo::Lab32f>()[j].b /= static_cast<float>(num);
				}

				ufo::transformInPlace(ufo::execution::par, prev, acc);
				// new code from here
				std::vector<bool> keep_inner(acc.size(), true);
				for (std::size_t j{}; acc.size() > j; ++j) {
					auto const& p = acc.view<ufo::Vec3f>()[j];
					if (!renderer.inner_boundary.contains(p.x, p.y)) {
						keep_inner[j] = false;
					}
				}
				// new code ends here
				auto nodes = map.create(ufo::execution::par, acc.view<ufo::Vec3f>());
				for (std::size_t j{}; acc.size() > j; ++j) {
					// new code from here
					if (!keep_inner[j]) continue;
					// new code ends here
					// auto const& p = ufo::TreeCoord(cloud.view<ufo::Vec3f>()[i], 0);
					auto const& c = acc.view<ufo::Lab32f>()[j];
					auto        n = nodes[j];
					map.occupancyUpdate(n, 0.6f, false);
					map.colorAdd(n, c, false);
				}
			}

			acc = cloud;
			min_dist.resize(cloud.size());
			max_dist.resize(cloud.size());
			for (std::size_t j{}; cloud.size() > j; ++j) {
				float dist  = ufo::normSquared(cloud.view<ufo::Vec3f>()[j]);
				min_dist[j] = dist;
				max_dist[j] = dist;
			}
			prev = tf;
			num  = 1;
		} else {
			if (acc.size() != cloud.size()) {
				std::cerr << BOLDRED << "Cloud size mismatch\n" << RESET;
				return EXIT_FAILURE;
			}

			for (std::size_t j{}; cloud.size() > j; ++j) {
				acc.view<ufo::Vec3f>()[j] += cloud.view<ufo::Vec3f>()[j];
				acc.view<ufo::Lab32f>()[j].lightness += cloud.view<ufo::Lab32f>()[j].lightness;
				acc.view<ufo::Lab32f>()[j].a += cloud.view<ufo::Lab32f>()[j].a;
				acc.view<ufo::Lab32f>()[j].b += cloud.view<ufo::Lab32f>()[j].b;

				float dist  = ufo::normSquared(cloud.view<ufo::Vec3f>()[j]);
				min_dist[j] = std::min(min_dist[j], dist);
				max_dist[j] = std::max(max_dist[j], dist);
			}
			++num;
		}

		// integrator(ufo::execution::par, map, cloud, tf);

		// ufo::transformInPlace(ufo::execution::par, tf, cloud);
		// auto nodes = map.create(ufo::execution::par, cloud.view<ufo::Vec3f>());
		// for (std::size_t i{}; cloud.size() > i; ++i) {
		// 	// auto const& p = ufo::TreeCoord(cloud.view<ufo::Vec3f>()[i], 0);
		// 	auto const& c = cloud.view<ufo::Lab32f>()[i];
		// 	auto        n = nodes[i];
		// 	map.occupancyUpdate(n, 0.6f, false);
		// 	map.colorAdd(n, c, false);
		// }
	}

	if (0 < acc.size()) {
		for (std::size_t j{}; acc.size() > j; ++j) {
			if (0.10 < std::sqrt(max_dist[j]) - std::sqrt(min_dist[j])) {
				acc.view<ufo::Vec3f>()[j] =
				    ufo::transform(ufo::inverse(prev), ufo::Vec3f(10, 10, 10));
			} else {
				acc.view<ufo::Vec3f>()[j] = acc.view<ufo::Vec3f>()[j] / static_cast<float>(num);
			}
			acc.view<ufo::Lab32f>()[j].lightness /= static_cast<float>(num);
			acc.view<ufo::Lab32f>()[j].a /= static_cast<float>(num);
			acc.view<ufo::Lab32f>()[j].b /= static_cast<float>(num);
		}

		ufo::transformInPlace(ufo::execution::par, prev, acc);
		// new code from here
		std::vector<bool> keep_final(acc.size(), true);
		for (std::size_t j{}; acc.size() > j; ++j) {
			auto const& p = acc.view<ufo::Vec3f>()[j];
			if (!renderer.inner_boundary.contains(p.x, p.y)) {
				keep_final[j] = false;
			}
		}
		// new code ends here
		auto nodes = map.create(ufo::execution::par, acc.view<ufo::Vec3f>());
		for (std::size_t i{}; acc.size() > i; ++i) {
			// new code from here
			if (!keep_final[i]) continue;
			// new code ends here
			// auto const& p = ufo::TreeCoord(cloud.view<ufo::Vec3f>()[i], 0);
			auto const& c = acc.view<ufo::Lab32f>()[i];
			auto        n = nodes[i];
			map.occupancyUpdate(n, 0.6f, false);
			map.colorAdd(n, c, false);
		}
	}

	map.propagate(ufo::execution::par);
	map.modifiedReset();

	renderer.render(map);

	return EXIT_SUCCESS;
}