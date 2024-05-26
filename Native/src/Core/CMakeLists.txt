project(Core)

make_native_lib()

target_sources(${PROJECT_NAME} PRIVATE
	src/Instrumentation/Logging.cpp
	src/Instrumentation/ISink.cpp
	src/Instrumentation/LogWriter/StdOutLogWriter.cpp
 )
