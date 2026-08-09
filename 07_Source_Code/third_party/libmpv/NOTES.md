# Drop libmpv here

Download the latest `mpv-dev-x86_64` package from
https://sourceforge.net/projects/mpv-player-windows/files/libmpv/
and unzip its contents directly into this folder, so you end up with:

```
third_party/libmpv/
  include/
    mpv/client.h
    mpv/render.h
    mpv/render_gl.h
    ...
  mpv.dll
  mpv.lib      <- generate this if it's not included, see below
```

If `mpv.lib` isn't in the download (MSVC needs an import library, the raw
`.dll` isn't enough to link against), generate it from the `.def` file that
is included, from an "x64 Native Tools Command Prompt for VS 2022":

```
lib /def:mpv-1.def /out:mpv.lib /machine:x64
```

This folder is intentionally left out of the repo's normal source tree —
treat it as a vendored binary dependency, not code you edit.
