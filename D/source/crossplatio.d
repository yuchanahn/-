// http://dpldocs.info/this-week-in-d/Blog.Posted_2019_11_25.html#tip-of-the-week
module crossplatio;

// this detects if the output is a console, or something else.
bool isConsole(int fd)
{
  import core.sys.windows.windows;
  import std.conv;

  version (Posix)
  {
    import core.sys.posix.unistd;

    return cast(bool) isatty(fd);
  }
  else version (Windows)
  {
    auto hConsole = GetStdHandle(fd == 0 ? STD_INPUT_HANDLE : STD_OUTPUT_HANDLE);
    return GetFileType(hConsole) == FILE_TYPE_CHAR;
  }
  else
    static assert(0);
}

version (Windows) string readln()
{
  import std.conv;
  import std.windows.syserror;
  import core.sys.windows.windows;

  if (isConsole(0))
  {
    // if in a console, we want to
    // use ReadConsoleW and convert that
    // input data to UTF-8 to return.

    wchar[] input;
    wchar[2048] staticBuffer;
    // this loops because the buffer might be too small for a line
    // (though I doubt that will actually happen)
    while (input.length == 0 || input[$ - 1] != '\n')
    {
      // if we are on a second loop, we need to copy the input
      // away so the next write doesn't smash data we must store
      if (input.ptr is staticBuffer.ptr)
        input = input.dup;

      DWORD chars = staticBuffer.length;
      if (!ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), staticBuffer.ptr, chars, &chars, null))
        throw new Exception("read stdin failed: " ~ sysErrorString(GetLastError()));

      if (input is null)
        input = staticBuffer[0 .. chars];
      else
        input ~= staticBuffer[0 .. chars];
    }

    char[] buffer;
    auto got = WideCharToMultiByte(CP_UTF8, 0, input.ptr, cast(int) input.length, null, 0, null, null);
    if (got == 0)
      throw new Exception("conversion preparation failed " ~ sysErrorString(GetLastError()));
    buffer.length = got;

    got = WideCharToMultiByte(CP_UTF8, 0, input.ptr, cast(int) input.length,
      buffer.ptr, cast(int) buffer.length, null, null);
    if (got == 0)
      throw new Exception("conversion actual failed " ~ sysErrorString(GetLastError()));

    // drop the terminator, or maybe you can convert it
    // from \r\n to \n or whatever.
    auto ret = cast(string) buffer[0 .. got];
    if (ret.length && ret[$ - 1] == 10)
      ret = ret[0 .. $ - 1];
    if (ret.length && ret[$ - 1] == 13)
      ret = ret[0 .. $ - 1];
    return ret;
  }
  else
  {
    // for pipe or file redirection, just use the normal
    // thing, utf-8 may be cool there, but you should do
    // whatever is best for interoperability there.
    import std.stdio;

    return stdin.readln(); // maybe trim the \n too btw
  }
}

version (Windows) void writeln(string s)
{ // you might actually do variadic template for full compatibility, but that's trivial
  import std.conv;
  import std.windows.syserror;
  import core.sys.windows.windows;

  // again, it is important to branch on output
  // being the console or redirected.
  if (isConsole(1))
  {
    wchar[2048] staticBuffer;
    wchar[] buffer = staticBuffer[];
    DWORD i = 0;

    // in here I do the conversion in D instead of
    // using the Windows function, since I know it is
    // going UTF-8 to UTF-16, but you could do
    // it the other way too.
    foreach (wchar c; s)
    {
      if (i + 2 >= buffer.length)
        buffer.length = buffer.length * 2;
      buffer[i++] = c;
    }
    if (i + 2 > buffer.length)
      buffer.length = buffer.length + 2;
    // adding the new line
    buffer[i++] = 13;
    buffer[i++] = 10;
    DWORD actual;
    // and now calling the wide char function
    if (!WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer.ptr, i, &actual, null))
      throw new Exception("write console failed " ~ sysErrorString(GetLastError()));
  }
  else
  {
    // non-console should still be utf-8, let it work normally
    // or whatever for interop (WriteConsoleW would fail here anyway)
    import std.stdio;

    stdout.writeln(s);
  }
}

version (Posix)
{
  import std.stdio : readln, writeln; // these are OK there
}
