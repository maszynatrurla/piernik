import sys

PER_LINE = 20

snips = []
infile = sys.argv[1]

with open(infile, "rb") as fp:
    txt = fp.read()
    name = infile.split(".")[0]
    
    while True:
        fi = txt.find(b"<#")
        if fi < 0:
            break
            
        fe = txt.find(b"#>", fi)
        assert fi > 0
        
        label = txt[fi+2:fe].strip()
        assert b"<=" not in label
    
        snips.append((fi, label.decode()))
        txt = txt[:fi] + txt[fe + 2:]
    
    #print(txt)
    #print()
    for i,l in snips:
        print("  %-3d %s" % (i, l))
        
    #ntxt = b""
    #idx = 0
    #kiszka = {"mbur": b"raz", "ciach": b"dwa", "sewdup" : b"trzy"}
    
    #for i, l in snips:
    #    ntxt += txt[idx:i]
    #    ntxt += kiszka[l]
    #    idx = i
        
    #print()
    #print(ntxt)
    
with open("template_" + name + ".c", "w") as fp:
    fp.write("""
#include "templater.h"

static const struct 
{
    const char * const_text;
    insertion_t inserts[%d];
} 
s_template_%s = {
    .const_text = 
""" % (len(snips), name))

    for idx in range(0, len(txt), PER_LINE):
        row = txt[idx:idx+PER_LINE]
        fp.write("        \"" + "".join("\\x%02X" % b for b in row) + "\"\n")
    fp.write(""",
    .inserts =
    {
""")
    for idx, label in snips:
        fp.write("""        {
                %d,
                "%s",
        },
""" % (idx, label))

    fp.write("""    }
};


void template_get_%s(template_t * pTemplate)""" % name)
    fp.write("""{
    pTemplate->const_text = s_template_%s.const_text;
    pTemplate->inserts = s_template_%s.inserts;
    pTemplate->count = sizeof(s_template_%s.inserts) / sizeof(*(s_template_%s.inserts));
}

""" % (name, name, name, name))
    
    
    #print("\nconst char http_index_html [] = {")
    #for idx in range(0, len(txt), PER_LINE):
    #    row = txt[idx:idx+PER_LINE]
    #    print("    " + ", ".join("0x%02X" % b for b in row) + ",")
    #print("};\n")
    
    


