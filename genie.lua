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
    defines         { "_CRT_SECURE_NO_WARNINGS" }
    links           { }
    includedirs     { }
    libdirs         { }

    excludes        { "CC.C", "CCARY.C", "CCCLI.C", "CCNOD.C", "CCTOK.C", "CCASM.C", "CCDBG.C", "TESTS/*.C" }

    files           {
      "CCCLI.C",
      "CCNOD.C",
      "CCTOK.C",
      "CCASM.C",
      "CCARY.C",
      "CCDBG.C",
      "MODERN.C",
      "CC.C",
      "MODERN/*.C",
      "TESTS/*.C",
      "HEADERS/*.H",
      "genie.lua"
    }
