mod-example

Purpose
- Example module that shows config usage, database updates, and a simple script.

Configuration
- Config files are loaded from: <server_root>/modules/
- The build copies this module's config template to:
  <build>/bin/<Config>/modules/mod_example.conf.dist
- Copy/rename it to mod_example.conf in <server_root>/modules/ and edit values.

Config keys
- ModExample.Enable (default 0)
- ModExample.Updates.EnableDatabases (default 1)

Auto SQL updates (module)
- The module registers a DatabaseScript hook and inserts a path into updates_include:
  $/modules/mod-example/data/sql/world
- DBUpdater will automatically apply SQL files from that path when the server starts.

SQL standards (safe default)
- No multiple blank lines.
- No trailing whitespace.
- File ends with a single newline.
- For DML-only update files (no CREATE/ALTER/DROP), include a DELETE before INSERT for the same rows.
- DDL-heavy files are exempt from the DELETE-before-INSERT rule.
