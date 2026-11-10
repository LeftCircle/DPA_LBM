#pragma once

#include <GL/glut.h>
#include <cstddef>
#include <filesystem>

#include "image_data.h"
#include "lbmd2q9.h"
#include "lbm_data.h"
#include "color.h"
#include "lbm_substeps.h"

namespace dpa_lbm{


class LBMViewer2D{
    
public:
    enum Views{DENSITY, VORTICITY};
    
    LBMViewer2D(
        std::shared_ptr<LBMData> data,
        std::shared_ptr<LBMd2q9> solver,
        std::shared_ptr<SolverSystem> sys
    );

    void start_viewer();
    // Uses the solver to advance lbm data based on solver properties.
    void tick();

    // Save the displayed view every interval simulation steps. Zero disables saving.
    // Each enabled sequence is written to a new run directory under output_directory.
    void configure_image_sequence(std::size_t interval,
                                  const std::filesystem::path& output_directory = "frames");

    int img_width() const;
    int img_height() const;

    void set_min_color(const Color& col);
    void set_max_color(const Color& col);
    Color get_pixel(int x, int y) const;

    void reset();
    
    void set_view(Views view) { _view = view; }
    
    
private:
    void _color_pixels();
    void _color_density();
    void _color_vorticity();
    void _init_viewer();
    void _display();
    void _color_bounds();
    void _keyboard(unsigned char key, int x, int y);

public:
    BounceBackHalfway* bounds = nullptr;

private:
    Color _max_color = Color(1, 1, 1);
    Color _min_color = Color(0, 0, 0);
    
    std::shared_ptr<LBMData> _data;
    std::shared_ptr<LBMd2q9> _solver;
    std::shared_ptr<SolverSystem> _solver_system;
    ImageData _img;
    std::size_t _save_interval = 0;
    std::size_t _sequence_frame = 0;
    std::filesystem::path _sequence_directory;
    float _total_density;
    float _estimated_max_speed;
    float _t = 0;

    // Some glut viewer voodoo
    static LBMViewer2D* _active_viewer;
    static void _display_callback();
    static void _keyboard_callback(unsigned char key, int x, int y){if (_active_viewer != nullptr) {
        _active_viewer->_keyboard(key, x, y);
    }}
    
    int _view = Views::DENSITY;

};


} // end namespace dpa_lbm





