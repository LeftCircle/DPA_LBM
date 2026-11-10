
# Saving a simulation image sequence

Run the viewer with `--save-every N` to save the currently displayed density or
vorticity view every N simulation steps. For example:

```sh
./build/bin/dpa_lbm_fluids --save-every 10 --output-dir frames
```

Each run creates a new directory such as `frames/run_0001/`, containing
`frame_00000010.png`, `frame_00000020.png`, and so on. The counter starts when
saving is configured; the viewer constructor advances one step before then.
Use `d` or `v` to select density or vorticity. Without `--save-every`, no images
are saved. `--output-dir` defaults to `frames` relative to the working directory.
