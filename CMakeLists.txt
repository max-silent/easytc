PROJECT(easytc)

FILE(GLOB SOURCE_FILES src/*.cpp)
SET(MOC_HEADERS src/FormCreateImage.hpp src/FormMain.hpp src/FormMountImage.hpp src/FormPleaseWait.hpp)    
SET(UI_FILES ui/FormCreateImage.ui ui/FormMain.ui ui/FormMountImage.ui ui/FormPleaseWait.ui)
  
FIND_PACKAGE(Qt4 REQUIRED)
  
INCLUDE(${QT_USE_FILE})
  
QT4_WRAP_UI(UI_HEADERS ${UI_FILES})
QT4_WRAP_CPP(MOC_SOURCES ${MOC_HEADERS})
  
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

ADD_EXECUTABLE(easytc ${SOURCE_FILES} ${MOC_SOURCES} ${UI_HEADERS})
TARGET_LINK_LIBRARIES(easytc ${QT_LIBRARIES})
