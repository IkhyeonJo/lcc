"""Static check: src/x86linux.md must mirror src/x86nasm.md rule-for-rule.
Same rules, same order, same costs; only templates differ.  The two AT&T-only
nonterminals are allowed as exact mirrors:
  ibase  == base   (rendered as an open "disp(basereg" prefix for addr: ADDx(index,ibase))
  iindex == index  (rendered as a complete operand for addr: iindex)
usage: python3 -I check_grammar.py <lcc-root>      exit 0 = mirrored
"""
import os, re, sys

def rules(path):
    lines = open(path).read().split("\n")
    marks = [i for i, l in enumerate(lines) if l.strip() == "%%"]
    out = []
    for l in lines[marks[0] + 1:marks[1]]:
        m = re.match(r'^\s*(\w+)\s*:\s*(.*?)\s+"((?:[^"\\]|\\.)*)"\s*(.*)$', l)
        if m:
            out.append((m.group(1), re.sub(r"\s+", "", m.group(2)), m.group(4).strip()))
    return out

root = sys.argv[1]
att = rules(os.path.join(root, "src", "x86linux.md"))
nasm = rules(os.path.join(root, "src", "x86nasm.md"))
ok = True

mapped = [(nt, pat.replace("ibase", "base").replace("iindex", "index") if nt == "addr" else pat, cost)
          for nt, pat, cost in att if nt not in ("ibase", "iindex")]
if mapped != nasm:
    ok = False
    import difflib
    print("x86linux.md and x86nasm.md rules differ (nonterminal, pattern, cost):")
    for l in list(difflib.unified_diff([str(x) for x in nasm], [str(x) for x in mapped], "x86nasm.md", "x86linux.md", n=1, lineterm=""))[:30]:
        print("   ", l)
for extra, orig in (("ibase", "base"), ("iindex", "index")):
    a = [(p, c) for nt, p, c in att if nt == extra]
    b = [(p, c) for nt, p, c in nasm if nt == orig]
    if a != b:
        ok = False
        print("%s does not mirror %s:\n    %s\n    %s" % (extra, orig, a, b))

print("grammar: %d AT&T rules, %d NASM rules -> %s" % (len(att), len(nasm), "mirrored" if ok else "NOT mirrored"))
sys.exit(0 if ok else 1)
