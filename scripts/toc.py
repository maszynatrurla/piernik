import sys

PER_LINE = 20

with open(sys.argv[1], "rb") as fp:
    txt = fp.read()
    print("\nconst char http_index_html [] = {")
    for idx in range(0, len(txt), PER_LINE):
        row = txt[idx:idx+PER_LINE]
        print("    " + ", ".join("0x%02X" % b for b in row) + ",")
    print("};\n")
    
    


