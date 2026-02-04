# Fichier source temporaire généré
set(MODULES_LOADER_TEMP "${CMAKE_CURRENT_BINARY_DIR}/gen_scriptloader/ModulesLoader.cpp")
set(MODULES_LOADER_TEMPLATE "${CMAKE_SOURCE_DIR}/modules/ModulesLoader.cpp.in.cmake")

# Fonction pour récupérer tous les sous-dossiers d'un répertoire donné
function(get_subdirectories result curdir)
    file(GLOB children LIST_DIRECTORIES true ${curdir}/*)
    set(subdirs "")
    foreach(child ${children})
        if(IS_DIRECTORY ${child})
            get_filename_component(folder_name ${child} NAME)
            list(APPEND subdirs ${folder_name})
        endif()
    endforeach()
    set(${result} ${subdirs} PARENT_SCOPE)
endfunction()

# Détection automatique des modules (tous les sous-dossiers de modules/)
get_subdirectories(MODULE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Détecter les modules valides
set(VALID_MODULES "")
foreach(MODULE ${MODULE_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE})
        # Vérifie qu'il contient des fichiers .cpp ou .h
        file(GLOB_RECURSE MODULE_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.h")
        if(MODULE_SRC_FILES)
            list(APPEND VALID_MODULES ${MODULE})
        endif()
    endif()
endforeach()
if(NOT PLAYERBOTS)
    list(REMOVE_ITEM VALID_MODULES "mod_playerbots")
endif()

message(STATUS "Modules : ${VALID_MODULES}")

set(MODULES_LIST "")
foreach(MODULE ${VALID_MODULES})
    if(MODULES_LIST STREQUAL "")
        set(MODULES_LIST "${MODULE}")
    else()
        set(MODULES_LIST "${MODULES_LIST},${MODULE}")
    endif()
endforeach()

if (NOT MODULES_LIST STREQUAL "")
    if (TARGET worldserver)
        target_compile_definitions(worldserver PRIVATE TC_MODULES_LIST="${MODULES_LIST}")
    endif()
    if (TARGET authserver)
        target_compile_definitions(authserver PRIVATE TC_MODULES_LIST="${MODULES_LIST}")
    endif()
endif()

# Collect module config templates
file(GLOB_RECURSE MODULE_CONF_DISTS
  "${CMAKE_CURRENT_SOURCE_DIR}/*/conf/*.conf.dist"
  "${CMAKE_CURRENT_SOURCE_DIR}/*/config/*.conf.dist"
)

# Ajout des fichiers dans les groupes Visual Studio
set(MODULES_SOURCES "")
foreach(MODULE ${VALID_MODULES})
    string(REPLACE "-" "_" MODULE_FUNC_NAME ${MODULE})

    # Récupération des fichiers source du module
    file(GLOB_RECURSE MODULE_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/${MODULE}/*.h")
    list(APPEND MODULES_SOURCES ${MODULE_SRC_FILES})

    # Créer un groupe Visual Studio reflétant l'arborescence des fichiers
    foreach(SRC_FILE ${MODULE_SRC_FILES})
        get_filename_component(SRC_PATH ${SRC_FILE} DIRECTORY)
        file(RELATIVE_PATH REL_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SRC_PATH})
        source_group(${REL_PATH} FILES ${SRC_FILE})
    endforeach()
endforeach()

set(MODULES_INCLUDES "")
set(MODULES_DECLARATIONS "")
set(MODULES_CALLS "")

foreach(MODULE ${VALID_MODULES})
    string(REPLACE "-" "_" MODULE_FUNC_NAME ${MODULE})

    # AzerothCore-style module loader naming: Add<module>Scripts()
    set(MODULES_INCLUDES "${MODULES_INCLUDES}void Add${MODULE_FUNC_NAME}Scripts();\n")

    # Ajout des déclarations et appels pour ModulesLoader.cpp
    set(MODULES_DECLARATIONS "${MODULES_DECLARATIONS}void Add${MODULE_FUNC_NAME}Scripts();\n")
    set(MODULES_CALLS "${MODULES_CALLS}    Add${MODULE_FUNC_NAME}Scripts();\n")
endforeach()

# Génération du fichier ModulesLoader.cpp dans le build tree
configure_file(${MODULES_LOADER_TEMPLATE} ${MODULES_LOADER_TEMP} @ONLY)

# Inclure explicitement ModulesLoader.cpp (généré) et ModulesLoader.h
set(MODULES_LOADER_FILES
    ${MODULES_LOADER_TEMP}
    ${CMAKE_SOURCE_DIR}/modules/ModulesLoader.h
)

# Ajouter ces fichiers dans les sources
list(APPEND MODULES_SOURCES ${MODULES_LOADER_FILES})
source_group("ModulesLoader" FILES ${MODULES_LOADER_FILES})

# Base sources which are used by every script project
if (USE_COREPCH)
  set(PRIVATE_PCH_HEADER ModulesPCH.h)
endif()

# Création de la bibliothèque statique "modules"
add_library(modules STATIC ${MODULES_SOURCES})

# Copy module configs to build output (AzerothCore-style)
if (MODULE_CONF_DISTS)
  if (WIN32)
    set(TC_MODULES_BIN_DIR "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/modules")
  else()
    set(TC_MODULES_BIN_DIR "${CMAKE_BINARY_DIR}/bin/modules")
  endif()
  foreach(MODULE_CONF_DIST ${MODULE_CONF_DISTS})
    get_filename_component(MODULE_CONF_NAME ${MODULE_CONF_DIST} NAME)

    add_custom_command(TARGET modules POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${TC_MODULES_BIN_DIR}"
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${MODULE_CONF_DIST}" "${TC_MODULES_BIN_DIR}/${MODULE_CONF_NAME}"
      COMMENT "Copying module configs"
    )
  endforeach()
endif()

# Fonction pour récupérer récursivement tous les sous-dossiers
function(get_all_subdirectories result curdir)
    file(GLOB_RECURSE children LIST_DIRECTORIES true ${curdir}/*)
    set(subdirs "")
    foreach(child ${children})
        if (IS_DIRECTORY ${child})
            list(APPEND subdirs ${child})
        endif()
    endforeach()
    set(${result} ${subdirs} PARENT_SCOPE)
endfunction()

# Récupérer tous les sous-dossiers dans "modules/"
get_all_subdirectories(ALL_MODULE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})

# Ajout des chemins d'inclusion nécessaires pour accéder aux fichiers comme "Player.h"
target_include_directories(modules PRIVATE
  ${ALL_MODULE_INCLUDES}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/modules
  game
  common
  database
)

target_link_libraries(modules
  PUBLIC
    game
    common
    database
)

add_dependencies(modules revision_data.h)
