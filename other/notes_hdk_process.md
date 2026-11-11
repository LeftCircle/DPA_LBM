I would start by building a **custom C++ SOP that advances your LBM solver**, then place that SOP inside Houdini’s built-in **Solver SOP**. Houdini can retain the previous simulation state, while your C++ code performs collision, streaming, boundary updates, and exports the results as Houdini attributes.

This gives you parameter controls and access to every \(f_i\) without immediately having to write an entire DOP solver. You can move to a native C++ DOP solver later if you need deeper interaction with Houdini’s simulation system.

I reviewed your current `compare_to_others` branch and the SideFX HDK documentation. The example below is an integration design, not code I have compiled against your Houdini installation.

## 1. Suggested Houdini network

The first version could look like this:

```text
LBM Initialize       Custom C++ SOP: create the lattice and initial f_i
      ↓
Solver              Houdini Solver SOP: holds the previous frame’s geometry
      ↓
LBM Fields          Custom C++ SOP: optional volumes for visualization
```

Inside the Solver SOP:

```text
Prev_Frame → LBM Step → Output
              C++ SOP
```

`LBM Step` reads the previous geometry, runs a specified number of LBM timesteps, and returns the updated geometry. The Solver SOP feeds that result into the next frame as `Prev_Frame`. SideFX documents this previous-frame feedback directly in the Solver SOP. [sidefx.com](https://www.sidefx.com/docs/houdini/nodes/sop/solver.html?utm_source=chatgpt.com)

This distinction matters: Houdini can cook nodes multiple times or request frames out of order. A SOP that blindly advances a hidden C++ solver object every time it is cooked can give different answers depending on how you scrub the timeline. Passing the simulation state through `Prev_Frame` makes the dependency explicit.

## 2. Decide what to store in Houdini

For a first implementation, I recommend **one Houdini point per lattice cell**. Give each point the following attributes:

| Attribute | Type | Purpose |
|---|---|---|
| `P` | 3-component position | Position of the lattice cell in Houdini |
| `grid_x`, `grid_y` | integer | Stable coordinates in your LBM grid |
| `density` | float | Your \(\rho\) |
| `v` | vector | Your velocity, mapped to 3D |
| `f0` … `f8` | nine scalar floats | The D2Q9 distribution populations |
| `solid` | integer or boolean | Whether the cell is an obstacle or wall |

You could use a nine-component `f` tuple instead of nine attributes. Separate `f0`–`f8` attributes are easier to inspect in the Geometry Spreadsheet and to start with in C++.

For your current 2D solver, a straightforward convention is:

```cpp
P = (grid_x * cell_size, grid_y * cell_size, 0)
v = (u_x, u_y, 0)
```

You can choose the XZ plane instead if that fits your Houdini scenes better, as long as you use the same convention everywhere.

### Why store all nine \(f_i\)?

Your solver’s true evolving state is the set of populations \(f_i\), not just density and velocity:

\[
\rho=\sum_i f_i,
\qquad
\rho\mathbf u=\sum_i\mathbf c_i f_i
\quad\text{for the force-free case}.
\]

There are many different sets of nine populations that produce the same \(\rho\) and \(\mathbf u\). In particular, the non-equilibrium parts of \(f_i\) contain information needed for the next timestep. Reconstructing \(f_i\) from density and velocity alone at every frame would discard that information and change the simulation.

Your existing [`LBMData`](https://github.com/LeftCircle/DPA_LBM/blob/compare_to_others/include/lbm_data.h) already exposes exactly what the adapter needs:

```cpp
data.get_density(x, y);
data.velocity(x, y);
data.get_f(x, y, q);       // q = 0, ..., 8
```

The point attributes should be the **authoritative saved state** for the first implementation. On each cook, the C++ SOP copies the nine populations into `LBMData`, computes the required moments, advances the solver, and copies the updated values back to Houdini.

Your `fstar` array is an intermediate collision buffer; with the current step order, it can be regenerated during the next collision and does not need to be exported as persistent state.

**Precision:** your solver uses `double`. I would initially create the persistent `f0`–`f8` attributes as 64-bit floats and check that any Houdini caching/export path you use preserves that precision. Repeatedly converting the solver state to 32-bit floats may introduce differences over long runs.

## 3. Make your solver usable without the viewer

Your code is already fairly close to being separable:

- [`LBMData`](https://github.com/LeftCircle/DPA_LBM/blob/compare_to_others/include/lbm_data.h) owns the grid data.
- `LBMd2q9` owns lattice properties such as \(\tau\).
- [`SolverSystem`](https://github.com/LeftCircle/DPA_LBM/blob/compare_to_others/include/lbm_substeps.h) runs your configured substeps.
- `LBMViewer2D` handles the interactive display and image capture.

Right now, however, your `dpalbm` CMake target includes all files in `src/`, including the viewer, and links to the image-processing library. For an HDK plugin, I would separate it into:

```text
dpalbm_core
    lbm_data.cpp
    lbmd2q9.cpp
    lbm_equilibrium.cpp
    lbm_substeps.cpp
    bounce_back_halfway.cpp

dpalbm_viewer
    lbm_viewer2D.cpp
    third-party image processing / OpenGL

SOP_LBM
    Houdini adapter
    links to dpalbm_core
```

That way, Houdini provides the interface and viewport; the plugin does not need to launch your GLUT window or use `DPA_ImageProcessing` to display the simulation.

I would also wrap solver setup in a small C++ class rather than duplicating the setup currently in `main.cpp`. Conceptually:

```cpp
class LBMSimulation {
public:
    void initialize(int nx, int ny, double density, double inlet_velocity);
    void set_tau(double tau);
    void set_inlet_velocity(double velocity);
    void set_solid(int x, int y, bool solid);

    // Runs collision, streaming, boundaries, and moment update.
    void step();

    LBMData& data();
    const LBMData& data() const;

private:
    // Own LBMData, lattice, boundary solver, SolverSystem, etc.
};
```

This is a **proposed interface**, not a class that exists in your repository yet.

One detail to address during that refactor: `BoundaryZouHeInlet` currently receives inlet velocity in its constructor and has no setter. If you want to change inlet speed in Houdini during a simulation, add a setter or otherwise update the boundary solver’s parameters before stepping.

Likewise, remove or make optional the file output currently opened by the `LBMd2q9` constructor. A Houdini node should not silently create simulation log files merely because it is cooked.

## 4. Implement the two SOPs

### `LBM Initialize`

This SOP would expose parameters such as:

- Grid width and height.
- Initial density.
- Initial velocity.
- Lattice \(\tau\).
- Cell size for Houdini display.
- Initial obstacle shape or an input mask.

When cooked, it would:

1. Construct and initialize your `LBMData`.
2. Call your existing `set_to_equilibrium()`.
3. Create one Houdini point per cell.
4. Copy `density`, `v`, `f0`–`f8`, and the solid mask to point attributes.
5. Store grid dimensions and other necessary configuration as **detail attributes** on the geometry.

SideFX’s HDK uses `GU_Detail` for a SOP’s geometry, `appendPoint()` to create points, and attribute APIs to create and populate attributes. [sidefx.com](https://www.sidefx.com/docs/hdk/_h_d_k__op_basics__sop_generator.html?utm_source=chatgpt.com)

### `LBM Step`

This is the SOP placed inside the Solver SOP. It would expose:

- \(\tau\).
- Inlet velocity.
- Number of lattice steps per Houdini frame.
- Boundary options.
- Eventually, acceleration and force options.

Its logic would be approximately:

```cpp
// Pseudocode: the HDK attribute-reading calls are omitted.

// 1. Read grid dimensions and point attributes from Prev_Frame.
LBMSimulation sim;
sim.initialize_from_houdini_geometry(input_geometry);

// 2. Apply parameters evaluated at the current Houdini time.
sim.set_tau(tau);
sim.set_inlet_velocity(inlet_velocity);

// 3. Advance a whole number of lattice timesteps.
for (int step = 0; step < steps_per_frame; ++step) {
    sim.step();
}

// 4. Write the updated f_i, density, velocity, and solid values
//    into the output GU_Detail.
write_to_houdini_geometry(sim.data(), output_geometry);
```

For your existing algorithm, preserve the current substep order:

```text
collision → streaming → bounce-back → inlet/outlet → moment update
```

When loading a frame’s `f_i` back into your solver, make sure `density` and velocity are consistent with those populations **before the first collision**. For a force-free run, calculate the moments from the restored `f_i`. If force is enabled, use the matching half-force correction and force source together—we found earlier that mixing a force-free collision with a nonzero acceleration correction injects unintended momentum.

Also, don’t rely only on Houdini point order. Use `grid_x` and `grid_y` to map points back to cells, or validate that the point topology and order have not changed. If a downstream operation deletes or rearranges points before they feed back into the solver, the node should report an error instead of silently assigning populations to the wrong cells.

## 5. Make the data useful in Houdini

Once the points have attributes, you can inspect `density`, `v`, and `f0`–`f8` in the **Geometry Spreadsheet**. Other SOPs and VEX can also read them.

For example, a Point Wrangle downstream could visualize velocity magnitude:

```c
f@speed = length(v@v);
```

Or inspect a particular population:

```c
f@population_1 = f@f1;
```

I would keep visualization operations **after** the Solver SOP so that they don’t accidentally modify the state being fed back.

### Optional: output volume fields

For a more natural fluid workflow, add a separate C++ SOP that converts the point data into volumes:

- `density`: one scalar volume.
- `vel.x`, `vel.y`, `vel.z`: three scalar volumes, with `vel.z = 0` for your current 2D solver.
- Optionally `f0` through `f8`: nine scalar volumes.

Houdini’s HDK provides `GU_PrimVolume::build()` and a voxel write handle for populating volume data. SideFX includes a small [volume-generation example](https://www.sidefx.com/docs/hdk/standalone_2gengeovolume_8_c-example.html). [sidefx.com](https://www.sidefx.com/docs/hdk/_h_d_k__volumes.html?utm_source=chatgpt.com)

For your 2D grid, use a one-voxel-thick volume initially and explicitly define its transform so the voxel centers line up with your lattice cells. Keep the point attributes as the simulation state until the basic solver works; volumes can initially be an output representation.

One physical distinction: your LBM `density` is fluid mass density, generally near a reference value such as \(1\). Houdini smoke tools often interpret a field named `density` as visible smoke concentration. For visualization you might export a separate field based on `density - reference_density`, or eventually add a transported dye field.

## 6. Handle time and parameter changes deliberately

I recommend exposing **“LBM steps per frame”** rather than immediately tying one lattice timestep to Houdini’s frame duration.

Your current D2Q9 propagation moves populations by one grid cell per lattice step. Its velocity vectors are fixed integer directions, so changing `dt` to Houdini’s \(1/24\) second without revisiting your lattice scaling would not simply make the solver run at 24 fps—it would change the relationship among timestep, lattice speed, and viscosity.

A practical first setup is:

```text
LBM lattice timestep: 1
Houdini frames:        presentation / caching times
Steps per frame:       user parameter, e.g. 5 or 10
```

Then explicitly choose how lattice units map to Houdini world distance and seconds. This also determines how exported velocity should be scaled.

For parameter edits:

- Changing **grid dimensions or initial conditions** should reset and rebuild the simulation.
- Changing **\(\tau\), inlet velocity, or force** should be evaluated at the intended simulation time. To reproduce an animated parameter, the solver needs the history of those parameter values, not just their current value.
- Reset or invalidate cached simulation frames when a parameter change affects earlier timesteps.
- Validate parameters rather than relying on silent clamping. For example, your current `set_tau()` clamps values near the \(\tau=\Delta t/2\) limit; Houdini should show the effective value or report an invalid setting.

## 7. Build on Rocky Linux 10

SideFX’s current Houdini system-requirements page lists **Rocky 8+**, which includes Rocky Linux 10. Check the requirements for your *specific Houdini version and build*, including graphics, CPU, and compiler requirements. [SideFX](https://www.sidefx.com/Support/system-requirements/?sck=organicjLj68994109bf5df66a5a2c9632hQwK21wXxRhQwK21wXxRhQwK21wXxRhQwK21wXxR\&xcod=organicjLj68994109bf5df66a5a2c9632hQwK21wXxRhQwK21wXxRhQwK21wXxRhQwK21wXxR\&utm_source=chatgpt.com)

The main HDK rule is to build the plugin against the **same Houdini installation you intend to run**, using a compatible compiler and C++ ABI. SideFX recommends checking the compiler version used by the Houdini build; the Linux build filename includes a GCC identifier. [sidefx.com](https://www.sidefx.com/docs/hdk/_h_d_k__intro__getting_started.html?utm_source=chatgpt.com)

First, initialize Houdini’s environment in a shell:

```bash
cd /opt/hfsX.Y.ZZZ   # Replace with your installed version
source houdini_setup

echo "$HFS"
g++ --version
```

The HDK headers and examples should then be under:

```bash
$HFS/toolkit/include
$HFS/toolkit/samples
```

A good sanity check is to compile one of SideFX’s small sample SOPs with `hcustom` before involving your LBM library. That verifies your Houdini environment and compiler setup independently. SideFX documents both `hcustom` and CMake-based HDK builds. [sidefx.com](https://www.sidefx.com/docs/hdk/_h_d_k__intro__getting_started.html?utm_source=chatgpt.com)

For the full project, CMake is a better fit. A **starting shape** for the Houdini part would be:

```cmake
# Illustrative: assumes dpalbm_core and SOP_LBM.cpp have been created.

find_package(Houdini REQUIRED)

# dpalbm_core is a static library linked into the Houdini shared plugin.
set_target_properties(dpalbm_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

add_library(SOP_LBM SHARED
    houdini/SOP_LBM.cpp
)

target_link_libraries(SOP_LBM
    PRIVATE
        dpalbm_core
        Houdini
)

houdini_configure_target(SOP_LBM)
```

Configure CMake with Houdini’s package path:

```bash
cmake -S . -B build-hdk \
  -DCMAKE_PREFIX_PATH="$HFS/toolkit/cmake" \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo

cmake --build build-hdk
```

You would first need to add the `dpalbm_core` target and the SOP source file; those commands won’t build the integration unchanged today. Houdini’s `Houdini` CMake target supplies HDK headers, libraries, and relevant build definitions. Build the core and plugin with compatible settings, and check the plugin’s dependencies if Houdini reports that the resulting `.so` cannot load. [sidefx.com](https://www.sidefx.com/docs/hdk/_h_d_k__intro__compiling.html?utm_source=chatgpt.com)

## Recommended implementation order

1. **Build a minimal HDK SOP** that creates one point and a `density` attribute. Confirm that Houdini loads it.
2. **Split out `dpalbm_core`**, and link it into that SOP.
3. **Create the initialized grid** with `f0`–`f8`, density, velocity, and grid coordinates.
4. **Implement one force-free timestep** inside a Solver SOP and compare its output against a standalone run of your solver for the same initial state.
5. Add multiple steps per frame, obstacle input, visualization volumes, and force support.
6. Test timeline scrubbing, reset, caching, and changing parameters before optimizing.

A native HDK DOP solver is a reasonable second stage if you want your LBM solver to participate more directly in Houdini’s DOP simulation data and solver interactions. SideFX supports custom C++ solvers through classes such as `SIM_SingleSolver`, but the Solver SOP approach is a smaller first step for your current code. [sidefx.com](https://www.sidefx.com/docs/hdk/_h_d_k__op_basics__sim_solver.html?utm_source=chatgpt.com)

Which Houdini version and Linux build are you using—for example, Houdini 20.5, 21, or 22, and the GCC version in its installer name? That determines which HDK examples and compiler settings to use for a concrete implementation.