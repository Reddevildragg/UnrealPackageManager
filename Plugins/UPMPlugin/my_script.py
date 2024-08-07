import subprocess
import os
import sys

# Get the npm command from the command-line arguments
npm_command = sys.argv[1:]

# Define paths
npm_path = r'C:\Program Files\nodejs\npm.cmd'
appdata_dir = os.path.join(os.getenv('APPDATA'), "MyApp")
os.makedirs(appdata_dir, exist_ok=True)

output_file = os.path.join(appdata_dir, "npm_output.txt")
log_file = os.path.join(appdata_dir, "script_log.txt")

print("Starting subprocess...")
result = subprocess.run([npm_path] + npm_command, capture_output=True, text=True, check=True, creationflags=subprocess.CREATE_NO_WINDOW)
output = result.stdout.strip()
print(f"Command succeeded, output: {output}")

with open(output_file, "w") as file:
    file.write(output)
print(f"Output written to {output_file}")

print("Script finished.")