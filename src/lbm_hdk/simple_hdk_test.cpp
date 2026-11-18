#include "simple_hdk_test.h"

// a very basic simple LBM hdk example. All this should do is create LBM data 
// and set all values to zero. 

// The user should be able to specify the x/y dims of the data, the lattice being used, and the
// initial density and velocity.

#include "simple_hdk_test.h"

#include "lbm_data.h"
#include "lbmd2q9.h"

#include <GA/GA_Handle.h>
#include <GA/GA_Iterator.h>
#include <GU/GU_Detail.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <SOP/SOP_Node.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Vector3.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <vector>

namespace
{

// Attribute names for the nine D2Q9 populations.
constexpr std::array<const char*, 9> population_names = {
    "f0", "f1", "f2",
    "f3", "f4", "f5",
    "f6", "f7", "f8"
};


// ------------------------------------------------------------
// Houdini parameter definitions
// ------------------------------------------------------------

static PRM_Name nx_name("nx", "Grid Width");
static PRM_Name ny_name("ny", "Grid Height");
static PRM_Name density_name("density", "Initial Density");
static PRM_Name ux_name("ux", "Initial X Velocity");
static PRM_Name uy_name("uy", "Initial Y Velocity");

static PRM_Default nx_default(16);
static PRM_Default ny_default(12);
static PRM_Default density_default(1.0);
static PRM_Default ux_default(0.0);
static PRM_Default uy_default(0.0);


// ------------------------------------------------------------
// The SOP class
// ------------------------------------------------------------

class SOP_LBMDemo : public SOP_Node
{
public:
    static OP_Node* create(
        OP_Network* network,
        const char* name,
        OP_Operator* op
    )
    {
        return new SOP_LBMDemo(network, name, op);
    }

    static PRM_Template parameters[];

protected:
    SOP_LBMDemo(
        OP_Network* network,
        const char* name,
        OP_Operator* op
    )
        : SOP_Node(network, name, op)
    {
    }

    OP_ERROR cookMySop(OP_Context& context) override;
};


PRM_Template SOP_LBMDemo::parameters[] = {
    PRM_Template(PRM_INT_J, 1, &nx_name, &nx_default),
    PRM_Template(PRM_INT_J, 1, &ny_name, &ny_default),

    PRM_Template(
        PRM_FLT_J, 1, &density_name, &density_default
    ),

    PRM_Template(PRM_FLT_J, 1, &ux_name, &ux_default),
    PRM_Template(PRM_FLT_J, 1, &uy_name, &uy_default),

    // A default-constructed template marks the end of the list.
    PRM_Template()
};


// Create a 64-bit scalar point attribute.
//
// Your LBM solver uses double for its populations. Creating the
// attributes as 64-bit avoids immediately reducing them to float32.
GA_Attribute* add_double_point_attribute(
    GU_Detail* geometry,
    const char* name
)
{
    return geometry->addFloatTuple(
        GA_ATTRIB_POINT,
        name,
        1,
        GA_Defaults(0.0),
        nullptr,
        nullptr,
        GA_STORE_REAL64
    );
}


OP_ERROR SOP_LBMDemo::cookMySop(OP_Context& context)
{
    const fpreal time = context.getTime();

    const int nx = evalInt("nx", 0, time);
    const int ny = evalInt("ny", 0, time);

    const double initial_density = evalFloat("density", 0, time);
    const double initial_ux = evalFloat("ux", 0, time);
    const double initial_uy = evalFloat("uy", 0, time);

    // Keep the example bounded so an accidental parameter value
    // doesn't allocate an enormous lattice.
    if (nx < 1 || nx > 1024 || ny < 1 || ny > 1024) {
        addError(
            SOP_MESSAGE,
            "Grid Width and Height must each be between 1 and 1024."
        );
        return error();
    }

    if (!std::isfinite(initial_density) ||
        !std::isfinite(initial_ux) ||
        !std::isfinite(initial_uy) ||
        initial_density <= 0.0)
    {
        addError(
            SOP_MESSAGE,
            "Initial density must be positive and all values must be finite."
        );
        return error();
    }

    // This is your actual LBM data structure.
    dpa_lbm::LBMData data(nx, ny, 9);

    // For this demonstration, acceleration is always zero.
    for (int y = 0; y < ny; ++y) {
        for (int x = 0; x < nx; ++x) {
            data.set_acceleration(x, y, pba::vec2d(0.0, 0.0));
        }
    }

    // dt = 1, tau = 0.9. No simulation steps are run here.
    dpa_lbm::LBMd2q9 lattice(1.0, 0.9);

    const bool has_input = getInput(0) != nullptr;

    // Houdini input geometry must remain locked while we use it.
    // OP_AutoLockInputs unlocks it automatically on return.
    OP_AutoLockInputs inputs(this);

    if (has_input) {
        if (inputs.lock(context) >= UT_ERROR_ABORT) {
            return error();
        }

        // Copy the incoming geometry into this SOP's output detail.
        duplicateSource(0, context);

        if (gdp->getNumPoints() !=
            static_cast<GA_Size>(nx) * static_cast<GA_Size>(ny))
        {
            addError(
                SOP_MESSAGE,
                "Input point count must equal Grid Width × Grid Height."
            );
            return error();
        }

        // Handles provide access to Houdini point attributes.
        GA_ROHandleI grid_x(
            gdp->findPointAttribute("grid_x")
        );

        GA_ROHandleI grid_y(
            gdp->findPointAttribute("grid_y")
        );

        std::array<GA_ROHandleD, 9> populations;

        for (int q = 0; q < 9; ++q) {
            populations[q] = GA_ROHandleD(
                gdp->findPointAttribute(population_names[q])
            );
        }

        if (!grid_x.isValid() || !grid_y.isValid()) {
            addError(
                SOP_MESSAGE,
                "Input must contain integer grid_x and grid_y point attributes."
            );
            return error();
        }

        for (int q = 0; q < 9; ++q) {
            if (!populations[q].isValid()) {
                addError(
                    SOP_MESSAGE,
                    "Input must contain scalar point attributes f0 through f8."
                );
                return error();
            }
        }

        // Detect duplicate grid coordinates. Point order itself
        // does not have to match the order in which cells were created.
        std::vector<unsigned char> seen(
            static_cast<std::size_t>(nx) * ny,
            0
        );

        GA_Offset point;

        GA_FOR_ALL_PTOFF(gdp, point) {
            const int x = grid_x.get(point);
            const int y = grid_y.get(point);

            if (x < 0 || x >= nx || y < 0 || y >= ny) {
                addError(
                    SOP_MESSAGE,
                    "An input point has grid coordinates outside the grid."
                );
                return error();
            }

            const std::size_t index =
                static_cast<std::size_t>(x) +
                static_cast<std::size_t>(nx) * y;

            if (seen[index]) {
                addError(
                    SOP_MESSAGE,
                    "Two input points have the same grid coordinates."
                );
                return error();
            }

            seen[index] = 1;

            double rho = 0.0;

            for (int q = 0; q < 9; ++q) {
                const double value = populations[q].get(point);

                if (!std::isfinite(value)) {
                    addError(
                        SOP_MESSAGE,
                        "Input populations must be finite."
                    );
                    return error();
                }

                // Houdini -> your C++ LBMData.
                data.set_f(x, y, q, value);
                rho += value;
            }

            if (!std::isfinite(rho) || rho <= 0.0) {
                addError(
                    SOP_MESSAGE,
                    "Each input cell must have positive, finite density."
                );
                return error();
            }
        }
    }
    else {
        // With no input, initialize a fresh LBM state.
        data.set_all_density(initial_density);
        data.set_all_velocity(initial_ux, initial_uy);

        // Populate f_i using your existing D2Q9 equilibrium.
        lattice.set_to_equilibrium(data);

        // Start with empty Houdini geometry.
        gdp->clearAndDestroy();

        GA_RWHandleI grid_x(
            gdp->addIntTuple(GA_ATTRIB_POINT, "grid_x", 1)
        );

        GA_RWHandleI grid_y(
            gdp->addIntTuple(GA_ATTRIB_POINT, "grid_y", 1)
        );

        std::array<GA_RWHandleD, 9> populations;

        for (int q = 0; q < 9; ++q) {
            populations[q] = GA_RWHandleD(
                add_double_point_attribute(gdp, population_names[q])
            );
        }

        // Create one Houdini point for every lattice cell.
        for (int y = 0; y < ny; ++y) {
            for (int x = 0; x < nx; ++x) {
                const GA_Offset point = gdp->appendPointOffset();

                // Display the 2D lattice in Houdini's XY plane.
                gdp->setPos3(point, x, y, 0.0);

                grid_x.set(point, x);
                grid_y.set(point, y);

                for (int q = 0; q < 9; ++q) {
                    // Your C++ LBMData -> Houdini.
                    populations[q].set(
                        point,
                        data.get_f(x, y, q)
                    );
                }
            }
        }
    }

    // For the no-force case:
    //
    // rho = sum_i f_i
    // u   = sum_i(c_i f_i) / rho
    //
    // On input, this derives density and velocity from the f_i
    // values that were just read from Houdini.
    lattice.compute_moments(data);

    GA_ROHandleI grid_x(gdp->findPointAttribute("grid_x"));
    GA_ROHandleI grid_y(gdp->findPointAttribute("grid_y"));

    GA_RWHandleD density(
        add_double_point_attribute(gdp, "density")
    );

    GA_RWHandleV3D velocity(
        gdp->addFloatTuple(
            GA_ATTRIB_POINT,
            "v",
            3,
            GA_Defaults(0.0),
            nullptr,
            nullptr,
            GA_STORE_REAL64
        )
    );

    GA_Offset point;

    GA_FOR_ALL_PTOFF(gdp, point) {
        const int x = grid_x.get(point);
        const int y = grid_y.get(point);

        const auto& u = data.velocity(x, y);

        density.set(
            point,
            data.get_density(x, y)
        );

        velocity.set(
            point,
            UT_Vector3D(u.X(), u.Y(), 0.0)
        );
    }

    return error();
}

} // end anonymous namespace


// Houdini calls this function when it loads the plugin.
void newSopOperator(OP_OperatorTable* table)
{
    table->addOperator(
        new OP_Operator(
            "dpa_lbm_demo",
            "DPA LBM Demo",
            SOP_LBMDemo::create,
            SOP_LBMDemo::parameters,
            0u,  // Minimum inputs
            1u   // Maximum inputs
        )
    );
}

