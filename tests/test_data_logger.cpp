#include "catch_helpers.h"
#include "tessendorf_vecendorf.h"
#include <string>


std::string output_path = std::string(SOURCE_DIR) + "/build/output/";


// TEST_CASE("test writing data to a file"){
//     std::string filename = "test_data_logger.txt"
//     DataLogger logger(output_path + filename);
//     float t = 0;
//     pba::Vector vec(1, 2.1, -3.7);
//     double val = 99.99;
//     logger.set_keys("time", "vecs", "vals");
//     for (int i = 0; i < 3; i++){
//         logger.log_keyed(t * i, vec * i, val * i);
//     }
//     logger.save_to_file();

//     DataLogger reader(output_path + filename);
//     reader.read_from_file();

//     for (int i = 0; i < 3; i++){
//         REQUIRE(reader.get_val("time", i) == t * i);
//         REQUIRE(reader.get_val("vecs", i) == vec * i);
//         REQUIRE(reader.get_val("vals", i) == val * i);
//     }

// }