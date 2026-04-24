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
        "ref",
        nargs="?",
        default="master",
        help="Base reference for restyle (default: master)",
    )

    args = parser.parse_args()

    # 1. Run restyle script
    restyle_cmd = ["scripts/helpers/restyle-diff.sh"]
    if args.ref:
        restyle_cmd.append(args.ref)

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
