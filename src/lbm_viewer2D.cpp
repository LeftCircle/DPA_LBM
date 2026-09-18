#include "lbm_viewer2D.h"
#include "numeric"



namespace dpalbm{

LBMViewer2D* LBMViewer2D::_active_viewer = nullptr;


LBMViewer2D::LBMViewer2D(const std::shared_ptr<LBMData> data, const std::shared_ptr<LBMd2q9> solver) :
    _data(std::move(data)), _solver(std::move(solver)) {
        _img.set_dimensions(_data->dimension(0), _data->dimension(1), 3);
        _total_density = _data->get_total_density();
        tick();
}


void LBMViewer2D::_init_viewer(){
    int argc = 1;
    char program_name[] = "dpa_lbm";
    char* argv[] = {program_name, nullptr};
    glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowSize( _img.get_width(), _img.get_height() );
	glutCreateWindow( "View" );

	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	glEnable( GL_DEPTH_TEST );

	glutKeyboardFunc(LBMViewer2D::_keyboard_callback);
	//glutSpecialFunc([](int key, int x, int y) { Controller::instance()->special_keys(key, x, y); });
	//glutReshapeFunc( [](int w, int h){ View::instance() -> reshape(w,h); } );
	//glutIdleFunc( [](){ View::instance() -> idle(); } );
	
    _active_viewer = this;
    glutDisplayFunc(LBMViewer2D::_display_callback);
}

void LBMViewer2D::start_viewer(){
    _init_viewer();
    glutMainLoop();

}

void LBMViewer2D::tick(){
    _t = _solver->advance(*_data, _t);
    _color_pixels();
}

void LBMViewer2D::_display(void){
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	tick();
    _img.gl_draw_pixels();

    std::string time = std::to_string(_t);
    glutSetWindowTitle(time.c_str());
	glutSwapBuffers();
	glutPostRedisplay();

}

void LBMViewer2D::_keyboard(unsigned char key, int x, int y){
    switch (key)
	{
		case 27: // esc
			exit(0);
			break;
        case 'c':{
            // Lower speed of sound
            _solver->set_speed_of_sound(_solver->get_speed_of_sound() * 0.99);
            std::printf("Speed of sound is now %f\n", _solver->get_speed_of_sound());
            break;
        }
        case 'C':{
            // Lower speed of sound
            _solver->set_speed_of_sound(_solver->get_speed_of_sound() * 1.01);
            std::printf("Speed of sound is now %f\n", _solver->get_speed_of_sound());
            break;
        }
        case 'P':{
            // print data
            auto max_vel = _data->get_max_u();
            double maxmach = _solver->compute_max_mach_number(max_vel);
            double estimated_reynolds = _solver->estimate_reynolds_number(max_vel, _data->dimension(0));
            
            printf("Reynolds = %f, Max mach = %f\n", estimated_reynolds, maxmach);
            break;
        }
        case 'R':{
            // reset
            reset();
            break;
        }
		case 't':{
            // lower tau
            _solver->set_tau(_solver->get_tau() * 0.99);
            std::printf("Tau is now %f\n", _solver->get_tau());
            break;
        }
        case 'T':{
            // Raise tau
            _solver->set_tau(_solver->get_tau() * 1.01);
            std::printf("Tau is now %f\n", _solver->get_tau());
            break;
        }
        
    
    }
}

void LBMViewer2D::reset(){

}

void LBMViewer2D::_display_callback() {
    if (_active_viewer != nullptr) {
        _active_viewer->_display();
    }
}

void LBMViewer2D::_color_pixels(){
    #pragma omp parallel for
    for (int j = 0; j < _data->dimension(1); j++){
        for (int i = 0; i < _data->dimension(0); i++){
            auto dens = _data->dens(i, j);
            dens -= 1.0;
            auto t = dens / (1.1 - 1);
            //t = std::clamp(dens, 0.0, 1.0);
            auto c = _min_color * (1.0 - t) + _max_color * t;
            _img.set_first_three_channels(i, j, c.r, c.g, c.b);
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