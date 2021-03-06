
set(TARGET pe_resource)
set(DIR Resource)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/PE/${DIR})
set(SRCROOT ${PROJECT_SOURCE_DIR}/src/${DIR})

set(SRC
        ${INCROOT}/Defines.h
        ${INCROOT}/IFileMonitor.h
        ${INCROOT}/WindowsFileMonitor.h
        ${SRCROOT}/WindowsFileMonitor.cpp
        ${INCROOT}/ResourceCounter.h
        ${SRCROOT}/ResourceCounter.cpp
        ${INCROOT}/IResource.h
        ${SRCROOT}/IResource.cpp
        ${INCROOT}/Resource.h
        ${SRCROOT}/Resource.cpp
        ${INCROOT}/ResourceManager.h
        ${SRCROOT}/ResourceManager.cpp
        ${INCROOT}/ResourcePool.h
        ${INCROOT}/ResourceHandle.h
        ${INCROOT}/Properties.h
        ${SRCROOT}/Properties.cpp
        )

add_library(${TARGET} SHARED ${SRC})

target_include_directories(${TARGET} PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(${TARGET} PUBLIC pe_utils CONAN_PKG::gsl_microsoft CONAN_PKG::stb CONAN_PKG::yaml-cpp)

