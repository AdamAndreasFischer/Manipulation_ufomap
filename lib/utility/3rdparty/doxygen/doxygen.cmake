Include(FetchContent)

FetchContent_Declare(
  doxygen
  GIT_REPOSITORY https://github.com/doxygen/doxygen.git
  GIT_TAG        ebc57c6dd303a980bd19dd74b8b61c8f3f5180ca # 1.10.0
  GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(doxygen)