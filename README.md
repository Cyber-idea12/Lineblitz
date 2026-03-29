# LineBlitz

LineBlitz is a high-performance, SIMD-accelerated line counting utility written in C. It leverages modern CPU instruction sets (AVX2 and SSE2) and memory-mapped file I/O to count lines at near-theoretical hardware limits.

The project is specifically architected for Linux and BSD environments to exploit low-level POSIX memory mapping and filesystem efficiency, aiming to outperform standard tools like `wc -l`.

---

# Prerequisites & Installation

Before cloning, ensure you have a C compiler (GCC) and the `make` utility installed. Because apparently modern software still begins with ritual sacrifices to your package manager.

## 1. Install Dependencies

### Linux (Debian / Ubuntu / Ubuntu-based)

```bash
sudo apt update
sudo apt install build-essential gcc make
``````

Linux (Arch / Manjaro)

```bash
sudo pacman -S base-devel gcc make
``````

Linux (Fedora / RHEL)
```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install gcc make
``````
FreeBSD
```bash
pkg install gcc make

``````
---

## 2. Build the Project

Once the dependencies are ready, follow these steps:

# Clone the repository
```bash
git clone https://github.com/your-username/lineblitz.git

``````

### Enter the project directory
```bash
cd lineblitz
``````
### Compile the project
```bash
make
``````

«Note: The build process is self-cleaning; object files (".o") are automatically removed after the final binary is linked.»

---

## 3. Reset Environment

To revert the project directory to its pre-compiled state:
```bash
make clean
``````
This command removes the "lineblitz" binary and any residual build artifacts.

---

Usage

Run the binary followed by the target path:
```bash
./lineblitz [FLAGS] <path>
``````
---

### Command-Line Flags

| Flag | Long Form | Description |
| :--- | :--- | :--- |
| `-b` | `--blitz` | **Nitro Mode**: Enables the 256-bit AVX2 engine (Requires AVX2 support). |
| `-s` | `--summary` | **Quiet Mode**: Suppresses per-file output and shows only the final summary. |
| `-f` | `--force` | **Chaos Mode**: Forces the tool to count every file, bypassing the extension whitelist. |

### File Extensions & Filtering

LineBlitz is designed for developers. By default, it prioritizes source code and configuration files:
*   **Whitelisted**: `.c`, `.cpp`, `.h`, `.rs`, `.go`, `.py`, `.js`, `.ts`, `.json`, `.sql`, etc.
*   **Interactive Fallback**: If an unknown extension is encountered, the tool will prompt for permission before processing.

---

## Performance Notes

LineBlitz is optimized for speed. When using the `--blitz` (AVX2) engine, the tool processes **32 bytes per cycle**. By utilizing `mmap`, it bypasses the overhead of standard I/O buffers, making it significantly faster than scalar implementations on Unix-like systems.

**Safety Warning**: If `--blitz` is invoked on a CPU that does not support AVX2, the tool will perform a "graceful exit" to prevent instruction set illegal exceptions (SigIll).

---

## Project Structure

```text
LineBlitz/
├── Makefile            # Build configuration
├── lineblitz           # Generated binary (after make)
└── src/                # Source code directory
    ├── lineblitz.h     # Core definitions and structs
    ├── main.c          # Entry point and CLI parsing
    ├── discovery.c     # Filesystem traversal logic
    ├── engine_simd.c   # SSE2 and AVX2 implementation
    └── utils.c         # Hardware validation and CPUID checks
``````
---

## License

This project is licensed under the [Mozilla Public License 2.0 (MPL-2.0)](./LICENSE).
