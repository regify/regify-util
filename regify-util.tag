<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.1">
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
    <name>ruFamEvent_</name>
    <filename>structru_fam_event__.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>eventType</name>
      <anchorfile>structru_fam_event__.html</anchorfile>
      <anchor>aa73ed1819d01004c05b8b1908ef91ef4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>perm_chars</type>
      <name>srcPath</name>
      <anchorfile>structru_fam_event__.html</anchorfile>
      <anchor>abb1a6c225865ea8d47fda5952978c8f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>perm_chars</type>
      <name>dstPath</name>
      <anchorfile>structru_fam_event__.html</anchorfile>
      <anchor>a99c8743dc4a60b79ab3c548496d20f09</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>ruTimeVal</name>
    <filename>structru_time_val.html</filename>
    <member kind="variable">
      <type>sec_t</type>
      <name>sec</name>
      <anchorfile>structru_time_val.html</anchorfile>
      <anchor>ab79c03c47db0065ef4c2a2d00f33c94f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>usec_t</type>
      <name>usec</name>
      <anchorfile>structru_time_val.html</anchorfile>
      <anchor>aebdd1a6e510874064a7f3a3a6ff935bd</anchor>
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
      <name>RU_SIZE_AUTO</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga0282856ae8c37b20f28093d6cdfdd8b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_NON_BLOCK</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga934df7587a942f224560208b555e8826</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_NO_TIMEOUT</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga3a6bd0d17663435e5e96679ffe03f900</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruSleep</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gacd1b05c76a2e6c682e67f216c34c7241</anchor>
      <arglist>(secs)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruRetWithCode</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga0bd4973612c1de9454caa98b7792fb8e</anchor>
      <arglist>(ptr, code, res)</arglist>
    </member>
    <member kind="typedef">
      <type>signed long</type>
      <name>rusize_s</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gac23a4c7337daa8f22a561f49096b06b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>pid_t</type>
      <name>ru_pid</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga623467b9d56a745927eb36236c8ebdb2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const char *</type>
      <name>perm_chars</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga7dd744dc0ee350f7de2c38f51f5c0bbe</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const char *</type>
      <name>trans_chars</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga5a4bcdee6a3cc6c7ed75da5bb77d49bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>char *</type>
      <name>alloc_chars</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga972d4f4f858964bb2381d186ea96d88d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const uint16_t *</type>
      <name>perm_uni</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga85d684615b50943fd01e79d07bb1c6d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const uint16_t *</type>
      <name>trans_uni</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga583a16c595210574018e6bd10fe925b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>uint16_t *</type>
      <name>alloc_uni</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga3b8ffe65acbdde945eb6668ac1226f5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const unsigned char *</type>
      <name>perm_bytes</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gab07884b0d918059732da53117d12b8f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const unsigned char *</type>
      <name>trans_bytes</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gab5445910fbc90fb0a6db407f8f608422</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>unsigned char *</type>
      <name>alloc_bytes</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga282bddf4c8f74d03e5d1203a240cfa4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const void *</type>
      <name>perm_ptr</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga517aac830d9ce5c6ab7deec46adf54d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const void *</type>
      <name>trans_ptr</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga147d80ce3325dd56fbfb0e07cedcf757</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>alloc_ptr</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga9f2753cc325fa665aaf7925682462ecb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ptr</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gab4cf5759c3753c2308f163953ee154f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>long</type>
      <name>sec_t</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga8573cc8637a470b127a19d54b0e94cb9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int64_t</type>
      <name>msec_t</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gae416893e8160c5b7c6da3765e4707d5d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int64_t</type>
      <name>usec_t</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga02db3ac43a5917535ca3cbc37ac72884</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>long</type>
      <name>ru_int</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga9304b6c6b14b62616d983b36f15a7210</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>unsigned long</type>
      <name>ru_uint</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga6bccfa079bb265c356aca7fb702dd71b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>size_t</type>
      <name>rusize</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gaaa0102d444a5dd14474b7951caae0459</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *(*</type>
      <name>ruCloneFunc</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga1b23e084ffff15cf965a438497c9f8e2</anchor>
      <arglist>)(void *)</arglist>
    </member>
    <member kind="typedef">
      <type>void *(*</type>
      <name>ruClearFunc</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga91dce9b2f482277a0d5ec778e54c5d14</anchor>
      <arglist>)(void *)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>ruFreeFunc</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga8eb4179750e158d78b0b3bc1e4b1bdce</anchor>
      <arglist>)(void *)</arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>ruCompFunc</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga6f2df974d7fa3462ee789585fcd510ed</anchor>
      <arglist>)(trans_ptr a, trans_ptr b)</arglist>
    </member>
    <member kind="typedef">
      <type>unsigned char</type>
      <name>uchar</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga65f85814a8290f9797005d3b28e7e5fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI const char *</type>
      <name>ruVersion</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga71dec8ccc9b4947b4142514696e78795</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI const char *</type>
      <name>ruLastError</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gaea3d529fecc1bbad25f943943b6e9ef2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI perm_chars</type>
      <name>ruGetOs</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga9514522bf5289236d07a8b714ab8e0b0</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruGetHostname</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gab28def77befe23dc2785aff9b1623105</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI trans_chars</type>
      <name>ruGetenv</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga86dd88de865e954dc1d0ea82fd20f21c</anchor>
      <arglist>(const char *variable)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruIsInt64</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga61f7ab20b2760c5cb849d3b442eb4402</anchor>
      <arglist>(const char *numstr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ru_pid</type>
      <name>ruProcessId</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gafb168a8376876c132f91f3568110a676</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruRunProg</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gaa1ee1acfc9e71c9beba03b4b547f8abc</anchor>
      <arglist>(const char **argv, sec_t timeout)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruGetLanguage</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga2c70eecb13cf89033737e26a9ff08817</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI unsigned long</type>
      <name>ruSemiRandomNumber</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gaf71c9663b075c4dbe4f665b51cde059b</anchor>
      <arglist>(unsigned long max, long offset)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruVersionComp</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gad5f20c00063066599020981359d2eba6</anchor>
      <arglist>(trans_chars ver1, trans_chars ver2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruGetTimeVal</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga23dfc8d19e01fe0aba08aa87b2966989</anchor>
      <arglist>(ruTimeVal *result)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI usec_t</type>
      <name>ruTimeUs</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gae4c0911efb3cf5eeb0f67330f1c074de</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI msec_t</type>
      <name>ruTimeMs</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga035a3e39ed5699fc6eede1d3304d3e9c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI sec_t</type>
      <name>ruTimeSec</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga5a69dfb50754239f299d2267ed06151d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruTimeUsEllapsed</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gac70a35ed3009974a77b3724429c294c4</anchor>
      <arglist>(usec_t stamp)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruTimeMsEllapsed</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga7f318e6671a7da7811c9f15ef4de2bdd</anchor>
      <arglist>(msec_t stamp)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruTimeEllapsed</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gac6492a980d295e60e0f6946968176613</anchor>
      <arglist>(sec_t stamp)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruSleepUs</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga40a0d0aefa3051c7808feae2809b2741</anchor>
      <arglist>(usec_t microseconds)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruSleepMs</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gacb5828ac39a00aa84781c841afb64623</anchor>
      <arglist>(msec_t milliseconds)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI sec_t</type>
      <name>ruTimeParse</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga5d78052ada3c62b7a77e773ae53158c1</anchor>
      <arglist>(trans_chars dateformat, trans_chars datestr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI sec_t</type>
      <name>ruUtcParse</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gabad41e4d218b3038e4e6df69b4f85926</anchor>
      <arglist>(trans_chars dateformat, trans_chars datestr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruTimeFormat</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga56f8f6fec10fa579342976d111813f62</anchor>
      <arglist>(trans_chars format, rusize len, alloc_chars timeStr, sec_t timesecs)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruUtcFormat</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>gaecf4a0b06c4d0268fdd64c378d9facc1</anchor>
      <arglist>(trans_chars format, rusize len, alloc_chars timeStr, sec_t timesecs)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI sec_t</type>
      <name>ruTimeLocalToUtc</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga671f13d90510aaf61e87b680d999e491</anchor>
      <arglist>(sec_t stamp)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI sec_t</type>
      <name>ruTimeUtcToLocal</name>
      <anchorfile>group__misc.html</anchorfile>
      <anchor>ga3e7596f5512ddf7baa26a049a8ada132</anchor>
      <arglist>(sec_t stamp)</arglist>
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
      <name>ruReplace</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>gafdbf421329129ad39395ca1c48694f9f</anchor>
      <arglist>(p, n)</arglist>
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
      <type>RUAPI alloc_ptr</type>
      <name>ruMallocSize</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>ga9aa9dcd1bd646dfc857f63eba32e923c</anchor>
      <arglist>(rusize count, rusize ofsize)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_ptr</type>
      <name>ruReallocSize</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>ga94f0a9650d1a2b773e4e85f2341eea02</anchor>
      <arglist>(alloc_ptr buf, rusize count, rusize ofsize)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_ptr</type>
      <name>ruMemDup</name>
      <anchorfile>group__memory.html</anchorfile>
      <anchor>ga5b6817501d84107573e6c7dc15c06fd6</anchor>
      <arglist>(trans_ptr buf, rusize size)</arglist>
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
      <name>rcWriteFn</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gaff866bff30cab44b2f0a14c3e0c6fb2b</anchor>
      <arglist>)(perm_ptr ctx, trans_ptr buf, rusize len)</arglist>
    </member>
    <member kind="typedef">
      <type>rusize_s(*</type>
      <name>rcReadFn</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>ga3adc6576a1cb10dade26d7ff7cfc04e5</anchor>
      <arglist>)(perm_ptr ctx, ptr buf, rusize len)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>ruCleanerCb</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>ga13a8daf645da7138fbd06c9d7af8b732</anchor>
      <arglist>)(perm_ptr user_data, trans_chars key, trans_chars subst)</arglist>
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
      <anchor>gafae28f8ce03c8fa58af8f36a603e424e</anchor>
      <arglist>(ruCleaner cp, ruCleanerCb lf, perm_ptr user_data)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanAdd</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>ga10601de10e51a7ce8cb59044be5489fa</anchor>
      <arglist>(ruCleaner rc, trans_chars instr, trans_chars substitute)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanRemove</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gaa6b618e644812ee41282ae5fd5d449c0</anchor>
      <arglist>(ruCleaner rc, trans_chars instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanIo</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gae47e57317359df9bc5ccff1124ba31f0</anchor>
      <arglist>(ruCleaner rc, rcReadFn reader, perm_ptr readCtx, rcWriteFn writer, perm_ptr writeCtx)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanToWriter</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gaf308c52b6c3b3dbaec13f85ff6a81df1</anchor>
      <arglist>(ruCleaner rc, trans_chars in, rusize len, rcWriteFn writer, perm_ptr writeCtx)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruCleanToString</name>
      <anchorfile>group__cleaner.html</anchorfile>
      <anchor>gac9cd9445123baff979282056b18430fe</anchor>
      <arglist>(ruCleaner rc, trans_chars in, rusize len, ruString *out)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ruerror</name>
    <title>regify-util Error Codes</title>
    <filename>group__ruerror.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>RUE_FORK_FAILED</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>ga5571b7590304c5b53813def045533696</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_RUN_FAILED</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>ga0e8db464671617e7b055e90cfb898f78</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_TIMEOUT</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gae7f248eb7648a198f04a2ee4ebfa023b</anchor>
      <arglist></arglist>
    </member>
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
      <name>RUE_OVERFLOW</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gaaa0637cd2eaa0773f5d45c266cb744f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_FEATURE_NOT_SUPPORTED</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gaebe638fa43dc94bf0a20f2e2211e7322</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RUE_USER_ABORT</name>
      <anchorfile>group__ruerror.html</anchorfile>
      <anchor>gae20a9bc093af1bbbb5e42ef8ed05deec</anchor>
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
    <name>fam</name>
    <title>File Access Monitoring</title>
    <filename>group__fam.html</filename>
    <class kind="struct">ruFamEvent_</class>
    <member kind="define">
      <type>#define</type>
      <name>RU_FAM_CREATED</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gaeb006bd51ec0b1b48dde9a1118326d34</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_FAM_DELETED</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gaa1b8655438f02fa07923b686ea9f771d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_FAM_MODIFIED</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gabc64c4708760b9befa3223f8be0fac34</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_FAM_MOVED</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga6364d697be2b0fe1ae48d370dd91f06c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_FAM_ATTRIB</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gae775d59b18c99a740b8c9867dd922a20</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_FAM_KILL_TIMEOUT</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga1a5b02165889ade1f056edbaae41140f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_FAM_POLL_TIMEOUT</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gafef8e830bccb297b129c907c2b79ebb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_FAM_QUEUE_TIMEOUT</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gad684e723f3a7a9d8d359ea1fcf9c4005</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruFamEventLog</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga4a567d573035d0781f94089583c58c1f</anchor>
      <arglist>(lvl, fe, prefix)</arglist>
    </member>
    <member kind="typedef">
      <type>struct ruFamEvent_</type>
      <name>ruFamEvent</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gaa411004c8a592a6d7fbc750e65c055ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>ruFamHandler</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga93b95063a11aa259b3d687cd9a9a821e</anchor>
      <arglist>)(ruFamEvent *fe, perm_ptr ctx)</arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruFamCtx</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga41131e438be9a9c17155ef338a83d899</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruFamCtx</type>
      <name>ruFamMonitorFilePath</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gac2029a63025c44b72df8d44402b7c0ff</anchor>
      <arglist>(trans_chars filePath, trans_chars threadName, ruFamHandler eventCallBack, perm_ptr ctx)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruFamCtx</type>
      <name>ruFamKillMonitor</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga97d5fefcb5f68d497a48a8b3b3a09f26</anchor>
      <arglist>(ruFamCtx o)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruFamQuit</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga1b541b29be7c92345b151dca0623230e</anchor>
      <arglist>(ruFamCtx o)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI const char *</type>
      <name>ruFamEventTypeString</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gab3a4b072242d9faaeb8a8459b6392677</anchor>
      <arglist>(int32_t action)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruFamLogEventFunc</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga7608998aca3460e7f98b8014cb0eb162</anchor>
      <arglist>(uint32_t loglevel, trans_chars file, trans_chars func, int32_t line, ruFamEvent *fe, trans_chars prefix)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruFamEventString</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga2a2c124ab9905147782051eb9d65c14d</anchor>
      <arglist>(ruFamEvent *fe)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruFamEvent *</type>
      <name>ruFamEventNew</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga4f6feaed01cb324940b4956baafa8652</anchor>
      <arglist>(int eventType, trans_chars filePath, trans_chars destPath)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruFamEvent *</type>
      <name>ruFamEventFree</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>gafb1cfbeed368d7e0968b73b6c2b57430</anchor>
      <arglist>(ruFamEvent *fe)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruFamEventFreeV</name>
      <anchorfile>group__fam.html</anchorfile>
      <anchor>ga50ed123f5f20cdd662f5a4fddeaee32e</anchor>
      <arglist>(void *fe)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ini</name>
    <title>Ini File Handling</title>
    <filename>group__ini.html</filename>
    <member kind="typedef">
      <type>void *</type>
      <name>ruIni</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga67d0d212c119eddcdfb53125e8e27e19</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruIni</type>
      <name>ruIniFree</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga5806935fc091c15b8a2a398e573c4f65</anchor>
      <arglist>(ruIni ini)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruIni</type>
      <name>ruIniNew</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga311aef3d56820892091456418381534d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruIniWrite</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga443f36d34ca689b78ab37fa15e17874b</anchor>
      <arglist>(ruIni iniOb, trans_chars filename)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruIniRead</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga7f42bce2eff7de4e4184c0d0dae6c6d1</anchor>
      <arglist>(trans_chars filename, ruIni *iniOb)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruIniSections</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga5033e18d3a910e4600fd3f922b01e04a</anchor>
      <arglist>(ruIni iniOb, ruList *sections)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruIniKeys</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>gac3565e49cdc4d7fc9f397c1c14986d1c</anchor>
      <arglist>(ruIni iniOb, trans_chars section, ruList *keys)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI perm_chars</type>
      <name>ruIniGetDef</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga6e64a31917c5bd73257a5839cfda14e2</anchor>
      <arglist>(ruIni iniOb, trans_chars section, trans_chars key, trans_chars def, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruIniGet</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga3e12017d4a6672f5e3ca63c0504cfb20</anchor>
      <arglist>(ruIni iniOb, trans_chars section, trans_chars key, perm_chars *value)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruIniSet</name>
      <anchorfile>group__ini.html</anchorfile>
      <anchor>ga2b9057f8f137ab2f3839a697e84dd44f</anchor>
      <arglist>(ruIni iniOb, trans_chars section, trans_chars key, trans_chars value)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>io</name>
    <title>File I/O</title>
    <filename>group__io.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>RU_WALK_FOLDER_FIRST</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga9d311d1dbbf9b207dd8e6832178aa5ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_WALK_FOLDER_LAST</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga055a96a5883241e585332496a09cf671</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RU_WALK_NO_RECURSE</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga7f8970ea7304cf95817cac910c9070dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>struct stat</type>
      <name>ruStat_t</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gae389f0b8ef30d38c2c5ee1be60330ede</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>bool(*</type>
      <name>entryFilter</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga96b2df183649f58bd746edc4a0c6161c</anchor>
      <arglist>)(trans_chars folderPath, trans_chars fileName, bool isFolder, ptr ctx)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>entryMgr</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gae838891e096d47726baeec65b792fd51</anchor>
      <arglist>)(trans_chars fullPath, bool isFolder, ptr ctx)</arglist>
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
      <type>RUAPI int32_t</type>
      <name>ruDiskFree</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gaee9962bbf1900c33cf88330b0ca0da23</anchor>
      <arglist>(trans_chars path, uint64_t *total, uint64_t *avail)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStat</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga3c01cc11c4ab4e14c29a12fa5cb1bc7e</anchor>
      <arglist>(trans_chars filepath, ruStat_t *dest)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI rusize</type>
      <name>ruFileSize</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga8916e7542df6ccf0eedd28d8377d1625</anchor>
      <arglist>(trans_chars filePath, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI sec_t</type>
      <name>ruFileUtcTime</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga7e71901a25248028c5b9c186c6347035</anchor>
      <arglist>(trans_chars filePath, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFileSetUtcTime</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gaa4ef68118fe7d34985d605e2eb4d5b84</anchor>
      <arglist>(trans_chars filePath, sec_t date)</arglist>
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
      <anchor>ga2280aaaa8bb5c208cb53a696444c5ebd</anchor>
      <arglist>(const char *filename, const char *contents, rusize length)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruFileGetContents</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gac2d2628ae0710cfa181d65116b0786c6</anchor>
      <arglist>(trans_chars filename, alloc_chars *contents, rusize *length)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruFileCopy</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga56bef69d453747f997eac91cf5a92448</anchor>
      <arglist>(trans_chars srcpath, trans_chars destpath)</arglist>
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
      <name>ruFileTryRename</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga73b475d19298205315ec716522eb5005</anchor>
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
      <name>ruFilteredFolderWalk</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga12acc50821b94d002938710d27c03aa6</anchor>
      <arglist>(trans_chars folder, uint32_t flags, entryFilter filter, entryMgr actor, ptr ctx)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFolderWalk</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga1c03fc4af3efaa1db4c1ffccf3f321bf</anchor>
      <arglist>(trans_chars folder, uint32_t flags, entryMgr actor, ptr ctx)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ru_int</type>
      <name>ruFolderEntries</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga1c58cc0d6dd45ca69fbf9878327a0695</anchor>
      <arglist>(trans_chars folder)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruFolderRemove</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga3770186e0678760f4c6ab93751d6e4ad</anchor>
      <arglist>(trans_chars folder)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruMkdir</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga3602170134b8290324e6aca3a0b23650</anchor>
      <arglist>(trans_chars pathname, int mode, bool deep)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruDirName</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gabf71d082e151f87d4f31b819a293236b</anchor>
      <arglist>(trans_chars filePath)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI perm_chars</type>
      <name>ruBaseName</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gab0cd59807f557f48554a5a4b3a9dc78a</anchor>
      <arglist>(perm_chars filePath)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI perm_chars</type>
      <name>ruFileExtension</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gab0d2743898f8e94ddb0ff54c836862e3</anchor>
      <arglist>(perm_chars filePath)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruFullPath</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga89ef11f352fb52312b7f46ec11ec1103</anchor>
      <arglist>(trans_chars filePath)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruPathJoin</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>ga8f9b928f302905a50cd91860e384c7fa</anchor>
      <arglist>(trans_chars base, trans_chars file)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruPathMultiJoin</name>
      <anchorfile>group__io.html</anchorfile>
      <anchor>gaa0bd3e2ab8ca42cf6c26dda0f0bf5836</anchor>
      <arglist>(int parts,...)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>kvstore_sec</name>
    <title>Key Value Storage</title>
    <filename>group__kvstore__sec.html</filename>
    <class kind="struct">KvStore_</class>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>kvset</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga9048fec5cec2283bac868aa9a4a42c74</anchor>
      <arglist>)(struct KvStore_ *kvs, const char *key, const char *val, rusize len)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>kvget</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga446330a04500577d5331d33472f19d32</anchor>
      <arglist>)(struct KvStore_ *kvs, const char *key, char **val, rusize *len)</arglist>
    </member>
    <member kind="typedef">
      <type>int32_t(*</type>
      <name>kvlist</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga6ca4b7cac97b25236e64580ed0adfe7d</anchor>
      <arglist>)(struct KvStore_ *kvs, const char *key, ruList *list)</arglist>
    </member>
    <member kind="typedef">
      <type>struct KvStore_</type>
      <name>KvStore</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga9abc942c577335834b99c80262ed4c84</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruFileStore</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga1315d700b19c1627f8d71b0e7fb5f9a4</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruNullStore</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga125fcd1fe6efdcfeaee1c6464e814224</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI KvStore *</type>
      <name>ruNewStore</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga04115021798bd77f9cba634740c97de7</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruFreeStore</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga7fe8e5566d73bb19a7de73edfa043f4a</anchor>
      <arglist>(void *obj)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruValidStore</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>gaa59d479c0140bcb5986b507349e470f5</anchor>
      <arglist>(void *obj)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI KvStore *</type>
      <name>ruNewFileStore</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga6ca53476182ed415e0b2093318c57b10</anchor>
      <arglist>(const char *folderPath, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFileStoreSet</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>gae33879bb447ccb00cb2ab57e4842a09c</anchor>
      <arglist>(KvStore *kvs, const char *key, const char *val, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFileStoreGet</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>gaa08a41b66983aedb1ba42fc7f7402197</anchor>
      <arglist>(KvStore *kvs, const char *key, char **val, rusize *len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruFileStoreList</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga45c4a5338130991a3b589f6eb95926ed</anchor>
      <arglist>(KvStore *kvs, const char *key, ruList *result)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI KvStore *</type>
      <name>ruNewNullStore</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>gac738af5f4a57eb5a8c6c5d31e585bf8e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruNullStoreSet</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>gaf32a7d103a068e73e1eb0fb102af2f4c</anchor>
      <arglist>(KvStore *kvs, const char *key, const char *val, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruNullStoreGet</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>gaf2726dd2b47a21c01328ba9aec9d2298</anchor>
      <arglist>(KvStore *kvs, const char *key, char **val, rusize *len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruNullStoreList</name>
      <anchorfile>group__kvstore__sec.html</anchorfile>
      <anchor>ga7a9a050d8ed3e0be993cbb6a913e39a6</anchor>
      <arglist>(KvStore *kvs, const char *key, ruList *list)</arglist>
    </member>
    <docanchor file="group__kvstore__sec.html" title="Key Value Storage">kvstore_sec</docanchor>
    <docanchor file="group__kvstore__sec.html" title="Key Value Storage">kvstore</docanchor>
    <docanchor file="group__kvstore__sec.html" title="kvstore key">kvkey</docanchor>
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
      <name>ruListIter</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gacc2bc91f4f76b9863603a004dae43194</anchor>
      <arglist>(rl)</arglist>
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
      <anchor>gaaa06a57ba6d6104137eb2092933deac6</anchor>
      <arglist>(ruFreeFunc destructor)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruList</type>
      <name>ruListFree</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga8f7ce9a43ad50071bc1161ecb21a38d5</anchor>
      <arglist>(ruList rl)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruListClear</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gab3649ffbd2db481ba7ad5398d1ff5794</anchor>
      <arglist>(ruList rl)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruListAppendPtr</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gafe233eb17a583b06810e50372781207a</anchor>
      <arglist>(ruList rl, perm_ptr data)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruListInsertAt</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gad362bd738fbd5fdf7e8571ec183249ef</anchor>
      <arglist>(ruList rl, int32_t index, perm_ptr data)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruListInsertAfter</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gad87a85ba0ad40486b640c08f5030ee2f</anchor>
      <arglist>(ruList rl, ruListElmt rle, perm_ptr data)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ptr</type>
      <name>ruListRemove</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga00f37a668ae4e6d7b92af8034179c951</anchor>
      <arglist>(ruList rl, ruListElmt *rle, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ptr</type>
      <name>ruListPop</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gac38106311ed611ce8706246e5ebba17a</anchor>
      <arglist>(ruList rl, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ptr</type>
      <name>ruListTryPop</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga2bbfbf34ff189ebf934273395a11f224</anchor>
      <arglist>(ruList rl, msec_t timeoutMs, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI uint32_t</type>
      <name>ruListSize</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gaf068a6ca647d47215c5a805d35ca5ec4</anchor>
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
      <type>RUAPI ptr</type>
      <name>ruListNextData</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga8a5796acc55b08010cb2bd7160255477</anchor>
      <arglist>(ruListElmt *re, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ptr</type>
      <name>ruListElmtData</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga5d70dcfd4646dea79cfe1ae1ae482cc0</anchor>
      <arglist>(ruListElmt re, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ptr</type>
      <name>ruListIdxElmtData</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gae2fedd16002646e35cc328b88e941cce</anchor>
      <arglist>(ruList rl, int32_t index, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruListJoin</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>ga3f5916035e33df40b498642137ea769f</anchor>
      <arglist>(ruList rl, trans_chars delim, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruListSort</name>
      <anchorfile>group__list.html</anchorfile>
      <anchor>gace02c8adb52bae50e49aac8123e7e21a</anchor>
      <arglist>(ruList rl, ruCompFunc sort)</arglist>
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
      <name>RU_LOG_DBUG</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga87450aef2aa1444e5f86301c28d0907d</anchor>
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
    <member kind="define">
      <type>#define</type>
      <name>ruDbgLog</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga6d7b666987402eb3af7d97e448f021ba</anchor>
      <arglist>(msg)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruDbgLogf</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga3d3ffaf6bf4fa6d4c69329ad93f301b3</anchor>
      <arglist>(fmt,...)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruDump8</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga350ca5f040ec24dc02dd77155fbb61d0</anchor>
      <arglist>(msg, start)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruDump16</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga28a12b3d8bfbc6022ad920435b2cef72</anchor>
      <arglist>(msg, start)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>ruLogFunc</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga20be8e436d37b23248d98325f40a5d9e</anchor>
      <arglist>)(perm_ptr user_data, trans_chars message)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruStdErrorLogger</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>gafbb4cdd0ff940ab6a253c3b69dba1fa1</anchor>
      <arglist>(perm_ptr udata, trans_chars msg)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruSetLogger</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga45e42503b652bda7bc147f5c514f5172</anchor>
      <arglist>(ruLogFunc logger, uint32_t logLevel, perm_ptr userData)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI uint32_t</type>
      <name>ruGetLogLevel</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>ga55a8b3d268484f95bc91525110491606</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruDoesLog</name>
      <anchorfile>group__logging.html</anchorfile>
      <anchor>gaccddd3017ad9c9fd2f1bf9865dcbf699</anchor>
      <arglist>(uint32_t log_level)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>map</name>
    <title>Hash Map</title>
    <filename>group__map.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ruMapPut</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga91b1a3cae7702eb9ebb82ba77d896571</anchor>
      <arglist>(map, key, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruMapTryPut</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga5ca65fc49ff448367a2f1a799f7fe123</anchor>
      <arglist>(map, key, val, existing)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruMapRemove</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gae7b22005687e27d6dffe3aecc834cb65</anchor>
      <arglist>(rm, key, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruMapGet</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga864d402bda9961fc1b07a0b99e44197c</anchor>
      <arglist>(rm, key, val)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruMapHas</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gadcb291d67d6d0b58e6f2e8c85c1e574a</anchor>
      <arglist>(rm, key, code)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruMapFirst</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gaddd443e421d563bfd40135306d5d2b1a</anchor>
      <arglist>(rm, key, value)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruMapNext</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gae132393338c2bb004fe49b31e64570ff</anchor>
      <arglist>(rm, key, value)</arglist>
    </member>
    <member kind="typedef">
      <type>ru_uint(*</type>
      <name>ruHashFunc</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga71667e232bf6a6db737a5ac33535b39b</anchor>
      <arglist>)(trans_ptr key)</arglist>
    </member>
    <member kind="typedef">
      <type>bool(*</type>
      <name>ruMatchFunc</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga0f46f4998b506c57695ff31d21f6ea65</anchor>
      <arglist>)(trans_ptr key1, trans_ptr key2)</arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruMap</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gaad879dbca17e7740cbab5651cafe677d</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ru_uint</type>
      <name>ruIntHash</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga6cfd49276d15ac796a1d49067ef6f6df</anchor>
      <arglist>(trans_ptr key)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruIntMatch</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga430f8d96302a13f878dde7c2180493ca</anchor>
      <arglist>(trans_ptr s1, trans_ptr s2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ru_uint</type>
      <name>ruStrHash</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga180c35fbcfa6b598af925bc5b35fa996</anchor>
      <arglist>(trans_ptr key)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrMatch</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga0b39d859aa373ea70eeb6feee16b0ba1</anchor>
      <arglist>(trans_ptr s1, trans_ptr s2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruMap</type>
      <name>ruMapNew</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga01203552d640f866d8aaa967beb2fa81</anchor>
      <arglist>(ruHashFunc hash, ruMatchFunc match, ruFreeFunc keyFree, ruFreeFunc valFree, uint32_t expectedSize)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruMap</type>
      <name>ruMapNewString</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga9ebe0435352661033425c80b2f502fe6</anchor>
      <arglist>(ruFreeFunc keyFree, ruFreeFunc valFree)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruMap</type>
      <name>ruMapFree</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga533dee52a4d05df820c614bfc9ea426e</anchor>
      <arglist>(ruMap rm)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapPutData</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga1e118478f0538bf381369dffd6bd300d</anchor>
      <arglist>(ruMap rm, ptr key, ptr val, ptr *exisitingVal)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapRemoveData</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>gae929957828624616adebeec1bbadb5a1</anchor>
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
      <name>ruMapHasKey</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga742ae660c4c892899834e8f762752624</anchor>
      <arglist>(ruMap rm, void *key, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapFirstSet</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga946ba2c67280dabacdd04e25994cbe1a</anchor>
      <arglist>(ruMap rm, void **key, void **value)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapNextSet</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga1873d8fcd5947497ae6b18f6f0ad6371</anchor>
      <arglist>(ruMap rm, void **key, void **value)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruMapKeyList</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga789ce12d5f474b500df9a63f065c0b1d</anchor>
      <arglist>(ruMap rm, ruList *keys, ruCloneFunc copy, ruFreeFunc listFree)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI uint32_t</type>
      <name>ruMapSize</name>
      <anchorfile>group__map.html</anchorfile>
      <anchor>ga80853f0b20cccf61661b809304029cf5</anchor>
      <arglist>(ruMap rm, int32_t *code)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>set</name>
    <title>Hash Set</title>
    <filename>group__set.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ruSetPut</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>gaca9e36c0372c7f4ebf7a0d073ce09530</anchor>
      <arglist>(rs, item)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruSetRemove</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga9a4873571e91bbc91d6da8accd739917</anchor>
      <arglist>(rs, item)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruSetHas</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>gabd3f4fd671310f44ac6bc899866a909e</anchor>
      <arglist>(rs, item, code)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruSetFirst</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>gab4d2404553a40aeb9600be8987a2db6c</anchor>
      <arglist>(rs, item)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruSetNext</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>gaaa3f47575f70b0b9414892e735928a91</anchor>
      <arglist>(rs, item)</arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruSet</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>gae7552ee2efa05ecb72079b00ccd81466</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruSet</type>
      <name>ruSetNew</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga5152c782ff1c0b189f2c081ff8c862af</anchor>
      <arglist>(ruHashFunc hash, ruMatchFunc match, ruFreeFunc itemFree, uint32_t expectedSize)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruSet</type>
      <name>ruSetNewString</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga92741e23f4a38993fb8457734af3b74f</anchor>
      <arglist>(ruFreeFunc itemFree)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruSet</type>
      <name>ruSetFree</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga9959e3bb0312f3c6cc7724d2395ae21a</anchor>
      <arglist>(ruSet rs)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruSetPutItem</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>gacfe5e07efc9ccb09baebd6f1e6bc58da</anchor>
      <arglist>(ruSet rs, ptr item)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruSetRemoveItem</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga51c3e995357b9df42e98d9ec388f16ec</anchor>
      <arglist>(ruMap rs, void *item)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruSetHasItem</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>gab6a2f03665c7e591d85e37e4ddb5f321</anchor>
      <arglist>(ruSet rs, void *item, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruSetFirstSet</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga3a03823a946407da763f03e96e6951ae</anchor>
      <arglist>(ruSet rs, void **item)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruSetNextSet</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga15682f6ff5de251efe3016d51bb407d0</anchor>
      <arglist>(ruSet rs, void **item)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruSetItemList</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>gaeab100699b313d914de89c0d82c299cc</anchor>
      <arglist>(ruSet rs, ruList *items, ruCloneFunc copy, ruFreeFunc listFree)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruSetRemoveAll</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga17efcfbea5d931d75b25a195174a4ecc</anchor>
      <arglist>(ruSet rs)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI uint32_t</type>
      <name>ruSetSize</name>
      <anchorfile>group__set.html</anchorfile>
      <anchor>ga0ed8990a61b254c49ebbd98c0a4753c7</anchor>
      <arglist>(ruSet rs, int32_t *code)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>regex</name>
    <title>Regular Expressions</title>
    <filename>group__regex.html</filename>
    <member kind="typedef">
      <type>enum ruRegexFlag_</type>
      <name>ruRegexFlag</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga3120f309b1d9233ced1c84f80ec6482d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruRegex</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga5c28c3ff50463237640750e9f18b1e56</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>ruRegexFlag_</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga648259273846263eeb1ea75d19bd71bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_CANON_EQ</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfa486efddc99fa5ac1cf915da34ce83386</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_CASE_INSENSITIVE</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfacce8f5274ca447940366aea7489a667d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_COMMENTS</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfa45e5a14bc42b51d42f2e9a65d2bb8a6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_DOTALL</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfad8b335ddf5890171941d7e37a3dd06d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_LITERAL</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfa67007fc0d7b98bf94b5ec0e6950e0480</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_MULTILINE</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfa26331f11738b7b1168720821762a5895</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_UNIX_LINES</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfaea2d63f16337c7515893b2769832fd40</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_UWORD</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfa07fb14fbf2beb461874506b804d854dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>RUREGEX_ERROR_ON_UNKNOWN_ESCAPES</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>gga648259273846263eeb1ea75d19bd71bfa1f99eb030fea407e3c045c2a0cfc6b85</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruRegex</type>
      <name>ruRegexFree</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga28f90f28c13dc0e773ef94470600cfb2</anchor>
      <arglist>(ruRegex rr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruRegex</type>
      <name>ruRegexNew</name>
      <anchorfile>group__regex.html</anchorfile>
      <anchor>ga9632dec916aacd02c0c93bd245d4c567</anchor>
      <arglist>(const char *pattern, ruRegexFlag flags, int32_t *code)</arglist>
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
      <name>ruUtf8NoCase</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga40665d092306a601400f51c82b5f5346</anchor>
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
    <member kind="enumeration">
      <type></type>
      <name>ruTrimSide</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga1f4eb9b5f88a1b4cb67b9c9414a1377d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ruTrimBoth</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gga1f4eb9b5f88a1b4cb67b9c9414a1377da4d5a24d0b5b4cee3ad211730487a0e0e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ruTrimStart</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gga1f4eb9b5f88a1b4cb67b9c9414a1377da35b70d5acd6a65d5849b0a42640b0808</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ruTrimEnd</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gga1f4eb9b5f88a1b4cb67b9c9414a1377da4a5ff93d4ab6a21bcc11735930bb999b</anchor>
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
      <type>RUAPI alloc_chars</type>
      <name>ruStrFromNfd</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gae99bd2511930a6b56942af01a0988557</anchor>
      <arglist>(trans_chars instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruStrToNfd</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gacb4048b4c5656829311166d9d5f58a3c</anchor>
      <arglist>(trans_chars instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruStrByteReplace</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gaa92819ba13930511d9849adaefc2e5bd</anchor>
      <arglist>(char *string, char search, char replace)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruStrReplace</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga08fe1c4d77bd06c06ac50a32c2a5ae14</anchor>
      <arglist>(trans_chars instr, trans_chars search, trans_chars replace)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStrCmp</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gaf8c3433629ebca6c711a379ea72b70fe</anchor>
      <arglist>(trans_chars str1, trans_chars str2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrEquals</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga3e2fbb5baaa755249855216fc8f00e53</anchor>
      <arglist>(trans_chars str1, trans_chars str2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrNEquals</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gae9a59f040a02565995096cbf36afa041</anchor>
      <arglist>(trans_chars str1, rusize s1len, trans_chars str2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStrCaseCmp</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga2538deeaef8c7fd41e49cd812729405c</anchor>
      <arglist>(trans_chars str1, trans_chars str2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrCaseEquals</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga080f9ff044104c9acc0e78d13e012b4e</anchor>
      <arglist>(trans_chars str1, trans_chars str2)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrStartsWith</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga433ae2cd0e78e3fd3420f44be56f65b6</anchor>
      <arglist>(trans_chars str, trans_chars prefix, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrCaseStartsWith</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga9607e1b7ca1bb13fbc7e8e5d4977d6dd</anchor>
      <arglist>(trans_chars str, trans_chars prefix, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrEndsWith</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga5a22ba001c2d50fe0c5225e9a4a678a2</anchor>
      <arglist>(trans_chars str, trans_chars suffix, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrCaseEndsWith</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gabab41f441838fb278c5a4d44bbcbfd46</anchor>
      <arglist>(trans_chars str, trans_chars suffix, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI trans_chars</type>
      <name>ruStrCaseStrLen</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga4717f67372cbb5d732775f2f41983e37</anchor>
      <arglist>(trans_chars haystack, trans_chars needle, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI trans_chars</type>
      <name>ruStrStrLen</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga2ae6e27059a437ad29a699f5ff4f5cac</anchor>
      <arglist>(trans_chars haystack, trans_chars needle, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI trans_chars</type>
      <name>ruStrStr</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga702a3526b7fbc5354773879185819c60</anchor>
      <arglist>(trans_chars haystack, trans_chars needle)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI trans_chars</type>
      <name>ruLastSubStrLen</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga5ab766d04763f885efa64087f8193504</anchor>
      <arglist>(trans_chars haystack, trans_chars needle, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI trans_chars</type>
      <name>ruLastSubStr</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga581337c92d1965478d330b050a41e5f4</anchor>
      <arglist>(trans_chars haystack, trans_chars needle)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruList</type>
      <name>ruStrNSplit</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga71cb977988cd498751cb012d65bda22c</anchor>
      <arglist>(trans_chars instr, rusize inlen, trans_chars delim, int32_t maxCnt)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruList</type>
      <name>ruStrSplit</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga34694b838688c90b72cb5a1ad50a79a9</anchor>
      <arglist>(trans_chars instr, trans_chars delim, int32_t maxCnt)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char</type>
      <name>ruAsciiCharToLower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gae86eb432a281131ad5f1cb4eac025396</anchor>
      <arglist>(char in)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruAsciiToLower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga8f33fc35f1c6e0beae717ef06ec8625e</anchor>
      <arglist>(trans_chars instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruAsciiNToLower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga35547041f39b88e4e5f77e1e3018a1f4</anchor>
      <arglist>(trans_chars instr, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char</type>
      <name>ruAsciiCharToUpper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga2185c4486748b4f5c18afb1688fe06cf</anchor>
      <arglist>(char in)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruAsciiToUpper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga6dd19f08ad0f98b841cf0b7648475aa8</anchor>
      <arglist>(trans_chars instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruAsciiNToUpper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gaa00e586d44f2b85b9077f29f2ad97794</anchor>
      <arglist>(trans_chars instr, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruUtf8ToLower</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga6b96ca3b0c9644b84eec7f80361f904b</anchor>
      <arglist>(trans_chars instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruUtf8ToUpper</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga4e4c57837044aa39cda523770373ddee</anchor>
      <arglist>(trans_chars instr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruStrFromNUtf16</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gae17294d8e290f1656e32835b10f51f06</anchor>
      <arglist>(trans_uni unistr, int32_t bytelen)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_chars</type>
      <name>ruStrFromUtf16</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga16107e273f2737161cb0dae2aa6375bc</anchor>
      <arglist>(trans_uni unistr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_uni</type>
      <name>ruStrNToUtf16</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga04c1b9410f8e6b30335c020e7509060c</anchor>
      <arglist>(trans_chars str, int32_t bytelen)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI alloc_uni</type>
      <name>ruStrToUtf16</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gafaf4b6b047bdac6169dce9671e44b219</anchor>
      <arglist>(trans_chars str)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruStrDup</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga337c8eba8887cba279a13dce4d59c7b0</anchor>
      <arglist>(const char *str)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI rusize</type>
      <name>ruStrLen</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gae6486802d2223a0178fbe4b60f1192fe</anchor>
      <arglist>(trans_chars str)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruStrNDup</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gaa7b86a3217edaba9202710d76e142842</anchor>
      <arglist>(const char *str, rusize len)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruDupvPrintf</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga1125ada1e1d67bb70bc518015726fc97</anchor>
      <arglist>(const char *format, va_list arglist)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI char *</type>
      <name>ruDupPrintf</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gac02a41b8d206a2e7518420e8647c28b7</anchor>
      <arglist>(const char *format,...)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStrParseInt64</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga7e64fd093ac04b3a5d981894faeee746</anchor>
      <arglist>(trans_chars str, perm_chars *endptr, uint32_t base, int64_t *num)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int64_t</type>
      <name>ruStrToInt64</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga4e55be57400b2d05ddb2f444c746e5cb</anchor>
      <arglist>(trans_chars str)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStrParseLong</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga6e95ab22909cd73d93c47270bedc0053</anchor>
      <arglist>(trans_chars str, perm_chars *endptr, uint32_t base, long *num)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI long</type>
      <name>ruStrToLong</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga29383f55a77054166e1ed9f2f4abb2be</anchor>
      <arglist>(trans_chars str)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStrParseInt</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga6a5fe3244fd42e0be9bd1fb70cb5bdcf</anchor>
      <arglist>(trans_chars str, perm_chars *endptr, uint32_t base, int32_t *num)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruStrToInt</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>gad2a5254547aafccfdafe768d5422785f</anchor>
      <arglist>(trans_chars str)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrHasChar</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga7c492d4d6fd91aa66e514c11a5e8c083</anchor>
      <arglist>(trans_chars haystack, char needle)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI perm_chars</type>
      <name>ruStrStrip</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga38b87e895b58b7f3a2c4f7ee733766cc</anchor>
      <arglist>(perm_chars instr, trans_chars unwanted, alloc_chars *newstr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI perm_chars</type>
      <name>ruStrTrim</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga258258f27f95aebd563a5a440f84cd79</anchor>
      <arglist>(perm_chars instr, trans_chars unwanted, enum ruTrimSide ends, alloc_chars *newstr)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI trans_chars</type>
      <name>ruStrTrimBounds</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga03bf3ea34de08214815df2b1a1a3628a</anchor>
      <arglist>(trans_chars inStart, rusize inLen, rusize *outLen)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrFindKeyVal</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga7d513fb55d178072c12906fabfb75698</anchor>
      <arglist>(trans_chars inStart, rusize inLen, trans_chars delim, trans_chars *keyStart, rusize *keyLen, trans_chars *valStart, rusize *valLen)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruStrEmpty</name>
      <anchorfile>group__string.html</anchorfile>
      <anchor>ga5df8af06cf764fc696b3bfc0efc3b39c</anchor>
      <arglist>(trans_chars str)</arglist>
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
      <type>RUAPI int32_t</type>
      <name>ruStringReset</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga0a63deacf7d5394f3938e17b99e7d397</anchor>
      <arglist>(ruString rs)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruString</type>
      <name>ruStringFree</name>
      <anchorfile>group__ru_string.html</anchorfile>
      <anchor>ga8d7b3a121134529c02c7f1df28c11224</anchor>
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
    <member kind="define">
      <type>#define</type>
      <name>ruBufferNew</name>
      <anchorfile>group__buffer.html</anchorfile>
      <anchor>gad7c452e2590a64b22d213d1b452b95d9</anchor>
      <arglist>(initialSize)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferAppend</name>
      <anchorfile>group__buffer.html</anchorfile>
      <anchor>gabd7d3693b52b676494d59aaf4057fbed</anchor>
      <arglist>(rb, buf, len)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferGetData</name>
      <anchorfile>group__buffer.html</anchorfile>
      <anchor>gad876921a72ac668e23e841ce57272ef9</anchor>
      <arglist>(rb)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferLen</name>
      <anchorfile>group__buffer.html</anchorfile>
      <anchor>ga29e00c5f225545b0e7d8ad218ef185fe</anchor>
      <arglist>(rb, code)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruBufferFree</name>
      <anchorfile>group__buffer.html</anchorfile>
      <anchor>ga5ce21c10a6e895c5afb738aeb0881e86</anchor>
      <arglist>(rb, keep)</arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruBuffer</name>
      <anchorfile>group__buffer.html</anchorfile>
      <anchor>ga1e5e2b41241f550622126cbecb4820e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruBufferAppendUriEncoded</name>
      <anchorfile>group__buffer.html</anchorfile>
      <anchor>ga768c2165496db6fd441c4baf4d36608e</anchor>
      <arglist>(ruString rs, const char *buf, rusize len)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>ruBuffer</name>
    <title>Buffer Object</title>
    <filename>group__ru_buffer.html</filename>
  </compound>
  <compound kind="group">
    <name>threading</name>
    <title>Threading Related</title>
    <filename>group__threading.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ruCounterInc</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga492be40dd95c10610c21289a393539f2</anchor>
      <arglist>(counter, value)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruCounterRead</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga5f9e0635ce4714e64afebc1e2ad56bd6</anchor>
      <arglist>(counter)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ruCountSet</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gaa34d27a2f63804d04f88054856ea5d20</anchor>
      <arglist>(counter, value)</arglist>
    </member>
    <member kind="typedef">
      <type>unsigned long</type>
      <name>ru_tid</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gadaf55aee31fee401108c789338f5a5d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>pthread_t</type>
      <name>ruThreadId</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gacc47be754ef522e477d51aedd1361301</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruThread</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gac7df455fe262f5fe1d6a40c48fb5a343</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruCount</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga938ce40d266424abe35f3a21705d829e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void *(*</type>
      <name>ruStartFunc</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga9fb7315bef26c1ae9b399b5779ed230b</anchor>
      <arglist>)(void *context)</arglist>
    </member>
    <member kind="typedef">
      <type>void *</type>
      <name>ruMutex</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gaa2f2224294aab80c5b5fdadf8c50a2de</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>RUAPI ru_tid</type>
      <name>ruThreadGetId</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga6966399aea06d9be87db2004b5b624d2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI void</type>
      <name>ruThreadSetName</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga9e6cbe26799920ac795bc967c504ae9f</anchor>
      <arglist>(trans_chars name)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruThread</type>
      <name>ruThreadCreate</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gabb9b39b7a1629a15dac89ba0a450ef13</anchor>
      <arglist>(ruStartFunc start, void *context)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruThread</type>
      <name>ruThreadCreateBg</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga435c7c75b1ecb5175a42b8010c9581fd</anchor>
      <arglist>(ruStartFunc start, void *context)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruThreadFinished</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga1a85ffdcb7817b11ef0279ddfcc80ee6</anchor>
      <arglist>(ruThread tid, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruThreadId</type>
      <name>ruThreadNativeId</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga60572955d0909ffed16ce77a6c8e4058</anchor>
      <arglist>(ruThread tid, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruThreadWait</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gac09b1c7af6581c4cdbbd8232f80a1e53</anchor>
      <arglist>(ruThread tid, sec_t tosecs, void **exitVal)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int</type>
      <name>ruThreadJoin</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gad6e96ffbf89103bede5fbd2274cadac4</anchor>
      <arglist>(ruThread tid, void **exitVal)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int32_t</type>
      <name>ruThreadKill</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga7f548388deccb5a3e695a8b335684a6c</anchor>
      <arglist>(ruThread tid)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruMutex</type>
      <name>ruMutexInit</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gac8236a5017586c076516fb17dae0809c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI bool</type>
      <name>ruMutexTryLock</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga0856c60adc3b35d8a5eec3937ebcdb51</anchor>
      <arglist>(ruMutex m)</arglist>
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
    <member kind="function">
      <type>RUAPI ruCount</type>
      <name>ruCounterNew</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gafc1d31eb0e26e2aae3ed53a1b38295b2</anchor>
      <arglist>(int64_t initialCount)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int64_t</type>
      <name>ruCounterIncValue</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>gadc7eb7e92f2f1b0f574f9a1d5779436a</anchor>
      <arglist>(ruCount counter, int64_t value, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI int64_t</type>
      <name>ruCountSetValue</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga3c1c5ad2c95c127a45b20cca74a8b9cc</anchor>
      <arglist>(ruCount counter, int64_t value, int32_t *code)</arglist>
    </member>
    <member kind="function">
      <type>RUAPI ruCount</type>
      <name>ruCountFree</name>
      <anchorfile>group__threading.html</anchorfile>
      <anchor>ga1d4cb711bdf68899277b323cce866233</anchor>
      <arglist>(ruCount counter)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title></title>
    <filename>index.html</filename>
  </compound>
</tagfile>
