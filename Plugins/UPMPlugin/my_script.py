import subprocess
import os
import sys

# Get the npm command from the command-line arguments
output_file = sys.argv[1]
npm_command = sys.argv[2:]


print(output_file);
# Define paths
npm_path = r'C:\Program Files\nodejs\npm.cmd'
output_dir = os.path.dirname(output_file)
os.makedirs(output_dir, exist_ok=True)

print("Starting subprocess...")
result = subprocess.run([npm_path] + npm_command, capture_output=True, text=True, check=True, creationflags=subprocess.CREATE_NO_WINDOW)
output = result.stdout.strip()
print(f"Command succeeded, output: {output}")

with open(output_file, "w") as file:
    file.write(output)
print(f"Output written to {output_file}")

print("Script finished.")