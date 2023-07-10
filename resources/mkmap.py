#!/usr/bin/python3
import json, os, sys

def getStrSeq(codepoint):
    str = chr(codepoint).encode('utf-8')
    out = ''
    for c in str:
        out += ("\\x%02x" % c)
    return out

def doAdd(entity, codepoints):
    val = ''
    com = '// ( '
    for p in codepoints:
        val += getStrSeq(p)
        if (p == 0xa):
            com += 'LF'
        elif (p == 0x9):
            com += 'HT'
        else:
            com += chr(p)
    print('        ruMapPut(entities, "%s", "%s"); %s )' % (entity, val, com))

def main():
    myFullName = sys.argv.pop(0)
    myDir = os.path.realpath(os.path.dirname(myFullName))
    # The following page
    # https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
    # referred me to: https://html.spec.whatwg.org/entities.json
    # which is where entities.json came from.
    entfile = os.path.join(myDir, 'entities.json')

    with open(entfile, 'r') as rh:
        j = json.load(rh)
        print('''// GENERATED with make entities
static ruMap getEntityMap(void) {
    static ruMap entities = NULL;
    if (!entities) {
        entities = ruMapNew(ruTypeStrDup(), ruTypeStrDup());
''')
        i = 10000 # make i smaller for testing just a few rows
        maxlen = 0
        for  ent, obj in j.items():
            if ent[-1] != ';': continue # skip html2 entities w/o ;
            maxlen = max(maxlen, len(ent)-2)
            doAdd(ent[1:-1], obj['codepoints'])
            i -= 1
            if i < 1:
                break
        print('''    }
    return entities;
}
''')
        print("// so the parser doesn't search endlessly")
        print('#define MAX_ENTITY_LEN %d' % maxlen)
        return 0

exit(main())
