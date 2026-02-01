import io
import os
import sys
import re
import glob
import subprocess

base_dir = os.getcwd()

# TrinityCore SQL layout
pending_patterns = [
    os.path.join(base_dir, "sql", "updates", "pending*"),
]
updates_patterns = [
    os.path.join(base_dir, "sql", "updates", "*"),
]
base_patterns = [
    os.path.join(base_dir, "sql", "base", "*"),
]
archive_patterns = [
    os.path.join(base_dir, "sql", "archive", "*"),
]

pending_directories = []
for pattern in pending_patterns:
    pending_directories.extend(glob.glob(pattern))

updates_directories = []
for pattern in updates_patterns:
    updates_directories.extend(glob.glob(pattern))

base_directories = []
for pattern in base_patterns:
    base_directories.extend(glob.glob(pattern))

archive_directories = []
for pattern in archive_patterns:
    archive_directories.extend(glob.glob(pattern))

error_handler = False
results = {
    "Multiple blank lines check": "Passed",
    "Trailing whitespace check": "Passed",
    "SQL codestyle check": "Passed",
    "INSERT & DELETE safety usage check": "Passed",
    "Missing semicolon check": "Passed",
    "Backtick check": "Passed",
    "Directory check": "Passed",
    "Table engine check": "Passed",
}


def collect_files_from_directories(directories: list) -> list:
    all_files = []
    for directory in directories:
        for root, _, files in os.walk(directory):
            for file in files:
                if not file.endswith(".sh"):
                    all_files.append(os.path.join(root, file))
    return all_files


def get_changed_files() -> list:
    subprocess.run(["git", "fetch", "origin", "master"], check=True)
    result = subprocess.run(
        ["git", "diff", "--name-status", "origin/master"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    changed_files = []
    for line in result.stdout.strip().splitlines():
        if not line:
            continue
        status, path = line.split(maxsplit=1)
        if status in ("A", "M"):
            changed_files.append(path)
    return changed_files


def parsing_file(files: list) -> None:
    print("Starting TrinityCore SQL codestyle check...")
    print(" ")
    print("Please read the SQL standards for TrinityCore.")
    print(" ")

    for file_path in files:
        if "base" not in file_path and "archive" not in file_path:
            try:
                with open(file_path, "r", encoding="utf-8") as file:
                    multiple_blank_lines_check(file, file_path)
                    trailing_whitespace_check(file, file_path)
                    sql_check(file, file_path)
                    insert_delete_safety_check(file, file_path)
                    semicolon_check(file, file_path)
                    backtick_check(file, file_path)
                    non_innodb_engine_check(file, file_path)
            except UnicodeDecodeError:
                print(f"\nERROR: Could not decode file {file_path}")
                sys.exit(1)

    changed_files = get_changed_files()
    for file_path in changed_files:
        if "base" in file_path or "archive" in file_path:
            try:
                with open(file_path, "r", encoding="utf-8") as f:
                    directory_check(f, file_path)
            except UnicodeDecodeError:
                print(f"\nERROR: Could not decode file {file_path}")
                sys.exit(1)

    print("\n ")
    for check, result in results.items():
        print(f"{check} : {result}")
    if error_handler:
        print("\n ")
        print("\nERROR: Please fix the codestyle issues above.")
        sys.exit(1)
    else:
        print("\n ")
        print("\nOK: Everything looks good")


def multiple_blank_lines_check(file: io, file_path: str) -> None:
    global error_handler, results
    file.seek(0)
    check_failed = False
    consecutive_blank_lines = 0
    for line_number, line in enumerate(file, start=1):
        if line.strip() == "":
            consecutive_blank_lines += 1
            if consecutive_blank_lines > 1:
                print(f"ERROR: Multiple blank lines found in {file_path} at line {line_number - 1}")
                check_failed = True
        else:
            consecutive_blank_lines = 0
    if consecutive_blank_lines >= 1:
        print(f"ERROR: Multiple blank lines found at the end of: {file_path}")
        check_failed = True
    if check_failed:
        error_handler = True
        results["Multiple blank lines check"] = "Failed"


def trailing_whitespace_check(file: io, file_path: str) -> None:
    global error_handler, results
    file.seek(0)
    check_failed = False
    for line_number, line in enumerate(file, start=1):
        if line.endswith(" \n"):
            print(f"ERROR: Trailing whitespace found: {file_path} at line {line_number}")
            check_failed = True
    if check_failed:
        error_handler = True
        results["Trailing whitespace check"] = "Failed"


def sql_check(file: io, file_path: str) -> None:
    global error_handler, results
    file.seek(0)
    check_failed = False

    for line_number, line in enumerate(file, start=1):
        if "EntryOrGuid" in line:
            print(
                f"ERROR: Please use entryorguid syntax instead of EntryOrGuid in {file_path} at line {line_number}"
            )
            check_failed = True
        if ";;" in line:
            print(f"ERROR: Double semicolon (;;) found in {file_path} at line {line_number}")
            check_failed = True
        if re.match(r"\t", line):
            print(f"ERROR: Tab found! Replace it with 4 spaces: {file_path} at line {line_number}")
            check_failed = True

    if check_failed:
        error_handler = True
        results["SQL codestyle check"] = "Failed"


def insert_delete_safety_check(file: io, file_path: str) -> None:
    global error_handler, results
    file.seek(0)
    not_delete = ["creature_template", "gameobject_template", "item_template", "quest_template"]
    check_failed = False
    previous_line = ""

    for line_number, line in enumerate(file, start=1):
        if line.startswith("--"):
            continue
        if "INSERT" in line and "DELETE" not in previous_line:
            print(
                f"ERROR: No DELETE keyword found before the INSERT in {file_path} at line {line_number}"
            )
            check_failed = True
        previous_line = line
        match = re.match(r"DELETE FROM\s+`([^`]+)`", line, re.IGNORECASE)
        if match:
            table_name = match.group(1)
            if table_name in not_delete:
                print(
                    f"ERROR: Entries from {table_name} should not be deleted! {file_path} at line {line_number}"
                )
                check_failed = True

    if check_failed:
        error_handler = True
        results["INSERT & DELETE safety usage check"] = "Failed"


def semicolon_check(file: io, file_path: str) -> None:
    global error_handler, results

    file.seek(0)
    check_failed = False

    query_open = False
    in_block_comment = False
    inside_values_block = False

    lines = file.readlines()
    total_lines = len(lines)

    def get_next_non_blank_line(start):
        for idx in range(start, total_lines):
            next_line = lines[idx].strip()
            if next_line and not next_line.startswith("--") and not next_line.startswith("/*"):
                return next_line
        return None

    for line_number, line in enumerate(lines, start=1):
        stripped_line = line.strip()

        if stripped_line.startswith("--"):
            continue

        if in_block_comment:
            if "*/" in stripped_line:
                in_block_comment = False
                stripped_line = stripped_line.split("*/", 1)[1].strip()
            else:
                continue
        else:
            if "/*" in stripped_line:
                query_open = False
                in_block_comment = True
                stripped_line = stripped_line.split("/*", 1)[0].strip()

        if not stripped_line and not inside_values_block:
            continue

        stripped_line = stripped_line.split("--", 1)[0].strip()

        if stripped_line.upper().startswith("SET") and not stripped_line.endswith(";"):
            print(f"ERROR: Missing semicolon in {file_path} at line {line_number}")
            check_failed = True

        if not query_open and any(
            keyword in stripped_line.upper()
            for keyword in ["SELECT", "INSERT", "UPDATE", "DELETE", "REPLACE"]
        ):
            query_open = True

        if any(kw in stripped_line.upper() for kw in ["INSERT", "REPLACE"]) and "VALUES" in stripped_line.upper():
            inside_values_block = True
            query_open = True

        if inside_values_block:
            if not stripped_line:
                continue

            if stripped_line.startswith("("):
                next_line = get_next_non_blank_line(line_number)
                if next_line and next_line.startswith("("):
                    if not stripped_line.endswith(","):
                        print(f"ERROR: Missing comma in {file_path} at line {line_number}")
                        check_failed = True
                else:
                    if not stripped_line.endswith(";"):
                        print(f"ERROR: Missing semicolon in {file_path} at line {line_number}")
                        check_failed = True
                        inside_values_block = False
                        query_open = False
                    else:
                        inside_values_block = False
        elif query_open and not inside_values_block:
            if line_number == total_lines and not stripped_line.endswith(";"):
                print(f"ERROR: Missing semicolon in {file_path} at the last line {line_number}")
                check_failed = True
                query_open = False
            elif stripped_line.endswith(";"):
                query_open = False

    if check_failed:
        error_handler = True
        results["Missing semicolon check"] = "Failed"


def backtick_check(file: io, file_path: str) -> None:
    global error_handler, results
    file.seek(0)
    check_failed = False

    pattern = re.compile(
        r"\b(SELECT|FROM|JOIN|WHERE|GROUP BY|ORDER BY|DELETE FROM|UPDATE|INSERT INTO|SET|REPLACE|REPLACE INTO)\s+(.*?)(?=;$|(?=\b(?:WHERE|SET|VALUES)\b)|$)",
        re.IGNORECASE | re.DOTALL,
    )

    quote_pattern = re.compile(r"'(?:\\'|[^'])*'|\"(?:\\\"|[^\"])*\"")

    for line_number, line in enumerate(file, start=1):
        if line.startswith("--"):
            continue

        sanitized_line = quote_pattern.sub("", line)
        matches = pattern.findall(sanitized_line)

        for clause, content in matches:
            words = re.findall(r"\b(?<!@)([a-zA-Z_][a-zA-Z0-9_]*)\b", content)

            for word in words:
                if word.upper() in {
                    "SELECT",
                    "FROM",
                    "JOIN",
                    "WHERE",
                    "GROUP",
                    "BY",
                    "ORDER",
                    "DELETE",
                    "UPDATE",
                    "INSERT",
                    "INTO",
                    "SET",
                    "VALUES",
                    "AND",
                    "IN",
                    "OR",
                    "REPLACE",
                    "NOT",
                    "BETWEEN",
                    "DISTINCT",
                    "HAVING",
                    "LIMIT",
                    "OFFSET",
                    "AS",
                    "ON",
                    "INNER",
                    "LEFT",
                    "RIGHT",
                    "FULL",
                    "OUTER",
                    "CROSS",
                    "NATURAL",
                    "EXISTS",
                    "LIKE",
                    "IS",
                    "NULL",
                    "UNION",
                    "ALL",
                    "ASC",
                    "DESC",
                    "CASE",
                    "WHEN",
                    "THEN",
                    "ELSE",
                    "END",
                    "CREATE",
                    "TABLE",
                    "ALTER",
                    "DROP",
                    "DATABASE",
                    "INDEX",
                    "VIEW",
                    "TRIGGER",
                    "PROCEDURE",
                    "FUNCTION",
                    "PRIMARY",
                    "KEY",
                    "FOREIGN",
                    "REFERENCES",
                    "CONSTRAINT",
                    "DEFAULT",
                    "AUTO_INCREMENT",
                    "UNIQUE",
                    "CHECK",
                    "SHOW",
                    "DESCRIBE",
                    "EXPLAIN",
                    "USE",
                    "GRANT",
                    "REVOKE",
                    "BEGIN",
                    "COMMIT",
                    "ROLLBACK",
                    "SAVEPOINT",
                    "LOCK",
                    "UNLOCK",
                    "WITH",
                    "RECURSIVE",
                    "COLUMN",
                    "ENGINE",
                    "CHARSET",
                    "COLLATE",
                    "IF",
                    "ELSEIF",
                    "LOOP",
                    "WHILE",
                    "DO",
                    "HANDLER",
                    "LEAVE",
                    "ITERATE",
                    "DECLARE",
                    "CURSOR",
                    "FETCH",
                    "OPEN",
                    "CLOSE",
                }:
                    continue

                if not re.search(rf"`{re.escape(word)}`", content):
                    print(f"ERROR: Missing backticks around ({word}). {file_path} at line {line_number}")
                    check_failed = True

    if check_failed:
        error_handler = True
        results["Backtick check"] = "Failed"


def directory_check(file: io, file_path: str) -> None:
    global error_handler, results
    file.seek(0)
    check_failed = False

    normalized_path = os.path.normpath(file_path)
    path_parts = normalized_path.split(os.sep)

    if "base" in path_parts:
        print(f"WARN: {file_path} is changed/added in the base directory.")
        check_failed = True

    if "archive" in path_parts:
        print(f"WARN: {file_path} is changed/added in the archive directory.")
        check_failed = True

    if check_failed:
        error_handler = True
        results["Directory check"] = "Failed"


def non_innodb_engine_check(file: io, file_path: str) -> None:
    global error_handler, results
    file.seek(0)
    check_failed = False

    engine_pattern = re.compile(r"ENGINE\s*=\s*([a-zA-Z0-9_]+)", re.IGNORECASE)

    for line_number, line in enumerate(file, start=1):
        match = engine_pattern.search(line)
        if match:
            engine = match.group(1).lower()
            if engine != "innodb":
                print(f"ERROR: Non-InnoDB engine found: '{engine}' in {file_path} at line {line_number}")
                check_failed = True

    if check_failed:
        error_handler = True
        results["Table engine check"] = "Failed"


all_files = (
    collect_files_from_directories(pending_directories)
    + collect_files_from_directories(updates_directories)
    + collect_files_from_directories(base_directories)
    + collect_files_from_directories(archive_directories)
)

parsing_file(all_files)
