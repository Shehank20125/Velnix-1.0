# Solution file

`Velnix.sln` is now included at the repo root, referencing all 12 `src/`
projects and all 12 `tests/` projects. It was hand-authored in an
environment with no .NET SDK or network access — the project GUIDs and
format are valid Visual Studio solution syntax, but it has **not** been
opened in Visual Studio or run through `dotnet build`/`dotnet sln list`.

Before trusting it:

```
dotnet sln Velnix.sln list      # sanity-check it parses and lists all 24 projects
dotnet build Velnix.sln         # first real compile of the whole scaffold
dotnet test Velnix.sln          # runs the one real test file (Settings.Tests)
```

If `dotnet sln list` or `dotnet build` complains about the solution file
itself (rather than project code), regenerate it properly instead of
patching by hand:

```
rm Velnix.sln
dotnet new sln -n Velnix
for proj in src/*/*.csproj tests/*/*.csproj; do
  dotnet sln add "$proj"
done
```
