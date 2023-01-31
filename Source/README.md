## Background
Wally was initially written in 1997 using Visual C++ version 5.0.  Over the years the codebase was migrated to VC6, where it remained since roughly the 2003 timeframe, which was that last time the code was actively worked on.  Much of the original code was designed when Windows 98 was the dominant OS, and eventually adapted for Windows XP.  Since that time, much has changed with Windows and Visual Studio, to the point that much of the original code no longer worked with modern OSes and the codebase could not be converted as Visual Studio did not support converting the older DSW solution format.  As a result, the solution and project files were regenerated from scratch and the old source files from the prior VC6 environment were copied over.  This version is still being tested using Windows 10 and Windows 11, and there may be many bugs still.  To explore the current debug version of Wally, visit the [binaries](../Binaries) page and download the WallyD_xxx.exe version, which will be the most up to date developmental version.  It is likely to have many untested features and bugs, so beware.  Copy the debug version into the same folder as Wally.exe so that it can find the various support files.

## Dependencies
Wally makes use of [libpng](http://www.libpng.org/pub/png/libpng.html), [jpeg-9e](https://www.ijg.org/), [zlib](https://www.zlib.net/), and the [boost](https://www.boost.org/users/download/) libraries.  Except for boost, the source code and project files for these external dependencies are included under the Wally root source code structure, as some modifications have been made to their respective project files (no pre-compiled binaries are provided for these libs).  The boost library is inordinately large and for practical reasons it has been excluded from the Wally source code structure.  Pre-compiled binaries are available from the [boost website](https://www.boost.org/users/download/).  To build Wally you will need to download at least the boost header files and point to the correct location using the WallyPropertySheet user macro named *BOOSTDIR*.

## Contributing

Wally welcomes contributions. When contributing, please follow the code below.

### Issues

Feel free to submit issues and enhancement requests.

Please help us by providing **minimal reproducible examples**, because source code is easier to let other people understand what happens.
For crash problems on certain platforms, please bring stack dump content with the detail of the OS, compiler, etc.

Please try breakpoint debugging first, tell us what you found, see if we can start exploring based on more information been prepared.

### Workflow

In general, we follow the "fork-and-pull" Git workflow.

 1. **Fork** the repo on GitHub
 2. **Clone** the project to your own machine
 3. **Checkout** a new branch on your fork, start developing on the branch
 4. **Test** the change before commit, Make sure the changes pass all the tests, including `unittest` and `preftest`, please add test case for each new feature or bug-fix if needed.
 5. **Commit** changes to your own branch
 6. **Push** your work back up to your fork
 7. Submit a **Pull request** so that we can review your changes

NOTE: Be sure to merge the latest from "upstream" before making a pull request!

### Copyright and Licensing

You can copy and paste the license summary from below.

```
Copyright (C) 1998 Ty Matthews and Neal White III

Licensed under the MIT License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

http://opensource.org/licenses/MIT

Unless required by applicable law or agreed to in writing, software distributed 
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
CONDITIONS OF ANY KIND, either express or implied. See the License for the 
specific language governing permissions and limitations under the License.
```

## Revision History
###2023-01-31 (current debug release):
- Added support for custom Quake2 engine flags and contents when editing .WAL file properties.  Users can now specify their own custom bitmask settings using JSON following [this schema](https://ty-matthews.github.io/Wally/json/schema/texture_properties.wal.json), or just copy and edit the default files.  The application will search for folders underneath the .\Wally\json\.wal folder structure.  Any folder found there will be searched for two files: **flags.json** and **content.json**.  If those files are found and the JSON is valid, the application will add that folder name as a custom engine that is selectable when viewing the .WAL file properties dialog box.  Changing the engine will dynamically enable/disable the appropriate flags and contents checkboxes.
- Corrected some memory leak issues with CColorOptimizer
- Rebuilt many parts of Batch Conversion multi-threading to correct for spurious crashes
- Rebuilt Visual Studio solution using source files for Visual Studio 2022
- Updated JPEG library to version 9e
- Updated libpng library to version 1.6.39
- Updated zlib library to version 1.2.13
- Added boost library version 1.81.0 for a variety of useful functions (JSON parsing a primary one)
- Updated RegistryHelper and MemBuffer classes to more modern versions
- Added InitFromResource function to MemBuffer class to support loading of RCDATA objects out of .exe file
