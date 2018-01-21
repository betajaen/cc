solution "CC"

  configurations      { "Debug", "Release" }
  platforms           { "x32", "x64" }

  targetdir           "Build"
  debugdir            "Build"

  configuration "Debug"
    defines           { "DEBUG",  }
    flags             { "Symbols" }

  configuration "Release"
    defines           { "NDEBUG"}
    flags             { "Optimize" }

  project "CC"
    kind            "ConsoleApp"
    language        "C"
    objdir          "Build-Object"

    flags           { "NoExceptions", "NoRTTI" }
    defines         { }
    links           { }
    includedirs     { }
    libdirs         { }

    excludes        { "CCCLI.C", "CCNOD.C", "CCTOK.C", "CCASM.C", "TESTS/*.C" }

    files           {
      "CCCLI.C",
      "CCNOD.C",
      "CCTOK.C",
      "CCASM.C",
      "CC.C",
      "TESTS/*.C",
      "genie.lua"
    }
