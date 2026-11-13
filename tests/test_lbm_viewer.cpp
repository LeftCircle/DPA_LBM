#include "catch_helpers.h"


// #include "lbm_viewer2D.h"
// #include <chrono>
// #include <filesystem>

// namespace dpa_lbm{

// TEST_CASE("viewer saves a numbered image sequence at the requested interval") {
//     namespace fs = std::filesystem;
//     const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
//     const auto root = fs::temp_directory_path() / ("dpa_lbm_sequence_" + std::to_string(stamp));
//     struct Cleanup {
//         fs::path path;
//         ~Cleanup() { std::error_code ec; fs::remove_all(path, ec); }
//     } cleanup{root};

//     auto data = std::make_shared<LBMData>(3, 3, 9);
//     data->set_all_density(1.0);
//     auto lattice = std::make_shared<LBMd2q9>(1.0, 1.0);
//     auto sys = std::make_shared<SolverSystem>();
//     LBMViewer2D viewer(data, lattice, sys);
//     BounceBackHalfway bounds(3, 3);
//     bounds.block(0, 0);
//     viewer.bounds = &bounds;

//     viewer.configure_image_sequence(2, root);
//     viewer.tick();
//     REQUIRE_FALSE(fs::exists(root / "run_0001/frame_00000001.png"));
//     viewer.tick();
//     const auto first = root / "run_0001/frame_00000002.png";
//     REQUIRE(fs::exists(first));

//     ImageData image(first.string().c_str());
//     REQUIRE(image.get_width() == 3);
//     REQUIRE(image.get_height() == 3);
//     // The bottom-left blocked cell appears at the bottom of the image file.
//     REQUIRE(image.get_pixel_value(0, 2, 0) > 0.9f);
//     REQUIRE(image.get_pixel_value(0, 2, 1) < 0.1f);
//     REQUIRE(image.get_pixel_value(0, 0, 0) < 0.1f);

//     viewer.configure_image_sequence(2, root);
//     viewer.tick();
//     viewer.tick();
//     REQUIRE(fs::exists(first));
//     REQUIRE(fs::exists(root / "run_0002/frame_00000002.png"));
//     viewer.configure_image_sequence(0, root);
//     viewer.tick();
//     viewer.tick();
//     REQUIRE_FALSE(fs::exists(root / "run_0002/frame_00000004.png"));
// }

// TEST_CASE("test lbm viewer image is sized correctly"){
//     // // feed the lbm viewer a solver and data
//     // // Confirm that the image is the correct size
//     // std::shared_ptr<LBMData> data = std::make_shared<LBMData>(1920, 1080, 9);
//     // std::shared_ptr<LBMd2q9> solver = std::make_shared<LBMd2q9>(1.0, 1.0);
//     // LBMViewer2D viewer(data, solver);

//     // REQUIRE(viewer.img_width() == 1920);
//     // REQUIRE(viewer.img_height() == 1080);
// }

// TEST_CASE("Test lbm viewer sets pixel on advance"){
//     // std::shared_ptr<LBMData> data = std::make_shared<LBMData>(1920, 1080, 9);
//     // std::shared_ptr<LBMd2q9> solver = std::make_shared<LBMd2q9>(1.0, 1.0);
//     // data->dens(0, 0) = 10;
//     // solver->set_to_equilibrium(*data);
//     // Color min_color = Color(0, 0, 1);
//     // Color max_color = Color(1, 0, 0);
//     // LBMViewer2D viewer(data, solver);
//     // viewer.set_min_color(min_color);
//     // viewer.set_max_color(max_color);
//     // REQUIRE(viewer.get_pixel(0, 0) != min_color);
//     // //EQUIRE(viewer.get_pixel(1, 0) == min_color);
    
//     // viewer.tick();
    
//     // auto c = viewer.get_pixel(1, 0);
//     // printf("color = %f, %f, %f\n", c.r, c.g, c.b);
//     // REQUIRE(viewer.get_pixel(1, 0) != min_color);
//     // // Difusion should have occured
//     // REQUIRE(viewer.get_pixel(0, 0) != max_color);

// }
// }
