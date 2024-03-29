## This whole directory exists just so I could define this extra 
## preprocessor value.

ADD_DEFINITIONS(-DSimTK_SIMBODY_BUILDING_SHARED_LIBRARY)

IF(BUILD_UNVERSIONED_LIBRARIES)

    ADD_LIBRARY(${SHARED_TARGET} SHARED 
			    ${SOURCE_FILES} ${SOURCE_INCLUDE_FILES} 
				${API_ABS_INCLUDE_FILES})
    
    TARGET_LINK_LIBRARIES(${SHARED_TARGET} 
                 debug ${SimTKMATH_SHARED_LIBRARY}_d 
				 optimized ${SimTKMATH_SHARED_LIBRARY}
                 debug ${SimTKCOMMON_SHARED_LIBRARY}_d 
				 optimized ${SimTKCOMMON_SHARED_LIBRARY}
                 ${MATH_LIBS_TO_USE})
    
    SET_TARGET_PROPERTIES(${SHARED_TARGET} PROPERTIES
    	PROJECT_LABEL "Code - ${SHARED_TARGET}")
    
    # install library; on Windows the .dll goes in the bin directory.
    
    INSTALL(TARGETS ${SHARED_TARGET}
                     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
					 GROUP_READ GROUP_WRITE GROUP_EXECUTE 
					 WORLD_READ WORLD_EXECUTE
                     LIBRARY DESTINATION ${SimTK_INSTALL_PREFIX}/lib${LIB64}
                     ARCHIVE DESTINATION ${SimTK_INSTALL_PREFIX}/lib${LIB64}
                     RUNTIME DESTINATION ${SimTK_INSTALL_PREFIX}/bin)
    

ENDIF(BUILD_UNVERSIONED_LIBRARIES)


IF(BUILD_VERSIONED_LIBRARIES)

    ADD_LIBRARY(${SHARED_TARGET_VN} SHARED 
			    ${SOURCE_FILES} ${SOURCE_INCLUDE_FILES} 
				${API_ABS_INCLUDE_FILES})
    
    TARGET_LINK_LIBRARIES(${SHARED_TARGET_VN} 
                 debug ${SimTKMATH_SHARED_LIBRARY_VN}_d 
				 optimized ${SimTKMATH_SHARED_LIBRARY_VN}
                 debug ${SimTKCOMMON_SHARED_LIBRARY_VN}_d 
				 optimized ${SimTKCOMMON_SHARED_LIBRARY_VN}
                 ${MATH_LIBS_TO_USE_VN})
    
    SET_TARGET_PROPERTIES(${SHARED_TARGET_VN} PROPERTIES
    	PROJECT_LABEL "Code - ${SHARED_TARGET_VN}")
    
    # install library; on Windows the .dll goes in the bin directory.
    
    INSTALL(TARGETS ${SHARED_TARGET_VN}
                     PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
					 GROUP_READ GROUP_WRITE GROUP_EXECUTE 
					 WORLD_READ WORLD_EXECUTE
                     LIBRARY DESTINATION ${SimTK_INSTALL_PREFIX}/lib${LIB64}
                     ARCHIVE DESTINATION ${SimTK_INSTALL_PREFIX}/lib${LIB64}
                     RUNTIME DESTINATION ${SimTK_INSTALL_PREFIX}/bin)
		
ENDIF(BUILD_VERSIONED_LIBRARIES)
