using Microsoft.Data.Sqlite;

namespace Velnix.MediaLibrary.Data;

/// <summary>
/// Owns the SQLite connection lifecycle and schema application for the
/// media library database. One instance per application; repositories
/// take this in their constructor and open short-lived connections from it.
///
/// Integration note for the host app: this class does not know where
/// Velnix's other local data (settings, cache) lives. Pass in the target
/// db path explicitly — do not hardcode a path here — so the host's
/// existing data-storage module stays the single source of truth for
/// on-disk layout.
/// </summary>
public sealed class LibraryDatabase
{
    private readonly string _connectionString;

    public LibraryDatabase(string databaseFilePath)
    {
        var dir = System.IO.Path.GetDirectoryName(databaseFilePath);
        if (!string.IsNullOrEmpty(dir))
        {
            Directory.CreateDirectory(dir);
        }

        _connectionString = new SqliteConnectionStringBuilder
        {
            DataSource = databaseFilePath,
            Cache = SqliteCacheMode.Shared,
        }.ToString();
    }

    /// <summary>Opens a new connection. Caller is responsible for disposing it.</summary>
    public SqliteConnection OpenConnection()
    {
        var conn = new SqliteConnection(_connectionString);
        conn.Open();
        return conn;
    }

    /// <summary>
    /// Applies the schema (idempotent — safe to call on every app start).
    /// Currently a single schema file; once the schema evolves, this
    /// becomes a list of versioned scripts applied in order, checked
    /// against schema_version.
    /// </summary>
    public void EnsureSchema()
    {
        using var conn = OpenConnection();
        using var tx = conn.BeginTransaction();

        var schemaSql = LoadEmbeddedOrAdjacentSchema();
        using (var cmd = conn.CreateCommand())
        {
            cmd.Transaction = tx;
            cmd.CommandText = schemaSql;
            cmd.ExecuteNonQuery();
        }

        using (var versionCheck = conn.CreateCommand())
        {
            versionCheck.Transaction = tx;
            versionCheck.CommandText = "SELECT COUNT(*) FROM schema_version;";
            var count = (long)(versionCheck.ExecuteScalar() ?? 0L);
            if (count == 0)
            {
                using var insert = conn.CreateCommand();
                insert.Transaction = tx;
                insert.CommandText =
                    "INSERT INTO schema_version (version, applied_utc) VALUES (1, @now);";
                insert.Parameters.AddWithValue("@now", DateTime.UtcNow.ToString("o"));
                insert.ExecuteNonQuery();
            }
        }

        tx.Commit();
    }

    private static string LoadEmbeddedOrAdjacentSchema()
    {
        // Ships as a plain file next to the assembly for now. If the host
        // project prefers embedded resources, mark 001_initial_schema.sql
        // as an EmbeddedResource in the .csproj and switch this to read
        // from the assembly's manifest resource stream instead.
        var baseDir = AppContext.BaseDirectory;
        var candidate = System.IO.Path.Combine(baseDir, "Schema", "001_initial_schema.sql");
        if (File.Exists(candidate))
        {
            return File.ReadAllText(candidate);
        }

        throw new FileNotFoundException(
            "Schema file 001_initial_schema.sql not found next to the assembly. " +
            "Ensure it is set to 'Copy to Output Directory' in the project file.",
            candidate);
    }
}
