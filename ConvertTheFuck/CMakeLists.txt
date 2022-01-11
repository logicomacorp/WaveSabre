set(CONVERT_THE_FUCK_SOURCES
	Program.cs

	Properties/AssemblyInfo.cs
)

add_executable(ConvertTheFuck
	App.config

	${CONVERT_THE_FUCK_SOURCES}
)

target_link_libraries(ConvertTheFuck WaveSabreConvert)

csharp_set_designer_cs_properties(${CONVERT_THE_FUCK_SOURCES})
csharp_set_windows_forms_properties(${CONVERT_THE_FUCK_SOURCES})
set_property(TARGET ConvertTheFuck PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")
set_property(TARGET ConvertTheFuck PROPERTY VS_DOTNET_REFERENCES 
	"System"
	"System.Core"
	"System.Xml.Linq"
	"System.Data.DataSetExtensions"
	"Microsoft.CSharp"
	"System.Data"
	"System.Xml"
)
set_target_properties(ConvertTheFuck PROPERTIES VS_GLOBAL_ROOTNAMESPACE ConvertTheFuck)
