Include(FetchContent)

FetchContent_Declare(
  oneTBB
  GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
  GIT_TAG        1c4c93fc5398c4a1acb3492c02db4699f3048dea # 2021.13.0
  GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(oneTBB)