cmake_minimum_required(VERSION 3.11)

project(Monad.FLParser LANGUAGES CSharp)

cmake_policy(SET CMP0057 NEW)
include(CSharpUtilities)

# Note: The space suffix is required for the flag to work correctly.
set(CMAKE_CSharp_FLAGS "/langversion:latest ")

set(MONAD_FL_PARSER_SOURCES
	${MONAD_FL_PARSER_DIR}/Properties/AssemblyInfo.cs

	${MONAD_FL_PARSER_DIR}/AutomationData.cs
	${MONAD_FL_PARSER_DIR}/AutomationKeyframe.cs
	${MONAD_FL_PARSER_DIR}/Channel.cs
	${MONAD_FL_PARSER_DIR}/ChannelPlaylistItem.cs
	${MONAD_FL_PARSER_DIR}/Enums.cs
	${MONAD_FL_PARSER_DIR}/FlParseException.cs
	${MONAD_FL_PARSER_DIR}/GeneratorData.cs
	${MONAD_FL_PARSER_DIR}/IChannelData.cs
	${MONAD_FL_PARSER_DIR}/Insert.cs
	${MONAD_FL_PARSER_DIR}/InsertSlot.cs
	${MONAD_FL_PARSER_DIR}/IPlaylistItem.cs
	${MONAD_FL_PARSER_DIR}/Note.cs
	${MONAD_FL_PARSER_DIR}/Pattern.cs
	${MONAD_FL_PARSER_DIR}/PatternPlaylistItem.cs
	${MONAD_FL_PARSER_DIR}/Plugin.cs
	${MONAD_FL_PARSER_DIR}/PluginIoInfo.cs
	${MONAD_FL_PARSER_DIR}/Project.cs
	${MONAD_FL_PARSER_DIR}/ProjectParser.cs
	${MONAD_FL_PARSER_DIR}/Track.cs
)

add_library(Monad.FLParser SHARED ${MONAD_FL_PARSER_SOURCES})
set_property(TARGET Monad.FLParser PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.6.1")
set_property(TARGET Monad.FLParser PROPERTY VS_DOTNET_REFERENCES 
	"System"
	"System.Core"
)
