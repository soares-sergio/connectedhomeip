#!/usr/bin/env python3
import os
import subprocess
import sys


def run_command(command, check=True, capture=True):
    print(f"Running: {' '.join(command)}")
    try:
        result = subprocess.run(
            command, check=check, text=True, capture_output=capture
        )
        return result.stdout if capture else None
    except subprocess.CalledProcessError as e:
        print(f"Error running command: {e}")
        print(f"Stderr: {e.stderr}")
        if check:
            sys.exit(1)
        return None


def main():
    # 1. Save current branch
    current_branch = (
        run_command(["git", "branch", "--show-current"]).strip() or "master"
    )

    # 2. Create a test branch
    test_branch = "test_restyle_temp"
    run_command(["git", "checkout", "-b", test_branch])

    test_file = "test_file.py"

    try:
        # 3. Create a file with style issue
        with open(test_file, "w") as f:
            f.write("a = 1 \n")  # Trailing space

        # Track the file so git diff sees it
        run_command(["git", "add", test_file])

        # 4. Test "Just Restyle" (should fix trailing space)
        run_command(["python3", "scripts/helpers/restyle.py"], capture=False)

        with open(test_file, "r") as f:
            content = f.read()
        if content == "a = 1\n":
            print("SUCCESS: Just Restyle worked.")
        else:
            print("FAILURE: Just Restyle failed.")
            print(f"Actual file content: '{content}'")
            sys.exit(1)

        # 5. Test "Restyle and Commit"
        # Add bug again
        with open(test_file, "w") as f:
            f.write("a = 1 \n")

        run_command(["python3", "scripts/helpers/restyle.py", "--commit"])

        # Check if committed
        log_out = run_command(["git", "log", "-n", "1", "--oneline"])
        if "Apply restyling fixes" in log_out:
            print("SUCCESS: Restyle and Commit worked.")
        else:
            print("FAILURE: Restyle and Commit failed.")
            sys.exit(1)



    finally:
        # Cleanup
        print("Cleaning up...")
        run_command(["git", "checkout", "-f", current_branch])
        run_command(["git", "branch", "-D", test_branch])
        if os.path.exists(test_file):
            os.remove(test_file)


if __name__ == "__main__":
    main()
