-- Dummy solution (premake can't generate projects without a solution)
solution "VstPlugins"
   configurations { "Debug", "Release" }

   location("build")

include "Vsts"
