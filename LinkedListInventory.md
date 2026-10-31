# Hand-written linked lists in Wally

This is an inventory of the doubly linked lists written by hand in `Source/Wally`, for review before converting them to standard library containers. No code has been changed.

Line numbers refer to the source as of October 27, 2026. All paths are relative to `Source/Wally/` unless noted.

## How the lists are built

There is no shared base class. Each list repeats the same pattern:

- An **item class** with `m_pNext` and `m_pPrevious`, and `SetNext`, `GetNext`, `SetPrevious`, `GetPrevious`.
- A **list class** holding `m_pFirst` (head), `m_pWrite` (tail, for appending) and often `m_pRead`. `m_pRead` is a cursor stored in the list object that drives `GetFirstXxx()` / `GetNextXxx()` style iteration.

The only inheritance involved is `CPakBase` in `PakList.h`, which `CPakItem`, `CPakDirectory` and `CPakList` derive from. It holds display and error fields, not list links.

A few things hold across nearly every list:

- **`GetPrevious` is almost never called** outside the list's own add and remove code. No caller walks a list backwards. That removes the main reason to prefer `std::list`.
- **The `m_pRead` cursor is the biggest hazard.** It is shared state inside the list. Any call that resets it inside another iteration breaks the outer loop, and lists reached from worker threads race on it. Converting to containers removes the cursor entirely; callers become range-`for` loops.
- **Item addresses often escape.** Many lists put raw item pointers into list box, combo box, tree or list control item data (`SetItemData`, `lParam`), or keep them in other objects. A `std::vector<T>` holding objects directly moves them when it grows or when an element is erased, which would leave those pointers dangling. Where addresses escape, the safe choices are `std::vector<std::unique_ptr<T>>` or `std::list<T>`. This inventory recommends the vector of `unique_ptr` in those cases, because the code mostly appends, iterates and occasionally removes, and never splices.
- **None of the item classes define a copy constructor or assignment**, and most own raw buffers. Storing them in a container by value needs either rule-of-zero members (`std::vector<BYTE>` for buffers) or move-only semantics first.

## Summary

| # | Item / list | Defined in | Status | Pointers escape | Recommendation |
| --- | --- | --- | --- | --- | --- |
| 1 | `CWADItem` / `CWADList` | `WADList.h/.cpp` | **Done**: converted | Yes | `std::vector<std::unique_ptr<CWADItem>>` |
| 2 | `CBuildItem` / `CBuildList` | `BuildList.h/.cpp` | **Done**: converted | No | `std::vector<CBuildItem>` indexed by tile |
| 3 | `CPakItem` in `CPakDirectory` | `PakList.h/.cpp` | **Done**: converted | Yes | `std::vector<std::unique_ptr<CPakItem>>` |
| 4 | `CPakDirectory` child tree | `PakList.h/.cpp` | **Done**: converted | Yes | `std::vector<std::unique_ptr<CPakDirectory>>`, sorted |
| 5 | `CPakPasteItem` in `CPakList` | `PakList.h/.cpp` | **Done**: converted | Holds non-owning pointers | `std::vector<CPakBase*>` |
| 6 | `CWildCardItem` / `CWildCardList` | `ImageHelper.h/.cpp` | **Done**: converted | Yes | Fixed `std::vector<CWildCardItem>`, no cursor |
| 7 | `CRegistryItem` in `CRegistryHelper` | `RegistryHelper.h/.cpp` | **Done**: converted | No | `std::vector<CRegistryItem>` |
| 8 | `CFileItem` / `CFileList` | `Archive/FileList.h/.cpp` | **Done**: folded into `CDirectoryList`, archived | No | `std::vector<CString>` plus a no-case set |
| 9 | `CDirectoryEntry` / `CDirectoryList` | `DirectoryList.h/.cpp` | **Done**: converted to a scanner | No | Scanner that returns `std::vector<CString>` |
| 10 | `CBrowserCacheItem` / `CBrowserCacheList` | `BrowserCacheList.h/.cpp` | **Done**: converted | Short-lived | `std::vector<std::unique_ptr<...>>` plus a lock |
| 11 | `CWndItem` / `CWndList` | `WndList.h/.cpp` | **Done**: converted | No | `std::vector<CWndItem>` |
| 12 | `CTabDlgItem` / `CTabDlgList` | `STabCtrl.h/.cpp` | **Done**: converted | No | `std::vector<CTabDlgItem>` |
| 13 | `CImageIconItem` / `CImageIconList` | `ImageIconList.h/.cpp` | **Done**: converted | No | `std::unordered_map<int, int>` |
| 14 | `CToolSettingsPage` links | `ToolSettingsPage.h/.cpp` | **Done**: links removed | n/a | Delete the links |
| 15 | `CRuleItem` / `CRuleList` | `Archive/RuleList.h/.cpp` | **Done**: archived with the rules dialogs | n/a | Delete the links and empty `CRuleList` |
| 16 | `CWallyItem` / `CWallyList` | `Archive/WallyList.h/.cpp` | **Done**: archived | n/a | Delete the files |
| 17 | `CBrowseDIBItem` / `CBrowseDIBList` | `Archive/BrowseDIBList.h/.cpp` | **Done**: archived | n/a | Delete the files |

**Not included:** `CColorNode` in `ColorOpt.h`. Its "next" values are 24-bit indexes into a preallocated node array used by the color quantizer's hash buckets. That is a performance structure, not an object list, and it should stay as it is.

## Suggested order of work

1. **Delete the dead code** (items 14–17). No behavior changes, and the remaining list is shorter. *Done.*
2. **Small, self-contained lists** (items 7, 11, 12, 13). Each has one owner, no escaping pointers and no threads. These are good for settling the conversion style.
3. **File and directory scanning** (items 8, 9). Fix the reentrant constructor first (see item 9).
4. **Build tiles** (item 2).
5. **Wildcards** (item 6), which also fixes a worker-thread race.
6. **PAK** (items 3–5), which also fixes a use-after-free in copy/paste. *Done.*
7. **WAD** (item 1), the largest and most widely used. *Done.*
8. **Browser cache** (item 10), together with adding a lock. *Done.*

---

## 1. `CWADItem` / `CWADList`

**Done:** `CWADList` holds `std::vector<std::unique_ptr<CWADItem>>`. Item sibling links and the first/read/write cursors are gone. `GetFirstImage`/`GetNextImage`/`GetFirstLump`/`GetNextLump` are gone; callers use `GetItems()` range-`for`. `RemoveImage(..., FALSE)` releases the `unique_ptr` so merge can `AddToList` the same pointer. List-box item data still stores `CWADItem*`. Locking was not added.

**Defined in:** `WADList.h` (item 81, list 208), `WADList.cpp`.

**List API:**
- Append: `AddItem`, `AddMipItem` and `AddToList` (cpp 1415–1443).
- Remove from the middle: `RemoveImage(pItem, bDelete)` (1227–1273). Passing `bDelete = FALSE` hands ownership to the caller.
- Lookup by name: `IsNameInList` (1478).
- Lookup by position: `GetLumpAtPosition` (1368).
- Counts: `GetNumImages`, `GetNumLumps`, `GetTotalItemsSize`.
- Purge: `PurgeList`.
- Cursor iteration: `GetFirstImage`/`GetNextImage` and `GetFirstLump`/`GetNextLump` (1311–1366).
- No insert in the middle and no sort.

**Instances:**
- `CImageHelper::m_WADList` (`ImageHelper.h` 720), reached through `CPackageDoc::GetWADList()`.
- The private helpers in `CDecalToolbar` and `CToolSettingsToolbar`.
- A local helper in `CWallyDoc` (`Wallydoc.cpp` 921).
- `CWadMergeJob::m_WADList` (`WadMergeJob.h` 32).
- A stack local in `CWadMergeDlg::OnButtonGo` (`WadMergeDlg.cpp` 225).

**Used from:**

| File | What it does |
| --- | --- |
| `ImageHelper.cpp` | Thin wrappers for load, save, iterate, add, find and remove (1561, 1722, 3595–3687) |
| `PackageDoc.cpp/.h` | Add, remove and find by name, replace data, rebuild the view with `GetFirstImage`/`GetNextImage` (`UpdateViews` 500–528) |
| `PackageView.cpp` | Fills the list box with item pointers as item data (`OnInitialUpdate` 259, `FilterList` 3241, `AddImage` 2094–2286); delete, rename, remip, drag and drop |
| `PackageBrowseView.cpp` | Draws from list-box item data; keeps `m_pPreviousTiledItem` and `m_pdsTiledDIBs[].pWADItem` |
| `DecalToolbar.cpp`, `ToolSettingsToolbar.cpp` | Each loads a private list and stores item pointers in its list box |
| `PackageReMipThread.cpp` | Worker thread reads `CWADItem*` from the list box (71–123) |
| `WadMergeJob.cpp` | `MergeWAD` (169–330) pops items, looks up names and moves items between lists |
| `WadMergeDlg.cpp` | Passes a list pointer to the merge thread (328) |
| `Wallydoc.cpp`, `Wally.cpp`, `ColorDecalDlg.cpp`, `BatchJob.cpp` | Add or replace images in a package and link them to `CWallyDoc` |
| `PackageListBox.cpp`, `PackageTreeControl.cpp/.h` | Read `CWADItem*` from control data; the tree population code is `#if 0` |

**Pointer stability is required.** Item pointers are stored in list-box item data, in browse-view and toolbar members, in `CWADItem::m_pWallyDoc` back-links, and are read by worker threads.

**Cursor and thread hazards:**
- `IsNameInList`, `GetNumLumps`, `GetTotalItemsSize` and `GetLumpAtPosition` all move `m_pRead` as a side effect.
- Batch and merge worker threads modify a package's list while the UI thread iterates it.
- `CPackageDoc::m_CriticalSection` only guards `AddImage(bits)`, `IsNameInList` and `ReplaceImage`.
- `AddImage` calls `pView->AddString` (a `SendMessage` to the UI thread) while holding that lock, so it can deadlock against a UI thread waiting on the same lock.

**Order matters for saving:** lumps are written in list order. The UI list box is sorted, so the user doesn't see list order.

**Recommendation:** `std::vector<std::unique_ptr<CWADItem>>`.
- "Remove without delete" becomes releasing the `unique_ptr` to the caller.
- `GetLumpAtPosition` becomes an index.
- Cursor loops become range-`for` loops.
- Locking should cover every accessor that worker threads use.

**Other issues found:**
- `WadMergeDlg.cpp` used `dwFlags ^= WAD_MERGE_FIRST_WAD` (toggled the flag so the duplicate check was skipped on alternating merge jobs). Fixed to `&= ~`.
- `Serialize` leaks the file buffer or the `CFile` on several error paths (cpp 1832, 1862–1871, 1927, 2034).
- The empty-list check on save runs after the file has already been truncated.
- `PackageDoc::UpdateViews` re-adds images only, while the initial fill adds all lumps, so non-mip lumps disappear from the list after a merge.

## 2. `CBuildItem` / `CBuildList`

**Done:** `CBuildList` holds a `std::vector<CBuildItem>` indexed by `tile - m_iFirstTile`, so `GetTile` is constant time. Tile pixels are a `std::vector<BYTE>`. The cursor, the link fields, `RemoveImage` and the default constructor are gone. Tile bounds and item fields are initialized, each load clears the list first, and the `Serialize` buffer and `CFile` leaks are fixed. `ResetContent` still doesn't purge the build list, but its only call is commented out.

**Defined in:** `BuildList.h`, `BuildList.cpp`.

**List API:**
- Append: `AddItem` (349).
- Remove: `RemoveImage` (282), never called.
- Lookup by tile number: `GetTile` (601), a linear scan using the cursor.
- Cursor iteration: `GetFirstTile`/`GetNextTile`.
- Purge: `PurgeList`.
- `GetNumTiles` comes from the header range, not from counting the list.

**Instance:** `CImageHelper::m_BuildList` (`ImageHelper.h` 722), owned by `CBuildDoc`.

**Used from:**
- `ImageHelper.cpp`: load, `SaveART`, `GetBuildTile`.
- `BuildDoc.cpp`: tile lookups.
- `BuildView.cpp`:
  - `OnDraw` calls `GetBuildTile` once per visible tile (215). Each call is a linear scan, so a paint costs roughly n² steps.
  - Copy, paste and delete edit items in place. Delete calls `ReplaceData(NULL, 0, 0)` and never removes the item.

**Pointers do not escape.** Tiles are appended once in tile order and never removed or reordered.

**Recommendation:** `std::vector<CBuildItem>` indexed by `tile - m_iFirstTile`.
- `GetTile` becomes constant time.
- Change `m_pbyData` to `std::vector<BYTE>` so items can be moved safely.

**Other issues found:**
- `m_iFirstTile` and `m_iLastTile` are not initialized in the constructor.
- `CImageHelper::ResetContent` purges the WAD list but not the build list, so reloading into the same helper appends duplicate tiles.
- `Serialize` leaks its buffer on load and leaks the `CFile` on out-of-memory save paths.

## 3. `CPakItem` chained in `CPakDirectory`

**Done:** `CPakDirectory` holds `std::vector<std::unique_ptr<CPakItem>>`. The item cursor, sibling links, and `GetFirstPakItem`/`GetNextPakItem` are gone. List-control `lParam` still points at the item's `PAK_PARAM`. `DeleteItem` is `find` + `erase`.

**Defined in:** `PakList.h` (item 191), `PakList.cpp`.

**List API (on `CPakDirectory`):**
- Append: `AddToList` (911), used on load, and `AddFile` (1579), which checks for duplicates first.
- Remove: `DeleteItem` (1458).
- Find by name: `FindNameInList` (1196).
- Count and size: `GetItemCount` (1135).
- Cursor iteration: `GetFirstPakItem`/`GetNextPakItem` (1119–1133) through `m_pReadItem`.
- Purge: `PurgeAll`.

**Used from:**
- `PakDoc.cpp`: add files, delete, import, export, open.
- `PakListView.cpp`:
  - `UpdateList` (165–253) is the only user of the cursor.
  - It stores each item's `PAK_PARAM*` as the list control `lParam`.
  - Sorting is done by `CListCtrl::SortItems` on the view only, never on the chain.
- `PakTreeView.cpp`: counts.

**Pointer stability is required:**
- Each item's `PAK_PARAM` points back at the item itself (`PakList.cpp` 82–83).
- List control `lParam` values point into items.
- The paste list holds `CPakBase*` to items.
- `~CPakItem` deletes the item's temp file on disk, so destroying a copy would delete a live file.

**Order:** items are written to the PAK file in chain order (depth-first: a directory's items, then its subdirectories). Each entry stores its own file position, so the order only fixes the file layout.

**Recommendation:** `std::vector<std::unique_ptr<CPakItem>>`.
- `DeleteItem` becomes `find_if` on `.get()` followed by `erase`.
- Replace the cursor with a const accessor so `UpdateList` can use a range-`for` loop.

**Other issues found:**
- `m_pReadItem` is never initialized and is not reset by `DeleteItem` or `PurgeAll`.
- `CPakBase` has no virtual destructor, so always own these objects through their concrete type, never through `unique_ptr<CPakBase>`.

## 4. `CPakDirectory` tree

**Done:** children are `std::vector<std::unique_ptr<CPakDirectory>>` with sorted insert (`CompareNoCase > 0`). `m_pRoot` is `unique_ptr`. `m_pParent` stays a raw non-owning pointer. Sibling links and the first/write cursors are gone. Rename still does not re-sort.

**Structure:**
- `CPakList::m_pRoot` holds the root directory.
- Each directory holds a child chain (`m_pFirstDirectory` / `m_pWriteDirectory`), sibling links (`m_pNext` / `m_pPrevious`) and `m_pParent`.

**List API:**
- Sorted insert: `AddChildDirectory` (1357–1456). `SORT_PAK_ITEM_LIST` is hard-coded to 1, so every insert goes in sorted position, including the middle.
- Delete: `DeleteDirectory` (1495).
- Find: `FindDirectoryInList` (direct children only) and `FindDirectory` (recursive path).
- Recursive forward walks: `BuildTree`, `Serialize`, `ExportFiles`, `IsModified`, `GetItemCount`, `CreateTempDirectory`, `SetDirectoryName`.
- Up-walk: `GetPathFromHere` follows `m_pParent`.

**Used from:**
- `PakDoc.cpp` (`m_pCurrentDirectory`, `DeleteEntry`, import, export).
- `PakListView.cpp` (`UpdateList`, rename, new folder, delete).
- `PakTreeView.cpp`: the tree control stores `CPakDirectory*` as `lParam`.
- `SelectPakTreeDlg.cpp`, `PakImportDlg.cpp`, `PakExportDlg.cpp`.

**Pointer stability is required:**
- Tree control `lParam`, list control `PAK_PARAM*` and the paste list all point at directories.
- `CPakDoc::m_pCurrentDirectory`, every child's `m_pParent`, and the import and export dialog members do too.

**Recommendation:** `std::vector<std::unique_ptr<CPakDirectory>>` for the children, with sorted insert via `std::lower_bound` and a case-insensitive compare.
- Make `m_pRoot` a `std::unique_ptr<CPakDirectory>`.
- Keep `m_pParent` as a raw, non-owning pointer.
- Folders have few children, so inserting into the middle of a vector is cheap.

**Other issues found:**
- **The sort compare is fragile.** `AddChildDirectory` at line 1402 tests `CompareNoCase(...) == 1`, but `CompareNoCase` only guarantees a positive value. Sibling order may not be reliably sorted; the test should be `> 0`.
- Renaming a directory doesn't move it to its new sorted position.
- The tree view's rename skips the duplicate-name check that the list view performs.
- `OnPakPopupNewFolder` adds a duplicate list row when "New folder" already exists.

## 5. `CPakPasteItem` chained in `CPakList`

**Done:** paste list is `std::vector<CPakBase*>`. `CPakPasteItem` is gone. `DeleteEntry` prunes the deleted item or directory subtree from the paste list before destroying it.

**API:** append with `AddToPasteList` (2298), count with `GetPasteCount`, purge with `PurgePasteList`, and a forward walk from `GetFirstPasteItem`.

**Used from:** `PakListView.cpp`:
- `AddPasteItems` (863) fills the list when the user copies or starts a drag.
- `RenderData` (742–861) walks it later, when the clipboard data is actually requested.

**Ownership:** the wrappers are owned by `CPakList`, but the `CPakBase*` pointers inside them point at items and directories owned by the tree.

**Recommendation:** `std::vector<CPakBase*>`, since the list is append-only, walked forward and doesn't own anything.

**Use-after-free:** nothing removes paste entries when items are deleted.
- Copying, then deleting the item in Wally, then pasting in Explorer dereferences freed memory.
- The same happens if the document closes while clipboard data is still pending.
- The conversion should clear or prune the paste list in `DeleteItem` and `DeleteDirectory`, or store paths instead of pointers.

## 6. `CWildCardItem` / `CWildCardList`

**Done:** `CWildCardList` holds a `std::vector<CWildCardItem>` reserved to `WILDCARD_LIST_COUNT` and never resized, so combo-box item pointers stay valid. `GetAt` is a bounds-checked index. The global `m_pRead` cursor is gone; `CImageHelper` keeps its own `m_iWildCardCursor` for filtered walks. `GetWildCardExtension` no longer races across batch threads. `SetRead`'s self-assignment is gone with the cursor. Shared `.m8`/`.swl` registry keys and `DWORD` item-data casts are unchanged.

**Defined in:** `ImageHelper.h` (581–688), `ImageHelper.cpp` (101–723).

**Instance:**
- A single global, `g_WildCardList` (`ImageHelper.cpp` 94).
- It is built during static initialization from `g_strTypeDescriptions` and never changes size afterwards.
- List position equals the image type value (`IH_*_TYPE`).

**API:**
- `GetAt(int)` and `GetCount()`, both of which walk using `m_pRead`.
- Cursor iteration: `GetFirstSupportedImage` / `GetNextSupportedImage`.
- `PrepareForChanges`, `SolidifyAllChanges`, `CancelAllChanges`, `ReadRegistry`, `WriteRegistry`.
- `CImageHelper` filter wrappers (`GetFirstSupportedPackage`, and so on) and `GetWildCardExtension(int)`, which also walk the global cursor.

**Used from:**
- `BatchDlg.cpp`, `BatchAdvancedDlg.cpp`, `PackageExportDlg.cpp`, `wallypropertypage1.cpp`: fill combo boxes and list boxes with `CWildCardItem*` as item data, then read the pointers back and modify the items.
- `NewRuleDlg.cpp`: fills a combo box (stores the type, not a pointer).
- `BrowseView.cpp` 1164: `GetAt(type)`.
- `MainFrm.cpp` 486: reads the registry settings.
- `BatchJob.cpp` 269 and 278: call `ConvertImage` and `GetWildCardExtension` on worker threads.
- File-filter string builders, called from many dialogs.

**Pointer stability is required:** item pointers are stored in item data of several sorted controls.

**Race between worker threads:**
- Several batch conversion threads run at once (`ThreadManager.cpp` 197), and each walks the global `m_pRead` through `GetWildCardExtension` and `GetAt`.
- Concurrent jobs can corrupt each other's walk, producing a wrong file extension or a null dereference.

**Recommendation:** a `std::vector<CWildCardItem>` (or `std::array`) filled once and never resized, so item addresses stay fixed.
- Remove `m_pRead`.
- `GetAt` becomes a bounds-checked index.
- The filtered walks become range-`for` loops.

**Other issues found:**
- **`SetRead` doesn't set anything.** `ImageHelper.cpp` 623 is `m_pRead = m_pRead;`, assigning the member to itself and ignoring the argument.
- `GetAt` has no bounds check.
- **Shared registry keys.** The HereticII `.m8` row in `g_strTypeDescriptions` uses the same registry keys as the SiN `.swl` row, so one silently overwrites the other in the registry.
- Item data is cast to `DWORD`/`ULONG`, which would truncate pointers in an x64 build. It should be `DWORD_PTR`.

## 7. `CRegistryItem` in `CRegistryHelper`

**Defined in:** `RegistryHelper.h` (item 25, owner `CRegistryHelper` 142), `RegistryHelper.cpp`.

**API:**
- Overloaded `AddItem` (append), `PurgeList`.
- `ReadRegistry` and `WriteRegistry`, each a single forward walk.
- No cursor, no find, no remove.

**Instances:**
- The global `g_rhHelper` (`Globals.cpp` 47), filled by about 173 `ADD_REG_*` registrations.
- Short-lived locals in `ImageHelper.cpp` (file association helpers).

**Pointers do not escape.** Items point *into* the variables they persist, not the other way around.

**Recommendation:** `std::vector<CRegistryItem>`; `PurgeList` becomes `clear()`.

**Order note:** `ReadRegistry` and `WriteRegistry` stop at the first `RegCreateKeyEx` failure, so later items are skipped.

## 8. `CFileItem` / `CFileList`

**Defined in:** `FileList.h`, `FileList.cpp`.

**API:**
- `AddFile` (45) appends after a linear, case-insensitive duplicate check, so building a directory's list costs roughly n² steps.
- `HasFile` (78) is a linear scan; it is only reached through the unused `CDirectoryList::FindFile`.
- `PurgeList`.

**Instance:** only embedded as `CDirectoryEntry::m_FileList`.

**Used from:** `DirectoryList.cpp` (`GetFileCount`, `GetAllFiles`), `BatchDlg.cpp` (394–415) and `PakDoc.cpp` (660–696), always walking forward to collect file names.

**Pointers do not escape.** Callers copy the file name strings.

**Recommendation:** `std::vector<CString>` plus a case-insensitive `std::set` or `std::unordered_set` for the duplicate check.

**Inconsistency:** `HasFile` compares with `MakeUpper` while `AddFile` uses `CompareNoCase`.

## 9. `CDirectoryEntry` / `CDirectoryList`

**Done** (together with #8): `CDirectoryList` now scans recursively into a `std::vector<CString>` of full paths, keeping the post-order. `CDirectoryEntry`, `FindFile` and `HasFile` are gone, and the find-handle leak, `GetBuffer` and `long` handle issues are fixed. The disabled `OLD_BATCH_CONVERSION` block in `BatchDlg.cpp` and the `#if 0` `OnWizardTest` in `Wally.cpp` still reference the old classes.

**Defined in:** `DirectoryList.h`, `DirectoryList.cpp`.

**API:**
- `AddDirectory` (append), `SearchDirectories`, `SetRoot`.
- `AddWildcard`, `GetFileCount`, `GetAllFiles`.
- `FindFile` is broken (it builds `dir\fullpath`) and unused.

**Instances:** always stack locals, in `BatchDlg.cpp` 333, `PakDoc.cpp` 625 and `PakList.cpp` 1226.

**How callers use it:** every live caller just flattens the result into a list of full paths. `PakList.cpp` already uses `GetAllFiles`.

**Reentrant constructor.** The `CDirectoryEntry` constructor scans the disk and calls `m_pParent->AddDirectory()` for each subdirectory. As a result:
- The whole tree is built inside nested constructors.
- Entries end up in post-order: deepest subdirectories first, the root last.
- Converting this naively to `emplace_back` would grow the vector while one of its elements is still being constructed, which is undefined behavior.
- Move the scan out of the constructor first, and decide whether the post-order (which sets the order of batch jobs and PAK inserts) should be kept.

**Recommendation:** turn `CDirectoryList` into a scanner that returns `std::vector<CString>` of full paths, deduplicated case-insensitively. That removes `CDirectoryEntry` and `CFileList` entirely.

**Other issues found:**
- `_findfirst` is never paired with `_findclose` (lines 206 and 242), so every directory searched leaks a find handle.
- `GetBuffer` is called without a matching `ReleaseBuffer`.
- `long hFile` would truncate the find handle on x64.

## 10. `CBrowserCacheItem` / `CBrowserCacheList`

**Done:** items are `std::vector<std::unique_ptr<CBrowserCacheItem>>`. `GetAtPosition` is an index. Sorted insert uses `lower_bound`. A recursive critical section guards the list, directory name, and cache-thread status. Selection walks snapshot into `GetSelectedItems()`. The GROOM `switch` fall-through and uncompressed-cache `case 0` fall-through are fixed.

**Defined in:** `BrowserCacheList.h`, `BrowserCacheList.cpp`.

**API:**
- Sorted insert by lower-case file name: `AddItem` (1125).
- Remove from the middle: `RemoveItem` (1078).
- Indexed lookup: `GetAtPosition(i)` (1045), a linear scan.
- Count: `GetCount` (1064), also a linear walk.
- Selection cursor through `m_pSelected`.
- `Initialize` (1192–1687) purges the list and rebuilds it from the cache file and the directory.

**Instance:** `CBrowseDoc::m_BrowserCacheList` (`BrowseDoc.h` 47).

**Used from:**
- `BrowseView.cpp`:
  - `OnDraw` (202) calls `GetAtPosition` for every visible cell, so a paint costs roughly n² steps.
  - Hit-testing, paste (sorted insert) and delete-selected (removal from the middle).
- `BrowseWorkerThread.cpp`: runs `Initialize` on a worker thread, which purges and inserts.

**Pointer stability:** item pointers are only held briefly, and the UI re-fetches by index. Items own raw buffers and have no copy constructor, so they still must not be stored by value.

**Race with the worker thread:**
- The worker thread purges and rebuilds the list while the UI thread paints from it and adds or removes items, with no lock anywhere.
- A use-after-free is possible.

**Recommendation:** `std::vector<std::unique_ptr<CBrowserCacheItem>>`, guarded by a critical section.
- Indexed access (the dominant use) becomes constant time.
- Sorted insert uses `lower_bound`.

**Other issues found:**
- **Missing `break` in `BrowserCacheThread.cpp`.** The `CACHE_THREAD_MESSAGE_GROOM` case (340–355) falls through into the `STOP` case, so the cache thread stops after the first groom.
- **Missing `break` in `BrowserCacheList.cpp`.** Compression case 0 (649–651) falls through into case 1.
- `m_pSelected` is not initialized in the constructor.

## 11. `CWndItem` / `CWndList`

**Defined in:** `WndList.h`, `WndList.cpp`.

**API:**
- `AddItem` (append; asserts on duplicates).
- `IsInList(CWnd*)` (linear lookup).
- `InvalidateAll(flag)` (forward walk).
- `PurgeList`.

**Instance:** the global `g_InvalidateList` (`Globals.cpp` 107).

**Used from:**
- `PackagePreviewWnd.cpp` 119 registers the window.
- `ColorPaletteWnd.cpp` 508 and `ToolSettingsToolbar.cpp` 597 call `InvalidateAll`.

In practice only the tool settings toolbar's preview window ever registers.

**Recommendation:** `std::vector<CWndItem>` (or `std::vector<std::pair<CWnd*, int>>`).

**Missing unregister:** there is no way to remove a window. A registered window destroyed before the global list would leave a dangling `CWnd*`. It's safe today only because the one registrant lives as long as the main frame.

## 12. `CTabDlgItem` / `CTabDlgList`

**Defined in:** `STabCtrl.h` (91–159), `STabCtrl.cpp`.

**API:** two `AddItem` overloads (append), `NeverShowControl(id)` and `HideShowItems(tab)` (forward walks), and `PurgeList`. There is no cursor.

**Instance:** `CSTabCtrl::m_TabItemList`.

**Used from:** `ToolSettingsToolbar.cpp` (106–110), `ToolSettingsPage.cpp` (251) and `DecalToolbar.cpp` (the toolbar class is never instantiated).

**Recommendation:** `std::vector<CTabDlgItem>`.

## 13. `CImageIconItem` / `CImageIconList`

**Done**: converted to `std::unordered_map<int, int>`; the destructor bugs are gone with the explicit constructor and destructor.

**Defined in:** `ImageIconList.h`, `ImageIconList.cpp`. `CImageIconList` derives from `CImageList`.

**What it does:** maps a system icon index to an index in the image list. `FindMatch` is a linear lookup by key; `AddToList` appends.

**Instance:** `CPakListView::m_ImageList`, used at `PakListView.cpp` 228 and 1112.

**Recommendation:** `std::unordered_map<int, int>`.

**Destructor bugs:**
- The destructor explicitly calls `CImageList::~CImageList()` (cpp 72), so the base destructor runs twice.
- The constructor's explicit `CImageList::CImageList()` call (cpp 64) does nothing.

## 14. `CToolSettingsPage` links

`m_pNext` and `m_pPrevious` exist (`ToolSettingsPage.h` 18–19), but they are only ever set to NULL. The two pages are fixed members, `CToolSettingsToolbar::m_Page1` and `m_Page2`.

**Recommendation:** delete the links and their accessors.

## 15. `CRuleItem` / `CRuleList`

- `CRuleList` is an empty class.
- `CRulesDlg::OnButtonNew` creates a single `CRuleItem`, edits it and always deletes it. The add-to-list line is a stub comment.
- `ID_VIEW_RULES` is in no menu or toolbar, so the dialog can't be reached.

**Recommendation:** delete the links and the empty `CRuleList`. If the rules feature is finished later, a `std::vector<CRuleItem>` owned by the dialog is enough.

**Other issue found:** `RemoveOption` uses XOR, so it turns the option on if it wasn't set.

## 16. `CWallyItem` / `CWallyList`

Nothing includes `WallyList.h` except `WallyList.cpp`, but the file is still compiled (`Wally.vcxproj` 701).

Its `Serialize` is broken in three ways:
- At line 270 it rejects files whose header *is* `"WALLY"`.
- Loading from a memory buffer always fails its bounds check.
- The save branch writes nothing.

**Recommendation:** delete `WallyList.h` and `WallyList.cpp` and remove them from the project.

## 17. `CBrowseDIBItem` / `CBrowseDIBList`

`BrowseDIBList.cpp`, `BrowseDocument.cpp` and `BrowseDocument.h` are not in `Wally.vcxproj`. Their only user, `CBrowseDocument`, no longer compiles (it includes a missing header). The live browser is `CBrowseDoc`, covered in item 10.

**Recommendation:** delete the files.
