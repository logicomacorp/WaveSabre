set(WAVESABRE_CONVERT_TESTS_SOURCES
	Diff.cs
	packages.config
	Printer.cs
	Program.cs
	Test.cs

	Properties/AssemblyInfo.cs
)

add_executable(WaveSabreConvertTests
	App.config

	${WAVESABRE_CONVERT_TESTS_SOURCES}
)

target_link_libraries(WaveSabreConvertTests WaveSabreConvert)

csharp_set_designer_cs_properties(${WAVESABRE_CONVERT_TESTS_SOURCES})
csharp_set_windows_forms_properties(${WAVESABRE_CONVERT_TESTS_SOURCES})
set_property(TARGET WaveSabreConvertTests PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")
set_property(TARGET WaveSabreConvertTests PROPERTY VS_DOTNET_REFERENCES 
	"System"
	"System.Core"
	"System.Xml.Linq"
	"System.Data.DataSetExtensions"
	"Microsoft.CSharp"
	"System.Data"
	"System.Xml"
	"Newtonsoft"
	"Newtonsoft.json"
)
set_property(TARGET WaveSabreConvertTests PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_6.0.8")
set_target_properties(WaveSabreConvertTests PROPERTIES VS_GLOBAL_ROOTNAMESPACE WaveSabreConvertTests)
