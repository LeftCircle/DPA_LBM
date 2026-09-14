#include "lbm_viewer2D.h"
#include "numeric"


namespace dpalbm{


LBMViewer2D::LBMViewer2D(const std::shared_ptr<LBMData> data, const std::shared_ptr<LBMd2q9> solver) :
    _data(std::move(data)), _solver(std::move(solver)) {
        _img.set_dimensions(_data->dimension(0), _data->dimension(1), 3);
        _total_density = _data->get_total_density();
        _color_pixels();
}


void LBMViewer2D::start_viewer(){

}

void LBMViewer2D::tick(){
    _t = _solver->advance(*_data, _t);
}


void LBMViewer2D::_color_pixels(){
    for (int j = 0; j < _data->dimension(1); j++){
        for (int i = 0; i < _data->dimension(0); i++){
            auto dens = _data->dens(i, j);
            auto t = dens / _total_density;
            auto c = _min_color * (1.0 - t) + _max_color * t;
            _img.set_first_three_channels(i, j, c.r, c.b, c.g);
        }
    }
}

Color LBMViewer2D::get_pixel(int x, int y) const {
    auto p = _img.get_pixel_values(x, y);
    return Color(p[0], p[1], p[2]);
}

void LBMViewer2D::set_max_color(const Color& col){
    _max_color = col;
    _color_pixels();
}

void LBMViewer2D::set_min_color(const Color& col){
    _min_color = col;
    _color_pixels();
}

int LBMViewer2D::img_width() const {
    return _img.get_width();
}

int LBMViewer2D::img_height() const {
    return _img.get_height();
}





} // end namespace dpalbm