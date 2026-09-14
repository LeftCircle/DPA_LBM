#include "catch_helpers.h"


#include "lbm_viewer2D.h"

using namespace dpalbm;

TEST_CASE("test lbm viewer image is sized correctly"){
    // feed the lbm viewer a solver and data
    // Confirm that the image is the correct size
    std::shared_ptr<LBMData> data = std::make_shared<LBMData>(1920, 1080, 9);
    std::shared_ptr<LBMd2q9> solver = std::make_shared<LBMd2q9>(1.0, 1.0);
    LBMViewer2D viewer(data, solver);

    REQUIRE(viewer.img_width() == 1920);
    REQUIRE(viewer.img_height() == 1080);
}

TEST_CASE("Test lbm viewer sets pixel on advance"){
    std::shared_ptr<LBMData> data = std::make_shared<LBMData>(1920, 1080, 9);
    std::shared_ptr<LBMd2q9> solver = std::make_shared<LBMd2q9>(1.0, 1.0);
    data->dens(0, 0) = 10;
    solver->set_to_equilibrium(*data);
    Color min_color = Color(0, 0, 1);
    Color max_color = Color(1, 0, 0);
    LBMViewer2D viewer(data, solver);
    viewer.set_min_color(min_color);
    viewer.set_max_color(max_color);
    REQUIRE(viewer.get_pixel(0, 0) == max_color);
    REQUIRE(viewer.get_pixel(1, 0) == min_color);
    
    viewer.tick();

    REQUIRE(viewer.get_pixel(1, 0) != min_color);
    REQUIRE(viewer.get_pixel(0, 0) != max_color, "Diffusion should have occured");



}