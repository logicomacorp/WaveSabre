newoption {
   trigger     = "vstdir",
   value       = "path",
   description = "VST system directory"
}

if (_OPTIONS.vstdir~=nil) then
   -- remove trailing slash
   -- something fishy is going on here with the backspaces, it somehow escapes the double quote
   -- that's why we remove trailing "
   if (string.sub(_OPTIONS.vstdir,-1)=="\\" or string.sub(_OPTIONS.vstdir,-1)=="/" or string.sub(_OPTIONS.vstdir,-1)=="\"") then
      _OPTIONS.vstdir = string.sub(_OPTIONS.vstdir, 1, -2)
   end
end

vsts = {
   "Twister",
   "Chamber",
   "Crusher",
   "Echo",
   "Falcon",
   "Leveller",
   "Scissor",
   "Slaughter",
   "Smasher",
   "Thunder",
   "Cathedral",
   "Adultery",
   "Specimen"
}

function generate_def(projectname, path)
   file = io.open(path, "w+")
   file:write(string.format("LIBRARY \"%s\"\n", projectname))
   file:write("EXPORTS\n")
   file:write("VSTPluginMain\n")
   file:write("main=VSTPluginMain\n")
   file:close()
end

function generate_project(projectname)
   project(projectname)
      kind "SharedLib"

      filter "Debug"
         defines { "DEBUG" }
         flags { "Maps" }
         symbols "On"
         optimize "Off"

      filter "Release"
         defines { "NDEBUG" }
         optimize "Size"

      targetdir("build/%{prj.name}/bin/%{cfg.longname}")
      objdir   ("build/%{prj.name}/obj/%{cfg.longname}")

      defines { "_CRT_SECURE_NO_WARNINGS" }
      defines { "WIN32" }

      flags { "LinkTimeOptimization" }

      def_path = projectname.."/"..projectname..".def"

      generate_def(projectname, def_path)

      files { projectname.."/*.h", projectname.."/*.cpp" }
      files { def_path }
      files { "../Data/data.rc" }

      includedirs { "../WaveSabreCore/include" }
      includedirs { "../WaveSabrePlayerLib/include" }
      includedirs { "../WaveSabreVstLib/include" }
      includedirs { "../Vst3.x/" }
      includedirs { "../Vst3.x/vstgui.sf/zlib" }
      includedirs { "../Vst3.x/vstgui.sf/libpng" }
      includedirs { "../Vst3.x/public.sdk/source/vst2.x" }

      resincludedirs { "../Data/*" }

      links { "../WaveSabreVstLib/lib/WaveSabreVstLib" }
      links { "../WaveSabreCore/lib/WaveSabreCore" }
      links { "Msacm32" }

      floatingpoint "fast"

      if (_OPTIONS.vstdir~=nil) then
         postbuildcommands { "copy \"$(TargetPath)\" \"".._OPTIONS.vstdir.."\" " }
      end

      debugcommand "$(TargetDir)\\$(TargetName).exe" -- this is for testing with SAVIhost, which should be placed in the target folders, and be named the same as the target file (e.g. Falcon.exe)
end

for key,value in pairs(vsts) do
   generate_project(value)
end
