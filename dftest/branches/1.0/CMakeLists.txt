PROJECT (dftest)
SET (TINYXML_SOURCES
	tinyxml/tinystr.cpp
	tinyxml/tinyxml.cpp
	tinyxml/tinyxmlerror.cpp
	tinyxml/tinyxmlparser.cpp
)
SET (DFTEST_SOURCES
	${TINYXML_SOURCES}
	fileutils.cpp
	procutils.cpp
	logging.cpp
	compiling.cpp
	xmlwriter.cpp
	main.cpp
	config.cpp
	testing.cpp
	validating.cpp
)
ADD_EXECUTABLE (dftest ${DFTEST_SOURCES})

