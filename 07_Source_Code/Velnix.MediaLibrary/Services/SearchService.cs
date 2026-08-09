using Velnix.MediaLibrary.Data;
using Velnix.MediaLibrary.Models;

namespace Velnix.MediaLibrary.Services;

/// <summary>
/// Full-text search over title/artist/album/genre via the tracks_fts
/// virtual table, which is kept in sync with the tracks table by triggers
/// (see schema). No separate reindex step is needed.
/// </summary>
public sealed class SearchService
{
    private readonly LibraryDatabase _db;

    public SearchService(LibraryDatabase db) => _db = db;

    public IReadOnlyList<Track> Search(string query, int limit = 100)
    {
        if (string.IsNullOrWhiteSpace(query))
        {
            return Array.Empty<Track>();
        }

        // FTS5 query syntax: append '*' per term for prefix matching, which
        // gives useful results while the user is still typing.
        var ftsQuery = string.Join(" ",
            query.Split(' ', StringSplitOptions.RemoveEmptyEntries)
                 .Select(term => term.Replace("\"", "") + "*"));

        using var conn = _db.OpenConnection();
        using var cmd = conn.CreateCommand();
        cmd.CommandText = """
            SELECT t.* FROM tracks t
            JOIN tracks_fts fts ON fts.rowid = t.id
            WHERE tracks_fts MATCH @query
              AND t.missing = 0
            ORDER BY rank
            LIMIT @limit;
            """;
        cmd.Parameters.AddWithValue("@query", ftsQuery);
        cmd.Parameters.AddWithValue("@limit", limit);

        using var reader = cmd.ExecuteReader();
        var results = new List<Track>();
        while (reader.Read())
        {
            results.Add(TrackMapper.Map(reader));
        }
        return results;
    }
}
