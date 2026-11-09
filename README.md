# CodeQuest — Learn, Code & Quiz
Developer: Munah Sarah Sameer

Qt6 Widgets desktop learning app for C++, Java and Python.

## Features
- Dashboard and editable local player profile
- 30 guided lessons (10 per language) with explanation, code, expected output and exercises
- 135 reviewed-format multiple-choice questions: 15 for each language/difficulty combination
- 15 questions per quiz, with answer feedback, score and full review
- Saved history, accuracy summary and exportable printable HTML report
- Optional WAV sound feedback and cream/navy/gold theme

## Windows build (MSYS2 UCRT64)
Install the UCRT64 Qt6 base, Qt6 multimedia, GCC and Ninja packages.
Run in PowerShell from the project directory:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64
cmake --build build
$env:PATH = "C:\msys64\ucrt64\bin;$env:PATH"
.\build\CodeQuest.exe
```

## Notes
- The app teaches Python syntax but does not execute user-entered Python code.
- `questions.json` and `sounds/` are copied beside the executable by CMake.
- Player data is stored in the user's Qt AppDataLocation as `codequest-profile.json`.
- This source has been statically checked; a Windows Qt6 build and GUI acceptance test are still required.
