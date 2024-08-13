import subprocess
import sys

def install_package(package_name, plugins_dir, registry_url):
    command = f"npm install {package_name} --prefix {plugins_dir} --registry={registry_url}"
    result = subprocess.run(command, shell=True, capture_output=True, text=True)

    if result.returncode == 0:
        print(f"Successfully installed package: {package_name} at: {plugins_dir}")
    else:
        print(f"Failed to install package: {package_name}\nError: {result.stderr}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: install_package.py <package_name> <plugins_dir> <registry_url>")
        sys.exit(1)

    package_name = sys.argv[1]
    plugins_dir = sys.argv[2]
    registry_url = sys.argv[3]

    install_package(package_name, plugins_dir, registry_url)