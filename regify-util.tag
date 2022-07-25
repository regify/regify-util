<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="struct">
    <name>KvStore_</name>
    <filename>struct_kv_store__.html</filename>
    <member kind="variable">
      <type>kvset</type>
      <name>set</name>
      <anchorfile>struct_kv_store__.html</anchorfile>
      <anchor>ac3a3b7caea49b0e917e6db9a26476357</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>kvget</type>
      <name>get</name>
      <anchorfile>struct_kv_store__.html</anchorfile>
      <anchor>a86e65a7a3b43fae351d8c70f705a3748</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>kvlist</type>
      <name>list</name>
      <anchorfile>struct_kv_store__.html</anchorfile>
      <anchor>a52a333e638a82b71314503253a6c5252</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>ctx</name>
      <anchorfile>struct_kv_store__.html</anchorfile>
      <anchor>a1f6d09b30c54854dfca659b7463ac207</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>ruTimeVal</name>
    <filename>structru_time_val.html</filename>
    <member kind="variable">
      <type>long</type>
      <name>sec</name>
      <anchorfile>structru_time_val.html</anchorfile>
      <anchor>ae730cf043720ac0a7b94c64e7356d9ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>long</type>
      <name>usec</name>
      <anchorfile>structru_time_val.html</anchorfile>
      <anchor>a63e59ad14d61e70117ccdf2b9cf4d577</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>misc</name>
    <title>Miscellaneous Functions</title>
    <filename>group__misc.html</filename>
    <class kind="struct">ruTimeVal</class>
    <member kind="define">
      <type>#define</type>
      <name>ruRetWithCode</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga0bd4973612c1de9454caa98b7792fb8e</anchor>
      <arglist>(ptr, code, res)</arglist>
    </member>
    <member kind="typedef">
      <type>size_t</type>
      <name>rusize</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gaaa0102d444a5dd14474b7951caae0459</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>const RUAPI char *</type>
      <name>ruVersion</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga04667affdd77323501331d4d04737a24</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const RUAPI char *</type>
      <name>ruGetOs</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga8d136c4e95597fe6e31a1e074330e520</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruGetHostname</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga62baf9c58b3a0d19676181b43a052a90</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const RUAPI char *</type>
      <name>ruGetenv</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gab2d871fd2739c23e069b20057b198248</anchor>
      <arglist>(const char *variable)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruGetTimeVal</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga23dfc8d19e01fe0aba08aa87b2966989</anchor>
      <arglist>(ruTimeVal *result)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruGetLanguage</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga4c2f6c1d4bc86515b05798a6a9ec6f1b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruUsleep</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga9185a0fbb023f946e2b55424d14e7143</anchor>
      <arglist>(unsigned long microseconds)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI unsigned long</type>
      <name>ruSemiRandomNumber</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gaf71c9663b075c4dbe4f665b51cde059b</anchor>
      <arglist>(unsigned long max, long offset)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>threading</name>
    <title>Threading Related</title>
    <filename>group__threading.html</filename>
    <member kind="typedef">
      <type>void *</type>
      <name>ruMutex</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gaa2f2224294aab80c5b5fdadf8c50a2de</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruMutex</type>
      <name>ruMutexInit</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gaab6a38e671e130ff0915670c1d6f8019</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruMutexLock</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga1ba4661b6f200a9a01182c062b5fc463</anchor>
      <arglist>(ruMutex m)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruMutexUnlock</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga29343db7caf7ceb43470dff97cf2668b</anchor>
      <arglist>(ruMutex m)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruMutex</type>
      <name>ruMutexFree</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gafa04ae8066591ed11d6494f80527cfa2</anchor>
      <arglist>(ruMutex m)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>memory</name>
    <title>Memory Management</title>
    <filename>group__memory.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ruFree</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>gafcc3628238450c77b4aa61e9d7001a56</anchor>
      <arglist>(p)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruMalloc0</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>ga7c611f586df1bdfe2b096966c189f853</anchor>
      <arglist>(count, ctype)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruRealloc</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>ga2b0db047653765afa9f51710f491b734</anchor>
      <arglist>(buf, count, ctype)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void *</type>
      <name>ruMallocSize</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>gadc6d4640d79d30847e5a69423151e6d6</anchor>
      <arglist>(rusize count, rusize ofsize)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void *</type>
      <name>ruReallocSize</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>ga20d255456f8dc1cdb107e90ee8b84c73</anchor>
      <arglist>(void *buf, rusize count, rusize ofsize)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>cleaner</name>
    <title>Data Cleaning Utility</title>
    <filename>group__cleaner.html</filename>
    <member kind="typedef">
      <type>void *</type>
      <name>ruCleaner</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>ga2ef14c6aa2f1434ccc78e1df73d5a31d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>rusize_s(*</type>
      <name>ioFunc</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gae4bb48307fae422df9dfa3bbbab66cc1</anchor>
      <arglist>)(void *ctx, void *buf, rusize len)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>ruCleanerCb</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>ga76ef45a462467ab9cc874e52cfd50cad</anchor>
      <arglist>)(void *user_data, const char *key, const char *subst)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruCleaner</type>
      <name>ruCleanNew</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gae1f001a43bf9b80bbebc76855815fc68</anchor>
      <arglist>(rusize chunkSize)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruCleaner</type>
      <name>ruCleanFree</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gab618d626b1824ba7fd21ac61c5132902</anchor>
      <arglist>(ruCleaner rc)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanDump</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gaac05659ca6cad93ae186f7c5910a21f8</anchor>
      <arglist>(ruCleaner cp, ruCleanerCb lf, void *user_data)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanAdd</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>ga2c747bc6e3a5f21c4c638c1be3157c0a</anchor>
      <arglist>(ruCleaner rc, const char *instr, const char *substitute)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanRemove</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>ga4bb547fcaf2a8b792282ec675925a55a</anchor>
      <arglist>(ruCleaner rc, const char *instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanIo</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>ga70ae896895986b6521043071ca46026e</anchor>
      <arglist>(ruCleaner rc, ioFunc reader, void *readCtx, ioFunc writer, void *writeCtx)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanNow</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gac95b23302e469eaebd780fd4238038cb</anchor>
      <arglist>(ruCleaner rc)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ruerror</name>
    <title>regify-util Error Codes</title>
    <filename>group__ruerror.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>RUE_OK</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>ga7e2f97cce8af8020834e685a2482556d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_FILE_NOT_FOUND</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gadb4058c56ea545e1d20d2efa9d6611fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_FILE_EXISTS</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>ga0fdc939abefe81ccbdb0496d232c5064</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_CANT_OPEN_FILE</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gae9ed1060b946292df9590533fea757f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_CANT_WRITE</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gaa39bfd48b57d338e83cc9f1140f77874</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_PARAMETER_NOT_SET</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>ga616dba9a5dafed8351aa98c68fab25e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_INVALID_PARAMETER</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>ga0860dfd6b6c4771bbdeb5c070c35d7d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_OUT_OF_MEMORY</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gae1cbda5746960b9745a36874eaa9bb45</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_GENERAL</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>ga2f7444b837379ce89b69a94db10781d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_INVALID_STATE</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gae2f0ae9ffefaed4e5e72dc15b7bc4699</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>io</name>
    <title>File I/O</title>
    <filename>group__io.html</filename>
    <member kind="typedef">
      <type>struct stat</type>
      <name>ruStat_t</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gae389f0b8ef30d38c2c5ee1be60330ede</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>entryMgr</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga02d14b0d2f110532e98796373b5c6b1a</anchor>
      <arglist>)(const char *fullPath, bool isFolder, void *ctx)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruFileExists</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga920bc0bfa5bb35a204bdaf8172ba228e</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruIsFile</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gae7723e5ef579dbfd3d862286365d41f1</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruIsDir</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga1d8e3aea6c159a286cb6c410b32893ca</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruIsExecutable</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga8de64b54e1e68f46b1a79e5c264138ba</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruIsSymlink</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gab7628a26fded54a4e19f5a5043d46e16</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruStat</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gaaf51c8fc40cedc893fa5a5397e6882af</anchor>
      <arglist>(const char *filepath, ruStat_t *dest)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruOpen</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gae5dc81a0de9eafb956c253be9e66948e</anchor>
      <arglist>(const char *filepath, int flags, int mode, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI FILE *</type>
      <name>ruFOpen</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga3716b147f53dd26c805547556b9608be</anchor>
      <arglist>(const char *filepath, const char *mode, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruOpenTmp</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gacf30a0eee9096e022b112167988a4f06</anchor>
      <arglist>(char *pathTemplate, int flags, int mode, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFileSetContents</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga9610819e2f2bc69b9215c9263a3db93b</anchor>
      <arglist>(const char *filename, const char *contents, rusize_s length)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruFileGetContents</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga62d813403b374ccf7de8f72a957914e1</anchor>
      <arglist>(const char *filename, char **contents, rusize *length)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruFileRename</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga8f8cf9398240f654b7168706eb8a1095</anchor>
      <arglist>(const char *oldName, const char *newName)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruFileRemove</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga037f61b6aa2a41e5966e75799c3b7626</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFolderWalk</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga0e6c319fd0cd0f9cab8e1371209089f7</anchor>
      <arglist>(const char *folder, entryMgr actor, void *ctx)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruFolderRemove</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gac255625488ddf62907efa965ea5b54e2</anchor>
      <arglist>(const char *folder)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruMkdir</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gad899c45123b9d7a04f4b8475ac9bf78f</anchor>
      <arglist>(const char *pathname, int mode, bool deep)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruDirName</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga11de9515b4c216de73d8481b7decc5ba</anchor>
      <arglist>(const char *filePath)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruBaseName</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga5e1c319186a031bc8ac39f171da4ebc1</anchor>
      <arglist>(const char *filePath)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruFullPath</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gac911387bc1382fb6bffa4a944d983e94</anchor>
      <arglist>(const char *filePath)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>kvstore</name>
    <title>Key Value Storage</title>
    <filename>group__kvstore.html</filename>
    <class kind="struct">KvStore_</class>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>kvset</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>gab748563c391cb2554af5f832b20fac6e</anchor>
      <arglist>)(struct KvStore_ *kvs, const char *key, const char *val, rusize_s len)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>kvget</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga446330a04500577d5331d33472f19d32</anchor>
      <arglist>)(struct KvStore_ *kvs, const char *key, char **val, rusize *len)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>kvlist</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga6ca4b7cac97b25236e64580ed0adfe7d</anchor>
      <arglist>)(struct KvStore_ *kvs, const char *key, ruList *list)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>kvctxfree</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga7d17c84bea150afde0b11a74635a071e</anchor>
      <arglist>)(void *)</arglist>
    </member>
    <member kind="typedef">
      <type>struct KvStore_</type>
      <name>KvStore</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga9abc942c577335834b99c80262ed4c84</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruFileStore</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga1315d700b19c1627f8d71b0e7fb5f9a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruNullStore</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga125fcd1fe6efdcfeaee1c6464e814224</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI KvStore *</type>
      <name>ruNewStore</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>gae49e7687dcafb2b6387042917dab6cff</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruFreeStore</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga7fe8e5566d73bb19a7de73edfa043f4a</anchor>
      <arglist>(void *obj)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruValidStore</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>gaa59d479c0140bcb5986b507349e470f5</anchor>
      <arglist>(void *obj)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI KvStore *</type>
      <name>ruNewFileStore</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga6ca53476182ed415e0b2093318c57b10</anchor>
      <arglist>(const char *folderPath, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFileStoreSet</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>gafb83ceebcad2dbc68d27fc954e4d916f</anchor>
      <arglist>(KvStore *kvs, const char *key, const char *val, rusize_s len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFileStoreGet</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>gaa08a41b66983aedb1ba42fc7f7402197</anchor>
      <arglist>(KvStore *kvs, const char *key, char **val, rusize *len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFileStoreList</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga45c4a5338130991a3b589f6eb95926ed</anchor>
      <arglist>(KvStore *kvs, const char *key, ruList *result)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI KvStore *</type>
      <name>ruNewNullStore</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga830b088c3c6574a0caa24f6f511cf188</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruNullStoreSet</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>gafbbd2467b40399f8cfeae9ea6f35e7f1</anchor>
      <arglist>(KvStore *kvs, const char *key, const char *val, rusize_s len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruNullStoreGet</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>gaf2726dd2b47a21c01328ba9aec9d2298</anchor>
      <arglist>(KvStore *kvs, const char *key, char **val, rusize *len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruNullStoreList</name>
      <anchorfile>group__kvstore.html</anchorfile>
      <anchor>ga7a9a050d8ed3e0be993cbb6a913e39a6</anchor>
      <arglist>(KvStore *kvs, const char *key, ruList *list)</arglist>
    </member>
    <docanchor file="group__kvstore.html" title="kvstore key">kvkey</docanchor>
  </compound>
  <compound kind="group">
    <name>list</name>
    <title>List Collection</title>
    <filename>group__list.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ruListAppend</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga67286d2c5522bdb4428436c561f122e5</anchor>
      <arglist>(rl, data)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruListPush</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gacb4e82b923c400fc91cd8f40b4844b79</anchor>
      <arglist>(rl, data)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruListPop</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga77c6f1f771fb7f2183a3477b25aac5d1</anchor>
      <arglist>(rl, code)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruListIter</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gaa62b97357e6bb871a09fed6cd392e628</anchor>
      <arglist>(list)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruListNext</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga86fab59859faf2b181db8f975eb97efc</anchor>
      <arglist>(re, type, code)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruIterNext</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gacb34c9de3d00243458bc91f9a76e2344</anchor>
      <arglist>(re, type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruListData</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga4d30ec3e3d2ea85b7f46a0f6d6d791e9</anchor>
      <arglist>(re, type, code)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruIterCurrent</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga894fca3f8274753560dc915d6347b8b4</anchor>
      <arglist>(re, type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruListIdxData</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga019dbdda890922f4b19e68750b845bed</anchor>
      <arglist>(rl, index, type, code)</arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruList</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gab25c1662a91a5e0d68a3f04efd1a0cc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruListElmt</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga8a10b640cc881a4205684a5730c949a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruIterator</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga88f5af8c2f8986a0708a06c1d0bb2183</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruList</type>
      <name>ruListNew</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gaaa6b03bd2b243385bdd7326ebf4d9be4</anchor>
      <arglist>(void(*destructor)(void *data))</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruListFree</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga0175927761511efe289173b07203ab9b</anchor>
      <arglist>(ruList rl)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruListAppendPtr</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga12f894b8ac166c434ba7a12f84353102</anchor>
      <arglist>(ruList rl, const void *data)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruListInsertAfter</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga1e34ce50419806d906cad8a8236362bc</anchor>
      <arglist>(ruList rl, ruListElmt rle, const void *data)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void *</type>
      <name>ruListRemoveAfter</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gac5dd2279c7701d6abb475ef0b75dd4c1</anchor>
      <arglist>(ruList rl, ruListElmt rle, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruListSize</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gaacc40c1777c3ff2ddc3c16d0ef6fe223</anchor>
      <arglist>(ruList rl, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruListElmt</type>
      <name>ruListHead</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga515d2cfd15a9d647800254d4c78f4dad</anchor>
      <arglist>(ruList rl, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruListElmt</type>
      <name>ruListTail</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gaa97a983c396666bdb305c2047abd70a7</anchor>
      <arglist>(ruList rl, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruListElmtIsHead</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga92dbcac157bd0ffd1a696d9b46ad42d1</anchor>
      <arglist>(ruList rl, ruListElmt re, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruListElmtIsTail</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga45b8b301a3af9ef2f51ece8df37b099f</anchor>
      <arglist>(ruListElmt re, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruListElmt</type>
      <name>ruListNextElmt</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga662f949b26672d0793e8b78dc44b568b</anchor>
      <arglist>(ruListElmt re, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void *</type>
      <name>ruListNextData</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gaafa5dcdbd257934d86eb05708f3345ef</anchor>
      <arglist>(ruListElmt *re, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void *</type>
      <name>ruListElmtData</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga684224f1d65319c3e8a011afc98eb452</anchor>
      <arglist>(ruListElmt re, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void *</type>
      <name>ruListIdxElmtData</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gab5bc226942ac6a563040f45227664e43</anchor>
      <arglist>(ruList rl, int32_t index, int32_t *code)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>logging</name>
    <title>Logging Facility</title>
    <filename>group__logging.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>RU_LOG_NONE</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga4ab0a01a9d5b8c0fe1228e22a4a98b28</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_LOG_CRIT</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga5ce14b97da761ebb505b022d7b777600</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_LOG_WARN</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga4388ae060e29101e3458cc625b53f327</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_LOG_INFO</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>gac3860f00aa62056d7ca6cb40fd194bf6</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_LOG_VERB</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>gaa48441dca350811c725a8096ccbd59f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruCritLog</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga52e07e352dbd0c7be2b0d37bc48cc8ac</anchor>
      <arglist>(msg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruCritLogf</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga6cee3b7459f97c8805d9e31ec46ca2c5</anchor>
      <arglist>(fmt,...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruWarnLog</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga21164d698d4775724fd710a6bf6cfa15</anchor>
      <arglist>(msg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruWarnLogf</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>gaebf1e2035fa3c2d930c925c93e2e6706</anchor>
      <arglist>(fmt,...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruInfoLog</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>gaf9ca44fd51225d8a837745dda302341f</anchor>
      <arglist>(msg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruInfoLogf</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga4bf3472aaaf37ec918433cb983102338</anchor>
      <arglist>(fmt,...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruVerbLog</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga497ce15cdccb100b54759a94220e6d7f</anchor>
      <arglist>(msg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruVerbLogf</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga75167ec8202e6e2a632afdb0a60fabb0</anchor>
      <arglist>(fmt,...)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>ruLogFunc</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga85b04f67f72140eacdded6c016bdac96</anchor>
      <arglist>)(void *user_data, const char *message)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruStdErrorLogger</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga0a90f3d97148451be043efff143fd5a7</anchor>
      <arglist>(void *udata, const char *msg)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruSetLogger</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>gae2da372352496e9d84b59c07b15f82ed</anchor>
      <arglist>(ruLogFunc logger, u_int32_t logLevel, void *userData)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI u_int32_t</type>
      <name>ruGetLogLevel</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga195ad93c9316d9f4e9a2062b60302cea</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruDoesLog</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>gab3ef0712708ff05243d4275723c1cba4</anchor>
      <arglist>(u_int32_t log_level)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>map</name>
    <title>Hash Map</title>
    <filename>group__map.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ruMapGet</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga864d402bda9961fc1b07a0b99e44197c</anchor>
      <arglist>(rm, key, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruMapNext</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gae132393338c2bb004fe49b31e64570ff</anchor>
      <arglist>(rm, key, value)</arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruMap</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gaad879dbca17e7740cbab5651cafe677d</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruMap</type>
      <name>ruMapNew</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gae6d0414b8f4c72f7d6eb45d0e52b51e5</anchor>
      <arglist>(u_int32_t(*hash)(const void *key), bool(*match)(const void *key1, const void *key2), void(*keyFree)(void *key), void(*valFree)(void *val), u_int32_t expectedSize)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI u_int32_t</type>
      <name>ruStrHash</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga4aa3c0219c76fbee26d1ca44514dd6d1</anchor>
      <arglist>(const void *key)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrMatch</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga3a9ac52b3a1b5086b42dc736bb6978a1</anchor>
      <arglist>(const void *s1, const void *s2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruMap</type>
      <name>ruMapNewString</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga8a937c4eb4c5e982dcc53a8ea77b8ffc</anchor>
      <arglist>(void(*keyFree)(void *key), void(*valFree)(void *val))</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruMapFree</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gabd78b2a8f568024c5808c7c1529d6c55</anchor>
      <arglist>(ruMap rm)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapPut</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga318750f573bdf4e1a69faecacb234f55</anchor>
      <arglist>(ruMap map, void *key, void *val)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapRemove</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga352a74aa67f4690fe3854e9dbbbb8f76</anchor>
      <arglist>(ruMap rm, void *key, void **val)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapRemoveAll</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gaab1b6e1eeca5ef6eaa37ca3d846522b9</anchor>
      <arglist>(ruMap rm)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapGetValue</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga0a286928984aedf4411aa97323d71f39</anchor>
      <arglist>(ruMap rm, void *key, void **val)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruMapHas</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga359981dee75566ae4ad8c38f88b6b26e</anchor>
      <arglist>(ruMap rm, void *key, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapIterInit</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga41c0dbda38f8231292cb349255cb39f3</anchor>
      <arglist>(ruMap rm)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapNextSet</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga1873d8fcd5947497ae6b18f6f0ad6371</anchor>
      <arglist>(ruMap rm, void **key, void **value)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI u_int32_t</type>
      <name>ruMapSize</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gae7f245035a146ef206b48fc33772e609</anchor>
      <arglist>(ruMap rm, int32_t *code)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>regex</name>
    <title>Regular Expressions</title>
    <filename>group__regex.html</filename>
    <member kind="typedef">
      <type>void *</type>
      <name>ruRegex</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga5c28c3ff50463237640750e9f18b1e56</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruRegexFree</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gac66396b88ee7d14069d55411000c4dc7</anchor>
      <arglist>(ruRegex rr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruRegex</type>
      <name>ruRegexNew</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga397ff4a9c7f1948ed6ae4114392dedbb</anchor>
      <arglist>(const char *pattern, ruRegexpFlag flags, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruRegexReplace</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga9ad8451ec30e8fc57db784f827489327</anchor>
      <arglist>(ruRegex rr, const char *original, const char *replacement, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruRegexMatch</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga42a94fd1130e550b7af978c45b88f617</anchor>
      <arglist>(ruRegex rr, const char *original, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruRegexFind</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gab8df1e1ae93fb29b72e9c366124032c3</anchor>
      <arglist>(ruRegex rr, const char *original, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruList</type>
      <name>ruRegexMatchGroups</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gac1a8be0de8243bda230b1f4ed1159e26</anchor>
      <arglist>(ruRegex rr, const char *original, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruList</type>
      <name>ruRegexFindGroups</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga4890454d0d4fb06a6f32bd4b13e33236</anchor>
      <arglist>(ruRegex rr, const char *original, int32_t *code)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>string</name>
    <title>String Functions</title>
    <filename>group__string.html</filename>
    <subgroup>ruString</subgroup>
    <member kind="define">
      <type>#define</type>
      <name>ruUtf8Nfc</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga2f2962a67397abade01d08a2db66ee4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruUtf8Nfd</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gaec4a7305ff2149489adba515b70480db</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruUtf8Lower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga3db4b4277d45b0557ad66500ea3faa38</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruUtf8Upper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gac57f5894f870fd6f7380305811ec773d</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruUtf8CaseNormalize</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga5618cc6aab057d1af3254e05a5127ace</anchor>
      <arglist>(const char *instr, int32_t normMode, int32_t caseMode)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStrcmp</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga6bfc96ae55637fdb4dcc9364f630f8fe</anchor>
      <arglist>(const char *str1, const char *str2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStrcasecmp</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gaaad20d826d045c61abb5e708423c5ea3</anchor>
      <arglist>(const char *str1, const char *str2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrStartsWith</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga9355e81bbc66654d086d59bdb65cd604</anchor>
      <arglist>(const char *str, const char *prefix, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrEndsWith</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gafecbe84249235a804db2aab4ec936b63</anchor>
      <arglist>(const char *str, const char *suffix, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrEndsCaseWith</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga558c46b3f4e6e44dcbbe1cd47fbb3c29</anchor>
      <arglist>(const char *str, const char *suffix, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruStrStrLen</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gad5274e886bf38df0ac0675f0753efa6c</anchor>
      <arglist>(const char *haystack, const char *needle, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruStrStr</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga4d37273205efbebfc894057e2a54839f</anchor>
      <arglist>(const char *haystack, const char *needle)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruLastSubstrLen</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gab3d07bb4cd0e514534a479d637fe9375</anchor>
      <arglist>(const char *haystack, const char *needle, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruLastSubstr</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gabf84afa80858cb26fb5fd0648185e4e8</anchor>
      <arglist>(const char *haystack, const char *needle)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruList</type>
      <name>ruStrsplit</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga837c4d29739e4baf89442f4e9fd6ca58</anchor>
      <arglist>(const char *instr, const char *delim, int32_t maxCnt)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char</type>
      <name>ruAsciiCharToLower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gae86eb432a281131ad5f1cb4eac025396</anchor>
      <arglist>(char in)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruAsciiToLower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga4662fca69acac8f44ed704ef0a25dfb4</anchor>
      <arglist>(char *instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruAsciiNToLower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gadcff737cc3aa383ca46c44a5cd745b96</anchor>
      <arglist>(char *instr, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char</type>
      <name>ruAsciiCharToUpper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga2185c4486748b4f5c18afb1688fe06cf</anchor>
      <arglist>(char in)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruAsciiToUpper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga1134e95af367533005a4b0cee4918735</anchor>
      <arglist>(char *instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruAsciiNToUpper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gadfb863e1aa1bd1c8a7dd072e75aa3d94</anchor>
      <arglist>(char *instr, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruUtf8ToLower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga70c3b427727d7949f148be67fa7225b8</anchor>
      <arglist>(const char *instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruUtf8ToUpper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga736095869cdda732f5704ce657ee85ce</anchor>
      <arglist>(const char *instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruStrdup</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gaad01659abccce6ab4d9f9defced8cb0b</anchor>
      <arglist>(const char *str)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruStrndup</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gafe5a69e0a362bf5019628738c462c059</anchor>
      <arglist>(const char *str, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruDupPrintf</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gac02a41b8d206a2e7518420e8647c28b7</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int64_t</type>
      <name>ruStrToll</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gab1bd1770a3b7d2485c0cb5d31d995ea9</anchor>
      <arglist>(const char *instr, char **endptr, int base)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruStripChars</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga16c4d5ce379e0ed2ac5f70dd43215a53</anchor>
      <arglist>(char *instr, const char *chars)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ruString</name>
    <title>String Object</title>
    <filename>group__ru_string.html</filename>
    <member kind="typedef">
      <type>void *</type>
      <name>ruString</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga673263d6113c0dada89347f2b31b5884</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruString</type>
      <name>ruStringNew</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>gae4a6f57b247f69721a3f8d5a2b7cab33</anchor>
      <arglist>(const char *instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruString</type>
      <name>ruStringNewn</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga71f72b16353217d759ffd92ef17d7916</anchor>
      <arglist>(const char *instr, rusize size)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruString</type>
      <name>ruStringNewf</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>gad71a92239d506e7f33218175f20b859f</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruStringFree</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga80a284cf5abd412068766c166b232450</anchor>
      <arglist>(ruString rs, bool keepBuffer)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStringAppend</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga00bd7d3c6100907423124fc90f13a19a</anchor>
      <arglist>(ruString rs, const char *str)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStringAppendUriEncoded</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga44d833c3518fb1d781eb10a3ee026320</anchor>
      <arglist>(ruString rs, const char *instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStringAppendn</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>gae8aed675b983449c62a3e3dece07988e</anchor>
      <arglist>(ruString rs, const char *instr, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStringAppendf</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga9176c3990569bcfdcbd7e5e0ccbd47bb</anchor>
      <arglist>(ruString rs, const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruStringGetCString</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga5a7397cd16cf53b6290a2072da091e81</anchor>
      <arglist>(ruString rs)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI rusize</type>
      <name>ruStringLen</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga6af930574e9d67811ac0851a13a3c6f6</anchor>
      <arglist>(ruString rs, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStringEndsWith</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga47ae4c3012bd28c7700db37810ff238c</anchor>
      <arglist>(ruString rs, const char *suffix, int32_t *code)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>buffer</name>
    <title>Buffer Functions</title>
    <filename>group__buffer.html</filename>
    <subgroup>ruBuffer</subgroup>
  </compound>
  <compound kind="group">
    <name>ruBuffer</name>
    <title>Buffer Object</title>
    <filename>group__ru_buffer.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferNew</name>
      <anchorfile>group__ru_buffer.html</anchorfile>
      <anchor>gad7c452e2590a64b22d213d1b452b95d9</anchor>
      <arglist>(initialSize)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferAppend</name>
      <anchorfile>group__ru_buffer.html</anchorfile>
      <anchor>gabd7d3693b52b676494d59aaf4057fbed</anchor>
      <arglist>(rb, buf, len)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferGetData</name>
      <anchorfile>group__ru_buffer.html</anchorfile>
      <anchor>gad876921a72ac668e23e841ce57272ef9</anchor>
      <arglist>(rb)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferLen</name>
      <anchorfile>group__ru_buffer.html</anchorfile>
      <anchor>ga29e00c5f225545b0e7d8ad218ef185fe</anchor>
      <arglist>(rb, code)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferFree</name>
      <anchorfile>group__ru_buffer.html</anchorfile>
      <anchor>ga5ce21c10a6e895c5afb738aeb0881e86</anchor>
      <arglist>(rb, keep)</arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruBuffer</name>
      <anchorfile>group__ru_buffer.html</anchorfile>
      <anchor>ga1e5e2b41241f550622126cbecb4820e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruBufferAppendUriEncoded</name>
      <anchorfile>group__ru_buffer.html</anchorfile>
      <anchor>ga768c2165496db6fd441c4baf4d36608e</anchor>
      <arglist>(ruString rs, const char *buf, rusize len)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index</filename>
  </compound>
</tagfile>
