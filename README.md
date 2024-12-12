# INSTALL.md

## Prerequisites

### Operating System
- Linux (e.g., Ubuntu)

### Dependencies
- **Build tools**: gcc/g++, make, cmake
- **System libraries**: libreadline-dev
- **Python 3**: (version 3.6+ recommended)

---

## Steps

### Install System Packages
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libreadline-dev python3-venv python3-pip git
```

### Clone the Repository
```bash
git clone <your_repo_url> process_manager_project
cd process_manager_project
```

### Set Up Python Virtual Environment
```bash
python3 -m venv .venv
source .venv/bin/activate
pip install --upgrade pip
pip install -r requirements.txt
```

### Install Conan Dependencies (If Any)
```bash
conan install . --build=missing --output-folder=build
```
*Note*: If no external dependencies are listed in `conanfile.txt`, this step will just create the default toolchain files.

### Configure and Build the Project

#### With Conan (if toolchain file is generated):
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

#### Without Conan (or if no external dependencies):
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Run the Program
```bash
./build/process_manager_project
```
Use commands like:
- `start_monitor`
- `stop_monitor`
- `list_processes`
- `help`

### Testing (Optional)
```bash
cd build
ctest --output-on-failure
```

---

## Troubleshooting

### Missing readline support:
- Ensure `libreadline-dev` is installed.

### Conan not found:
- Confirm the virtual environment is active and `pip install -r requirements.txt` was successful.

### Permission issues or missing tools:
- Run `sudo apt-get update` and re-install the required system packages.

### Additional Help:
- If issues persist, consult the project's documentation or open an issue in the repository.
