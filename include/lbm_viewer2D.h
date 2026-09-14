#pragma once

#include "image_data.h"
#include "lbmd2q9.h"
#include "lbm_data.h"
#include "color.h"

namespace dpalbm{


class LBMViewer2D{

public:
    
    LBMViewer2D(const std::shared_ptr<LBMData> data, const std::shared_ptr<LBMd2q9> solver);

    void start_viewer();
    // Uses the solver to advance lbm data based on solver properties.
    void tick();

    int img_width() const;
    int img_height() const;

    void set_min_color(const Color& col);
    void set_max_color(const Color& col);
    Color get_pixel(int x, int y) const;

private:
    void _color_pixels();



private:
    Color _max_color = Color(1, 0, 0);
    Color _min_color = Color(0, 0, 0);
    std::shared_ptr<LBMData> _data;
    std::shared_ptr<LBMd2q9> _solver;
    ImageData _img;
    float _total_density;
    float _estimated_max_speed;
    float _t = 0;
    


};




} // end namespace dpalbm




