  file(GLOB_RECURSE XWAX src/xwax/*.c src/xwax/*.h)

  source_group("xwax" FILES ${XWAX})

  target_include_directories(${PROJECT_NAME} PUBLIC src/xwax)
  target_sources(${PROJECT_NAME} PRIVATE ${XWAX})