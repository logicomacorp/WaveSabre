set(WAVESABRE_CONVERT_SOURCES
	FLConverter.cs
	ILog.cs
	LiveConverter.cs
	LiveParser.cs
	LiveProject.cs
	MetaPlugin.cs
	ProjectConverter.cs
	ReaperConverter.cs
	ReaperParser.cs
	RenoiseConverter.cs
	RenoiseInject.cs
	RenoiseParser.cs
	RenoiseSong63.cs
	SendItConfig.cs
	Serializer.cs
	Song.cs
	Utils.cs

	ReaperElements/ReaperElement.cs
	ReaperElements/ReaperProject.cs
	ReaperElements/ReaperTag.cs
	ReaperElements/ReaperTrack.cs

	Properties/AssemblyInfo.cs
	Properties/Resources.Designer.cs
	Properties/Resources.resx

	Resources/Song.xml
	Resources/template.xrns
)

add_library(WaveSabreConvert SHARED ${WAVESABRE_CONVERT_SOURCES})

csharp_set_designer_cs_properties(${WAVESABRE_CONVERT_SOURCES})
csharp_set_windows_forms_properties(${WAVESABRE_CONVERT_SOURCES})
set_property(TARGET WaveSabreConvert PROPERTY VS_DOTNET_REFERENCE_Monad.FLParser "${CMAKE_CURRENT_SOURCE_DIR}/FLParser/Monad.FLParser.dll")
set_property(TARGET WaveSabreConvert PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")
set_property(TARGET WaveSabreConvert PROPERTY VS_DOTNET_REFERENCES 
	"System"
	"System.Core"
	"System.IO.Compression"
	"System.IO.Compression.FileSystem"
	"System.Xml.Linq"
	"System.Data.DataSetExtensions"
	"Microsoft.CSharp"
	"System.Data"
	"System.Xml"
	"System.ComponentModel"
)
set_target_properties(WaveSabreConvert PROPERTIES VS_GLOBAL_ROOTNAMESPACE WaveSabreConvert)
