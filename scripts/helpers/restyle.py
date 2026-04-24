#!/usr/bin/env python3
import argparse
import subprocess
import sys
from typing import List, Optional


def run_command(command: List[str], check: bool = True, capture: bool = True) -> Optional[str]:
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


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Automate restyling and Git workflow."
    )
    parser.add_argument(
        "--commit",
        action="store_true",
        help="Commit files modified by restyler",
    )
    parser.add_argument(
        "--push", action="store_true", help="Push changes to tracking branch"
    )
    parser.add_argument(
        "--on-uncommitted",
        choices=["abort", "commit", "continue"],
        help="How to handle uncommitted changes: abort, commit, or continue",
    )
    parser.add_argument(
        "ref",
        nargs="?",
        default="master",
        help="Base reference for restyle (default: master)",
    )

    args = parser.parse_args()

    # Check for uncommitted changes (excluding untracked files and submodules)
    status_out = run_command(["git", "status", "--porcelain"])
    has_uncommitted = False
    if status_out:
        for line in status_out.splitlines():
            if not line.startswith("??") and not line.startswith(" M third_party/"): 
                has_uncommitted = True
                break

    if has_uncommitted:
        mode = args.on_uncommitted
        if not mode:
            print("Uncommitted changes found.")
            print("What would you like to do?")
            print("  1. Abort")
            print("  2. Commit changes before running restyle")
            print("  3. Just restyle and commit all changes later")
            
            choice = input("Enter choice [1-3]: ").strip()
            if choice == "1":
                mode = "abort"
            elif choice == "2":
                mode = "commit"
            elif choice == "3":
                mode = "continue"
            else:
                print("Invalid choice. Aborting.")
                sys.exit(1)

        if mode == "abort":
            print("Aborting as requested.")
            sys.exit(1)
        elif mode == "commit":
            print("Committing uncommitted changes...")
            run_command(["git", "add", "-u"]) # Add all tracked modifications
            run_command(["git", "commit", "-m", "Pre-restyle commit of uncommitted changes"])
        elif mode == "continue":
            print("Continuing with uncommitted changes...")

    # 1. Find paths to restyle
    git_cmd = ["git", "diff", "--ignore-submodules", "--name-only", "--merge-base", args.ref]
    paths_out = run_command(git_cmd)
    
    paths = []
    if paths_out:
        paths = paths_out.splitlines()
        
    if not paths:
        print("No files need restyling.")
        return

    print(f"Found {len(paths)} files to restyle.")

    # 2. Run restyle command
    restyle_cmd = ["restyle", "--config-file=.restyled.yaml"] + paths
    run_command(restyle_cmd, capture=False)

    # 2. If --commit or --push, handle Git operations
    if args.commit or args.push:
        # Check for modified tracked files after restyle
        diff_out = run_command(["git", "diff", "--name-only"])

        modified_files = []
        if diff_out:
            for line in diff_out.splitlines():
                path = line.strip()
                if path and not path.startswith("third_party/"):
                    modified_files.append(path)

        if not modified_files:
            print("No files were modified by the restyler.")
        else:
            print(f"Restyler modified {len(modified_files)} files:")
            for f in modified_files:
                print(f"  {f}")

            # Stage modified files
            for f in modified_files:
                run_command(["git", "add", f])

            # Commit
            run_command(
                ["git", "commit", "-m", "Apply restyling fixes by restyle.py"]
            )

    # 3. If --push, push to remote
    if args.push:
        # Get current branch name
        branch_out = run_command(["git", "branch", "--show-current"])
        branch_name = branch_out.strip() if branch_out else ""

        if not branch_name:
            print("Error: Could not determine current branch name.")
            sys.exit(1)

        print(f"Pushing branch {branch_name} to origin...")
        run_command(["git", "push", "origin", branch_name])


if __name__ == "__main__":
    main()
