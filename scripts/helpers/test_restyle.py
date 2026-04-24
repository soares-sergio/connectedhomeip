#!/usr/bin/env python3
import os
import subprocess
import sys


def run_command(command, check=True):
    print(f"Running: {' '.join(command)}")
    try:
        result = subprocess.run(
            command, check=check, text=True, capture_output=True
        )
        return result.stdout
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
            f.write("import os \n")  # Trailing space

        # Track the file so git diff sees it
        run_command(["git", "add", test_file])

        # 4. Test "Just Restyle" (should fix trailing space)
        run_command(["python3", "scripts/helpers/restyle.py", "--on-uncommitted", "continue"])

        with open(test_file, "r") as f:
            content = f.read()
        if content == "import os\n":
            print("SUCCESS: Just Restyle worked.")
        else:
            print("FAILURE: Just Restyle failed.")
            sys.exit(1)

        # 5. Test "Restyle and Commit"
        # Add bug again
        with open(test_file, "w") as f:
            f.write("import os \n")

        run_command(["python3", "scripts/helpers/restyle.py", "--commit", "--on-uncommitted", "continue"])

        # Check if committed
        log_out = run_command(["git", "log", "-n", "1", "--oneline"])
        if "Apply restyling fixes" in log_out:
            print("SUCCESS: Restyle and Commit worked.")
        else:
            print("FAILURE: Restyle and Commit failed.")
            sys.exit(1)

        # 6. Test "--on-uncommitted abort"
        # Introduce uncommitted changes (edits)
        with open(test_file, "a") as f:
            f.write("# some edit\n")
            
        # Run with abort, should fail
        print("Testing --on-uncommitted abort...")
        result = run_command(["python3", "scripts/helpers/restyle.py", "--on-uncommitted", "abort"], check=False)
        if result is None:
            print("SUCCESS: --on-uncommitted abort worked.")
        else:
            print("FAILURE: --on-uncommitted abort failed.")
            sys.exit(1)

        # 7. Test "--on-uncommitted commit"
        # Add bug again
        with open(test_file, "w") as f:
            f.write("import os \n")
        with open(test_file, "a") as f:
            f.write("# some edit\n")
            
        print("Testing --on-uncommitted commit...")
        run_command(["python3", "scripts/helpers/restyle.py", "--on-uncommitted", "commit"])
        
        # Verify pre-restyle commit exists
        log_out = run_command(["git", "log", "-n", "2", "--oneline"])
        if "Pre-restyle commit" in log_out:
            print("SUCCESS: --on-uncommitted commit worked.")
        else:
            print("FAILURE: --on-uncommitted commit failed.")
            sys.exit(1)

    finally:
        # Cleanup
        print("Cleaning up...")
        run_command(["git", "checkout", current_branch])
        run_command(["git", "branch", "-D", test_branch])
        if os.path.exists(test_file):
            os.remove(test_file)


if __name__ == "__main__":
    main()
