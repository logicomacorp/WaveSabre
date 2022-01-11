set(PROJECT_MANAGER_SOURCES
	Program.cs
	ProjectDetails.cs
	ProjectDetails.Designer.cs
	ProjectManager.cs
	ProjectManager.Designer.cs

	Properties/AssemblyInfo.cs
	Properties/Resources.Designer.cs
	Properties/Settings.Designer.cs

	Properties/Resources.resx
	Properties/Settings.settings

	logicoma_basic_CoA_icon.ico

	ProjectDetails.resx
	ProjectManager.resx
)

add_executable(ProjectManager
	App.config

	${PROJECT_MANAGER_SOURCES}
)

target_link_libraries(ProjectManager WaveSabreConvert)

set_property(TARGET ProjectManager PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/logicoma_basic_CoA_icon.ico")
csharp_set_designer_cs_properties(${PROJECT_MANAGER_SOURCES})
csharp_set_windows_forms_properties(${PROJECT_MANAGER_SOURCES})
set_property(TARGET ProjectManager PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")
set_property(TARGET ProjectManager PROPERTY VS_DOTNET_REFERENCES 
	"System"
	"System.Core"
	"System.Xml.Linq"
	"System.Data.DataSetExtensions"
	"Microsoft.CSharp"
	"System.Data"
	"System.Xml"
	"System.Windows"
	"System.Windows.Forms"
	"System.Drawing"
)
set_target_properties(ProjectManager PROPERTIES VS_GLOBAL_ROOTNAMESPACE ProjectManager)

add_custom_command(TARGET ProjectManager POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:WaveSabreStandAlonePlayer> $<TARGET_FILE_DIR:ProjectManager>)
