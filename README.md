# Hot Reload DLL
A utility to automatically reinject custom DLLs into processes.

# Usage

Uses the `RELOAD` environment variable to store the target DLL path.

Use the following powershell snippet or create an environment variable called `RELOAD` user variable. 
```powershell
$env:RELOAD = 'C:\Path\to\DLL'  # powershell
# start the application
```

Each time the DLL at `$RELOAD` is changed, it gets loaded automatically using `LoadLibraryW`.

# Building from source

You need meson and ninja. Get it from pip using:
```bash
pip install meson ninja
```

Clone the repo using:

```bash
git clone https://www.github.com/spider2048/HotReload --depth 1
cd HotReload
```

Next, use meson:
```bash
meson setup build --buildtype release # or debug
ninja -C build # to start building it
```

The resulting DLL can be found at `./build/HotReload.dll`.

Use [Injector](https://github.com/nefarius/Injector) to inject the DLL into the target process.